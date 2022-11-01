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
	if (parseArgs(argc, argv) != 0)
	{
		return -1;
	}

	runGame();

	return 0;
}

int parseArgs(int argc, char *argv[])
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
		printf("Invalid logging level, must be between 0 and 2 you gave %d\n", logging_level);
		return -1;
	}

	return 0;
}

int runGame()
{
	int input = 0;
	while (input != 4)
	{
		input = getInput("Enter 1 to access shop\nEnter 2 to fight\nEnter 3 to see your Statistics\n>>>: ", 4);
		switch (input)
		{
		case 1:
			// shop
			printf("this will be a greate shop!\n");
			break;

		case 2:
			// combat
			printf("welcome to the epic arena!\n");
			break;

		case 3:
			// stats
			printf("stats? 1/4 inputs implemented\n");
			break;

		default:
			break;
		}
		printf("\n"); // empty line for seperation of iterations
	}

	printf("Game ended!\n");

	return 0;
}

int getInput(char prompt[], int max)
{
	printf(prompt);
	while (1)
	{
		int input;
		int result = scanf("%d", &input);
		while (fgetc(stdin) != '\n') // clear stdin
			;
		if (result != 1)
			printf("Invalid input, try again: ");
		else if (input <= 0)
			printf("Input must be at least 0, try again: ");
		else if (input > max)
			printf("Input must be smaller than %d, try again: ", max);
		else
			return input;
	}
}
