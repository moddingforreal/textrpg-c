typedef struct CharSpan {
    char *str;
    int length;
} CharSpan;

// initializes a CharSpan with given start and length of str in span
void makeSpan(CharSpan *span, char *str, int start, int length);
// splits the given string str with seperator and saves the results in out, a maxium of maxResults will be created
int split(CharSpan *out, int maxResults, char *str, char *seperator);
// write the content of span into target, add 0-terminator at the end
void extractContent(char *target, CharSpan *span);
// return 1 if the content of the span arg0 is equal to the content of the string arg1
int compareSpan(CharSpan *arg0, char *arg1);
// converts a span into an integer
int spanToInt(CharSpan span);

// gets the length in character of a string
int lengthChar(char* str);
// compares input beginning from offset and compareTo, returns 1 if they match until the end of compareTo
int cmpSeq(char input[], int offset, char compareTo[]);