#include <stdlib.h>
#include <stdio.h>
#include "prototypes.h"

// Flags
int devmode = 0;
int godmode = 0;
int logging_level = 0;

int main(int argc, char *argv[]) {
	// Reading program arguments
	if (argc>0) {
		int doNotCheckNext = 0;
		for (int i = 1; i < argc; i++) {
			if (doNotCheckNext == 1) { 
				doNotCheckNext = 0; 
				continue; 
			}
			if (argv[i] == "-d") { devmode = 1; }
			else if (argv[i] == "-g") { godmode = 1; }
			else if (argv[i] == "-l") { 
				logging_level = (int)(argv[i+1]); 
				doNotCheckNext = 1;
			}
		}
	}

	// Verifying validity of arguments
	if (logging_level > 2 || logging_level < 0) {
		return -1;
	}
	return 0;
}



