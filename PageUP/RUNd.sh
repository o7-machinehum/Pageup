#!/bin/sh

COMMAND="./home/pageup/PageUP/PageUPdiscrete/PAGEUPd"
LOGFILE=restart_d.txt

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
