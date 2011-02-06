#!/usr/bin/perl

# Main variable server

require("kmuddy-usock-client.pl");

# Initialise connection to kmuddy's variable server
initVariableSocket();



$varserv_port = 41233;

use Socket;
use Carp;

$| = 1; # Disable output buffering

if (getservbyport($varserv_port,"tcp"))
{
   die("Port already in use\n");
   
} # endif port in use

$prototype = getprotobyname("tcp");

socket(SERV_SOCKET,
       PF_INET,
       SOCK_STREAM,
       $prototype)            || die("Socket creation failure\n");

setsockopt(SERV_SOCKET,
           SOL_SOCKET,
           SO_REUSEADDR, 
           pack("l", 1))      || die "Failure in setsockopt(): $!";

bind(SERV_SOCKET,
     sockaddr_in($varserv_port,
                 INADDR_ANY)) || die("Cannot bind socket\n");

listen(SERV_SOCKET,5)         || die("Cannot listen on socket\n");


print("Temporary test server for Kmuddy event notifications\n");
print("Server is running\n");

%values = ();

while (1)
{
   if (!($clientaddr = accept(MYCONNECTION,SERV_SOCKET)))
   {
      print("Can't accept connection\n");
      die();
   }
   else
   {
      select(MYCONNECTION);

      $| = 1; # Disable buffering on socket
      
      select(STDOUT);

      if (($input = <MYCONNECTION>) ne "")
      {
         chomp($input);
         
         print ("Received: '$input'\n");
         print ("Sending: 'bounce'\n");
         
         sendCommand("bounce");
      } # endif something received
      
      close(MYCONNECTION);
   }

} # endwhile more connections waiting

close(SERV_SOCKET);

# Terminate connection to kmuddy's variable server
closeVariableSocket();
