#!/bin/sh

# COMMAND="./home/pageup/PageUP/PageUPpager/PAGEUPp"
COMMAND="./home/pageup/nrf-rpi/main.py"
LOGFILE=restart_p.txt

writelog() {
  now=`date`
  echo "$now $*" >> $LOGFILE
}

writelog "Starting"
while true ; do
  $COMMAND
  writelog "Exited with status $?"
  writelog "Restarting"
done
