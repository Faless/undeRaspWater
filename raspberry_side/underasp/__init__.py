# -*- coding: utf-8 -*-

from subprocess import PIPE, Popen as popen
from threading  import Thread
from Queue import Queue, Empty
import syslog


ARDUI2C="/home/pi/ardui2c"
BROADCAST_PORT=50000
DELAY=5


def enqueue_output(out, queue):
    for line in iter(out.readline, b''):
        queue.put(line)
    out.close()


def ardIO(cmd):
   if cmd.startswith("Y"):
      syslog.syslog(syslog.LOG_ERR, "Sending ARDIO %s" % cmd)
   p = Popen([ARDUI2C, cmd], shell=False, stdin=None, stdout=PIPE, stderr=None, close_fds=True)
   p.wait()
   data = p.stdout.read()
   try:
       return float(data)
   except:
       syslog.syslog(syslog.LOG_ERR, "Error reading ardIO value: %s" % data)
   return -1


def quit(val, debug_mode=False):
   if not debug_mode:
      ardIO("Q")
      p = Popen(["sudo","halt"], stdin=None, stdout=None, stderr=None)
      if(val)==0:
	ardIO("Y")
   sys.exit(val)


def ardi2c(*args):
    p = popen([ARDUI2C, list(args)], stdin=None, stdout=PIPE, stderr=None, close_fds=True)
    return p.stdout.read()


def get_run_mode():
    """
    Return tuple (is_test, run_mode)
    """
    val = ardIO('m')
    runstatus = int(val[0:val.find(".")])
    return [(runstatus & 0x80) > 0, runstatus & 0x7f]
