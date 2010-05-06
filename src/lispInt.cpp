#include "lispInt.h"
  
TLispInt::TLispInt(char * string) {
  source = strdup(string);
  char * c = source;
  head = NULL;
  interpret(c,&head);
};
  
TLispInt::~TLispInt() {
  if (source) free(source);
  if (head) delete head;
//  if (bsource) free(bsource);
};

bool TLispInt::interpret(char * &str, TTreeElem ** place) {

#ifdef VERBOSE_LISP_READING
  printf("%s\n",str);
#endif

  while (isspace(*str)) str++;

  switch (*str) {
    case '(' : {
#ifdef VERBOSE_LISP_READING
      printf("Level down\n");
#endif
      str++;
      *place = new TTreeElem; //(TTreeElem*)calloc(1,sizeof(TTreeElem));
      (*place)->type = Operation;
      getname(str,(*place)->data.name);
#ifdef VERBOSE_LISP_READING
      printf("Opr: %s\n",(*place)->data.name);
#endif
      short i = 0;
      while (interpret(str,(*place)->operands + i++)) {};
      return true;
    }
    case ')' : {
#ifdef VERBOSE_LISP_READING
      printf("Level up\n");
#endif
      str++;
      return false;
    }
    case '\0' : return false;
    case 'x' :
    case 'y' :
    case 'z' : {
      *place = new TTreeElem();
      (*place)->type = Var;
      getname(str,(*place)->data.name);
#ifdef VERBOSE_LISP_READING
      printf("Var: %s\n",(*place)->data.name);
#endif
      return true;
    }
    default : {
      if (isdigit(*str) || (*str)=='-') {
        (*place) = new TTreeElem;
        (*place)->type = Constant;
        (*place)->data.d = strtod(str,&str);
#ifdef VERBOSE_LISP_READING
        printf("Num: %g\n",(*place)->data.d);
#endif
//        str++;
        return true;
      }
      else {
        fprintf(stderr,"Unknown format at position %s",str);
        return false;
      }
    }
  }
}

void getname( char * &str, char * dest )
{
#ifdef VERBOSE_LISP_GETNAME
  char * d = dest;
  printf("getname:\nSource : %s\n",str);
#endif
  while (isspace(*str)) str++;
  while (!isspace(*str) && (*str)!='(' && (*str)!=')')
    *dest++ = *(str++);
  *dest = '\0';
#ifdef VERBOSE_LISP_GETNAME
  printf("Result : %s\n",d);
#endif
}

double TLispInt::evaluate( double x, double y, double z )
{
  return evaluate(head,x,y,z);
}

double TLispInt::evaluate( TTreeElem * start, double x, double y, double z )
{
  if (!start) throw nvberr_invalid_input;
  switch (start->type) {
    case (Constant) : {
#ifdef VERBOSE_LISP_INTERPRETATION
      printf("Num: \'%g\'\n",start->data.d);
#endif
      return start->data.d;
      }
    case (Operation) : {    
#ifdef VERBOSE_LISP_INTERPRETATION
      printf("Opr: \'%s\'\n",start->data.name);
#endif
      switch (start->data.name[0]) {
        case ('+') : { // x1+x2+x3+...
          double result = 0;
          short i = -1;
          while (start->operands[++i])
            result += evaluate(start->operands[i],x,y,z);
          if (i<2) throw nvberr_invalid_input;
          return result;
          }
        case ('-') : { // x1-x2 | -x1
          if (start->operands[1])
            return evaluate(start->operands[0],x,y,z)-evaluate(start->operands[1],x,y,z);
          else
            return -evaluate(start->operands[0],x,y,z);
          }
        case ('*') : { // x1*x2*x3*...
          double result = 1;
          short i = -1;
          while (start->operands[++i])
            result *= evaluate(start->operands[i],x,y,z);
          if (i<2) throw nvberr_invalid_input;
          return result;
          }
        case ('/') : { // x1/x2
          return evaluate(start->operands[0],x,y,z)/evaluate(start->operands[1],x,y,z);
          }
        case ('%') : { // x1%x2
          return (((int)evaluate(start->operands[0],x,y,z))%((int)evaluate(start->operands[1],x,y,z)));
          }
        case ('c') : { // cos /1
          return cos(evaluate(start->operands[0],x,y,z));
          }
        case ('s') : { // sin /1
          return sin(evaluate(start->operands[0],x,y,z));
          }
        case ('e') : { // exp /1
          return exp(evaluate(start->operands[0],x,y,z));
          }
        case ('a') : { // abs /1
          return fabs(evaluate(start->operands[0],x,y,z));
          }
        default : {
          throw nvberr_unexpected_value;
          }
        }
      }
    case (Var) : {
#ifdef VERBOSE_LISP_INTERPRETATION
      printf("Var: \'%s\'\n",start->data.name);
#endif
      switch (start->data.name[0]) {
        case ('x') : {
          return x;
          }
        case ('y') : {
          return y;
          }
        case ('z') : {
          return z;
          }
        default : {
          throw nvberr_unexpected_value;
          }
        }
      }
/*    
    case () : {
      }
*/
    default : {
#ifdef VERBOSE_LISP_INTERPRETATION
      printf("Unk: %s\n",start->data.name);
#endif
      throw nvberr_unexpected_value;
      }
    }
}
