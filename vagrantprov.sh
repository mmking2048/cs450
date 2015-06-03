#!/usr/bin/env bash

apt-get update
apt-get install -y qemu-system-x86
apt-get install -y gdb
apt-get install -y tmux

echo "set auto-load safe-path /" > /home/vagrant/.gdbinit
