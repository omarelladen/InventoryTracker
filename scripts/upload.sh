#!/bin/sh

code=firmware/main/main.ino
port=/dev/ttyUSB0

arduino --upload "$code" --port "$port"
