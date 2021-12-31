#!/bin/bash
# Find duckyPad and put it into sleep mode. This is used
# together with systemd unit file to turn off lights and
# display before PC is turned off.
#
function get_duckyPad {
  # find out which /dev/hidraw points to duckyPad
  hid=$(find /sys/class/hidraw -ls | sed -n '/0483:D11C/s/.*\///p')
  if [ -z "${hid}" ]
  then
    echo "duckyPad not found" >&2
    return 1
  fi
 echo "/dev/${hid}"
}


function pad {
  # pad payload to 64 bytes
  printf "%-64s" "$(echo -en $1)" | sed 's/ /\x0/g'
}


function hidwrite { 
  # write data ($1) to duckyPad ($2)
  if [ $# -lt 2 ]
  then
    echo "Must specify two arguments for hidwrite" >&2
    exit 1
  fi 
  exec 3<> "$2"
  pad "$1" >&3
  read -u3 -t1 output
  result=$(echo -n "$output" | od -j2 -An -N1 -i)
  [ -z "${result}" ] && result=0
  if [ "${result}" -eq 1 ]
  then
    echo "ERROR" >&2
    return 1
  elif [ "${result}" -eq 2 ]
  then
    echo "BUSY" >&2
    return 2
  fi 
}

# put duckypad to sleep (HID command 15)
dP="$(get_duckyPad)"
if [ -n "${dP}" ]
then
  hidwrite '\x5\x1\x15' "${dP}"
fi
