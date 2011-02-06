//testing preprocessor script
//nothing useful, just a toy :)

#include <stdio.h>
#include <string.h>
#include <kvbox.h>

int main()
{
  char line[1001];
  char line2[1004];
  int prompt;
  while (1) {
    fgets (line, 1000, stdin);
    int len = strlen(line);
    if (line[len-1] != '\n') continue;
    line[len-1] = '\0';
    prompt = 0;
    if (len && (line[len-2] == 1))
    {
      line[len-2] = '\0';
      prompt = 1;
    }
    len = strlen(line);
    line2[0] = prompt ? '2' : '1';
    line2[1] = ' ';
    //we output all lines in reverse order :)
    //not really useful, just a toy :)
    //note that this breaks ANSI sequences
    for (int i = 0; i < len; i++)
      line2[2 + i] = line[len - 1 - i];
    line2[len+2] = '\n';
    line2[len+3] = '\0';
    fputs (line2,stdout);
    fflush(stdout);
  };
  return 0;
}

