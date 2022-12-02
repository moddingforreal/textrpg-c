// Structs
typedef struct Coordinates
{
	double xPos;
	double yPos;
	double stage;
} Coordinates;
typedef struct Entity
{
	Coordinates position;

	// base attributes
	double baseHealth;
	double baseAttack;

	// attributes
	double health;
	double attack;
	double level;
	double xp;
	double inventory[4][5];
} Entity;
typedef struct Player
{
	Entity self;
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
