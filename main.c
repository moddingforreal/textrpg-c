#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "prototypes.h"

// Flags
int devmode = 0;
int godmode = 0;
int logging_level = 0;

int main(int argc, char *argv[]) {
	// Parsing arguments
	if (parseArgs(argc, argv) != 0) {
		return -1;
	}
	log(3, "Running game...");
	runGame();
	printf("Goodbye!");
	return 0;
}

int parseArgs(int argc, char *argv[]) {
	// Reading program arguments
	if (argc > 0) {
		for (int i = 1; i < argc; i++) {
			if (strcmp(argv[i], "-d") == 0) {
				devmode = 1;
			} else if (strcmp(argv[i], "-g") == 0) {
				godmode = 1;
			} else if (strcmp(argv[i], "-l") == 0) {
				if (i + 1 < argc) {
					logging_level = (int)strtol(argv[++i],(char **)NULL, 10);
				}
			}
		}
	}

	// Verifying validity of arguments
	if (logging_level > 3 || logging_level < 0) {
		printf("FATAL: Invalid logging level supplied. It must be a number between 0 and 3 (inclusive). Supplied: %d\n", logging_level);
		return -1;
	}

	return 0;
}

int runGame() {
	printf("Welcome to TextRPG!\nType \"help\" to view a list of available commands\nType \"exit\" to exit the game\n\n");
	char input[255] = calloc(255);
	while (strcmp(input, "exit") != 0) {
		// Assign value to input using strcpy
		strcpy(input, getInput("> ", 255));
		// Switch statements for strings don't exist
		// so we have to use if-else ladders
		// First check if extended commands (devmode)
		// are enabled
		if (devmode == 1) {
			// Check the dev commands
			// To be implemented
		} else {
			// Go into the normal commands
			if (strcmp(input, "exit") == 0) {
				break;
			} else if (strcmp(input, "help") == 0) {
				printf("To be implemented.\n");
			} else {
				clearScreen();
				printf("Invalid command!\n");
			}
		}
	}

	printf("Game ended!\n");

	return 0;
}

char* getInput(char prompt[], int max) {
	printf(prompt);
	while (1) {
		char input[max];
		fgets(input, sizeof(input), stdin);
		char* readInput = calloc(max);
		return readInput;
	}
}

int log(int logLvl, char msg[]) {
	/* Logging convention for textrpg-c: 
	 * Level 0 : All logs suppressed 
	 * Level 1 : Recoverable errors (+ previous)
	 * Level 2 : Warnings (+ previous)
	 * Level 3 : Debug, full logs (+ previous)
	 * Fatal/Terminating errors will always notify the user
	 */
	if (!(logLvl > logging_level)) {
		switch(logLvl) {
			case 0:
				printf("[FATAL] %s", msg);
				break;
			case 1:
				printf("[ERROR] %s", msg);
				break;
			case 2:
				printf("[WARN] %s", msg);
				break;
			case 3:
				printf("[DEBUG] %s", msg);
		}
		return 0;
	} else {
		// Return required level of logging
		// incase the parent function wants to do something
		// with this
		return logLvl;
	}
}

// Auxiliary functions
int isInRangeInt(int value, int lowerBound, int upperBound) {
	if (value < upperBound && value > lowerBound) {
		return 0;
	} else {
		return 1;
	}
}
double distance2D(Coordinates position1, Coordinates position2) {
	return abs(sqrt(square(position2.xPos - position1.xPos) 
			+ square(position2.yPos - position1.yPos)));
}
double square(double x) { return x*x; }
void clearScreen() {
	// Should work cross-system
	// to clear screen reliably
	printf("\e[1;1H\e[2J");
}
