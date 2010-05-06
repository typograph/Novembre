#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "NVBLogger.h"

#ifndef MYSIMPLESCHEMAINT_H
#define MYSIMPLESCHEMAINT_H

using namespace NVBErrorCodes;

enum TTreeElemType { Constant, Operation, Var };

struct TTreeElem {
  TTreeElem() { memset(operands,0,10*sizeof(void*)) ;}
  ~TTreeElem() {
    for(int i = 0; i < 10; i++)
      if (operands[i]) delete operands[i];
    }
  TTreeElemType type;
  union {
    double d;
    char name[5];
  } data;
  TTreeElem * operands[10];
};

class TLispInt {
private:
  TTreeElem * head;
  char * source;
//  char * bsource; // correctly formatted source

  bool interpret(char * &str, TTreeElem ** place);
  double evaluate(TTreeElem * start, double x, double y, double z);
public:
  TLispInt(char * string);
  ~TLispInt();
  double evaluate(double x=0, double y=0, double z=0);
};

void getname(char * &str, char * dest);

#endif
