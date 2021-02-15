#! /bin/sh

BASE_DIR="$1"
_PPID="$2"

exec 1>/dev/null
exec 2>/dev/null

cd "$BASE_DIR/Languages/" && git pull

kill -10 "$_PPID" # send a signal to parent process
