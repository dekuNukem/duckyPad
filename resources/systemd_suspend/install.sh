#!/bin/sh
# install systemd unit and shell script that puts duckyPad  
# to sleep mode right before shutdown/reboot
set -u
set -e

if [ $(id -u) -ne 0 ]
then
  echo "Rerun as root"
  exit 1
fi

cp suspend_duckypad.service /etc/systemd/system/
install -Dm755 suspend_duckypad.sh /usr/share/duckypad/suspend_duckypad.sh
systemctl daemon-reload
systemctl enable suspend_duckypad
systemctl is-enabled suspend_duckypad
