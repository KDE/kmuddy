#!/usr/bin/perl

#---------------------------------------------------------------
# Kmuddy scripting - variable support
# 
# require() this file in a Perl script, if you want to use
# variables in that script
#---------------------------------------------------------------

# Public interface
#
# ** Call this at the beginning of your script
# initVariableSocket();
#
# ** Call this at the end of your script
# closeVariableSocket();
#
# ** Reads variable value and returns it
# $var = getVariable("myvarname");
#
# ** Sets variable value
# setVariable("myvarname","myvarvalue");
#
# ** Unsets a variable
# unsetVariable("myvarname");
#
# ** Sends a command to the MUD
# sendCommand("say hello");


#---------------------------------------------------------------
# Implementation details follow
#---------------------------------------------------------------

use Socket;

local $socket_filevar = "";
my $line;


#---------------------------------------------------------------
# Connect to kmuddy variable server
#---------------------------------------------------------------

sub initVariableSocket
{
   my $socket_name = $ENV{"KMUDDY_SOCKET"};
   
   if ($socket_name eq "")
   {
      die("Variable KMUDDY_SOCKET is not set - variable support won't work!\n");
   } # endif unknown value of kmuddy socket environment variable
   
   if (!socket(SOCK,PF_UNIX,SOCK_STREAM,0))
   {
      die("Could not reserve a socket to connect to Kmuddy\n");
   } # endif can't get a socket

   if (!connect(SOCK,sockaddr_un($socket_name)))
   {
      close(SOCK);
      die("Could not connect to Kmuddy\n");
   } # endif can't connect to kmuddy

   # Now we're connected, do some housekeeping

   # Turn off output buffering
   select(SOCK);
   $| = 1;
   select(STDOUT);

   # Remember socket filehandle
   $socket_filevar = \*SOCK;
   
} # initVariableSocket



#---------------------------------------------------------------
# Close connection to kmuddy variable server
#---------------------------------------------------------------

sub closeVariableSocket
{
   close($socket_filevar);
} # closeVariableSocket



#---------------------------------------------------------------
# Get a variable's value from the kmuddy variable server
#---------------------------------------------------------------

sub getVariable
{
   my($name) = @_;
   my($result);
   
   print $socket_filevar ("get $name\n");
 
   $result = <$socket_filevar>;
   chomp($result);
   
   return $result;

} # getVariable



#---------------------------------------------------------------
# Set a variable on the kmuddy variable server
#---------------------------------------------------------------

sub setVariable
{
   my($name,
      $value) = @_;
   
   my($reply);
   
   print $socket_filevar ("set $name $value\n");
   
   $reply = <$socket_filevar>;
   
} # setVariable



#---------------------------------------------------------------
# Remove a variable from the kmuddy variable server
#---------------------------------------------------------------

sub unsetVariable
{
   my($name) = @_;
   
   my($reply);
   
   print $socket_filevar ("unset $name\n");
   
   $reply = <$socket_filevar>;

} # unsetVariable



#---------------------------------------------------------------
# Send a command to the MUD
#---------------------------------------------------------------

sub sendCommand
{
   my($command) = @_;
   my($result);
   
   print $socket_filevar ("send $command\n");
   
   $result = <$socket_filevar>;
   chomp($result);
   
   return $result;
   
} # sendCommand

1;
