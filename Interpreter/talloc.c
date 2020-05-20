//talloc.c
#include "talloc.h"
#include <assert.h>
#include <stdio.h>


// Create a new NULL_TYPE value node.
Value *makeNull2(){
    Value *nullVal = malloc(sizeof(Value));
    (*nullVal).type = NULL_TYPE;
    (*nullVal).mark = 0;
    return nullVal;
}

Value *head; // Global head pointer needed for Talloc
int isInitialized = 0; //0 for not, 1 for is

// Utility to make it less typing to get car value. Use assertions to make sure
// that this is a legitimate operation.
Value *car2(Value *list) {
    assert((*list).type == CONS_TYPE);
    return (*list).c.car;
}

// Utility to make it less typing to get cdr value. Use assertions to make sure
// that this is a legitimate operation.
Value *cdr2(Value *list) {
    assert((*list).type == CONS_TYPE);
    return (*list).c.cdr;
}

/*Helper function that accumulates the length of a char array*/
int calculateLength2(char *string) {
    int length = 0;
    while (string[length] != '\0') {
        length++;
    }
    length++;
    return length;
}

// Create a new CONS_TYPE value node.
Value *cons2(Value *newCar, Value *newCdr){
    Value *cell = malloc(sizeof(Value));
    (*cell).type = CONS_TYPE;
    (*cell).c.car = newCar;
    (*cell).c.cdr = newCdr;

    return cell;
}



// Replacement for malloc that stores the pointers allocated. It should store
// the pointers in some kind of list; a linked list would do fine, but insert
// here whatever code you'll need to do so; don't call functions in the
// pre-existing linkedlist.h. Otherwise you'll end up with circular
// dependencies, since you're going to modify the linked list to use talloc.
void *talloc(size_t size){
    if(isInitialized == 0){
        head = makeNull2();
        isInitialized = 1;
    }
    void *cellCar = malloc(size);
    head = cons2(cellCar, head);

    return cellCar;
}

// How we deal with not being able to use default values in Structs
// A wrapper function that tallocs a new values and then sets mark to 0
// Input: size of new value || Returns: new value with mark 0
Value *initValue(int size){
    Value *newValue = talloc(size);
    (*newValue).mark = 0;
    return newValue;
}

// Input: a value || Returns: nothing, but sets value.mark to be 1
void mark(Value *value){
    (*value).mark = 1;
}

// Free all pointers allocated by talloc, as well as whatever memory you
// allocated in lists to hold those pointers.
void tfree(){
    Value *markedList = makeNull2();
    while((*head).type != NULL_TYPE){
        //if((*car2(head)).mark == 0){ //it isn't marked
            //printf("\nCAR: %i\n",  (*head).c.car);
            //printf("\nit was NOT marked\n");
            free(car2(head));
            Value *temp = cdr2(head);
            free(head);
            head = temp;
        /*} else{ //it is marked
            (*car2(head)).mark = 0;
            //printf("\nit was marked\n");
            Value *temp = cdr2(head);
            (*head).c.cdr = markedList;
            markedList = head;
            head = temp;
        }*/
    }
    if((*markedList).type == NULL_TYPE){
        free(head);
        free(markedList);
        isInitialized = 0;
    } else {
        free(head);
        head = markedList;

    }

}

// Replacement for the C function "exit", that consists of two lines: it calls
// tfree before calling exit. It's useful to have later on; if an error happens,
// you can exit your program, and all memory is automatically cleaned up.
void texit(int status) {
    tfree();
    exit(status);
}


// Return pointer to the global linked list head
Value *returnHeadFORTEST(){
    return head;
}
