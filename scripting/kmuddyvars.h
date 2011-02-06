/**
 * KMuddy scripting - variable support
 *
 * #include this file in a C/C++ script, if you want to use variables
 * in that script.
 *
 * */

/* PUBLIC INTERFACE: */

/** call this at the beginning of your script */
void initVariableSocket ();

/** call this at the end of your script */
void closeVariableSocket ();

/** call this to set your script up to listen on an IP port */
void initIPPort(const int ip_port);

/** wait for and retrieve event notification from IP port */
void getEvent (char *buffer, int buflen);

/** reads variable value and store it in the buffer (of size buflen+1) */
void getVariable (const char *name, char *buffer, int buflen);

/** sets a variable value */
char setVariable (const char *name, const char *value);

/** unsets a variable */
char unsetVariable (const char *name);

/** increases a variable value */
char incVariable (const char *name, int delta);

/** decreases a variable value */
char decVariable (const char *name, int delta);

/** provide a resource */
char provideResource (const char *name);

/** request a resource */
char requestResource (const char *name);

/** request variable lock */
char lockVariable (const char *name);

/** unlock a variable */
void unlockVariable (const char *name);

/** sends a command to the MUD */
char sendCommand (const char *command);




/* INTERNAL VARIABLES AND FUNCTIONS -- DO NOT USE IN THE SCRIPT */
#define MAX_PENDING_CONNECTIONS 5
char internalBuffer[51];
int variableSocket = -1;
int serverSocket   = -1;
int clientSocket   = -1;
char simpleCommand (const char *command, const char *name);

void sendAndReceive (const char *sendthis, char *buffer, int buflen);

/* IMPLEMENTATION */

#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <kvbox.h>


void initVariableSocket ()
{
   struct sockaddr_un sa;
   char *path;
   path = getenv ("KMUDDY_SOCKET");
   if (!path)
   {
      puts ("Variable KMUDDY_SOCKET is not set - variable support won't work!\n");
      fflush (stdout);
      return; /* bah! */
   }
   variableSocket = socket (PF_UNIX, SOCK_STREAM, 0);
   sa.sun_family = AF_UNIX;
   strcpy (sa.sun_path, path);
   if (connect (variableSocket, (struct sockaddr *) &sa, sizeof (sa)) == -1)
   {
      close (variableSocket);
      variableSocket = -1;  /* :( */
      puts ("Could not connect to KMuddy!\n");
      printf ("Error = %s\n", strerror (errno));
      fflush (stdout);
   }
   /* okay, we should be connected now... */
}

void closeVariableSocket ()
{
  if (variableSocket != -1)
     close (variableSocket);

  if (serverSocket != -1)
     close (serverSocket);

  if (clientSocket != -1)
     close(clientSocket);

}


/** call this to set your script up to listen on an IP port */
void initIPPort(const int ip_port)
{
   struct sockaddr_in server_address;
   int                val = 1;
   int                result;

   serverSocket = socket(AF_INET,
                         SOCK_STREAM,
                         0);

   if (serverSocket < 0)
   {
      puts ("Unable to create a socket - server support won't work!\n");
      fflush (stdout);
      return; /* bah! */
   } /* endif unable to obtain a socket */

   result = setsockopt(serverSocket,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       (char *)&val,
                       sizeof(val));

   if (result != 0)
   {
      close(serverSocket);
      puts ("Unable to modify socket options - server support won't work!\n");
      fflush (stdout);
      return; /* bah! */
   } /* endif unable to modify socket options */

   server_address.sin_family      = AF_INET;
   server_address.sin_addr.s_addr = htonl(INADDR_ANY);
   server_address.sin_port        = htons(ip_port);

   result = bind(serverSocket,
                 (struct sockaddr *)&server_address,
                 sizeof(server_address));

   if (result != 0)
   {
      close(serverSocket);
      puts ("Unable to bind socket to specified IP port - server support won't work!\n");
      fflush (stdout);
      return; /* bah! */
   } /* endif unable to modify socket options */

   result = listen(serverSocket,
                   MAX_PENDING_CONNECTIONS);

   if (result != 0)
   {
      close(serverSocket);
      puts ("Unable to listen on specified IP port - server support won't work!\n");
      fflush (stdout);
      return; /* bah! */
   } /* endif unable to modify socket options */

} /* initIPPort */


/** wait for and retrieve event notification from IP port */
void getEvent (char *buffer, int buflen)
{
   struct sockaddr_in client_address;

   int   client_name_size;
   char *dest;
   int   finished = 0;
   int   remaining;
   int   received;
   int   total;

   /* Buffer to store incoming TCP/IP data */
   static char recv_buffer[1024];
   const int   recv_buffer_maxsize = 1024;
   static int  recv_amount = 0;  /* Amount actually received from the network */
   static int  recv_offset = 0;  /* Amount processed so far */
   static int  need_skip   = 0;  /* Set if we need to ignore everything until next \n */

   int buffer_offset = 0;

   buffer[0] = '\0';

   if (serverSocket == -1)
   {
      return;
   } /* endif not listening on IP port */

   if (clientSocket == -1)
   {
      /* Debug message */
/*
      puts ("No previous connection from KMuddy - waiting for connection\n");
      fflush (stdout);
*/

      client_name_size = sizeof(client_address);

      clientSocket = accept(serverSocket,
                            (struct sockaddr *)&client_address,
                            (socklen_t *)&client_name_size);

   } // endif no previous dialogue with server

   if (clientSocket < 0)
   {
      return;
   } /* endif accept failed */


   while (!finished)
   {
      if (recv_offset >= recv_amount)
      {
         recv_offset = 0;

         recv_amount = recv(clientSocket,
                            recv_buffer,
                            recv_buffer_maxsize,
                            0);

      } /* endif need to receive more data from the network */

      if (need_skip)
      {
         while ((recv_offset               < recv_amount)&&
                (recv_buffer[recv_offset] != '\n')         )
         {
            recv_offset++;
         } /* endwhile more characters to skip */

         if (recv_offset < recv_amount)
         {
            recv_offset++;
            need_skip = 0;
         } /* endif found \n */

      } /* endif need to ignore everything until next \n */

      while ((recv_offset               < recv_amount)&&
             (buffer_offset             < buflen-1)   &&
             (recv_buffer[recv_offset] != '\n')         )
      {
         buffer[buffer_offset++] = recv_buffer[recv_offset++];

      } /* endwhile not found complete message yet */

      if (buffer_offset == buflen-1)
      {
         /* Truncate message */
         buffer[buflen-1] = '\0';
         finished         = 1;
         need_skip        = 1; /* need to ignore everything until next \n */
      } /* endif reached limits of message buffer */
      else if (recv_offset < recv_amount)
      {
         /* Found \n at recv_offset */
         buffer[buffer_offset] = '\0';
         finished = 1;
         recv_offset++;
      } /* endif received full message */

   } /* endwhile more work to do to receive complete message */

} /* getEvent */



void getVariable (const char *name, char *buffer, int buflen)
{
  int n;
   /* some initial tests */
   if (!name) return;
   if (!buffer) return;
   if (buflen <= 0) return;
   //alocate memory for request-string
   n = strlen (name);
   char *request = (char *) malloc (6 + n);
   //build a request
   strcpy (request, "get ");
   strcat (request, name);
   strcat (request, "\n");
   /* send it and receive the result */
   sendAndReceive (request, buffer, buflen);
   /* free up memory used by the request-string */
   free (request);
}

char setVariable (const char *name, const char *value)
{
  int n1, n2;
  /* some initial tests */
  if (!name) return 0;
   if (!value) return 0;
   /* allocate memory for request-string */
   n1 = strlen (name);
   n2 = strlen (value);
   char *request = (char *) malloc (7 + n1 + n2);
   /* build a request */
   strcpy (request, "set ");
   strcat (request, name);
   strcat (request, " ");
   strcat (request, value);
   strcat (request, "\n");
   /* send it and receive the result */
   sendAndReceive (request, internalBuffer, 51);
   /* free up memory used by the request-string */
   free (request);

   if (strncmp (internalBuffer, "OK", 2) == 0)
      return 1;  /* success */
   return 0;  /* failure */
}

char unsetVariable (const char *name)
{
  return simpleCommand ("unset", name);
}

char incVariable (const char *name, int delta)
{
  int n1, n2;
  /* some initial tests */
  if (!name) return 0;
   if (delta <= 0) return 0;
   //alocate memory for request-string
   n1 = strlen (name);
   n2 = 10;
   char *request = (char *) malloc (7 + n1 + n2);
   char *value = (char *) malloc (10);
   sprintf (value, "%d", delta);
   //build a request
   strcpy (request, "inc ");
   strcat (request, name);
   strcat (request, " ");
   strcat (request, value);
   strcat (request, "\n");
   /* send it and receive the result */
   sendAndReceive (request, internalBuffer, 51);
   /* free up memory used by the request-string */
   free (request);

   if (strncmp (internalBuffer, "OK", 2) == 0)
      return 1;  /* success */
   return 0;  /* failure */
}

char decVariable (const char *name, int delta)
{
  int n1, n2;
  /* some initial tests */
  if (!name) return 0;
   if (delta <= 0) return 0;
   /* allocate memory for request-string */
   n1 = strlen (name);
   n2 = 10;
   char *request = (char *) malloc (7 + n1 + n2);
   char *value = (char *) malloc (10);
   sprintf (value, "%d", delta);
   /* build a request */
   strcpy (request, "dec ");
   strcat (request, name);
   strcat (request, " ");
   strcat (request, value);
   strcat (request, "\n");
   /* send it and receive the result */
   sendAndReceive (request, internalBuffer, 51);
   /* free up memory used by the request-string */
   free (request);

   if (strncmp (internalBuffer, "OK", 2) == 0)
      return 1;  /* success */
   return 0;  /* failure */
}

char provideResource (const char *name)
{
  return simpleCommand ("provide", name);
}

char requestResource (const char *name)
{
  return simpleCommand ("request", name);
}

char lockVariable (const char *name)
{
  return simpleCommand ("lock", name);
}

void unlockVariable (const char *name)
{
  simpleCommand ("unlock", name);
}

char simpleCommand (const char *command, const char *name)
{
  int n,k;
  /* some initial tests */
  if (!name) return 0;
   /* allocate memory for request-string */
   n = strlen (name);
   k = strlen (command);
   char *request = (char *) malloc (3 + k + n);
   /* build a request */
   strcpy (request, command);
   strcat (request, " ");
   strcat (request, name);
   strcat (request, "\n");
   /* send it and receive the result */
   sendAndReceive (request, internalBuffer, 51);
   /* free up memory used by the request-string */
   free (request);

   if (strncmp (internalBuffer, "OK", 2) == 0)
      return 1;  /* success */
   return 0;  /* failure */
}

void sendAndReceive (const char *sendthis, char *buffer, int buflen)
{
  int n;
   int tobewritten, remaining;
   const char *sendme;
   char myBuf[101];
   char gotNewLine = 0;
   buffer[0] = '\0';

   if (variableSocket == -1)
      return;
  tobewritten = strlen (sendthis);
   if (tobewritten == 0)
      return;
   /* pointer to data that hasn't been written yet */
   sendme = sendthis;
   /* write it all */
   while (tobewritten > 0)
   {
      n = write (variableSocket, sendme, strlen (sendme));
      if (n == -1)
         /* TODO: some error reporting */
         return;
      /* shift the pointer */
      tobewritten -= n; // Alex change - moved from above
      sendme += n;
   }

   /* read reply from KMuddy */
   remaining = buflen;  /* buffer needs to have one extra byte for \0 */

   while (!gotNewLine)
   {
      n = read (variableSocket, myBuf, 100);

      if (n == -1) /* bah! */
         return;
      myBuf[n] = '\0';
      if (remaining)
      {
         strncat (buffer, myBuf, remaining);
         remaining -= n;
         if (remaining < 0)
            remaining = 0;
         n = strlen (buffer);
         if (n)
            if (buffer[n - 1] == '\n')
            {
               gotNewLine = 1;
               buffer[n - 1] = '\0';  /* don't return that newline */
            }
      }
   }
}



/* ALEX CHANGE */
char sendCommand (const char *command)
{
  int n1;
   /* some initial tests */
  if (!command) return 0;
   /* allocate memory for request-string */
   n1 = strlen (command);
   char *request = (char *) malloc (7 + n1);
   /* build a request */
   strcpy (request, "send ");
   strcat (request, command);
   strcat (request, "\n");
   /* send it and receive the result */
   sendAndReceive (request, internalBuffer, 51);
   /* free up memory used by the request-string */
   free (request);

   if (strncmp (internalBuffer, "OK", 2) == 0)
      return 1;  /* success */
   return 0;  /* failure */
}

/* END ALEX CHANGE */
