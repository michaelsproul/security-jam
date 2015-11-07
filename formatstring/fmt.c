#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int
terminate(char *s)
{
 printf(s);
 printf("\n");
 exit(0);
}

int
main(int argc, char *argv[])
{
	int i;

	if (argc < 2) terminate("Not enough arguments");

	//check for % signs
	for (i = 1; i < argc; i++) {
		if (strchr((const char *)argv[i], '%') != NULL)
			terminate("Invalid character found");
	}

	i = 0;
 	//print out the arguments
 	printf("You called me with:\n");
	while (i < argc) {
		printf(argv[i++]);
		printf(" ");
	}
	printf("\n");
}
