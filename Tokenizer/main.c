// Tester for linked list. Note that this does not test all the functionality
// you implemented; you should enhance it significantly. We will test your
// linked list code with a different main.c.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "linkedlist.h"
#include "tokenizer.h"
#include "value.h"
#include "talloc.h"


int main() {
    Value *list = tokenize();
    //printf("PRINT %s", (*(car(car(list)))).s);
    //display(list);
    displayTokens(list);
    tfree();
     //tokenize();

}
