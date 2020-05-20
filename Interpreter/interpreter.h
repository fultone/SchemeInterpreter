#include "value.h"
#ifndef _INTERPRETER
#define _INTERPRETER

struct Frame {
    Value *bindings;
    struct Frame *parent;
};

typedef struct Frame Frame;

void interpret(Value *tree);

void printToken(Value *car);

Value *makeBinding(Value *name, Value *val, Value *list);

Value *lookUpSymbol(Value *expr, Frame *frame);

int isDuplicate(Value *name, Value *bindings);

Value *evalLet(Value *args, Frame *frame);

Value *evalLetStar(Value *args, Frame *frame);

Value *evalLetrec(Value *args, Frame *frame);

Value *evalAnd(Value *args, Frame *frame);

Value *evalOr(Value *args, Frame *frame);

Value *evalQuote(Value *args, Frame *frame);

Value *evalCond(Value *args, Frame *frame);

Value *evalSetBang(Value *args, Frame *frame);

Value *evalIf(Value *args, Frame *frame);

Value *evalWhen(Value *args, Frame *frame);

Value *evalUnless(Value *args, Frame *frame);

Value *evalLambda(Value *args, Frame *frame);

Value *evalDefine(Value *args, Frame *frame);

Value *evalDisplay(Value *args, Frame *frame);

Value *evalBegin(Value *args, Frame *frame);

Value *evalEach(Value *args, Frame *frame);

Value *apply(Value *function, Value *args);

void bind(char *name, Value *(*function)(struct Value *), Frame *frame);

Value *primitiveAdd(Value *args);

Value *primitiveMultiply(Value *args);

Value *primitiveSubtract(Value *args);

Value *primitiveLessThan(Value *args);

Value *primitiveGreaterThan(Value *args);

Value *primitiveEquals(Value *args);

Value *primitiveLessThanEqualTo(Value *args);

Value *primitiveGreaterThanEqualTo(Value *args);

Value *primitiveDivide(Value *args);

Value *primitiveModulo(Value *args);

Value *primitiveNull(Value *args);

Value *primitiveCar(Value *args);

Value *primitiveCdr(Value *args);

Value *primitiveCons(Value *args);

Value *primitiveList(Value *args);

Value *primitiveAppend(Value *args);

Value *primitiveEq(Value *args);

Value *primitiveEqual(Value *args);

Value *eval(Value *expr, Frame *frame);

#endif
