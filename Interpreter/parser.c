#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "linkedlist.h"
#include "value.h"
#include "talloc.h"
#include "parser.h"


// Takes a list of tokens from a Racket program, and returns a pointer to a
// parse tree representing that program.
Value *parse(Value *tokens){
  Value *tempTokens = tokens;
  Value *tree = makeNull();  // tree = the stack
  Value *parenStack = makeNull(); // stack to keep track of paren types
  //int lastParen

  while((*tempTokens).type != NULL_TYPE){
      if( (*(*tempTokens).c.car).type != CLOSE_TYPE
          && (*(*tempTokens).c.car).type != CLOSE_BRACKET_TYPE){
          // push it onto stack
          Value *cell = talloc(sizeof(Value)); // create a new cons cell
          (*cell).type = CONS_TYPE;

          Value *carVal = talloc(sizeof(Value));
          *carVal = (*(*tempTokens).c.car);

          (*cell).c.car = carVal;
          (*cell).c.cdr = tree;
          tree = cell;
      }

      else {
        // Push close paren/bracket onto the stack
        Value *closeCell = talloc(sizeof(Value)); // create a new cons cell
        (*closeCell).type = CONS_TYPE;

        Value *cVal = talloc(sizeof(Value));
        *cVal = (*(*tempTokens).c.car);

        (*closeCell).c.car = cVal;
        (*closeCell).c.cdr = parenStack;
        parenStack = closeCell;
        // // // //

        Value *subtree = makeNull(); // create a new subtree, with root <expr>
        // while(top item on stack is NOT an open paren/bracket):
        while((*(*tree).c.car).type != OPEN_TYPE
              && (*(*tree).c.car).type != OPEN_BRACKET_TYPE) {
            // If current top item on stack isn't OPEN_TYPE, and the next
            // item on stack is null -- ie open paren never found
            if((*(*tree).c.cdr).type == NULL_TYPE) {
                perror("Syntax error: too many close parentheses");
                texit(EXIT_FAILURE);            //  QUIT --> throw parsing error
            }
            else {
              Value *cell = talloc(sizeof(Value)); // create a new cons cell
              (*cell).type = CONS_TYPE;

              Value *leaf = (*tree).c.car; // pop an item x from the stack
              (*leaf).type = (*(*tree).c.car).type;
              tree = (*tree).c.cdr; // (still popping off)
              (*cell).c.car = leaf; // ...and add it as left child to the subtree
              (*cell).c.cdr = subtree;
              subtree = cell;
            }
        }
      // Check to see if last item on parenStack was the same type as the
      // open paren/bracket currently being looked at
      if( (  (*(*tree).c.car).type == OPEN_TYPE && (*(*parenStack).c.car).type == CLOSE_TYPE)
          || ((*(*tree).c.car).type == OPEN_BRACKET_TYPE && (*(*parenStack).c.car).type == CLOSE_BRACKET_TYPE) ) {
        parenStack = (*parenStack).c.cdr; // pop off top value in parenStack
        tree = (*tree).c.cdr; // skip over the open paren
      }
      else {
        perror("Syntax error: inconsistent use of brackets and parentheses");
        texit(EXIT_FAILURE);            //  QUIT --> throw parsing error
      }

      //  push entire subtree onto the stack
       Value *cell = talloc(sizeof(Value)); // create a new cons cell
       (*cell).type = CONS_TYPE;
       (*cell).c.car = subtree;

       (*cell).c.cdr = tree;
       tree = cell;

     }
     tempTokens = (*tempTokens).c.cdr;

   }

   Value *temp = tree;
   while((*temp).type != NULL_TYPE){
     if((*(*temp).c.car).type == OPEN_TYPE || (*(*temp).c.car).type == OPEN_BRACKET_TYPE ||
        (*(*temp).c.car).type == CLOSE_TYPE || (*(*temp).c.car).type == CLOSE_BRACKET_TYPE) {
       perror("Syntax error: too many open parentheses");
       texit(EXIT_FAILURE);
     }
     temp = (*temp).c.cdr;
   }

   return reverse(tree);
}



void printCar(Value *car){
  switch((*car).type){
      case INT_TYPE:
          printf("%i", (*car).i);
          break;
      case STR_TYPE:
          printf("%s", (*car).s);
          break;
      case DOUBLE_TYPE:
          printf("%f", (*car).d);
          break;
      case OPEN_TYPE:
          printf("(");
          break;
      case CLOSE_TYPE:
          printf(")");
          break;
      case BOOL_TYPE:
        if ((*car).i == 1) {
            printf("#t");
        }
        else {
            printf("#f");
        }
        break;
      case SYMBOL_TYPE:
          printf("%s", (*car).s);
          break;
      case CLOSURE_TYPE:
          printf("#<procedure>");
          break;
      case NULL_TYPE:
          printf("()");
          break;
      default:
          break;
  }
}


// Takes in a root node, and prints
void printSubtree(Value *tree){
  Value *tempTree = tree;
  printf("(");

  while((*tempTree).type != NULL_TYPE){
        if((*(*tempTree).c.car).type != CONS_TYPE){
            printCar((*tempTree).c.car);

        }
        else {
            printSubtree((*tempTree).c.car);
        }
        if(tempTree->c.cdr->type != CONS_TYPE && tempTree->c.cdr->type != NULL_TYPE){
              printf(" . ");
              printCar(tempTree->c.cdr);
              break;
        }else{
            tempTree = (*tempTree).c.cdr;
            if((*tempTree).type != NULL_TYPE){
              printf(" ");
            }
        }
  }
  printf(")");
}

// Prints the tree to the screen in a readable fashion. It should look just like
// Racket code; use parentheses to indicate subtrees.
void printTree(Value *tree){
  Value *tempTree = tree;
  while((*tempTree).type != NULL_TYPE){
    if((*(*tempTree).c.car).type != CONS_TYPE){
      printCar((*tempTree).c.car);
    }
    else {
      printSubtree((*tempTree).c.car);
    }
    printf(" ");
    tempTree = (*tempTree).c.cdr;
  }
}
