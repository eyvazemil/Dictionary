#! /bin/sh

BASE_DIR="$1"
_PPID="$2"

exec 1>/dev/null
exec 2>/dev/null

cd "$BASE_DIR/Languages/" && git add .
cd "$BASE_DIR/Languages/" && git commit -m "Update languages"
cd "$BASE_DIR/Languages/" && git push origin master

kill -10 "$_PPID" # send a signal to parent process
