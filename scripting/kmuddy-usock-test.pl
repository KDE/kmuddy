#!/usr/bin/perl

require("kmuddy-usock-client.pl");

# Disable STDOUT buffering
$| = 1;

initVariableSocket();

setVariable("test","this is some test");

print("say Value of variable 'test' has been set\n");

print("KMUDDY_SOCKET: $ENV{'KMUDDY_SOCKET'}\n");

$test = getVariable("test");

print("say So, value of test = $test\n");

unsetVariable("test");

print("say The variable test is no longer set\n");

sleep(10000);

closeVariableSocket();

exit 0;

