#include "stringfuncs.h"
// Structs
typedef struct Coordinates
{
	double xPos;
	double yPos;
	double stage;
} Coordinates;
typedef struct Entity
{
	// Programmatically important attributes
	int type;

	// Movement
	Coordinates position;

	// Base attributes
	double baseHealth;
	double baseAttack;

	// Attributes
	double health;
	double attack;
	double level;
	double xp;
	double inventory[6][5];
} Entity;
typedef struct Player
{
	Entity self;
	int knownBlocks[12][12][12];
} Player;

typedef struct Statistics
{
	// # of times the player killed xyz
	double killsSlime;
	double killsZombie;
	double killsSkeleton;
	double killsOgre;
	double killsViper;
	double killsLitch;
	double killsGolem;
	double killsHydra;
	double killsUndeadWarrior;
	double killsDragon;

	// # of times the player was killed by xyz
	double killedSlime;
	double killedZombie;
	double killedSkeleton;
	double killedOgre;
	double killedViper;
	double killedLitch;
	double killedGolem;
	double killedHydra;
	double killedUndeadWarrior;
	double killedDragon;

	// Random statistics
	double totalDamageDealt;
	double totalDamageReceived;
	double totalHealthHealed;
	double totalFightsWon;
	double totalFightsLost;
} Statistics;

// Function prototypes

int parseArgs(int argc, char *argv[]);
int runGame();
void getInput(char *result, int max, char prompt[]);
int isInRangeInt(int value, int lowerBound, int upperBound);
double distance2D(Coordinates position1, Coordinates position2);
double square(double x);
void clearScreen();
int log_(int logLvl, char msg[]);
char* getCharSeq(char input[], int begin, int end);
int playerInit(Player *player);
int cmpSeq(char input[], int offset, char compareTo[]);
int initLog(int logLvl);
int stopLog();
int generate(int type, Player *player);
int movePlayer(CharSpan *args, Player *player, int argCount);
int randIntInRange(int min, int max);
int updatePlayerLevel(Player *player);
int updatePlayerBlockKnowledgeTable(Player *player);
int checkPlayerOnTransportSpace(Player *player);
int endGame(Player *player);
int invtools(CharSpan* args, Player* player, int argCount);
int getFreeSlotsCount(Player* player);
int newEntity(int type, Player* player);
int unusedEntitySpot();
