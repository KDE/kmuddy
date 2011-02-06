#!/usr/bin/env python

#---------------------------------------------------------------
# KMuddy scripting support
# 
# Run this file directly (as a script from within KMuddy)
# for a basic functionality test
#---------------------------------------------------------------

import os, sys, socket, select

#---------------------------------------------------------------

"""This module provides full scripting support for KMuddy.
Functions:

initSocket([bufsize]) -- create a new KMuddy connection
closeSocket() -- close the KMuddy connection
getVariable(var) -- get a variable
setVariable(var, value) -- set a variable
unsetVariable(var) -- unset a variable
incVariable(var, [amount]) -- increase var's value
decVariable(var, [amount]) -- decrease var's value
lockVariable(var) -- lock a variable
unlockVariable(var) -- unlock a variable
sendCommand(command) -- send command to MUD
provideResource(resource) -- provide a resource
requestResource(resource) -- request a resource
registerEvent(portno, [func]) -- register an event
unregisterEvent(portno) -- unregister an event
wait4Event() -- wait for an event\n
See method __doc__ properties for details\n"""

evSocks = {}
evPorts = {}
evHandlers = {}
socketName = ""
sock = None
bufferSize = 1024

def initSocket(bufsize = 1024):
	global sock, socketName
	"""initSocket([bufsize])\n
Connect to KMuddy server in order to get/set variables, send commands, etc.\n\n"
bufsize -- length of character buffer (optional, defaults to 32)\n"""
	bufferSize = bufsize
	socketName = os.environ.get("KMUDDY_SOCKET")
	if socketName is None:
		raise RuntimeError, "Environment variable KMUDDY_SOCKET is not set.\nDid you activate \"Communicate variables\" in the \"Input/Output\" section of the script properties?"

	sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
	if sock is None:
		raise RuntimeError, "Could not create socket"
	try:
		sock.connect(socketName)
	except:
		raise RuntimeError, "Could not connect to Kmuddy. Check KMUDDY_SOCKET environment variable."

#---------------------------------------------------------------

def closeSocket():
	global sock
	"""closeSocket()\n\nClose connection to KMuddy server. Doesn't hurt to call this at the end of your script."""

	sock.close()
	sock = None

#---------------------------------------------------------------

def getVariable(var):
	global sock, bufferSize
	"""getVariable(var) -> string\n\nReturn value of variable `var'"""

	sock.send("get %s\n" % var)
	sock.flush()
	return sock.recv(bufferSize)[:-2]

#---------------------------------------------------------------

def setVariable(var, value):
	global sock, bufferSize
	"""setVariable(var, value) -> boolean\n\nSet variable `var' to `value'"""

	sock.send("set %s %s\n" % (var, value))
	sock.flush()
	return sock.recv(bufferSize)[:-2] == "OK"

#---------------------------------------------------------------

def unsetVariable(var):
	global sock, bufferSize
	"""unsetVariable(var) -> boolean\n\nRemove variable `var'"""

	sock.send("unset %s\n" % var)
	sock.flush()
	return sock.recv(bufferSize)[:-2] == "OK"

#---------------------------------------------------------------

def incVariable(var, amount = 1):
	global sock, bufferSize
	"""incVariable(var, [amount]) -> boolean\n\nIncrease variable `var' by `amount' (defaults to 1)"""

	sock.send("inc %s %d\n" % (var, amount))
	sock.flush()
	return sock.recv(bufferSize)[:-2] == "OK"

#---------------------------------------------------------------

def decVariable(var, amount = 1):
	global sock, bufferSize
	"""decVariable(var, [amount]) -> boolean\n\nDecrease variable `var' by `amount' (defaults to 1)"""

	sock.send("dec %s %d\n" % (var, amount))
	sock.flush()
	return sock.recv(bufferSize)[:-2] == "OK"

#---------------------------------------------------------------

def lockVariable(var):
	global sock, bufferSize
	"""lockVariable(var) -> boolean\n\nLock variable `var'"""

	sock.send("lock %s\n" % var)
	sock.flush()
	return sock.recv(bufferSize)[:-2] == "OK"

#---------------------------------------------------------------

def unlockVariable(var):
	global sock, bufferSize
	"""unlockVariable(var) -> boolean\n\nUnlock variable `var'"""

	sock.send("unlock %s\n" % var)
	sock.flush()
	return sock.recv(bufferSize)[:-2] == "OK"

#---------------------------------------------------------------

def sendCommand(command):
	global sock, bufferSize
	"""sendCommand(command) -> boolean\n\nSend a command string to the MUD"""

	sock.send("send %s\n" % command)
	sock.flush()
	return sock.recv(bufferSize)[:-2] == "OK"

#---------------------------------------------------------------

def provideResource(resource):
	global sock, bufferSize
	"""provideResource(resource) -> boolean\n\nProvide a resource"""

	sock.send("provide %s\n" % resource)
	sock.flush()
	return sock.recv(bufferSize)[:-2] == "OK"

#---------------------------------------------------------------

def requestResource(resource):
	global sock, bufferSize
	"""requestResource(resource) -> boolean\n\n	Request a resource from the MUD"""

	sock.send("request %s\n" % resource)
	sock.flush()
	return sock.recv(bufferSize)[:-2] == "OK"

#---------------------------------------------------------------

def registerEvent(portno, func = None):
	global evSocks, evPorts, evHandlers
	"""registerEvent(portno, [func]) -> boolean\n
Register/update an event for notification by KMuddy\n
This function binds a TCP socket to the local loopback address on the given port.
The script can then be notified from KMuddy using /notify <portno> <data>.
If the respective port number has been registered before, its handler is updated.

portno -- TCP port number to bind to (1-65535, ports <= 1024 require root privileges)
func -- event handler function (optional)
	
If func is given, future calls to wait4Event() will transparently call it whenever
the event occurs.
If func is omitted, future calls to	wait4Event() will return whenever the event occurs,
yielding a list containing tuples of the form (portno, data_string).
	
func is a function taking a single string argument containing the text that was
sent to the script.

registerEvent() returns True on success and False in case of an error."""

	if portno in evSocks:
		if portno in evHandlers and func is None:
			del evHandlers[portno]
			return True
		return False
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	s.setblocking(0)
	s.bind(("", portno))
	s.listen(1)
	evSocks[portno] = s
	evPorts[s] = portno
	if func is not None:
		evHandlers[portno] = func
	return True
	
#---------------------------------------------------------------

def unregisterEvent(portno):
	global evSocks, evPorts, evHandlers
	"""unregisterEvent(portno) -> boolean\n
Unregister an event associated to port `portno'\n
Returns False if the event was not registered before, True on success"""
	if portno not in evSocks:
		return False
	del evPorts[evSocks[portno]]
	del evSocks[portno]
	if portno in evHandlers:
		del evHandlers[portno]
	return True

#---------------------------------------------------------------

def wait4Event(timeout = None):
	global evSocks, evPorts, evHandlers
	"""wait4Event([timeout]) -> list of tuples
Wait for an event to occur\n
timeout -- seconds to wait before giving up and returning None (optional),
           0 causes immediate return if no data is present.\n
wait4Event() efficiently suspends the script until one or more previously registered
events occur.
For events with an attached event handler the latter is called, getting the received
line passed as the only argument.
If at least one event without an attached event handler occured, a tuple containing the
port number and the received data is added to a list which is returned when all pending
events have been processed.
If all registered events possess an attached event handler, wait4Event() resumes infinitely.
If any event handler returns an actual value (i.e. not None), wait4Event() immediately
returns this value. This should be used for clean script termination.
However, there is no guarantee that all pending events are processed in that case
(although it's highly unlikely that more than one event occurs at a time).
Eventually, if the optional timeout expires, an empty list is returned."""

	while 1:
		if evSocks == {}:
			return None
		retval = []
		handler_retval = None
		for s in select.select(evSocks.values(), [], [], timeout)[0]:
			port = evPorts[s]
			try:
				in_s = s.recv(bufferSize) # socket already connected
			except socket.error: # new connection
				del evSocks[port]
				del evPorts[s]
				evSocks[port] = s.accept()[0]
				evPorts[evSocks[port]] = port
				in_s = evSocks[port].recv(bufferSize)
			if in_s == "": # conn reset by peer
				del evPorts[evSocks[port]]
				del evSocks[port]
				continue
			if port in evHandlers:
				for line in in_s[:-1].split("\n"):
					handlerRetval = evHandlers[port](line)
					if handlerRetval is not None:
						return handlerRetval
			else:
				retval.append((port, in_s))
		if retval != [] or timeout is not None:
			return retval

#---------------------------------------------------------------

def my_func(args):
	print "HANDLER CALLED: " + args
	return True

#---------------------------------------------------------------

if __name__ == "__main__":

	initSocket()
	print "KMUDDY_SOCKET: " + socketName
	setVariable("test", "this is some test")
	print "Value of variable 'test' has been set"
	test = getVariable("test")
	print "So, value of test = " + test
	unsetVariable("test")
	print "The variable test is no longer set"
	registerEvent(1234, my_func)
	wait4Event()
	closeSocket()

