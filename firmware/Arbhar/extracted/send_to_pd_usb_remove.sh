#!/bin/bash

# Send a message to Pure Data via the named pipe
echo "MIDI device removed: $1" > /tmp/pd_pipe
echo "MIDI device removed: $1" | nc -u localhost 1234
