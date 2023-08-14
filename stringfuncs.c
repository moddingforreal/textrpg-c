#include <stdlib.h>
#include <stdio.h>
#include "stringfuncs.h"

// initializes a CharSpan with given start and length of str in span
void makeSpan(CharSpan *span, char *str, int start, int length) {
    span->str = &str[start];
    span->length = length;
}

// gets the length in character of a string
int lengthChar(char* str) {
    int i;
    for(i = 0; str[i] != 0; i++);
    return i;
}

// compares input beginning from offset and compareTo, returns 1 if they match until the end of compareTo
int cmpSeq(char input[], int offset, char compareTo[]) {
	for(int i = 0; compareTo[i] != 0; i++) {
		if(input[i + offset] == 0 || input[i + offset]  != compareTo[i])
			return 0;
	}
	return 1;
}

// splits the given string str with seperator and saves the results in out, a maxium of maxResults will be created
int split(CharSpan *out, int maxResults, char *str, char *seperator) {
    int seperatorLenght = lengthChar(seperator);
    int resultCount = 0;
    int startPos = 0;
    int i;
    // split untill the end of the string is reached or maxResult - 1 is reached
    for(i = 0; resultCount < maxResults - 1 && str[i] != 0; i++) {
        if(cmpSeq(str, i, seperator)) {
            makeSpan(&out[resultCount], str, startPos, i - startPos);

            resultCount++;
            i += seperatorLenght - 1; // jump to the end of the seperator to ignor it
            startPos = i + 1; // next span starts one character after the end of the last seperator
        }
    }
    // add the remaining part to the string as the last result
    int remainderLength = lengthChar(&str[startPos]);
    makeSpan(&out[resultCount], str, startPos, remainderLength);
    return resultCount + 1;
}

// write the content of span into target, add 0-terminator at the end
void extractContent(char *target, CharSpan *span) {
    int i;
    for(i = 0; i  < span->length && span->str[i] != 0; i++) {
        target[i] = span->str[i];
    }
    target[i] = 0;
}

// return 1 if the content of the span arg0 is equal to the content of the string arg1
int compareSpan(CharSpan *arg0, char *arg1) {
    int i;
    for(i = 0; i < arg0->length; i++) {
        if(arg0->str[i] != arg1[i])
            return 0; // failed
        if(arg1[i] == 0)
            return 1; // end of string in span and str reached before defined end of span
    }
    return arg1[i] == 0; // 1 if end of span (caused exit of loop) is also end of str
}

// converts a span into an integer
int spanToInt(CharSpan span) {
    // max of 12 chars used for conversion as an int in base 10 can be max 11 chars long
    if(span.length > 12) {
        span.length = 12;
    }
    char str[13];
    extractContent(str, &span);
    // convert the extracted content
    return strtol(str, (char **)NULL, 10);
}

// coverts a span to a double
double spanToDouble(CharSpan span) {
    // my source says the largest representation of a double could be 1079 chars long
    if(span.length > 1079) {
        span.length = 1079;
    }
    char str[1080];
    extractContent(str, &span);
    // convert the extracted content
    return strtod(str, NULL);
}
