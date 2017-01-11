#!/bin/sh

USER=user
ROOTDIR=/home/$USER/root
UID=1001
GID=1001
HOSTNAME=localhost
PORT=10000
TIMER=180

$ROOTDIR/bin/simple-inetd $HOSTNAME $PORT 0 0 $ROOTDIR \
	./bin/timer-exec timer-exec $TIMER 9 $UID $GID $ROOTDIR \
	./bin/run run --h8300h ./bin/hello.elf
