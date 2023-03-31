#include "prototypes.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

// Flags
int devmode = 0;
int godmode = 0;
int logging_level = 0;

// Shared
FILE *logFilePtr; // Pointer to logfile
// Entity entities_global[]; // Global list of generated entities, EXCLUDING PLAYER
int passabilityBlock[12][12][12]; // Global list of nonpassable blocks [x][y][stage]

int main(int argc, char *argv[]) {
	log_(3, "Welcome to TextRPG C edition!");
	log_(3, "Logging for you!");
	log_(3, "Seeding random...");
	srand(time(NULL));
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
	log_(3, "Starting terrain generation (type 0)...");
	generate(0, &player);
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
				(int) player.self.position.yPos, 
				(int) player.self.position.stage);
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
		int devCommandInvoked = FALSE;
		// Switch statements for strings don't exist
		// so we have to use if-else ladders
		// First check if extended commands (devmode)
		// are enabled
		if (devmode == 1) {
			// Check the dev commands
			// To be implemented
			if (compareSpan(arg0, "dev_help")) {
				log_(3, "[DEV] DEV commands invoked!");
				printf("map <stage> | shows a map of the specified stage\n");
				devCommandInvoked = TRUE;
			} else if (compareSpan(arg0, "map")) {
				if (&args[1] != NULL) {
					int cmd_stage = spanToInt(args[1]);
					printf("Map for stage %d\n", &cmd_stage);
					for (int j = 0; j <= 12; j++) {
						for (int i = 0; i <= 12; i++) {
							if (passabilityBlock[i][j][cmd_stage] != 0) {
								printf("#");
							} else {
								printf("_");
							}
							if (i == 12) {
								printf("\n");
							}
						}
					}
				}
				devCommandInvoked = TRUE;
			}
		}
		// Go into the normal commands
		if (compareSpan(arg0, "exit")) {
			log_(3, "[CMD] Command 'exit' invoked! Breaking out of command loop!");
			exit = 1;
		} else if (compareSpan(arg0, "help")) {
			log_(3, "Command 'help' invoked!");
			printf("==         LIST OF AVAILABLE COMMANDS         ==\n");
			printf("help                          | shows this list \n");
			printf("exit                          | exits the game  \n");
			printf("move <direction:x|y> <amount> | moves the player\n");
		} else if(compareSpan(arg0, "move")) {
			log_(3, "[CMD] Command 'move' invoked!");
			movePlayer(args, &player, argCount);
		}
		else {
			if (devCommandInvoked == TRUE) {
				commandValid = FALSE;
			}
		}
	}
	stopLog();
	printf("Game ended!\n");

	return 0;
}

int movePlayer(CharSpan *args, Player *player, int argCount) {
	if(argCount >= 3) {
		int toMove = spanToInt(args[2]);
		CharSpan axis = args[1];
		if (compareSpan(&axis, "x")) {
			log_(3, "[MOV] Moving on the x axis!");
			// Check for out of bounds and adjust accordingly
			if (player->self.position.xPos + toMove > 12) {
				toMove = 12 - player->self.position.xPos;
				log_(2, "[MOV] Player attempted to move out of bounds! (x > 12)");
			} else if (player->self.position.xPos + toMove < 0) {
				toMove = -player->self.position.xPos;
				log_(2, "[MOV] Player attempted to move out of bounds! (x < 0)");
			}

			// Check for blocks in the way
			int blocksInWay = FALSE;
			for (int i = 0; i < toMove; i++) {
				if (passabilityBlock[((int)player->self.position.xPos) + i][(int)player->self.position.yPos][(int)player->self.position.stage] == 1) {
					log_(3, "[MOV] Block in player path detected!");
					blocksInWay = TRUE;
					player->self.position.xPos += i;
					break;
				}
			}
			if (!blocksInWay) {
				player->self.position.xPos += toMove;
			}
			log_(3, "[MOV] Moved on the x axis!");
		} else if (compareSpan(&axis, "y")) {
			log_(3, "[MOV] Moving on the y axis!");
			// Check for out of bounds and adjust accordingly
			if (player->self.position.yPos + toMove > 12) {
				toMove = 12 - player->self.position.yPos;
				log_(2, "[MOV] Player attempted to move out of bounds! (y > 12)");
			} else if (player->self.position.yPos + toMove < 0) {
				toMove = -player->self.position.yPos;
				log_(2, "[MOV] Player attempted to move out of bounds! (y < 0)");
			}

			// Check for blocks in the way
			int blocksInWay = FALSE;
			for (int i = 0; i < toMove; i++) {
				if (passabilityBlock[(int)player->self.position.xPos][((int)player->self.position.yPos) + i][(int)player->self.position.stage] == 1) {
					log_(3, "[MOV] Block in player path detected!");
					blocksInWay = TRUE;
					player->self.position.yPos += i;
					break;
				}
			}
			if (!blocksInWay) {
				player->self.position.yPos += toMove;
			}
			log_(3, "[MOV] Moved on the y axis!");
		} else {
			printf("Invalid direction\n");
		}
	} else {
		printf("Not enough args for 'move' provided: move <direction> <amount>\n");
	}
	return 0;
}

/* Generation function, generates the world initially
 * Types:
 * Terrain: 0
 * Entities: 1
 * */
int generate(int type, Player *player) {
	srand(time(NULL));
	log_(3, "[GEN] Entering generation for specified type...");
	switch(type) {
		default:
			log_(2, "[GEN] ERR: Generation behavior for type undefined!");
			break;
		case 0:
			for(int i = 0; i <= 12; i++) {
				for(int j = 0; j <= 12; j++) {
					for(int k = 0; k <= 12; k++) {
						passabilityBlock[i][j][k] = rand() % 2;
						log_(3, "[GEN] Determined block for current position!");
					}
				}
			}
			for (int i = 0; i <= 12; i++) {
				passabilityBlock[0][0][i] = 0;
			}
			break;
		case 1:
			// To be implemented
			break;
	}
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
	system("@cls||clear");
	// 	printf("\e[1;1H\e[2J");
}
char *getCharSeq(char input[], int begin, int end) {
	int range = end - begin; // Get size of ret
	char* ret = malloc(range * sizeof(char));
	strncpy(ret, input + begin, range);
	return ret;
}
