#!/bin/sh

COMMAND="./home/pageup/PageUP/PageUPclearlog/PAGEUPc"
LOGFILE=restart_c.txt

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
