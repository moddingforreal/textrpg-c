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
	stopLog();
	printf("\n\n\t\t\tGoodbye!\n\n\n");
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
	int exit = FALSE;
	int commandValid = TRUE;
	int loopIteration = 0;
	log_(3, "Entering command loop!");
	while (!exit) {
		log_(3, "Loop iteration started!");
		loopIteration++;
		log_(3, "Updating player...");
		updatePlayerBlockKnowledgeTable(&player);
		updatePlayerLevel(&player);
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
				printf("wmap <stage> | shows a map of the specified stage\n");
				devCommandInvoked = TRUE;
			} else if (compareSpan(arg0, "wmap")) {
				if (&args[1] != NULL) {
					int cmd_stage = spanToInt(args[1]);
					printf("Map for stage %d\n", cmd_stage);
					for (int j = 0; j < 12; j++) {
						for (int i = 0; i < 12; i++) {
							switch (passabilityBlock[i][j][cmd_stage]) {
								default:
									log_(1, "[CMD] Block id invalid!");
									break;
								case 0:
									printf("_");	// Empty space
									break;
								case 1:
									printf("#");	// Generic Block
									break;
								case 2:
									printf("X");	// Exit (to next stage)
									break;
								case 3:
									printf("O");	// Entrance (to this stage)
									break;
							}
						}
						printf("\n"); // Every line of 12 blocks, a newline shall be printed
					}
				}
				devCommandInvoked = TRUE; // Mark that a dev command already got invoked
			}
		}
		// Go into the normal commands
		if (compareSpan(arg0, "exit")) {
			log_(3, "[CMD] Command 'exit' invoked! Breaking out of command loop!");
			exit = TRUE;
		} else if (compareSpan(arg0, "help")) {
			log_(3, "Command 'help' invoked!");
			printf("==         LIST OF AVAILABLE COMMANDS         ==\n");
			printf("help                          | shows this list \n");
			printf("exit                          | exits the game  \n");
			printf("move <direction:x|y> <amount> | moves the player\n");
			printf("linv                          | list player inventory contents\n");
			printf("map <stage>                   | show map of player-known blocks for stage");
		} else if(compareSpan(arg0, "move")) {
			log_(3, "[CMD] Command 'move' invoked!");
			movePlayer(args, &player, argCount);
		} else if (compareSpan(arg0, "linv")) { // 'linv' is short for 'list inventory'
			int invalid = 0; // Any IDs unassigned
			int freeSlots = 0; // ID: 0
			int gold = 0; // ID: 1
			int stdPotion = 0; // ID: 2
			for (int i = 0; i < 6; i++) { // Iterate over the entire inventory
				for (int j = 0; j < 5; j++) {
					int currSlotID = (int) player.self.inventory[i][j];
					switch (currSlotID) {
						default:
							invalid++;
							break;
						case 0:
							freeSlots++;
							break;
						case 1:
							gold++;
							break;
						case 2:
							stdPotion++;
							break;
					}
				}
			}
			printf("Player Inventory: \nGold: %d \nStandard Potion: %d \n\nInvalid Items: %d \nEmpty Slots: %d \n", gold, stdPotion, invalid, freeSlots);
		} else if (compareSpan(arg0, "map")) {
			if (&args[1] != NULL && spanToInt(args[1]) < 12 && spanToInt(args[1]) > -1) {
				int cmd_stage = spanToInt(args[1]);
				printf("Map for stage %d\n", cmd_stage);
				for (int i = 0; i < 12; i++) {
					for (int j = 0; j < 12; j++) {
						if (player.knownBlocks[i][j][cmd_stage] == TRUE) {
							switch(passabilityBlock[j][i][cmd_stage]) {
								default:
									printf("!");	// Block invalid
									break;
								case 0:
									printf("_");	// Generic free space
									break;
								case 1:
									printf("#");	// Generic block
									break;
								case 2:
									printf("X");	// Exit (to next stage)
									break;
								case 3:
									printf("O");	// Entrance (from previous level)
									break;
							}
						} else {
							printf("?");
						}
					
					}
					printf("\n");
				}
			} else {
				log_(3, "[CMD] Invalid stage selected for command \"map\"");
				printf("Invalid map stage!\n");
			}
		}else {
			if (!devCommandInvoked) { // Only print a command invalidation error when there was no command at all, not even a dev command
				commandValid = FALSE;
			}
		}
	}
	printf("Game ended!\n");

	return 0;
}

int movePlayer(CharSpan *args, Player *player, int argCount) {
	if(argCount >= 3) {
		int toMove = spanToInt(args[2]);
		CharSpan axis = args[1]; // Gets the axis to move on
		if (compareSpan(&axis, "x")) {
			log_(3, "[MOV] Moving on the x axis!");
			// Check for out of bounds and adjust accordingly
			if (player->self.position.xPos + toMove >= 12) {
				toMove = 11 - player->self.position.xPos;
				log_(2, "[MOV] Player attempted to move out of bounds! (x >= 12)");
			} else if (player->self.position.xPos + toMove < 0) {
				toMove = -player->self.position.xPos;
				log_(2, "[MOV] Player attempted to move out of bounds! (x < 0)");
			}

			// Check for blocks in the way
			int direction = toMove / abs(toMove);
			for (int i = 0; i != toMove; i += direction) {
				// check if there is an obstacle infront of the player
				if (passabilityBlock[((int)player->self.position.xPos) + direction][(int)player->self.position.yPos][(int)player->self.position.stage] == 1) {
					log_(3, "[MOV] Block in player path detected! Halting!");
					break;
				} else {
					player->self.position.xPos += direction; // move by one unit
					updatePlayerBlockKnowledgeTable(player);
				}
			}

			log_(3, "[MOV] Moved on the x axis!");
		} else if (compareSpan(&axis, "y")) {
			log_(3, "[MOV] Moving on the y axis!");
			// Check for out of bounds and adjust accordingly
			if (player->self.position.yPos + toMove >= 12) {
				toMove = 11 - player->self.position.yPos;
				log_(2, "[MOV] Player attempted to move out of bounds! (y >= 12)");
			} else if (player->self.position.yPos + toMove < 0) {
				toMove = -player->self.position.yPos;
				log_(2, "[MOV] Player attempted to move out of bounds! (y < 0)");
			}

			// Check for blocks in the way
			int direction = toMove / abs(toMove);
			for (int i = 0; i != toMove; i += direction) {
				// check if there is an obstacle infront of the player
				if (passabilityBlock[(int)player->self.position.xPos][((int)player->self.position.yPos) + direction][(int)player->self.position.stage] == 1) {
					log_(3, "[MOV] Block in player path detected! Halting!");
					break;
				} else {
					player->self.position.yPos += direction; // move by one unit
					updatePlayerBlockKnowledgeTable(player);
				}
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
	int placedBlocks = 0;
	switch(type) {
		default:
			log_(2, "[GEN] ERR: Generation behavior for type undefined!");
			break;
		case 0:
			for(int i = 0; i < 11; i++) { // Generate for all 13 ** 3 blocks
				for(int j = 0; j < 11; j++) {
					for(int k = 0; k < 11; k++) {
						if (randIntInRange(0, 6) == 0) {
							passabilityBlock[i][j][k] = 1;
							placedBlocks++;
						}
						log_(3, "[GEN] Determined block for current position!");
					}
				}
			}
			printf("World generation finished, %d blocks placed in your world!\n", placedBlocks);
			double percentageBlocksInWorld = (((double) placedBlocks) / 1728) * 100;
			fprintf(logFilePtr, "[GEN] World generation finished, %d blocks placed in your world out of 1728! (~%lf%%)\n", placedBlocks, percentageBlocksInWorld);
			for (int i = 0; i < 11; i++) {
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
	for(int i = 0; i < 6; i++) {
		for(int j = 0; j < 5; j++) {
			playerEntity.inventory[i][j] = 0;
		}
	}

	// Assign playerEntity to player
	player->self = playerEntity;

	// Assign FALSE to every Block in the game
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 11; j++) {
			for (int k = 0; k < 11; k++) {
				player->knownBlocks[i][j][k] = FALSE;
			}
		}
	}
	return 0;
}

// Update world
int updatePlayerLevel(Player *player) {
	if (player->self.xp >= 100) {
		double overflow = ((int) player->self.xp) % 100;
		player->self.level += (player->self.xp - overflow) / 100;
		player->self.xp = 0 + overflow;
	}
	return 0;
}

int updatePlayerBlockKnowledgeTable(Player *player) {
	log_(3, "[WU] Updating player block knowledge table...");
	log_(3, "[WU] Checking for blocks in player sight");
	for (int i = 0; i < 3; i++) {
		log_(3, "[WU] Next row: ");
		for (int j = 0; j < 3; j++) {
			log_(3, "[WU] Next block: ");
			int playerXPos_rel = ((int) player->self.position.xPos) - 1 + j;
			int playerYPos_rel = ((int) player->self.position.yPos) - 1 + i;
			if (!(playerXPos_rel < 0) && !(playerXPos_rel >= 12) && 
					!(playerYPos_rel < 0) && !(playerYPos_rel >= 12)) {
				player->knownBlocks[playerXPos_rel][playerYPos_rel][(int) player->self.position.stage] = TRUE;
				log_(3, "[WU] Block in bounds, set to known.");
			} else {
				log_(3, "[WU] Block out of bounds, skipping set");
			}
		}
	}

	return 0;
}

// Auxiliary functions
double distance2D(Coordinates position1, Coordinates position2) { // Uses pythagorean theorem to calculate the distance
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
	char* ret = calloc(range, sizeof(char));
	strncpy(ret, input + begin, range);
	return ret;
}

int randIntInRange(int min, int max) {
	return (rand() % (max - min + 1)) + min;
}
