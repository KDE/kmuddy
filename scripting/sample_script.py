#!/usr/bin/env python

import sys, kmuddy

# Set this to False to use the tuple / loop approach
# In your own scripts you are actually free to use both at the same time
UseCallbacks = True

# An ordinary signal handler that just prints out the text received from
# KMuddy
def status_handler(line):
	sys.stdout.write("Event: %s\n" % line)
	sys.stdout.flush()

# This handler returns a value (not None) and will cause wait4Event() to
# abort execution and return this value. Use this for clean script
# termination, although it's little more than a style thing.
#
# It absolutely works if you just bail out whenever you want without
# ever calling closeSocket()
def status_handler_final(line):
	sys.stdout.write("Final Event: %s\n" % line)
	sys.stdout.flush()
	return True

if __name__ == "__main__":
	sys.stdout.write("KMuddy Python Test v1.0\n")
	# IMPORTANT: ALWAYS flush() your output streams if you want to
	# write data back to KMuddy.
	# Also remember that for this to actually do something you also have to
	# check any of:
	# "Enable script output" / "Enable error output" /
	# / "Send script output" / "Send error output"
	# in the script properties within KMuddy.
	sys.stdout.flush()
	# Initialize KMuddy socket
	# This requires "Communicate variables" being checked in the script
	# properties.
	kmuddy.initSocket()
	if UseCallbacks == True:
		# Approach 1: Attach event handling functions to TCP ports
		kmuddy.registerEvent(1234, status_handler)
		kmuddy.registerEvent(2345, status_handler_final)
		# Enter event loop
		kmuddy.wait4Event()
	else:
		# Approach 2: Register events on TCP ports
		kmuddy.registerEvent(1234)
		kmuddy.registerEvent(2345)
		# ...and let wait4Event() return a list of tuples of (port, data_string)
		while 1:
			evts = kmuddy.wait4Event()
			for evt in evts:
				sys.stderr.write("Event on port %d: %s" % evt)
				sys.stderr.flush()
	kmuddy.closeSocket()

