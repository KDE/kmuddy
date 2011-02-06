#include "kmuddyvars.h"

int main(void)
{
  initVariableSocket();
  setVariable("test", "this value set by LOCKTEST!");
	lockVariable("test");
	sleep(1000);
	closeVariableSocket();
	return 0;
}

