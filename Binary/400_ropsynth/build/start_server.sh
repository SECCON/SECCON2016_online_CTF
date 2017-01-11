#!/bin/sh
socat tcp-listen:11111,fork,reuseaddr,rcvbuf=0 exec:"python -u ./ropsynth.py"
