#include "value.h"

#ifndef _TOKENIZER
#define _TOKENIZER

Value *cell2;
int secondCellToAdd;
// Returns -1 if it is TRUE, -2 if it's FALSE, and nextChar otherwise
int isBool(char ch, char nextChar);

// returns 1 if digit is a number between 0-9, and 0 otherwise
int isNum(char digit);

//returns 1 if ch is a character that can start a symbol
int isSymbol(char ch);

//returns 1 if ch is a character that is allowed inside the symbol
int isSymbolSubsequent(char ch);

//constructing a string from input until close quote
char *constructString();

Value *constructNum(char currChar);


// Read all of the input from stdin, and return a linked list consisting of the
// tokens.
Value *tokenize();

void printStringWON(char *string);

// Displays the contents of the linked list as tokens, with type information
void displayTokens(Value *list);

#endif
