#pragma once

#include <system.hpp>
#include <string.hpp>
#include <dirent.h>
#include <sys/stat.h>
#include <ctime>
#include <cstdio>
#include <random>
#include <string>
#include <vector>
#include <mutex>
#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#endif

namespace chimera {
// Off-audio only. OS leases survive neither a crash nor process termination.
// A root lease serializes directory creation/deletion, including closing the
// session lease before unlink on Windows. Never unlink the root lease inode.
class CheckpointLease {
public:
    CheckpointLease() = default;
    CheckpointLease(const CheckpointLease&) = delete;
    CheckpointLease& operator=(const CheckpointLease&) = delete;
    ~CheckpointLease() { close(); }
    bool open(const std::string& path) {
        close();
#ifdef _WIN32
        handle_ = CreateFileW(rack::string::UTF8toUTF16(path).c_str(), GENERIC_READ | GENERIC_WRITE,
            0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        return handle_ != INVALID_HANDLE_VALUE;
#else
        fd_ = ::open(path.c_str(), O_CREAT | O_RDWR | O_NOFOLLOW, 0600);
        if (fd_ >= 0 && flock(fd_, LOCK_EX | LOCK_NB) == 0) return true;
        close(); return false;
#endif
    }
    void close() {
#ifdef _WIN32
        if (handle_ != INVALID_HANDLE_VALUE) CloseHandle(handle_);
        handle_ = INVALID_HANDLE_VALUE;
#else
        if (fd_ >= 0) ::close(fd_);
        fd_ = -1;
#endif
    }
private:
#ifdef _WIN32
    HANDLE handle_ = INVALID_HANDLE_VALUE;
#else
    int fd_ = -1;
#endif
};

class CheckpointSession {
public:
    ~CheckpointSession() { lease_.close(); } // Failed deletes remain eligible for the next sweep.
    const std::string& directory() const { return directory_; }
    bool ensure(const std::string& root) {
        if (!directory_.empty()) return true;
        if (!rack::system::createDirectories(root) && !rack::system::isDirectory(root)) return false;
        CheckpointLease rootLease;
        if (!rootLease.open(root + "/owner.lock")) return false;
        sweepLocked(root, std::time(nullptr), 86400);
        std::random_device random;
        for (unsigned attempt = 0; attempt < 8; ++attempt) {
            char id[40];
            std::snprintf(id, sizeof(id), "session-v1-%08x%08x%08x", random(), random(), random());
            const std::string candidate = root + "/" + id;
            if (rack::system::exists(candidate)) continue;
            if (!rack::system::createDirectory(candidate)) return false;
            if (!lease_.open(candidate + "/owner.lock")) return false;
            directory_ = candidate;
            return true;
        }
        return false;
    }
    // Returns number of removed sessions. Budget bounds entries visited, not
    // just successful deletions. Unknown files and links make a session ineligible.
    static unsigned sweep(const std::string& root, std::time_t now, unsigned graceSeconds = 86400) {
        CheckpointLease rootLease;
        return rootLease.open(root + "/owner.lock") ? sweepLocked(root, now, graceSeconds) : 0;
    }
private:
    static bool ordinary(const std::string& path, bool directory) {
#ifdef _WIN32
        const DWORD attr = GetFileAttributesW(rack::string::UTF8toUTF16(path).c_str());
        return attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_REPARSE_POINT) &&
            bool(attr & FILE_ATTRIBUTE_DIRECTORY) == directory;
#else
        struct stat info{};
        return lstat(path.c_str(), &info) == 0 &&
            (directory ? S_ISDIR(info.st_mode) : S_ISREG(info.st_mode));
#endif
    }
    static bool sessionName(const std::string& name) {
        if (name.size() != 35 || name.compare(0, 11, "session-v1-")) return false;
        return name.find_first_not_of("0123456789abcdef", 11) == std::string::npos;
    }
    static bool checkpointName(const std::string& name) {
        if (name.compare(0, 5, "reel-")) return false;
        const auto dash = name.find('-', 5), dot = name.find('.', 5);
        if (dash == std::string::npos || dot == std::string::npos || dash == 5 || dot <= dash + 1)
            return false;
        if (name.substr(dot) != ".wav" && name.substr(dot) != ".wav.tmp") return false;
        return name.substr(5, dash-5).find_first_not_of("0123456789") == std::string::npos &&
            name.substr(dash+1, dot-dash-1).find_first_not_of("0123456789") == std::string::npos;
    }
    static unsigned sweepLocked(const std::string& root, std::time_t now, unsigned grace) {
        // Resume bounded scans so live/unknown entries cannot indefinitely
        // starve sessions later in a large directory. One cached iterator only.
        struct Cursor {
            std::mutex mutex;
            std::string root;
            DIR* entries = nullptr;
            ~Cursor() { if (entries) closedir(entries); }
        };
        static Cursor cursor;
        std::lock_guard<std::mutex> guard(cursor.mutex);
        if (cursor.root != root) {
            if (cursor.entries) closedir(cursor.entries);
            cursor.entries = nullptr; cursor.root = root;
        }
        if (!cursor.entries) cursor.entries = opendir(root.c_str());
        DIR* entries = cursor.entries;
        if (!entries) return 0;
        unsigned visited = 0, removed = 0;
        while (visited < 128 && removed < 16) {
            dirent* entry = readdir(entries);
            if (!entry) { closedir(entries); cursor.entries = nullptr; break; }
            ++visited;
            const std::string name = entry->d_name, path = root + "/" + name;
            struct stat info{};
            if (!sessionName(name) || !ordinary(path, true) || stat(path.c_str(), &info) ||
                now < info.st_mtime || now - info.st_mtime < grace ||
                !ordinary(path + "/owner.lock", false)) continue;
            CheckpointLease sessionLease;
            if (!sessionLease.open(path + "/owner.lock")) continue;
            DIR* files = opendir(path.c_str());
            if (!files) continue;
            bool eligible = true;
            std::vector<std::string> paths;
            unsigned seen = 0;
            while (dirent* file = readdir(files)) {
                const std::string child = file->d_name;
                if (child == "." || child == ".." || child == "owner.lock") continue;
                if (++seen > 8 || !checkpointName(child) || !ordinary(path + "/" + child, false)) {
                    eligible = false; break;
                }
                paths.push_back(path + "/" + child);
            }
            closedir(files);
            if (!eligible) continue;
            for (const auto& file : paths) if (!rack::system::remove(file)) eligible = false;
            sessionLease.close();
            if (eligible && rack::system::remove(path + "/owner.lock") &&
                rack::system::remove(path)) ++removed; // Nonrecursive, empty directory only.
        }
        return removed;
    }
    CheckpointLease lease_;
    std::string directory_;
};
} // namespace chimera
