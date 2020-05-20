#include <stdbool.h>
#include "value.h"


#ifndef _LINKEDLIST
#define _LINKEDLIST

// Create a new NULL_TYPE value node.
Value *makeNull();

// Create a new CONS_TYPE value node.
//
// FAQ: If the newCdr contains a string within, do we need to make a copy of
// that string?
//
// ANS: Yes, you should make a copy of that string. The function cleanup (below)
// is then responsible for freeing the memory associated with this copy of the
// string.
Value *cons(Value *newCar, Value *newCdr);

/*A helper function that iterates through a char array, allowing us to print strings*/
void printString(char *string);

// Display the contents of the linked list to the screen in some kind of
// readable format
void display(Value *list);

// Return a new list that is the reverse of the one that is passed in. All
// content within the list should be duplicated; there should be no shared
// memory between the original list and the new one.
//
// FAQ: What if there are nested lists inside that list?
// ANS: There won't be for this assignment. There will be later, but that will
// be after we've got an easier way of managing memory.
Value *reverse(Value *list);

// Utility to make it less typing to get car value. Use assertions to make sure
// that this is a legitimate operation.
Value *car(Value *list);

// Utility to make it less typing to get cdr value. Use assertions to make sure
// that this is a legitimate operation.
Value *cdr(Value *list);

// Utility to check if pointing to a NULL_TYPE value. Use assertions to make sure
// that this is a legitimate operation.
bool isNull(Value *value);

// Measure length of list. Use assertions to make sure that this is a legitimate
// operation.
int length(Value *value);

// Takes two lists as input, and returns a single list (of the two merged together)
Value *append(Value *list1, Value *list2);

// Takes in the number of params, followed by all params (that can be any number of params)
// All params must be pointers
// Returns a list containing all given parameters
Value *list(int num, ...);



#endif
