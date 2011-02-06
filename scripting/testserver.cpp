#include <iostream>

using namespace std;

#include "kmuddyvars.h"


//****************************************************************
// Program to test kmuddyvars.h functionality
//****************************************************************

int main()
{
   char mybuffer[256];
   
   initVariableSocket();
   
   cout << "Initialising IP server\n";
   cout.flush();
   
   initIPPort(4132);
   
   cout << "Done. Receving event notification\n";
   cout.flush();
   
   for (int count = 0; count < 5; count++)
   {
      getEvent(mybuffer,sizeof(mybuffer));

      cout << "Event received: '" << mybuffer << "'\n";
      cout << "Sending reply\n";
      cout.flush();

      sendCommand("bounce");
   } // endfor more bounces to do
   
   closeVariableSocket();
   
} // main
