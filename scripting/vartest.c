
#include "kmuddyvars.h"

int main ()
{
  char variable[201];
	char x[251];
	initVariableSocket ();
  
 	if (setVariable ("test", "This is some test. :)"))
		puts ("say Value of variable test has been set.");
	else
		puts ("say Failed to set variable test.");
  
	fflush(stdout);
	
	getVariable ("test", variable, 200);
	strcpy (x, "say So, value of test = ");
	strcat (x, variable);
	//IMPORTANT: keep in mind that the variable may no longer have the same
	//value when the command is sent
	puts (x);

	if (unsetVariable ("test"))
		puts ("say The variable test is no longer set.");
	else
		puts ("say Failed to unset variable test.");

	fflush(stdout);


	setVariable ("number", "4");
	puts ("say variable number has been set to $number.");
	//we need to sleep, so that the output is displayed correctly
	fflush(stdout);
	incVariable ("number", 10);
	puts ("say variable number has been increased by 10, thus it's now $number.");
	//we need to sleep, so that the output is displayed correctly
	fflush(stdout);
	decVariable ("number", 7);
	puts ("say decreased test by 7, it's now $number.");
	fflush(stdout);
	provideResource("xxx");
	provideResource("xxx");
	puts ("say Provided two units of resource XXX.");
	int i;
	for (i = 0; i < 3; i++)
	{
    char r = requestResource("xxx");
		if (r)
			puts ("say Requesting resource XXX - OKAY");
		else
			puts ("say Requesting resource XXX - FAILED");
	}
	
	closeVariableSocket ();
	return 0;
}
	
