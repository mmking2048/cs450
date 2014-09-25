#!/usr/bin/env bash

apt-get update
apt-get install -y qemu-system-x86
apt-get install -y gdb
echo "set auto-load safe-path /" > /home/vagrant/.gdbinit
