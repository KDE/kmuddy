#!/usr/bin/perl

use Fcntl;
use Socket;

# Port to listen on - if you have multiple scripts, each must listen on
# different port number. Also, port numbers must be in range 1024-65535.
my $port = 2400; 

sub handleText
{
  my $line = $_[0];
  # handle incoming text here
  
}

sub handleInput
{
  my $line = $_[0];
  # handle user's commands here
  
}

sub parsePrompt
{
  my $line = $_[0];
  # handle prompt here
  
}

sub handleNotify
{
  my $line = $_[0];
  # handle notify requests here

}

sub fhbits
{
  my(@fhlist) = split(' ', $_[0]);
  my($bits);
  for (@fhlist) {
    vec ($bits, fileno ($_), 1) = 1;
  }
  $bits;
}

sub notifyPrepare
{
  # most of this comes from tmpserver.pl
  if (getservbyport (port,"tcp"))
  {
    die ("Port already in use\n");
  }

  my $prototype = getprotobyname ("tcp");

  socket(SERV_SOCKET, PF_INET, SOCK_STREAM, $prototype)
      or die("Socket creation failure\n");

  setsockopt(SERV_SOCKET, SOL_SOCKET, SO_REUSEADDR, pack ("l", 1))
      or die ("Failure in setsockopt(): $!");

  bind(SERV_SOCKET, sockaddr_in($port, INADDR_ANY))
      or die ("Cannot bind socket\n");

  listen (SERV_SOCKET, 1)
      or die ("Cannot listen on socket\n");

  $notifyOpen = 0;
}

$| = 1;

$flags = fcntl (STDIN, F_GETFL, 0);
fcntl (STDIN, F_SETFL, $flags | O_NONBLOCK)
    or die "Cannot set non-blocking mode\n";

# prepare the socket for notify
notifyPrepare ();

while (1)
{
  my $rin = fhbits ('STDIN SERV_SOCKET NOTIFY');
  my $nfound = select ($bits=$rin, undef, undef, undef);
  
  if (vec ($bits, fileno (STDIN), 1) == 1)
  {
    while ($rd = <STDIN>)
    {
      $type = substr ($rd, 0, 1);
      $text = substr ($rd, 2, length($rd) - 2);
      if (int ($text[length ($text) - 1]) < 32)
      {
        $text = substr ($text, 0, length($text) - 1);
      }

      if ($type eq "1")
      {
        handleText ($text);
      }
      elsif ($type eq "2")
      {
        handleInput ($text);
      }
      elsif ($type eq "3")
      {
        parsePrompt ($text);
      }
    }
  }
  if (vec ($bits, fileno (SERV_SOCKET), 1) == 1)
  {
    accept (NOTIFY, SERV_SOCKET);
    $notifyOpen = 1;
    $flags = fcntl (NOTIFY, F_GETFL, 0);
    fcntl (NOTIFY, F_SETFL, $flags | O_NONBLOCK)
        or die "Cannot set non-blocking mode\n";
  }
  if ($notifyOpen and (vec ($bits, fileno (NOTIFY), 1) == 1))
  {
    # read from socket
    while ($rd = <NOTIFY>) {
      # strip trailing newline
      if (int ($rd[length ($rd) - 1]) < 32)
      {
        $rd = substr ($rd, 0, length($rd) - 1);
      }
      handleNotify ($rd);
    }
  }

}

close (NOTIFY);
close (SERV_SOCKET);
  

