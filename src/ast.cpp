#include "ast.hpp"

SExp* makeSExp(std::string val, SExpType type) {
  SExp* sexp = new SExp;
  char* buffer = new char[val.length()+1];
  val.copy(buffer, val.size());
  buffer[val.size()] = '\0';
  sexp->type = type;
  first(sexp) = NULL;
  rest(sexp)  = NULL;
  val(sexp)   = buffer;
  return sexp;
}

SExp* cons(SExp* first, SExp* rest) {
  SExp* sexp  = new SExp;
  sexp->type  = LIST;
  first(sexp) = first;
  rest(sexp)  = rest;
  return sexp;
}

void push(SExp* list, SExp* obj) {
  SExp* p = list;
  p->type = LIST;
  while(rest(p) != NULL) {
    p = rest(p);
  }
  rest(p) = obj;
}

std::string print(SExp* sexp) {
  if(sexp) {
    if(listp(sexp)) {
      std::string out = "(";
      SExp* p = sexp;
      do {
        if(p != sexp)
          out += ' ';
        out += print(first(p));
        p = rest(p);
      } while(p != NULL);
      return out += ')';
    } else {
      return std::string(val(sexp));
    }
  } else
    return std::string("()");
}

void freeSExp(SExp* sexp) {
  if(sexp) {
    if(listp(sexp)) {
      /* Recur */
      if(first(sexp))
        freeSExp(first(sexp));
      if(rest(sexp))
        freeSExp(rest(sexp));
    } else {
      delete val(sexp);
    }
    delete sexp;
  }
}