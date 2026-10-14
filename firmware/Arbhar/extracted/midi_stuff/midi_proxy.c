#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/inotify.h>
#include <sys/ioctl.h>

#define MIDI_DEV_PATH "/dev/snd/"
//#define FIFO_PATH "/tmp/midi_fifo"
#define FIFO_PATH "/mnt/ramdisk/midi_fifo"
#define FIFO_PERMISSIONS 0666  // Read/Write permissions for all users
#define BUF_SIZE 1024
#define MAX_MIDI_DEVICES 10
#define INOTIFY_BUF_SIZE (1024 * (sizeof(struct inotify_event) + 16))

// Function to find and open a MIDI device
int open_midi_device(const char *device) {
    int fd = open(device, O_RDONLY | O_NONBLOCK);  // Non-blocking mode
    if (fd < 0) {
        perror("Failed to open MIDI device");
    }
    return fd;
}

// Function to scan for available midiC* devices and open them
int scan_and_open_midi_devices(int *midi_fds, int max_devices) {
    DIR *dir;
    struct dirent *entry;
    char midi_device[256];
    int count = 0;

    dir = opendir(MIDI_DEV_PATH);
    if (dir == NULL) {
        perror("Failed to open /dev/snd directory");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL && count < max_devices) {
        if (strncmp(entry->d_name, "midiC", 5) == 0) {
            snprintf(midi_device, sizeof(midi_device), "%s%s", MIDI_DEV_PATH, entry->d_name);
            int fd = open_midi_device(midi_device);
            if (fd >= 0) {
                midi_fds[count++] = fd;
                printf("Opened MIDI device: %s\n", midi_device);
            }
            else {
                printf("MIDI device: %s not connecting", midi_device);
            }
        }
    }

    closedir(dir);
    return count;
}

// Function to handle device removal
void close_midi_device(int *midi_fds, int index, int *num_devices) {
    printf("Closing MIDI device fd: %d\n", midi_fds[index]);
    close(midi_fds[index]);
    
    // Shift the remaining devices in the array
    for (int i = index; i < *num_devices - 1; i++) {
        midi_fds[i] = midi_fds[i + 1];
    }
    
    (*num_devices)--;
}

int main() {
    int midi_fds[MAX_MIDI_DEVICES] = {0};  // Array of MIDI device file descriptors
    int num_devices = 0;  // Number of currently open devices
    char buffer[BUF_SIZE];
    ssize_t bytes_read;
    fd_set read_fds;
    int fifo_fd;
    int max_fd = 0;

    // Check if the FIFO exists, and create it if it doesn't
    if (access(FIFO_PATH, F_OK) == -1) {  // Check if FIFO exists
        printf("FIFO does not exist, creating %s...\n", FIFO_PATH);
        if (mkfifo(FIFO_PATH, FIFO_PERMISSIONS) < 0) {
            perror("Failed to create FIFO");
            return 1;
        }
    }

    // Open FIFO for writing
    fifo_fd = open(FIFO_PATH, O_WRONLY);
    if (fifo_fd < 0) {
        perror("Failed to open FIFO");
        return 1;
    }

    // Initial scan for available MIDI devices
    num_devices = scan_and_open_midi_devices(midi_fds, MAX_MIDI_DEVICES);
    if (num_devices == 0) {
        printf("No MIDI devices found initially.\n");
    }

    // Set up inotify to monitor changes in the /dev/snd/ directory
    int inotify_fd = inotify_init();
    if (inotify_fd < 0) {
        perror("inotify_init failed");
        return 1;
    }

    int wd = inotify_add_watch(inotify_fd, MIDI_DEV_PATH, IN_CREATE | IN_DELETE);
    if (wd < 0) {
        perror("Failed to add inotify watch on /dev/snd/");
        return 1;
    }

    // Buffer to store inotify events
    char inotify_buffer[INOTIFY_BUF_SIZE];

    while (1) {
        // Clear the fd_set and add MIDI device fds
        FD_ZERO(&read_fds);
        max_fd = fifo_fd > inotify_fd ? fifo_fd : inotify_fd;

        for (int i = 0; i < num_devices; i++) {
            FD_SET(midi_fds[i], &read_fds);
            if (midi_fds[i] > max_fd) {
                max_fd = midi_fds[i];
            }
        }

        // Add inotify_fd to the set
        FD_SET(inotify_fd, &read_fds);

        // Wait for input on any MIDI device or inotify event
        int ready = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (ready < 0) {
            perror("select() failed");
            break;
        }

        // Handle inotify events
        if (FD_ISSET(inotify_fd, &read_fds)) {
            int length = read(inotify_fd, inotify_buffer, INOTIFY_BUF_SIZE);
            if (length < 0) {
                perror("inotify read failed");
                continue;
            }

            int i = 0;
            while (i < length) {
                struct inotify_event *event = (struct inotify_event *)&inotify_buffer[i];

                // Handle device creation
                if (event->mask & IN_CREATE && strncmp(event->name, "midiC", 5) == 0) {
                    printf("New MIDI device detected: %s\n", event->name);
                    char new_device[256];
                    snprintf(new_device, sizeof(new_device), "%s%s", MIDI_DEV_PATH, event->name);

                    if (num_devices < MAX_MIDI_DEVICES) {
                        int fd = open_midi_device(new_device);
                        if (fd >= 0) {
                            midi_fds[num_devices++] = fd;
                        }
                    }
                }

                // Handle device removal
                if (event->mask & IN_DELETE && strncmp(event->name, "midiC", 5) == 0) {
                    printf("MIDI device removed: %s\n", event->name);
                    
                    // Close the corresponding MIDI device
                    for (int j = 0; j < num_devices; j++) {
                        char midi_device[256];
                        snprintf(midi_device, sizeof(midi_device), "%s%s", MIDI_DEV_PATH, event->name);

                        if (fcntl(midi_fds[j], F_GETFD) == -1) {
                            // If the device is no longer accessible, close it
                            close_midi_device(midi_fds, j, &num_devices);
                        }
                    }
                }

                i += sizeof(struct inotify_event) + event->len;
            }
        }

        // Read from MIDI devices and forward to FIFO
        for (int i = 0; i < num_devices; i++) {
            if (FD_ISSET(midi_fds[i], &read_fds)) {
                bytes_read = read(midi_fds[i], buffer, BUF_SIZE);

                if (bytes_read > 0) {
                    // Forward data to FIFO
                    write(fifo_fd, buffer, bytes_read);
                } else if (bytes_read < 0) {
                    // Error reading from the device, assume it has been disconnected
                    if (errno == ENODEV || errno == EIO) {
                        printf("MIDI device disconnected (fd: %d)\n", midi_fds[i]);
                        close_midi_device(midi_fds, i, &num_devices);
                    } else {
                        perror("Error reading from MIDI device");
                    }
                }
            }
        }
    }

    // Cleanup: Close all devices and inotify
    close(fifo_fd);
    close(inotify_fd);
    for (int i = 0; i < num_devices; i++) {
        close(midi_fds[i]);
    }

    return 0;
}

