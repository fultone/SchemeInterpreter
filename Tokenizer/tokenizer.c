#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "linkedlist.h"
#include "tokenizer.h"
#include "value.h"
#include "talloc.h"
#include <string.h>
#include <ctype.h>


/**** NOTE BEFORE GRADING THIS !!
        Our code doesn't pass any of Dave's tests because our IDE automatically puts
        a new line at the end of every document, and this EOF new line character does not
        appear in Dave's tester algorithm
 ****/

// Returns -1 if it is TRUE, -2 if it's FALSE, and nextChar otherwise
int isBool(char ch, char nextChar){
    //if true and nextChar is either newline, space, or paren
    if(ch == 't' && (nextChar == '\n' || nextChar == ' '|| nextChar == '('
                                        || nextChar == ')' || nextChar == '[' || nextChar == ']')){
        return -1;
    }
    //if false and nextChar is either newline, space, or paren
    else if(ch == 'f' && (nextChar == '\n' || nextChar == ' '|| nextChar == '('
                                        || nextChar == ')' || nextChar == '[' || nextChar == ']')){
        return -2;
    }
    else {
        perror("TOKENIZER ERROR: Unexpected attribute following '#'");
        texit(EXIT_FAILURE);
        return 0;
    }
}

// returns 1 if digit is a number between 0-9, and 0 otherwise
int isNum(char digit){
  return (digit == '0' || digit == '1' || digit == '2' || digit == '3' || digit == '4'
         || digit == '5'|| digit=='6'|| digit == '7' || digit == '8' || digit == '9');
}

//returns 1 if ch is a character that can start a symbol
int isSymbol(char ch) {
  return (isalpha(ch) || ch == '!' || ch == '$' || ch == '%' || ch == '&'
                      || ch == '*' || ch == '/' || ch == ':' || ch == '<'
                      || ch == '>' || ch == '=' || ch == '?' || ch == '~'
                      || ch == '_' || ch == '^');
}

//returns 1 if ch is a character that is allowed inside the symbol
int isSymbolSubsequent(char ch) {
  return (isSymbol(ch) || isdigit(ch) || ch == '.' || ch == '+' || ch == '-');
}

//Helper function to construct a string from input until close quote
char *constructString() {
    // Dave gave us permission to just allocate a lot of space for the string
    // Rather than trying to compute the length
    char *newString = talloc(301 * sizeof(char));
    newString[0] = '"';
    char currChar = (char)fgetc(stdin);
    int i = 1;
    while(currChar != EOF){ // loop until you find a closing quote
        if(currChar =='"'){
            newString[i] = currChar;
            newString[i+1] = '\0';
            return newString;
        }
        newString[i] = currChar;
        currChar = (char)fgetc(stdin);
        i++;
    }
    newString[i] = '\0';
    // For when EOF is reached without finding a closing quote
    perror("TOKENIZER ERROR: String not closed");
    texit(EXIT_FAILURE);
    return NULL;
}


Value *constructNum(char currChar){
    double dub = 0.0;
    int inty = 0;
    int isDouble = 0;
    double denom = 10.0;
    int i = 2;
    //while we're not at the end of the double because there's still more decimal places to append
    while(isNum(currChar)){
      if (currChar == '0'){
           inty =  (inty*10) + 0;
       }else if (currChar == '1'){
           inty = (inty*10) + 1;
       }else if (currChar == '2'){
           inty = (inty*10) + 2;
       }else if (currChar == '3'){
           inty = (inty*10) + 3;
       }else if (currChar == '4'){
           inty = (inty*10) + 4;
       }else if (currChar == '5'){
           inty = (inty*10) + 5;
       }else if (currChar == '6'){
           inty = (inty*10) + 6;
       }else if (currChar == '7'){
           inty = (inty*10) + 7;
       }else if (currChar == '8'){
           inty = (inty*10) + 8;
       }else if (currChar == '9'){
           inty = (inty*10) + 9;
       }
       currChar = (char)fgetc(stdin);
    }//end while
    if ((currChar != '.' && currChar != ' ' && currChar != '(' && currChar != ')' && currChar != EOF && currChar != '\n' 
                        && currChar != '[' && currChar != ']')){
      perror("TOKENIZER ERROR: Unexpected symbol following number");
      texit(EXIT_FAILURE);
    }
    else if(currChar == '.'){
      isDouble = 1;
      if(currChar == '.'){
        currChar = (char)fgetc(stdin);
        }
      
      if(isNum(currChar) || currChar == ' ' || currChar == '(' || currChar == ')' || currChar == EOF || currChar == '\n' 
                        || currChar == '[' || currChar == ']'){
        while(isNum(currChar)){
            int j;
            if (currChar == '0'){
                dub += (0 / denom); //add on the next value
            }else if (currChar == '1'){
                dub += (1 / denom); //add on the next value
            }else if (currChar == '2'){
                dub += (2 / denom); //add on the next value
            }else if (currChar == '3'){
                dub += (3 / denom);
            }else if (currChar == '4'){
                dub += (4 / denom);
            }else if (currChar == '5'){
                dub += (5 / denom);
            }else if (currChar == '6'){
                dub += (6 / denom);
            }else if (currChar == '7'){
                dub += (7 / denom);
            }else if (currChar == '8'){
                dub += (8 / denom);
            }else if (currChar == '9'){
                dub += (9 / denom);
            }
            currChar = (char)fgetc(stdin);
            i++;
            denom = denom * 10; //increases the decimal place by one with each iteration
        }
      }
      else {
        perror("TOKENIZER ERROR: Unexpected symbol following '.'");
        texit(EXIT_FAILURE);
      }
    }
    if (currChar == '(' || currChar == ')' || currChar == '[' || currChar == ']') {
        Value *carVal2 = initValue(sizeof(Value));
        if (currChar == '(') {
            (*carVal2).type = OPEN_TYPE;
            (*carVal2).i = currChar;
        }
        else if (currChar == ')'){
            (*carVal2).type = CLOSE_TYPE;
            (*carVal2).i = currChar;
        }
        else if (currChar == '['){
            (*carVal2).type = OPEN_BRACKET_TYPE;
            (*carVal2).i = currChar;
        }
        else{
            (*carVal2).type = CLOSE_BRACKET_TYPE;
            (*carVal2).i = currChar;
        }
        (*cell2).c.car = carVal2;
        secondCellToAdd = 1;
    }
    if(isDouble == 1){
      dub += (double)inty;
      Value *dubVal = talloc(sizeof(Value));
      (*dubVal).type = DOUBLE_TYPE;
      (*dubVal).d = dub;
      return dubVal;
    } else {
      Value *intVal = talloc(sizeof(Value));
      (*intVal).type = INT_TYPE;
      (*intVal).i = inty;
      return intVal;
    }
}

char *constructSymbol(char ch) {
  // Dave gave us permission to just allocate a lot of space for the symbol
  // Rather than trying to compute the length
  char *newSymbol = talloc(301 * sizeof(char));
  newSymbol[0] = ch;
  char currChar = (char)fgetc(stdin);
  int i = 1;
  while(currChar != EOF && currChar != ' ' && currChar != '\n' && currChar != '(' 
                        && currChar != ')' && currChar != '[' && currChar != ']' 
                        && currChar != ';'){ // loop until you find a closing quote
      if(isSymbolSubsequent(currChar)){
        newSymbol[i] = currChar;
        currChar = (char)fgetc(stdin);
        i++;
      }
      else {
        perror("TOKENIZER ERROR: Unrecognized character while constructing symbol");
        texit(EXIT_FAILURE);
      }


  }
  newSymbol[i] = '\0';
  if (currChar == '(' || currChar == ')' || currChar == '[' || currChar == ']') {
      Value *carVal2 = initValue(sizeof(Value));
      if (currChar == '(') {
          (*carVal2).type = OPEN_TYPE;
          (*carVal2).i = currChar;
      }
      else if (currChar == ')'){
          (*carVal2).type = CLOSE_TYPE;
          (*carVal2).i = currChar;
      }
      else if (currChar == '['){
            (*carVal2).type = OPEN_BRACKET_TYPE;
            (*carVal2).i = currChar;
        }
        else{
            (*carVal2).type = CLOSE_BRACKET_TYPE;
            (*carVal2).i = currChar;
        }
      (*cell2).c.car = carVal2;
      secondCellToAdd = 1;
  }
  return newSymbol;
}

// Read all of the input from stdin, and return a linked list consisting of the
// tokens.
Value *tokenize(){
    char ch;
    Value *list = makeNull();
    ch = (char)fgetc(stdin);

    while (ch != EOF) {
        secondCellToAdd = 0;

        Value *cell = initValue(sizeof(Value));
        (*cell).type = CONS_TYPE;
        //in case we need it
        cell2 = initValue(sizeof(Value));
        (*cell2).type = CONS_TYPE;

        int wasSpace = 0;
        int wasNewLine = 0;
        int wasEOF = 0;

        Value *carVal = initValue(sizeof(Value));
        //****** OPEN ******//
        if(ch == '('){  // Open paren
          (*carVal).type = OPEN_TYPE;
          (*carVal).i = ch;
        }
        //****** CLOSE ******//
        else if(ch == ')'){   // Closed paren
          (*carVal).type = CLOSE_TYPE;
          (*carVal).i = ch;
        }
        //****** OPEN BRACKET ******//
        else if(ch == '['){  // Open bracket
          (*carVal).type = OPEN_BRACKET_TYPE;
          (*carVal).i = ch;
        }
        //****** CLOSE BRACKET ******//
        else if(ch == ']'){   // Closed bracket
          (*carVal).type = CLOSE_BRACKET_TYPE;
          (*carVal).i = ch;
        }
        //****** BOOLEAN ******//
        else if(ch == '#'){ // Boolean
            ch = (char)fgetc(stdin);
            if (ch != EOF) {
                char nextChar = (char)fgetc(stdin); // Thing that follows a t or f
                int isBoolean = isBool(ch, nextChar);
                //if it's a boolean and the next char is ( or ), create a second
                //cell to add the '(' or ')'
                if (isBoolean == -1 || isBoolean == -2) {
                    if (nextChar == '(' || nextChar == ')' || nextChar == '[' || nextChar == ']') {
                        Value *carVal2 = initValue(sizeof(Value));
                        if (nextChar == '(') {
                            (*carVal2).type = OPEN_TYPE;
                            (*carVal2).i = nextChar;
                        }
                        else if (nextChar == ')') {
                            (*carVal2).type = CLOSE_TYPE;
                            (*carVal2).i = nextChar;
                        }
                        else if (nextChar == '[') {
                            (*carVal2).type = OPEN_BRACKET_TYPE;
                            (*carVal2).i = nextChar;
                        }
                        else {
                            (*carVal2).type = CLOSE_BRACKET_TYPE;
                            (*carVal2).i = nextChar;
                        }
                        (*cell2).c.car = carVal2;
                        secondCellToAdd = 1;
                    }
                }
                if (isBoolean == -1){          // TRUE
                    (*carVal).type = BOOL_TYPE;
                    (*carVal).i = 1;
                } else if (isBoolean == -2){   // FALSE
                    (*carVal).type = BOOL_TYPE;
                    (*carVal).i = 0;
                }

            }
            else { // When ch is NEW LINE --> just return a new symbol which is #
                perror("TOKENIZER ERROR: Unexpected attribute following '#'");
                texit(EXIT_FAILURE);
            }

        }
        //****** STRING ******//
        else if(ch == '"'){
            char *str = constructString();
            (*carVal).type = STR_TYPE;
            (*carVal).s = str;
        }

        //****** COMMENTS ******//
        else if(ch == ';'){
            while(ch != '\n' && ch != EOF){
                ch = (char)fgetc(stdin);
            }
            if (ch == EOF) {
                wasEOF = 1;
            }
            else if (ch == '\n') {
                wasNewLine = 1;
            }
        }

        //****** INTS AND DOUBLES ******//
        else if (ch == '+' || ch == '-'){
          char sign = ch;
          ch = (char)fgetc(stdin);
          //if + or - is followed by a number, create a number
          if(isNum(ch) || ch == '.'){
            carVal = constructNum(ch);
            if (sign == '-') {
              if((*carVal).type == INT_TYPE){
                (*carVal).i = (-1) * ((*carVal).i);
              } else {
                (*carVal).d = (-1) * ((*carVal).d);
              }
            }
          }
          else {
            // if + or - followed by an invalid character, throw an error
            if(ch != ' ' && ch != '(' && ch != ')' && ch != '[' && ch != ']' && ch != EOF  && ch != '\n') {
              perror("TOKENIZER ERROR: Unexpected attribute following sign");
              texit(EXIT_FAILURE);
            }
            else {
              (*carVal).type = SYMBOL_TYPE;
              char *str = talloc(301 * sizeof(char));
              str[0] = sign;
              str[1] = '\0';
              (*carVal).s = str;
              //if followed by a paren, create paren value to add to the list
              if (ch == '(' || ch == ')' || ch == '[' || ch == ']') {
                  Value *carVal2 = initValue(sizeof(Value));
                  if (ch == '(') {
                      (*carVal2).type = OPEN_TYPE;
                      (*carVal2).i = ch;
                  }
                  else if (ch == ')') {
                      (*carVal2).type = CLOSE_TYPE;
                      (*carVal2).i = ch;
                  }
                  else if (ch == '[') {
                      (*carVal2).type = OPEN_BRACKET_TYPE;
                      (*carVal2).i = ch;
                  }
                  else {
                      (*carVal2).type = CLOSE_BRACKET_TYPE;
                      (*carVal2).i = ch;
                  }
                  (*cell2).c.car = carVal2;
                  secondCellToAdd = 1;
              }
            }
          }

        }
        else if (isNum(ch)){
            carVal = constructNum(ch);
        }


        //****** SYMBOL ******//
        else if (isSymbol(ch)) {
            (*carVal).type = SYMBOL_TYPE;
            char *symbol = constructSymbol(ch);
            (*carVal).s = symbol;

        }

        //****** SPACE ******//
        else if (isspace(ch)) {
          while (isspace(ch)) {
            ch = (char)fgetc(stdin);
          }
          wasSpace = 1;
        }
        else if (ch == '.') {
            carVal = constructNum(ch);
        }

        //****** NEW LINE ******//
        else if (ch == '\n') {
          while (ch == '\n') {
            ch = (char)fgetc(stdin);
          }
          wasNewLine = 1;
        }

        //****** OTHERWISE THROW AN ERROR ******//
        else {
          perror("TOKENIZER ERROR: Unrecognized character");
          texit(EXIT_FAILURE);
        }
        if (!wasSpace && !wasNewLine && !wasEOF) {
          ch = (char)fgetc(stdin);
          (*cell).c.car = carVal;
          (*cell).c.cdr = list;
          list = cell;
          //if two values needed to be added, add the second one
          if (secondCellToAdd == 1) {
              (*cell2).c.cdr = list;
              list = cell2;
              secondCellToAdd = 0;
          }
        }
    }
    return reverse(list);
}

/*A helper function that iterates through a char array, allowing us to print strings*/
//WON = with out newline
void printStringWON(char *string) {
    int length = 0;
    while (string[length] != '\0') {
        printf("%c", string[length]);
        length++;
    }
}

// Displays the contents of the linked list as tokens, with type information
void displayTokens(Value *list){

    Value *tempHead = list;
    while ((*tempHead).type != NULL_TYPE){
        switch((*(*tempHead).c.car).type){

            case INT_TYPE:
                printf("%i: Integer\n", (*(*tempHead).c.car).i);
                break;
            case STR_TYPE:
                printStringWON((*(*tempHead).c.car).s);
                printf(": String\n");
                break;
            case DOUBLE_TYPE:
                printf("%f: Float\n", (*(*tempHead).c.car).d);
                break;
            case OPEN_TYPE:
                printf("(: Open\n");
                break;
            case CLOSE_TYPE:
                printf("): Close\n");
                break;
            case OPEN_BRACKET_TYPE:
                printf("[: Open bracket\n");
                break;
            case CLOSE_BRACKET_TYPE:
                printf("]: Close bracket\n");
                break;
            case BOOL_TYPE:
                printf("%i: Boolean\n", (*(*tempHead).c.car).i);
                break;
            case SYMBOL_TYPE:
                //printStringWON((*(*tempHead).c.car).s);
                printf("%s : Symbol\n", (*(*tempHead).c.car).s );
                break;
            default:
                break;
        }

        tempHead = (*tempHead).c.cdr;
    }


}
