#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "linkedlist.h"
#include "value.h"
#include "talloc.h"
#include <string.h>
#include <stdarg.h>


// Create a new NULL_TYPE value node.
Value *makeNull(){
    Value *nullVal = initValue(sizeof(Value));
    (*nullVal).type = NULL_TYPE;
    return nullVal;
}

/*Helper function that accumulates the length of a char array*/
int calculateLength(char *string) {
    int length = 0;
    while (string[length] != '\0') {
        length++;
    }
    length++;
    return length;
}

// Create a new CONS_TYPE value node.
Value *cons(Value *newCar, Value *newCdr){
    Value *cell = initValue(sizeof(Value));
    (*cell).type = CONS_TYPE;
    //If cdr is string type then we have to allocate memory for the cdr string,
    //copy the values over to our new char arr[], and set newCdr.s's pointer to this array
    if((*newCdr).type == STR_TYPE){
        int lenNewCdr = calculateLength((*newCdr).s);
        char *temp = talloc(lenNewCdr * sizeof(char));
        int i;
        for(i = 0; i < lenNewCdr; i++){
            temp[i] = (*newCdr).s[i];
        }
        (*newCdr).s = temp;

    }
    //If car is string type then do the same thing
    if((*newCar).type == STR_TYPE){
        int lenNewCar = calculateLength((*newCar).s);
        char *temp = talloc(lenNewCar * sizeof(char));
        int i;
        for(i = 0; i < lenNewCar; i++){
            temp[i] = (*newCar).s[i];
        }
        (*newCar).s = temp;
    }
    (*cell).c.car = newCar;
    (*cell).c.cdr = newCdr;

    return cell;
}

/*A helper function that iterates through a char array, allowing us to print strings*/
void printString(char *string) {
    int length = 0;
    while (string[length] != '\0') {
        printf("%c", string[length]);
        length++;
    }
}



// Display contents of linked list to the screen based on the car and cdr types
// We are assuming that you are only passing in a linked list since we received no other instructions
void display(Value *list){
    if ((*list).type == CONS_TYPE){
        //assert((*(*list).c.car).type != CONS_TYPE); //the car of the cons cell can't be of cons type
        switch((*(*list).c.car).type){
            case INT_TYPE:
                printf("%i\n", (*(*list).c.car).i);
                break;
            case STR_TYPE:
                printString((*(*list).c.car).s);
                break;
            case DOUBLE_TYPE:
                printf("%f\n", (*(*list).c.car).d);
                break;
            case CONS_TYPE:
                break;
            case NULL_TYPE:
                printf("NULL\n");
                break;
            case PTR_TYPE:
                break;
            case OPEN_TYPE:
                printf("%c\n", (*(*list).c.car).i);
                break;
            case CLOSE_TYPE:
                printf("%c\n", (*(*list).c.car).i);
                break;
            case OPEN_BRACKET_TYPE:
                printf("%c\n", (*(*list).c.car).i);
                break;
            case CLOSE_BRACKET_TYPE:
                printf("%c\n", (*(*list).c.car).i);
                break;
            case BOOL_TYPE:
                printf("%i\n", (*(*list).c.car).i);
                break;
            case SYMBOL_TYPE:
                printf("%c\n", (*(*list).c.car).i);
                break;
        }

        switch((*(*list).c.cdr).type){
            case INT_TYPE:
                printf("%i\n", (*(*list).c.cdr).i);
                break;
            case STR_TYPE:
                printString((*(*list).c.cdr).s);
                break;
            case DOUBLE_TYPE:
                printf("%f\n", (*(*list).c.cdr).d);
                break;
            case CONS_TYPE:
                display((*list).c.cdr);
                break;
            case NULL_TYPE:
                printf("NULL\n");
                break;
            case PTR_TYPE:
                break;
            case OPEN_TYPE:
                break;
            case CLOSE_TYPE:
                break;
            case OPEN_BRACKET_TYPE:
                break;
            case CLOSE_BRACKET_TYPE:
                break;
            case BOOL_TYPE:
                break;
            case SYMBOL_TYPE:
                break;
        }
    }else{
        printf("NULL\n");
    }
}

// Return a new list that is the reverse of the one that is passed in
Value *reverse(Value *list){
    //assert((*list).type == CONS_TYPE);
    //assert((*car(list)).type != CONS_TYPE);

    Value *head = makeNull(); //will be the head of the reversed list
    Value *listCopy = list;
    while((*listCopy).type != NULL_TYPE){
        Value *newConsCell = initValue(sizeof(Value));
        (*newConsCell).type = CONS_TYPE;
        (*newConsCell).c.car = car(listCopy);
        (*newConsCell).c.cdr = head;
        head = newConsCell; //moving head along
        listCopy = cdr(listCopy); //copy of list -- moving
    }
    return head;
}

// Utility to make it less typing to get car value. Use assertions to make sure
// that this is a legitimate operation.
Value *car(Value *list) {
    assert((*list).type == CONS_TYPE);
    return (*list).c.car;
}

// Utility to make it less typing to get cdr value. Use assertions to make sure
// that this is a legitimate operation.
Value *cdr(Value *list) {
    assert((*list).type == CONS_TYPE);
    return (*list).c.cdr;
}

// Utility to check if pointing to a NULL_TYPE value. Use assertions to make sure
// that this is a legitimate operation.
bool isNull(Value *value){
    assert(value != NULL); //throws error if null
    return ((*value).type == NULL_TYPE);
}

// Measure length of a linked list. Use assertions to make sure that this is a legitimate
// operation.
int length(Value *value) {
    assert(value != NULL);
    if ((*value).type == CONS_TYPE) {
        return 1 + length((*value).c.cdr);
    }
    else {
        return 0;
    }
}

// Takes two lists as input, and returns a single list (of the two merged together)
Value *append(Value *list1, Value *list2){
    Value *revList1 = reverse(list1);
    Value *revList2 = reverse(list2);
    Value *newList = makeNull();
    Value *list2Copy = revList2;
    Value *list1Copy = revList1;

    while((*list2Copy).type != NULL_TYPE){
        Value *newConsCell = initValue(sizeof(Value));
        (*newConsCell).type = CONS_TYPE;
        (*newConsCell).c.car = car(list2Copy);
        (*newConsCell).c.cdr = newList;
        newList = newConsCell; //moving head along
        list2Copy = cdr(list2Copy); //copy of list -- moving
    }

    while((*list1Copy).type != NULL_TYPE){
        Value *newConsCell = initValue(sizeof(Value));
        (*newConsCell).type = CONS_TYPE;
        (*newConsCell).c.car = car(list1Copy);
        (*newConsCell).c.cdr = newList;
        newList = newConsCell; //moving head along
        list1Copy = cdr(list1Copy); //copy of list -- moving
    }

    return newList;
}


// Takes in the number of params, followed by all params (that can be any number of params)
// All params must be pointers
// Returns a list containing all given parameters
Value *list(int num, ...){
    va_list valist;
    int i;
    va_start(valist, num);
    Value *newList = makeNull();
    for(i = 0; i<num; i++){ // for each param given
        Value *val = va_arg(valist, Value *);
        Value *newConsCell = initValue(sizeof(Value));
        (*newConsCell).type = CONS_TYPE;
        (*newConsCell).c.car = val;
        (*newConsCell).c.cdr = newList;
        newList = newConsCell;
    }
    va_end(valist);
    Value *reversed = reverse(newList);
    return reversed;
}
