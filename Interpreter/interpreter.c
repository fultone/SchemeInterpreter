#include "interpreter.h"
#include "parser.h"
#include "value.h"
#include "linkedlist.h"
#include "tokenizer.h"
#include "talloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// a thin wrapper that calls eval for each top-level S-expression in the program. You
// should print out any necessary results before moving on to the next S-expression
void interpret(Value *tree){
    Frame *globalFrame = talloc(sizeof(Frame));
    globalFrame->parent = NULL;
    globalFrame->bindings = makeNull();

    bind("+",primitiveAdd, globalFrame);
    bind("null?",primitiveNull, globalFrame);
    bind("car",primitiveCar, globalFrame);
    bind("cdr",primitiveCdr, globalFrame);
    bind("cons",primitiveCons, globalFrame);
    bind("list", primitiveList, globalFrame);
    bind("append", primitiveAppend, globalFrame);
    bind("eq?", primitiveEq, globalFrame);
    bind("equal?", primitiveEqual, globalFrame);
    bind("*", primitiveMultiply, globalFrame);
    bind("-", primitiveSubtract, globalFrame);
    bind("<", primitiveLessThan, globalFrame);
    bind(">", primitiveGreaterThan, globalFrame);
    bind("/", primitiveDivide, globalFrame);
    bind("=", primitiveEquals, globalFrame);
    bind("<=", primitiveLessThanEqualTo, globalFrame);
    bind(">=", primitiveGreaterThanEqualTo, globalFrame);
    bind("modulo", primitiveModulo, globalFrame);
    while ((*tree).type != NULL_TYPE) {
        printToken(eval((*tree).c.car, globalFrame));
        tree = (*tree).c.cdr;
    }
}

void printToken(Value *car){
    switch((*car).type){
        case INT_TYPE:
            printf("%i\n", (*car).i);
            break;
        case STR_TYPE:
            printf("%s\n", (*car).s);
            break;
        case DOUBLE_TYPE:
            printf("%f\n", (*car).d);
            break;
        case OPEN_TYPE:
            printf("(");
            break;
        case CLOSE_TYPE:
            printf(")");
            break;
        case BOOL_TYPE:
            if ((*car).i == 1) {
                printf("#t\n");
            }
            else {
                printf("#f\n");
            }
            break;
        case SYMBOL_TYPE:
            printf("%s\n", (*car).s);
            break;
        case VOID_TYPE:
            break;
        case CONS_TYPE:
            printSubtree(car); // deal with dotted pair case here
            printf("\n");
            break;
        case NULL_TYPE:
            printf("()\n");
            break;
        case CLOSURE_TYPE:
            printf("#<procedure>\n");
            break;
        default:
            break;
    }
}

/*helper function that takes in the name and value of a binding and appends it to the bindings list for a frame*/
Value *makeBinding(Value *name, Value *val, Value *list){
    Value *consCell = talloc(sizeof(Value));
    Value *subCell = talloc(sizeof(Value));
    consCell->type = CONS_TYPE;
    subCell->type = CONS_TYPE;
    consCell->c.car = subCell;
    consCell->c.cdr = list; //attach bindings
    subCell->c.car = name;
    subCell->c.cdr = val;
    list = consCell;
    return list;
}

Value *lookUpSymbol(Value *expr, Frame *frame){
    Frame *currFrame = frame;
    int isGlobal = 0;
    while(!isGlobal) {
        Value *bindings = (*currFrame).bindings;
        while((*bindings).type != NULL_TYPE) {
            if(strcmp( (*(*(*bindings).c.car).c.car).s, (*expr).s) == 0){
               return  (*(*bindings).c.car).c.cdr;
            }
            bindings = bindings->c.cdr;
        }

        if((*currFrame).parent == NULL){
            isGlobal = 1;
        }
        currFrame = (*currFrame).parent;
    }
    perror("Evaluation Error: Unrecognized symbol");
    texit(EXIT_FAILURE);
    return NULL;
}

 // Takes in a name and list of bindings. Iterates through bindings to see if
 // name already exists in bindings list
int isDuplicate(Value *name, Value *bindings){
    Value *tempBindings = bindings;
    while(tempBindings->type != NULL_TYPE){
        if(!strcmp(tempBindings->c.car->c.car->s, name->s)){
            return 1;
        }
        tempBindings = cdr(tempBindings);
    }
    return 0;
}

//Adding bindings and evaluating body
Value *evalLet(Value *args, Frame *frame){
    Frame *letFrame = talloc(sizeof(Frame));
    letFrame->parent = frame;
    letFrame->bindings = makeNull();
    //make sure theres only 2 items in the args list
    if(length(args) >= 2){
        Value *argsList = args->c.car;
        // check to make sure let arguments are a list (and not a nested list)
        while(argsList->type != NULL_TYPE){
            if(argsList->type != CONS_TYPE || argsList->c.car->type != CONS_TYPE ||
                (argsList->c.car->type == CONS_TYPE && argsList->c.car->c.car->type == CONS_TYPE)){
                perror("Evaluation Error: incorrect syntax, arguments for expression 'let' must be in a list");
                texit(EXIT_FAILURE);
                return NULL;
            }
            //call helper function that creates a binding and adds to binding list
            if(isDuplicate(argsList->c.car->c.car, letFrame->bindings)){
                perror("Evaluation Error: duplicate identifier given to expression 'let'");
                texit(EXIT_FAILURE);
                return NULL;
            } else if(argsList->c.car->c.car->type != SYMBOL_TYPE){
                    perror("Evaluation Error: Bad syntax in 'let' (not an identifier)");
                    texit(EXIT_FAILURE);
                    return NULL;
            }
             else {
                letFrame->bindings = makeBinding(argsList->c.car->c.car,
                                              eval(argsList->c.car->c.cdr->c.car, frame),letFrame->bindings);
            }

            argsList = cdr(argsList);
        }
        //eval on the body
        Value *tempArgs = args;
        while(tempArgs->c.cdr->type != NULL_TYPE){
            eval(tempArgs->c.cdr->c.car, letFrame);
            tempArgs = tempArgs->c.cdr;
        }
        return eval(tempArgs->c.car, letFrame);


    }else{ //is this an error or should we find last and evaluate the last thing
        perror("Evaluation Error: incorrect number of arguments for 'let'");
        texit(EXIT_FAILURE);
        return NULL;
    }
}

//let* is similar to let except that the expressions expr ... are evaluated in
//sequence from left to right, and each of these expressions is within the scope
//of the variables to the left
Value *evalLetStar(Value *args, Frame *frame) {
    Frame *parentFrame = frame;
    Frame *letFrame = talloc(sizeof(Frame));
    letFrame->parent = parentFrame;
    letFrame->bindings = makeNull();
    //make sure theres only 2 items in the args list
    if(length(args) >= 2){
        Value *argsList = args->c.car;
        // check to make sure let arguments are a list (and not a nested list)

        while(argsList->type != NULL_TYPE){
            if(argsList->type != CONS_TYPE || argsList->c.car->type != CONS_TYPE ||
                (argsList->c.car->type == CONS_TYPE && argsList->c.car->c.car->type == CONS_TYPE)){
                perror("Evaluation Error: incorrect syntax, arguments for expression 'let' must be in a list");
                texit(EXIT_FAILURE);
                return NULL;
            }
            if(argsList->c.car->c.car->type != SYMBOL_TYPE){
                    perror("Evaluation Error: Bad syntax in 'let' (not an identifier)");
                    texit(EXIT_FAILURE);
                    return NULL;
            }
             else {
                letFrame->bindings = makeBinding(argsList->c.car->c.car,
                                              eval(argsList->c.car->c.cdr->c.car, parentFrame),letFrame->bindings);
            }
            //go on to the next arg and create a nested frame for this arg, assign parentFrame to be the previous frame
            argsList = cdr(argsList);
            Frame *tempFrame = talloc(sizeof(Frame));
            tempFrame->parent = letFrame;
            tempFrame->bindings = makeNull();
            parentFrame = letFrame;
            letFrame = tempFrame;
        }
        //eval on the body
        Value *tempArgs = args;
        while(tempArgs->c.cdr->type != NULL_TYPE){
            eval(tempArgs->c.cdr->c.car, parentFrame);
            tempArgs = tempArgs->c.cdr;
        }
        return eval(tempArgs->c.car, parentFrame);


    }else{ //is this an error or should we find last and evaluate the last thing
        perror("Evaluation Error: incorrect number of arguments for 'let'");
        texit(EXIT_FAILURE);
        return NULL;
    }
}

Value *evalLetrec(Value *args, Frame *frame) {

    Frame *letFrame = talloc(sizeof(Frame));
    letFrame->parent = frame;
    letFrame->bindings = makeNull();
    //make sure theres no less than 2 items in the args list
    if(length(args) >= 2){
        //initialize bindings to all be nulltype values
        Value *argsList = args->c.car;
        // check to make sure let arguments are a list (and not a nested list)
        while(argsList->type != NULL_TYPE){
            if(argsList->type != CONS_TYPE || argsList->c.car->type != CONS_TYPE ||
                (argsList->c.car->type == CONS_TYPE && argsList->c.car->c.car->type == CONS_TYPE)){
                perror("Evaluation Error: incorrect syntax, arguments for expression 'letrec' must be in a list");
                texit(EXIT_FAILURE);
                return NULL;
            }
            if(isDuplicate(argsList->c.car->c.car, letFrame->bindings)){
                perror("Evaluation Error: duplicate identifier given to expression 'letrec'");
                texit(EXIT_FAILURE);
                return NULL;
            } else if(argsList->c.car->c.car->type != SYMBOL_TYPE){
                    perror("Evaluation Error: Bad syntax in 'letrec' (not an identifier)");
                    texit(EXIT_FAILURE);
                    return NULL;
            }
             else {
                letFrame->bindings = makeBinding(argsList->c.car->c.car,
                                              makeNull(),letFrame->bindings);

            }

            argsList = cdr(argsList);
        }
        letFrame->bindings = reverse(letFrame->bindings);
        //bind variables to their actual values
        argsList = args->c.car;
        Value *tempBindings = letFrame->bindings;
        while(argsList->type != NULL_TYPE){
            tempBindings->c.car->c.cdr = eval(argsList->c.car->c.cdr->c.car, letFrame);
            argsList = cdr(argsList);
            tempBindings = cdr(tempBindings);
        }
        //eval on the body
        Value *tempArgs = cdr(args);
        while(tempArgs->c.cdr->type != NULL_TYPE){
            eval(tempArgs->c.car, letFrame);
            tempArgs = tempArgs->c.cdr;
        }
        return eval(tempArgs->c.car, letFrame);


    }else{ //is this an error or should we find last and evaluate the last thing
        perror("Evaluation Error: incorrect number of arguments for 'letrec'");
        texit(EXIT_FAILURE);
        return NULL;
    }
}

Value *evalAnd(Value *args, Frame *frame){
    Value *tempArgs = args;
    Value *isTrue = talloc(sizeof(Value));
    isTrue->type = BOOL_TYPE;
    isTrue->i = 1;
    while(tempArgs->type != NULL_TYPE){
        Value *evaledCarTempArgs = eval(car(tempArgs), frame);
        if(evaledCarTempArgs->type != BOOL_TYPE){
            perror("Evaluation Error: cannot evaluate non boolean types for 'and'");
            texit(EXIT_FAILURE);
            return NULL;
        }else{
            if(evaledCarTempArgs->i == 0){
                isTrue->i = 0;
                return isTrue;
            }
        }
        tempArgs = cdr(tempArgs);
    }
    return isTrue;
}

Value *evalOr(Value *args, Frame *frame){
    Value *tempArgs = args;
    Value *isTrue = talloc(sizeof(Value));
    isTrue->type = BOOL_TYPE;
    isTrue->i = 0;
    while(tempArgs->type != NULL_TYPE){
        Value *evaledCarTempArgs = eval(car(tempArgs), frame);
        if(evaledCarTempArgs->type != BOOL_TYPE){
            perror("Evaluation Error: cannot evaluate non boolean types for 'or'");
            texit(EXIT_FAILURE);
            return NULL;
        }else{
            if(evaledCarTempArgs->i == 1){
                isTrue->i = 1;
                return isTrue;
            }
        }
        tempArgs = cdr(tempArgs);
    }
    return isTrue;
}

Value *evalQuote(Value *args, Frame *frame){
    if(length(args) == 1){
        return args->c.car;
    }else{
        perror("Evaluation Error: incorrect number of arguments for 'quote'");
        texit(EXIT_FAILURE);
        return NULL;
    }
}

Value *evalCond(Value *args, Frame *frame){
    Value *voidVal = talloc(sizeof(Value));
    voidVal->type = VOID_TYPE;
    if(length(args) == 0){
        return voidVal;
    }
    Value *tempArgs = args;

    while(tempArgs->type != NULL_TYPE){
        Value *currClause = car(tempArgs);
        Value *evaledCurrClauseCar = eval(car(currClause), frame);
        if(length(currClause) != 2){
            perror("Evaluation Error: incorrect number of arguments in condition of 'cond'");
            texit(EXIT_FAILURE);
            return NULL;
        }

        if(evaledCurrClauseCar->type != BOOL_TYPE){
            perror("Evaluation Error: cannot evaluate non boolean types in condition of 'cond'");
            texit(EXIT_FAILURE);
            return NULL;
        }else{
            if(evaledCurrClauseCar->i == 1){
                return eval(car(cdr(currClause)), frame);
            }else{
                tempArgs = cdr(tempArgs);
            }
        }
        if(tempArgs->type == NULL_TYPE){ // at last item and no else case was found
            return voidVal;
        }
        else if(cdr(tempArgs)->type == NULL_TYPE){ // current "tempArgs" is the last clause
            currClause = car(tempArgs);
            if(length(currClause) != 2){
                perror("Evaluation Error: incorrect number of arguments in condition of 'cond'");
                texit(EXIT_FAILURE);
                return NULL;
            }
            if(car(currClause)->type == SYMBOL_TYPE && !(strcmp(car(currClause)->s, "else")) ){ // currClause is else
                return eval(car(cdr(currClause)), frame);
            }
        }
    }
    return voidVal;
}

Value *evalSetBang(Value *args, Frame *frame){
    Value *voidVal = talloc(sizeof(Value));
    voidVal->type = VOID_TYPE;
    if(length(args) != 2){
        perror("Evaluation Error: incorrect number of arguments for 'set!'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    Value *firstArg = car(args);
    Value *secondArg = car(cdr(args));

    if(firstArg->type != SYMBOL_TYPE){
        perror("Evaluation Error: first argument for 'set!' must be a symbol");
        texit(EXIT_FAILURE);
        return NULL;
    }
    Frame *tempFrame = frame;
    while(tempFrame != NULL){
        Value *bindings = tempFrame->bindings;
        while(bindings->type != NULL_TYPE){
            char *variableName = car(car(bindings))->s;
            if(!strcmp(variableName, firstArg->s)){
                bindings->c.car->c.cdr = eval(secondArg, frame);
                return voidVal;
            }
            bindings = cdr(bindings);
        }
        tempFrame = tempFrame->parent;
    }

    perror("Evaluation Error: cannot 'set!' variable before its definition");
    texit(EXIT_FAILURE);
    return NULL;
}

Value *evalIf(Value *args, Frame *frame){

    if (length(args) == 3) { // check if 'if' expression is given 3 args
        Value *evaledCarArgs = eval(args->c.car, frame);
        if(evaledCarArgs->type == BOOL_TYPE){ // check if condition given to 'if' evaluates to boolean
            if (evaledCarArgs->i) {
                return eval(args->c.cdr->c.car, frame);
            }
            else {
                return eval(args->c.cdr->c.cdr->c.car, frame);
            }
        }
        else{
            perror("Evaluation Error: argument for 'if' does not evaluate to type 'boolean'");
            texit(EXIT_FAILURE);
            return NULL;
        }
    }
    else {
        perror("Evaluation Error: incorrect number of arguments for 'if'");
        texit(EXIT_FAILURE);
        return NULL;
    }
}

Value *evalWhen(Value *args, Frame *frame){
    if (length(args) >= 2) { // check if 'when' expression is given at least 2 args
        if(eval(args->c.car, frame)->type == BOOL_TYPE){ // check if condition given to 'when' evaluates to boolean
            if (eval(args->c.car, frame)->i) {
                Value *tempArgs = args->c.cdr;
                while(tempArgs->c.cdr->type != NULL_TYPE){
                    eval(tempArgs->c.cdr->c.car, frame);
                    tempArgs = tempArgs->c.cdr;
                }
                return eval(tempArgs->c.car, frame);
            }
            Value *voidVal = talloc(sizeof(Value));
            voidVal->type = VOID_TYPE;
            return voidVal;
        }
        else{
            perror("Evaluation Error: argument for 'when' does not evaluate to type 'boolean'");
            texit(EXIT_FAILURE);
            return NULL;
        }
    }
    else {
        perror("Evaluation Error: incorrect number of arguments for 'when'");
        texit(EXIT_FAILURE);
        return NULL;
    }
}

Value *evalUnless(Value *args, Frame *frame){
    if (length(args) >= 2) { // check if 'unless' expression is given at least 2 args
        if(eval(args->c.car, frame)->type == BOOL_TYPE){ // check if condition given to 'unless' evaluates to boolean
            if (!eval(args->c.car, frame)->i) {
                Value *tempArgs = args->c.cdr;
                while(tempArgs->c.cdr->type != NULL_TYPE){
                    eval(tempArgs->c.cdr->c.car, frame);
                    tempArgs = tempArgs->c.cdr;
                }
                return eval(tempArgs->c.car, frame);
            }
            Value *voidVal = talloc(sizeof(Value));
            voidVal->type = VOID_TYPE;
            return voidVal;
        }
        else{
            perror("Evaluation Error: argument for 'unless' does not evaluate to type 'boolean'");
            texit(EXIT_FAILURE);
            return NULL;
        }
    }
    else {
        perror("Evaluation Error: incorrect number of arguments for 'unless'");
        texit(EXIT_FAILURE);
        return NULL;
    }
}

//We are assuming that lambda only takes a single body, as specified in the define/lambda assignment, section 1.2
Value *evalLambda(Value *args, Frame *frame){

    if(length(args) == 2){ //parameter and body
        if (car(args)->type == CONS_TYPE || car(args)->type == NULL_TYPE) {
            Value *closure = talloc(sizeof(Value));
            closure->type = CLOSURE_TYPE;
            closure->cl.paramNames = makeNull();
            closure->cl.functionCode = args->c.cdr->c.car; //body
            closure->cl.frame = frame;
            closure->cl.hasIndefiniteParams = 0;
            Value *temp = args->c.car;
            Value *tempParams = makeNull();
            while(temp->type != NULL_TYPE){
                Value *param = car(temp);
                tempParams = cons(param, tempParams);
                temp = cdr(temp);
            }
            closure->cl.paramNames = reverse(tempParams);
            return closure;
        }
        else if (car(args)->type == SYMBOL_TYPE) {
            Value *closure = talloc(sizeof(Value));
            closure->type = CLOSURE_TYPE;
            closure->cl.paramNames = makeNull();
            closure->cl.paramNames = cons(car(args), closure->cl.paramNames);
            closure->cl.functionCode = args->c.cdr->c.car; //body
            closure->cl.frame = frame;
            closure->cl.hasIndefiniteParams = 1;
            return closure;
        }
        else {
            perror("Evaluation error: invalid parameter declaration for 'lambda'");
            texit(EXIT_FAILURE);
            return NULL;
        }
    }else{
        perror("Evaluation error: incorrect number of arguments for 'lambda'");
        texit(EXIT_FAILURE);
        return NULL;
    }
}

Value *evalDefine(Value *args, Frame *frame){
    if (length(args) == 2){ //var and expr
        if(args->c.car->type == CONS_TYPE){
            Value *closure = talloc(sizeof(Value));
            closure->type = CLOSURE_TYPE;
            closure->cl.paramNames = makeNull();
            closure->cl.functionCode = args->c.cdr->c.car; //body
            closure->cl.frame = frame;
            closure->cl.hasIndefiniteParams = 0;
            Value *temp = args->c.car;
            temp = cdr(temp);
            Value *tempParams = makeNull();
            while(temp->type != NULL_TYPE){
                Value *param = car(temp);
                tempParams = cons(param, tempParams);
                temp = cdr(temp);
            }
            closure->cl.paramNames = reverse(tempParams);
            if(car(args->c.car)->type != SYMBOL_TYPE){
                perror("Evaluation Error: Function name is not of type SYMBOL_TYPE");
                texit(EXIT_FAILURE);
                return NULL;
            }
            frame->bindings = makeBinding(car(args->c.car), closure, frame->bindings);
            Value *voidVal = talloc(sizeof(Value));
            voidVal->type = VOID_TYPE;
            return voidVal;

        }else if(args->c.car->type == SYMBOL_TYPE){
            frame->bindings = makeBinding(args->c.car,
                                        eval(car(args->c.cdr), frame), frame->bindings);
            Value *voidVal = talloc(sizeof(Value));
            voidVal->type = VOID_TYPE;
            return voidVal;
        }else{
            perror("Evaluation Error: Incorrect syntax in function definition");
            texit(EXIT_FAILURE);
            return NULL;
        }
    }else{
        perror("Evaluation Error: incorrect number of arguments for 'define'");
        texit(EXIT_FAILURE);
        return NULL;
    }
}

Value *evalDisplay(Value *args, Frame *frame){

    if (length(args) == 1) { // check if 'display' expression is given 1 arg
        Value *voidVal = talloc(sizeof(Value));
        voidVal->type = VOID_TYPE;
        if( args->c.car->type == STR_TYPE){
            int i = 1;
            char *str = args->c.car->s;
            while(str[i] != '"'){
                printf("%c", str[i]);
                i++;
            }
            printf("\n");
        }else{
            Value *evaledArgs = eval(args->c.car, frame);
            printToken(evaledArgs);
        }
        return voidVal;
    }
    else {
        perror("Evaluation Error: incorrect number of arguments for 'display'");
        texit(EXIT_FAILURE);
        return NULL;
    }
}

Value *evalBegin(Value *args, Frame *frame) {

    Value *tempArgs = args;
    //if there are no args, return a void type Value
    if (tempArgs->type == NULL_TYPE) {
        Value *voidVal = talloc(sizeof(Value));
        voidVal->type = VOID_TYPE;
        return voidVal;
    }
    //if one arg, return the result of evalutaing it
    else if (tempArgs->c.cdr->type == NULL_TYPE) {
        return eval(tempArgs->c.car, frame);
    }
    else {
        //if more than one arg, evaluate all and return the last one
        while(tempArgs->c.cdr->type != NULL_TYPE){
            eval(tempArgs->c.car, frame);
            tempArgs = tempArgs->c.cdr;
        }
        return eval(tempArgs->c.car, frame);
    }
}

Value *evalEach(Value *args, Frame *frame){
    Value *temp = args;
    Value *evaledArgsList = makeNull();
    while(temp->type != NULL_TYPE){
        evaledArgsList = cons(eval(car(temp), frame),evaledArgsList);
        temp = cdr(temp);
    }
    return reverse(evaledArgsList);
}

Value *apply(Value *function, Value *args){
    Frame *applyFrame = talloc(sizeof(Frame));
    applyFrame->parent = function->cl.frame;
    applyFrame->bindings = makeNull();
    // we assume only closure type functions can be applied using this function
    // in theory, primitives could be applied, in function such as map and foldl,
    // but apply would not be able to deal with them as is written
    if (function->type != CLOSURE_TYPE) {
        perror("Evaluation error: expected user defined function");
        texit(EXIT_FAILURE);
        return NULL;
    }
    if(length(args) != length(function->cl.paramNames) && !(function->cl.hasIndefiniteParams)){
        perror("Evaluation error: number of parameters given does not match function definition");
        texit(EXIT_FAILURE);
        return NULL;
    }
    Value *tempArgs = args; // values
    Value *tempParams = function->cl.paramNames; // names

    if (function->cl.hasIndefiniteParams) {
        Value *newTempArgs = makeNull();
        while (tempArgs->type != NULL_TYPE) {
            newTempArgs = cons(eval(car(tempArgs), applyFrame), newTempArgs);
            tempArgs = cdr(tempArgs);
        }
        tempArgs = newTempArgs;
        while(tempParams->type != NULL_TYPE){
            applyFrame->bindings = makeBinding(car(tempParams), tempArgs, applyFrame->bindings);
            if (tempArgs->type != NULL_TYPE) {
                tempArgs = cdr(tempArgs);
            }
            tempParams = cdr(tempParams);
        }
    }
    else {
        while(tempParams->type != NULL_TYPE){
            applyFrame->bindings = makeBinding(car(tempParams), car(tempArgs), applyFrame->bindings);
            tempArgs = cdr(tempArgs);
            tempParams = cdr(tempParams);
        }
    }
    return eval(function->cl.functionCode, applyFrame); //
}

void bind(char *name, Value *(*function)(struct Value *), Frame *frame) {
    // Add primitive functions to top-level bindings
    Value *nameVal = talloc(sizeof(Value));
    nameVal->type = STR_TYPE;
    nameVal->s = name;
    Value *value = talloc(sizeof(Value));
    value->type = PRIMITIVE_TYPE;
    value->pf = function;
    frame->bindings = makeBinding(nameVal,eval(value,frame),frame->bindings);
}

Value *primitiveAdd(Value *args){
    Value *sum = talloc(sizeof(Value));
    sum->type = DOUBLE_TYPE;
    sum->d = 0.0;
    Value *tempArgs = args;
    while(tempArgs->type != NULL_TYPE){
        if(tempArgs->c.car->type != INT_TYPE && tempArgs->c.car->type != DOUBLE_TYPE){
            perror("Evaluation Error: incorrect argument type given to function '+'");
            texit(EXIT_FAILURE);
            return NULL;
        }
        if(tempArgs->c.car->type == INT_TYPE){
            sum->d += tempArgs->c.car->i;
        }
        else if(tempArgs->c.car->type == DOUBLE_TYPE){
            sum->d += tempArgs->c.car->d;
        }
        tempArgs = cdr(tempArgs);
    }
    return sum;
}

Value *primitiveMultiply(Value *args){
    Value *prod = talloc(sizeof(Value));
    prod->type = DOUBLE_TYPE;
    prod->d = 1.0;
    Value *tempArgs = args;
    int doubleArg = 0;
    while(tempArgs->type != NULL_TYPE){
        if(tempArgs->c.car->type != INT_TYPE && tempArgs->c.car->type != DOUBLE_TYPE){
            perror("Evaluation Error: incorrect argument type given to function '*'");
            texit(EXIT_FAILURE);
            return NULL;
        }
        if(tempArgs->c.car->type == INT_TYPE){
            prod->d *= tempArgs->c.car->i;
        }
        else if(tempArgs->c.car->type == DOUBLE_TYPE){
            doubleArg = 1;
            prod->d *= tempArgs->c.car->d;
        }
        tempArgs = cdr(tempArgs);
    }
    if(!doubleArg){
        Value *prodInt = talloc(sizeof(Value));
        prodInt->type = INT_TYPE;
        prodInt->i = (int)prod->d;
        return prodInt;
    }
    return prod;
}

Value *primitiveSubtract(Value *args){
    Value *diff = talloc(sizeof(Value));
    diff->type = DOUBLE_TYPE;
    diff->d = 0.0;
    Value *tempArgs = args;
    int doubleArg = 0;
    //add the first arg to diff->d
    if(tempArgs->c.car->type != INT_TYPE && tempArgs->c.car->type != DOUBLE_TYPE){
        perror("Evaluation Error: incorrect argument type given to function '-'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    if(tempArgs->c.car->type == INT_TYPE){
        diff->d += tempArgs->c.car->i;
    }
    else if(tempArgs->c.car->type == DOUBLE_TYPE){
        doubleArg = 1;
        diff->d += tempArgs->c.car->d;
    }
    tempArgs = cdr(tempArgs);
    //subtract the rest of the args from diff->d
    while(tempArgs->type != NULL_TYPE){
        if(tempArgs->c.car->type != INT_TYPE && tempArgs->c.car->type != DOUBLE_TYPE){
            perror("Evaluation Error: incorrect argument type given to function '-'");
            texit(EXIT_FAILURE);
            return NULL;
        }
        if(tempArgs->c.car->type == INT_TYPE){
            diff->d -= tempArgs->c.car->i;
        }
        else if(tempArgs->c.car->type == DOUBLE_TYPE){
            doubleArg = 1;
            diff->d -= tempArgs->c.car->d;
        }
        tempArgs = cdr(tempArgs);
    }
    if(!doubleArg){
        Value *diffInt = talloc(sizeof(Value));
        diffInt->type = INT_TYPE;
        diffInt->i = (int)diff->d;
        return diffInt;
    }
    return diff;
}

Value *primitiveDivide(Value *args){
    if(length(args) != 2){
        perror("Evaluation Error: incorrect number of arguments for '/'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    Value *tempArgs = args;
    if((tempArgs->c.car->type != INT_TYPE && tempArgs->c.car->type != DOUBLE_TYPE)
        || (car(tempArgs->c.cdr)->type != INT_TYPE && car(tempArgs->c.cdr)->type != DOUBLE_TYPE)){
        perror("Evaluation Error: incorrect argument type given to function '/'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    Value *quotient = talloc(sizeof(Value));
    quotient->type = DOUBLE_TYPE;

    if(tempArgs->c.car->type == INT_TYPE){
        quotient->d = tempArgs->c.car->i;
    }
    else if(tempArgs->c.car->type == DOUBLE_TYPE){
        quotient->d = tempArgs->c.car->d;
    }

    if(car(tempArgs->c.cdr)->type == INT_TYPE){
        if(car(tempArgs->c.cdr)->i == 0){
            perror("Evaluation Error: divide by 0");
            texit(EXIT_FAILURE);
            return NULL;
        }
        quotient->d /= car(tempArgs->c.cdr)->i;
    }
    else if(car(tempArgs->c.cdr)->type == DOUBLE_TYPE){
        if(car(tempArgs->c.cdr)->d == 0.0){
            perror("Evaluation Error: divide by 0");
            texit(EXIT_FAILURE);
            return NULL;
        }
        quotient->d /= car(tempArgs->c.cdr)->d;
    }

    int tempInt = (int)quotient->d;

    if(quotient->d == (double)tempInt){
        Value *quotientInt = talloc(sizeof(Value));
        quotientInt->type = INT_TYPE;
        quotientInt->i = (int)quotient->d;
        return quotientInt;
    }
    return quotient;
}

Value *primitiveLessThan(Value *args){

    if(length(args) != 2){
        perror("Evaluation Error: incorrect number of arguments for '<'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    Value *tempArgs = args;
    if((tempArgs->c.car->type != INT_TYPE && tempArgs->c.car->type != DOUBLE_TYPE)
        || (car(tempArgs->c.cdr)->type != INT_TYPE && car(tempArgs->c.cdr)->type != DOUBLE_TYPE)){
        perror("Evaluation Error: incorrect argument type given to function '<'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    Value *isLess = talloc(sizeof(Value));
    isLess->type = BOOL_TYPE;
    isLess->i = 0;
    double firstArg;
    double secondArg;
    if(tempArgs->c.car->type == INT_TYPE){
        firstArg = tempArgs->c.car->i;
    }
    else if(tempArgs->c.car->type == DOUBLE_TYPE){
        firstArg = tempArgs->c.car->d;
    }

    if(car(tempArgs->c.cdr)->type == INT_TYPE){
        secondArg = car(tempArgs->c.cdr)->i;
    }
    else if(car(tempArgs->c.cdr)->type == DOUBLE_TYPE){
        secondArg = car(tempArgs->c.cdr)->d;
    }

    if(firstArg < secondArg){
        isLess->i = 1;
    }

    return isLess;

}

Value *primitiveGreaterThan(Value *args){
    if(length(args) != 2){
        perror("Evaluation Error: incorrect number of arguments for '>'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    Value *tempArgs = args;
    if((tempArgs->c.car->type != INT_TYPE && tempArgs->c.car->type != DOUBLE_TYPE)
        || (car(tempArgs->c.cdr)->type != INT_TYPE && car(tempArgs->c.cdr)->type != DOUBLE_TYPE)){
        perror("Evaluation Error: incorrect argument type given to function '>'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    Value *isGreater = talloc(sizeof(Value));
    isGreater->type = BOOL_TYPE;
    isGreater->i = 0;
    double firstArg;
    double secondArg;
    if(tempArgs->c.car->type == INT_TYPE){
        firstArg = tempArgs->c.car->i;
    }
    else if(tempArgs->c.car->type == DOUBLE_TYPE){
        firstArg = tempArgs->c.car->d;
    }

    if(car(tempArgs->c.cdr)->type == INT_TYPE){
        secondArg = car(tempArgs->c.cdr)->i;
    }
    else if(car(tempArgs->c.cdr)->type == DOUBLE_TYPE){
        secondArg = car(tempArgs->c.cdr)->d;
    }

    if(firstArg > secondArg){
        isGreater->i = 1;
    }

    return isGreater;

}

Value *primitiveEquals(Value *args){

    if(length(args) != 2){
        perror("Evaluation Error: incorrect number of arguments for '='");
        texit(EXIT_FAILURE);
        return NULL;
    }
    Value *tempArgs = args;
    if((tempArgs->c.car->type != INT_TYPE && tempArgs->c.car->type != DOUBLE_TYPE)
        || (car(tempArgs->c.cdr)->type != INT_TYPE && car(tempArgs->c.cdr)->type != DOUBLE_TYPE)){
        perror("Evaluation Error: incorrect argument type given to function '='");
        texit(EXIT_FAILURE);
        return NULL;
    }
    Value *isEqual = talloc(sizeof(Value));
    isEqual->type = BOOL_TYPE;
    isEqual->i = 0;
    double firstArg;
    double secondArg;
    if(tempArgs->c.car->type == INT_TYPE){
        firstArg = tempArgs->c.car->i;
    }
    else if(tempArgs->c.car->type == DOUBLE_TYPE){
        firstArg = tempArgs->c.car->d;
    }

    if(car(tempArgs->c.cdr)->type == INT_TYPE){
        secondArg = car(tempArgs->c.cdr)->i;
    }
    else if(car(tempArgs->c.cdr)->type == DOUBLE_TYPE){
        secondArg = car(tempArgs->c.cdr)->d;
    }

    if(firstArg == secondArg){
        isEqual->i = 1;
    }

    return isEqual;

}

Value *primitiveLessThanEqualTo(Value *args){
    if(length(args) != 2){
        perror("Evaluation Error: incorrect number of arguments for '>'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    Value *tempArgs = args;
    if((tempArgs->c.car->type != INT_TYPE && tempArgs->c.car->type != DOUBLE_TYPE)
        || (car(tempArgs->c.cdr)->type != INT_TYPE && car(tempArgs->c.cdr)->type != DOUBLE_TYPE)){
        perror("Evaluation Error: incorrect argument type given to function '>'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    Value *isLE = talloc(sizeof(Value));
    isLE->type = BOOL_TYPE;
    isLE->i = 0;
    double firstArg;
    double secondArg;
    if(tempArgs->c.car->type == INT_TYPE){
        firstArg = tempArgs->c.car->i;
    }
    else if(tempArgs->c.car->type == DOUBLE_TYPE){
        firstArg = tempArgs->c.car->d;
    }

    if(car(tempArgs->c.cdr)->type == INT_TYPE){
        secondArg = car(tempArgs->c.cdr)->i;
    }
    else if(car(tempArgs->c.cdr)->type == DOUBLE_TYPE){
        secondArg = car(tempArgs->c.cdr)->d;
    }

    if(firstArg <= secondArg){
        isLE->i = 1;
    }

    return isLE;

}

Value *primitiveGreaterThanEqualTo(Value *args){
    if(length(args) != 2){
        perror("Evaluation Error: incorrect number of arguments for '>'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    Value *tempArgs = args;
    if((tempArgs->c.car->type != INT_TYPE && tempArgs->c.car->type != DOUBLE_TYPE)
        || (car(tempArgs->c.cdr)->type != INT_TYPE && car(tempArgs->c.cdr)->type != DOUBLE_TYPE)){
        perror("Evaluation Error: incorrect argument type given to function '>'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    Value *isGE = talloc(sizeof(Value));
    isGE->type = BOOL_TYPE;
    isGE->i = 0;
    double firstArg;
    double secondArg;
    if(tempArgs->c.car->type == INT_TYPE){
        firstArg = tempArgs->c.car->i;
    }
    else if(tempArgs->c.car->type == DOUBLE_TYPE){
        firstArg = tempArgs->c.car->d;
    }

    if(car(tempArgs->c.cdr)->type == INT_TYPE){
        secondArg = car(tempArgs->c.cdr)->i;
    }
    else if(car(tempArgs->c.cdr)->type == DOUBLE_TYPE){
        secondArg = car(tempArgs->c.cdr)->d;
    }

    if(firstArg >= secondArg){
        isGE->i = 1;
    }

    return isGE;

}

// modulo may further assume that its arguments are integers.
// We will not test non-integer cases for modulo
Value *primitiveModulo(Value *args){
    if(length(args) != 2){
        perror("Evaluation Error: incorrect number of arguments for 'modulo'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    Value *tempArgs = args;
    if((tempArgs->c.car->type != INT_TYPE) || (car(tempArgs->c.cdr)->type != INT_TYPE)){
        perror("Evaluation Error: incorrect argument type given to function 'modulo'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    Value *remainder = talloc(sizeof(Value));
    remainder->type = INT_TYPE;

    remainder->i = tempArgs->c.car->i;

    if(car(tempArgs->c.cdr)->i == 0){
        perror("Evaluation Error: divide by 0");
        texit(EXIT_FAILURE);
        return NULL;
    }
    remainder->i %= car(tempArgs->c.cdr)->i;

    return remainder;
}

Value *primitiveNull(Value *args){
    Value *isNull = talloc(sizeof(Value));
    isNull->type = BOOL_TYPE;
    if(length(args) != 1){
        perror("Evaluation Error: Incorrect number of arguments for 'null?'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    isNull->i = (car(args)->type == NULL_TYPE);
    return isNull;
}

Value *primitiveCar(Value *args) {
    if(length(args) != 1){
        perror("Evaluation Error: Incorrect number of arguments for 'car'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    if (car(args)->type != CONS_TYPE) {
        perror("Evaluation Error: Expected pair as argument for 'car'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    return car(car(args));
}

Value *primitiveCdr(Value *args) {
    if(length(args) != 1){
        perror("Evaluation Error: Incorrect number of arguments for 'cdr'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    if (car(args)->type != CONS_TYPE) {
        perror("Evaluation Error: Expected pair as argument for 'cdr'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    return cdr(car(args));
}

Value *primitiveCons(Value *args) {
    if(length(args) != 2){
        perror("Evaluation Error: Incorrect number of arguments for 'cons'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    return cons(car(args), car(cdr(args)));
}

Value *primitiveList(Value *args){
    return args;
}

Value *primitiveAppend(Value *args) {
    if(args->type == NULL_TYPE){
        return args;
    }
    Value *newList = makeNull();
    Value *tempArgs = args;
    while(tempArgs->c.cdr->type != NULL_TYPE){

        if(car(tempArgs)->type != CONS_TYPE && car(tempArgs)->type != NULL_TYPE){
            perror("Evaluation Error: Expected list as non-terminal argument in 'append'");
            texit(EXIT_FAILURE);
            return NULL;
        }
        Value *newNewList = car(tempArgs);
        while(newNewList->type != NULL_TYPE){
            newList = cons(car(newNewList), newList);
            newNewList = cdr(newNewList);

            if(newNewList->type != CONS_TYPE && newNewList->type != NULL_TYPE){ // last item is dotted pair
                perror("Evaluation Error: Expected list as non-terminal argument in 'append'");
                texit(EXIT_FAILURE);
                return NULL;
            }
        }
        tempArgs = cdr(tempArgs);
    }
    if(car(tempArgs)->type == CONS_TYPE){ // if the last item is a list
        Value *newNewList = car(tempArgs);
        while(newNewList->type != NULL_TYPE){
            newList = cons(car(newNewList), newList);
            newNewList = cdr(newNewList); // just the int now

            if(newNewList->type != CONS_TYPE){ // last item is dotted pair
                Value *tempList = newList;
                newList = newNewList;
                while(tempList->type != NULL_TYPE){
                    newList = cons(car(tempList), newList);
                    tempList = cdr(tempList);
                }
                return newList;
            }

        }
        newList = reverse(newList);
    }
    else{
        Value *tempList = newList;
        newList = car(tempArgs);
        while(tempList->type != NULL_TYPE){
            newList = cons(car(tempList), newList);
            tempList = cdr(tempList);
        }
    }


    return newList;
}

Value *primitiveEq(Value *args){
    if(length(args) != 2){
        perror("Evaluation Error: Incorrect number of arguments for 'eq?'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    Value *firstArg = car(args);
    Value *secondArg = car(cdr(args));
    Value *boolean = talloc(sizeof(Value));
    boolean->type = BOOL_TYPE;
    if(firstArg == secondArg){ //addresses are the same
        boolean->i = 1;
    }else{
        boolean->i = 0;
    }
    return boolean;
}

// if not a number string or symbol -- it'll just return false
Value *primitiveEqual(Value *args){
    if(length(args) != 2){
        perror("Evaluation Error: Incorrect number of arguments for 'equal?'");
        texit(EXIT_FAILURE);
        return NULL;
    }
    Value *firstArg = car(args);
    Value *secondArg = car(cdr(args));
    Value *boolean = talloc(sizeof(Value));
    boolean->type = BOOL_TYPE;
    // doubles, ints, strings, symbols
    if(firstArg->type != secondArg->type){
        boolean->i = 0;
    }
    else if(firstArg->type == DOUBLE_TYPE){
        if(secondArg->d == firstArg->d){
            boolean->i = 1;
        }
        else{
            boolean->i = 0;
        }
    }
    else if(firstArg->type == INT_TYPE){
        if(secondArg->i == firstArg->i){
            boolean->i = 1;
        }
        else{
            boolean->i = 0;
        }
    }
    else if(firstArg->type == STR_TYPE || firstArg->type == SYMBOL_TYPE){
        if(!strcmp(secondArg->s, firstArg->s)){
            boolean->i = 1;
        }
        else{
            boolean->i = 0;
        }
    }
    else{

        boolean->i = 0;
    }

    return boolean;
}

Value *loadFromFile(Value *args){
    Value *voidVal = talloc(sizeof(Value));
    voidVal->type = VOID_TYPE;
    if(length(args) == 1){
        if (car(args)->type != STR_TYPE) {
            perror("Evaluation Error: filename passed to 'load' must be a string");
            texit(EXIT_FAILURE);
            return voidVal;
        }
        int strLen = 0;
        char *tempString = car(args)->s;
        while (tempString[strLen] != '\0') {
            strLen++;
        }
        //strLen = length of the filename - quotes + null terminating char
        char *filename = talloc(strLen * sizeof(char));
        tempString = car(args)->s;
        int i = 1;
        while(tempString[i] != '"'){
            filename[i-1] = tempString[i];
            i++;
        }
        filename[i-1] = '\0';
        freopen(filename, "r", stdin);
        Value *list = tokenize(stdin);
        Value *tree = parse(list);
        interpret(tree);
        return voidVal;
    }else{
        perror("Evaluation Error: incorrect number of arguments for 'load'");
        texit(EXIT_FAILURE);
        return voidVal;
    }
}


Value *eval(Value *expr, Frame *frame) {
    switch (expr->type)  {
        case INT_TYPE: {
            return expr;
            break;
        }
        case BOOL_TYPE: {
            return expr;
            break;
        }
        case DOUBLE_TYPE: {
            return expr;
            break;
        }
        case STR_TYPE: {
            return expr;
            break;
        }
        case SYMBOL_TYPE: {
            // look in frame and see if it's been bound to anything
           return lookUpSymbol(expr, frame);
           break;
        }
        case CONS_TYPE: {
            Value *first = car(expr);
            Value *args = cdr(expr);
            Value *result = talloc(sizeof(Value));
            if(first->type == INT_TYPE || first->type == DOUBLE_TYPE || first->type == BOOL_TYPE
                                        || first->type == NULL_TYPE  || first->type == STR_TYPE){
                perror("Evaluation Error: argument is not a procedure");
                texit(EXIT_FAILURE);
                return NULL;
            }else{
                if (!strcmp(first->s,"if")) {
                    result = evalIf(args,frame);
                }
                else if(!strcmp(first->s,"let")){
                    result = evalLet(args, frame);
                }
                else if(!strcmp(first->s,"let*")){
                    result = evalLetStar(args, frame);
                }
                else if(!strcmp(first->s,"letrec")){
                    result = evalLetrec(args, frame);
                }
                else if(!strcmp(first->s,"display")){
                    result = evalDisplay(args, frame);
                }
                else if(!strcmp(first->s,"when")){
                    result = evalWhen(args, frame);
                }
                else if(!strcmp(first->s,"unless")){
                    result = evalUnless(args, frame);
                }
                else if(!strcmp(first->s, "quote")){
                    result = evalQuote(args, frame);
                }
                else if(!strcmp(first->s, "define")){
                    result = evalDefine(args, frame);
                }
                else if(!strcmp(first->s, "lambda")){
                    result = evalLambda(args, frame);
                }
                else if(!strcmp(first->s, "begin")){
                    result = evalBegin(args, frame);
                }
                else if(!strcmp(first->s, "and")){
                    result = evalAnd(args, frame);
                }
                else if(!strcmp(first->s, "or")){
                    result = evalOr(args, frame);
                }
                else if(!strcmp(first->s, "cond")){
                    result = evalCond(args, frame);
                }
                else if(!strcmp(first->s, "set!")){
                    result = evalSetBang(args, frame);
                }
                else if(!strcmp(first->s, "+")){
                    Value *evaledArgs = evalEach(args, frame);
                    result = primitiveAdd(evaledArgs);
                }
                else if(!strcmp(first->s, "null?")){
                    Value *evaledArgs = evalEach(args, frame);
                    result = primitiveNull(evaledArgs);
                }
                else if(!strcmp(first->s, "car")){
                    Value *evaledArgs = evalEach(args, frame);
                    result = primitiveCar(evaledArgs);
                }
                else if(!strcmp(first->s, "cdr")){
                    Value *evaledArgs = evalEach(args, frame);
                    result = primitiveCdr(evaledArgs);
                }
                else if(!strcmp(first->s, "cons")){
                    Value *evaledArgs = evalEach(args, frame);
                    result = primitiveCons(evaledArgs);
                }
                else if(!strcmp(first->s, "list")){
                    Value *evaledArgs = evalEach(args, frame);
                    result = primitiveList(evaledArgs);
                }
                else if(!strcmp(first->s, "append")){
                    Value *evaledArgs = evalEach(args, frame);
                    result = primitiveAppend(evaledArgs);
                }
                else if(!strcmp(first->s, "eq?")){
                    Value *evaledArgs = evalEach(args, frame);
                    result = primitiveEq(evaledArgs);
                }
                else if(!strcmp(first->s, "equal?")){
                    Value *evaledArgs = evalEach(args, frame);
                    result = primitiveEqual(evaledArgs);
                }
                else if(!strcmp(first->s, "*")){
                    Value *evaledArgs = evalEach(args, frame);
                    result = primitiveMultiply(evaledArgs);
                }
                else if(!strcmp(first->s, "-")){
                    Value *evaledArgs = evalEach(args, frame);
                    result = primitiveSubtract(evaledArgs);
                }
                else if(!strcmp(first->s, "/")){
                    Value *evaledArgs = evalEach(args, frame);
                    result = primitiveDivide(evaledArgs);
                }
                else if(!strcmp(first->s, "modulo")){
                    Value *evaledArgs = evalEach(args, frame);
                    result = primitiveModulo(evaledArgs);
                }
                else if(!strcmp(first->s, "<")){
                    Value *evaledArgs = evalEach(args, frame);
                    result = primitiveLessThan(evaledArgs);
                }
                else if(!strcmp(first->s, ">")){
                    Value *evaledArgs = evalEach(args, frame);
                    result = primitiveGreaterThan(evaledArgs);
                }
                else if(!strcmp(first->s, "=")){
                    Value *evaledArgs = evalEach(args, frame);
                    result = primitiveEquals(evaledArgs);
                }
                else if(!strcmp(first->s, "<=")){
                    Value *evaledArgs = evalEach(args, frame);
                    result = primitiveLessThanEqualTo(evaledArgs);
                }
                else if(!strcmp(first->s, ">=")){
                    Value *evaledArgs = evalEach(args, frame);
                    result = primitiveGreaterThanEqualTo(evaledArgs);
                }
                else if(!strcmp(first->s, "load")){
                    result = loadFromFile(args);
                }

                else {
                   // not a recognized special form
                   Value *evaledOperator = eval(first,frame);
                   Value *evaledArgs = evalEach(args,frame);
                   return apply(evaledOperator, evaledArgs);
               }
            return result;
            break;
        }
        }
        default: {
            Value *nullVal = talloc(sizeof(Value));
            nullVal->type = NULL_TYPE;
            return nullVal;
            break;
        }
    }
}
