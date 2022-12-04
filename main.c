#include "prototypes.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stringfuncs.h"
#include <time.h>

#define TRUE 1
#define FALSE 0

#ifdef _WIN32
#include <conio.h>
#endif

// Flags
int devmode = 0;
int godmode = 0;
int logging_level = 0;

// Shared
FILE *logFilePtr; // Pointer to logfile

int main(int argc, char *argv[]) {
	log_(3, "Welcome to TextRPG C edition!");
	log_(3, "Logging for you!");
	// Parsing arguments
	if (parseArgs(argc, argv) != 0) {
		return -1;
	}
	initLog(logging_level);
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
	playerInit(&player);
	int exit = 0;
	int commandValid = TRUE;
	int loopIteration = 0;
	log_(3, "Entering command loop!");
	while (exit != 1) {
		log_(3, "Loop iteration started!");
		loopIteration++;
		if (commandValid != TRUE) {
			printf("Invalid command!\n"); // Notify player when the previous command was invalid
			commandValid = TRUE;
		}
		printf("Coordinates: %d, %d @ stage %d\n", 
				(int) player.self.position.xPos, 
				(int )player.self.position.yPos, 
				(int )player.self.position.stage);
		printf("Health/Attack/Level/XP: %d/%d/%d/%d\n", 
				(int) player.self.health, 
				(int) player.self.attack, 
				(int) player.self.level, 
				(int) player.self.xp);
		char input[255];
		getInput(input, 255, "> ");
		clearScreen(); // Clear screen after input has been submitted
		CharSpan args[3];
		int argCount = split(args, 3, input, " ");
		CharSpan *arg0 = &args[0];
		// Switch statements for strings don't exist
		// so we have to use if-else ladders
		// First check if extended commands (devmode)
		// are enabled
		if (devmode == 1) {
			// Check the dev commands
			// To be implemented
		} else {
			// Go into the normal commands
			if (compareSpan(arg0, "exit")) {
				log_(3, "Command 'exit' invoked! Breaking out of command loop!");
				exit = 1;
			} else if (compareSpan(arg0, "help")) {
				log_(3, "Command 'help' invoked!");
				printf("==         LIST OF AVAILABLE COMMANDS         ==\n");
				printf("help                          | shows this list \n");
				printf("exit                          | exits the game  \n");
				printf("move <direction:x|y> <amount> | moves the player\n");
			} else if(compareSpan(arg0, "move")) {
				log_(3, "Command 'move' invoked!");
				if(argCount >= 3) {
					int toMove = spanToInt(args[2]);
					CharSpan *arg1 = &args[1];
					if (compareSpan(arg1, "x")) {
						log_(3, "Moving on the x axis!");
						player.self.position.xPos =
							player.self.position.xPos + toMove;
						if (player.self.position.xPos > 12) {
							player.self.position.xPos = 12;
							log_(2, "Player attempted to move out of bounds! (x > 12)");
						} else if (player.self.position.xPos < 0) {
							player.self.position.xPos = 0;
							log_(2, "Player attempted to move out of bounds! (x < 0)");
						} else;
						log_(3, "Moved on the x axis!");
					} else if (compareSpan(arg1, "y")) {
						log_(3, "Moving on the y axis!");
						player.self.position.yPos =
							player.self.position.yPos + toMove;
						if (player.self.position.yPos > 12) {
							player.self.position.yPos = 12;
							log_(2, "Player attempted to move out of bounds! (y > 12)");
						} else if (player.self.position.yPos < 0) {
							player.self.position.yPos = 0;
							log_(2, "Player attempted to move out of bounds! (y < 0)");
						} else;
						log_(3, "Moved on the y axis!");
					} else {
						printf("Invalid direction\n");
					}
				} else {
					printf("Not enough args for 'move' provided: move <direction> <amount>\n");
				}

			}
			else {
				commandValid = FALSE;
			}
		}
	}
	stopLog();
	printf("Game ended!\n");

	return 0;
}

void getInput(char *result, int max, char prompt[]) {
	printf(prompt);
	fgets(result, max, stdin);
	result[strcspn(result, "\r\n")] = 0;
}

int initLog(int logLvl) {
	logFilePtr = fopen("./latest.log", "w");
	fprintf(logFilePtr, "// Logging for textrpg game\n// Time (Unix timestamp): %d\n", (int)time(NULL));
	fprintf(logFilePtr, "Successfully started logging!\n");
	return 0;
}

int stopLog() {
	fclose(logFilePtr);
	return 0;
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
			// Terminating errors will still notify the user if logging level is 0
			// This is just for fatal errors that will break the game
			// but not terminate it
			printf("[FATAL] %s\n", msg);
			fprintf(logFilePtr, "[FATAL] %s\n", msg);
			break;
		case 1:
			printf("[ERROR] %s\n", msg);
			fprintf(logFilePtr, "[ERROR] %s\n", msg);
			break;
		case 2:
			printf("[WARN] %s\n", msg);
			fprintf(logFilePtr, "[WARN] %s\n", msg);
			break;
		case 3:
			printf("[DEBUG] %s\n", msg);
			fprintf(logFilePtr, "[DEBUG] %s\n", msg);
			break;
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
int playerInit(Player *player) {
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
			playerEntity.inventory[i][j] = 0;
		}
	}

	// Assign playerEntity to player
	player->self = playerEntity;
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
	#ifdef _WIN32
		clrscr();
	#else
		printf("\e[1;1H\e[2J");
	#endif
}
char *getCharSeq(char input[], int begin, int end) {
	int range = end - begin; // Get size of ret
	char* ret = malloc(range * sizeof(char));
	strncpy(ret, input + begin, range);
	return ret;
}
