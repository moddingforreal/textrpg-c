#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "prototypes.h"

// Flags
int devmode = 0;
int godmode = 0;
int logging_level = 0;

int main(int argc, char *argv[])
{
	// Reading program arguments
	if (argc > 0)
	{
		for (int i = 1; i < argc; i++)
		{
			if (strcmp(argv[i], "-d") == 0)
			{
				devmode = 1;
			}
			else if (strcmp(argv[i], "-g") == 0)
			{
				godmode = 1;
			}
			else if (strcmp(argv[i], "-l") == 0)
			{
				if (i + 1 < argc)
				{
					logging_level = (int)strtol(argv[++i], (char **)NULL, 10);
				}
			}
		}
	}

	// Verifying validity of arguments
	if (logging_level > 2 || logging_level < 0)
	{
		return -1;
	}

	return 0;
}
