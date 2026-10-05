#!/bin/bash

# Send a message to Pure Data via the named pipe
echo "MIDI device connected: $1" > /tmp/pd_pipe
echo "MIDI device connected: $1" | nc localhost 1234
