#include "prototypes.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Flags
int devmode = 0;
int godmode = 0;
int logging_level = 0;

int main(int argc, char *argv[]) {
	log_(3, "Welcome to TextRPG C edition!");
	log_(3, "Logging for you!");
	// Parsing arguments
	if (parseArgs(argc, argv) != 0) {
		return -1;
	}
	log_(3, "Running game...");
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
					logging_level = (int)strtol(argv[++i], (char **)NULL, 10);
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
	log_(3, "Creating player");
	Player player;
	log_(3, "Initializing player...");
	playerInit(player);
	int exit = 0;
	int loopIteration = 0;
	log_(3, "Entering command loop!");
	while (exit != 1) {
		log_(3, "Loop iteration started!");
		loopIteration++;
		printf("Coordinates: %lf, %lf @ stage %lf\n", 
				player.self.position.xPos, 
				player.self.position.yPos, 
				player.self.position.stage);
		printf("Health/Attack/Level/XP: %lf/%lf/%lf/%lf/%lf\n", 
				player.self.health, 
				player.self.attack, 
				player.self.level, 
				player.self.xp);
		char *input = getInput("> ", 255);
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
				log_(3, "Command 'exit' invoked! Breaking out of command loop!");
				break;
			} else if (strcmp(input, "help") == 0) {
				log_(3, "Command 'help' invoked! The command has not been implemented yet!");
				printf("To be implemented.\n");
			} else if(strcmp(getCharSeq(input, 0, 5), "move") == 0) {
				log_(3, "Command 'move' invoked!");
				if (strcmp(getCharSeq(input, 6, 7), "x") == 0) {
					log_(3, "Moving on the x axis!");
					int toMove = atoi(getCharSeq(input, 8, 255));
					player.self.position.xPos =
						player.self.position.xPos + toMove;
					log_(3, "Moved on the x axis!");
				}
			}
			else {
				clearScreen();
				printf("Invalid command!\n");
			}
		}
		free(input);
	}

	printf("Game ended!\n");

	return 0;
}

char *getInput(char prompt[], int max) {
	printf(prompt);
	char *input = calloc(max, sizeof(char));
	fgets(input, max, stdin);
	input[strcspn(input, "\r\n")] = 0;
	return input;
}

int log_(int logLvl, char msg[]) {
	/* Logging convention for textrpg-c:
	 * Level 0 : All logs suppressed
	 * Level 1 : Recoverable errors (+ previous)
	 * Level 2 : Warnings (+ previous)
	 * Level 3 : Debug, full logs (+ previous)
	 * Fatal/Terminating errors will always notify the user
	 */
	if (!(logLvl > logging_level)) {
		switch (logLvl) {
		case 0:
			printf("[FATAL] %s\n", msg);
			break;
		case 1:
			printf("[ERROR] %s\n", msg);
			break;
		case 2:
			printf("[WARN] %s\n", msg);
			break;
		case 3:
			printf("[DEBUG] %s\n", msg);
		}
		return 0;
	} else {
		// Return required level of logging
		// incase the parent function wants to do something
		// with this
		return logLvl;
	}
}

// Init player
int playerInit(Player player) {
	// Make players entity attribute
	Entity playerEntity;

	// Initialize player's entity's position
	Coordinates playerPos;
	playerPos.xPos = 0;
	playerPos.yPos = 0;
	playerPos.stage = 0;

	// Assign player's position
	playerEntity.position = playerPos;

	// Set playerEntity attributes
	playerEntity.baseHealth = 10; // Will be retained upon death
	playerEntity.baseAttack = 10;
	
	playerEntity.health = playerEntity.baseHealth; // Will be reset upon death
	playerEntity.attack = playerEntity.baseAttack;
	playerEntity.level = 1;
	playerEntity.xp = 0;

	// Item IDs, 0 for nothing. Each slot can only keep 1 item
	// Initialized to only 0s => empty inventory
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 5; j++) {
			player.self.inventory[i][j] = 0;
		}
	}

	// Assign playerEntity to player
	player.self = playerEntity;
	return 0;
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
	return abs(sqrt(square(position2.xPos - position1.xPos) +
					square(position2.yPos - position1.yPos)));
}
double square(double x) { return x * x; }
void clearScreen() {
	// Should work cross-system
	// to clear screen reliably
	printf("\e[1;1H\e[2J");
}
char *getCharSeq(char input[], int begin, int end) {
	int range = end - begin; // Get size of ret
	char* ret = malloc(range * sizeof(char));
	strncpy(ret, input+begin, end);
	return ret;
}
