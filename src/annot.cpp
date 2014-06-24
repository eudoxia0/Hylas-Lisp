#include "annot.hpp"
#include "annot-errors.cpp"

std::vector<AnnotAST*> annotate(std::vector<SExp*> list) {
  std::vector<AnnotAST*> out;
  for(size_t i = 0; i < list.size(); i++) {
    out.push_back(annotate(list[i]));
  }
  return out;
}

LetAST* processLet(Atom atom, SExp* bindings, SExp* body) {
  if(length(bindings) % 2 != 0) {
    letBadBindingsError(atom);
  }
  Bindings bind;
  SExp* bindp = bindings;
  while(bindp != NULL) {
    SExp* sym = first(bindp);
    bindp = rest(bindp);
    /* After the check above, the number of bindings in the let form is
       guaranteed to be even, so we don't have to check on every iteration. */
    SExp* value = first(bindp);
    if(value->type != IDENTIFIER) {
      letNonSymBindingError(atom, value);
    }
    bind.push_back(Binding(val(sym), annotate(value)));
    bindp = rest(bindp);
  }
  return new LetAST(bind, annotate(body));
}

LetAST* processLetForm(Atom atom, SExp* list) {
  switch(length(list)) {
    case 0:
      formError(atom, "No bindings.");
    case 1:
      noBodyError(atom);
    default: {
      /* Process the let */
      SExp* bindings = first(list);
      SExp* body = rest(list);
      return processLet(atom, bindings, body);
    }
  }
}

FunctionAST* processFnDefinition(Atom atom, SExp* name, SExp* args, SExp* ret,
                                 SExp* body) {
  return NULL;
}

AnnotAST* processFunctionForm(Atom atom, SExp* list) {
  switch(length(list)) {
    case 0:
      fnNoNameError(atom);
    case 1:
      fnNoArgsError(atom);
    case 2:
      fnNoRetError(atom);
    case 3:
      noBodyError(atom);
    default: {
      /* Process the function definition */
      SExp* name = first(list);
      SExp* args = first(rest(list));
      SExp* ret = first(rest(rest(list)));
      SExp* body = rest(rest(rest(list)));
      return processFnDefinition(atom, name, args, ret, body);
    }
  }
}

LambdaAST* processLambdaDefinition(Atom atom, SExp* args, SExp* body) {
  return NULL;
}

AnnotAST* processLambdaForm(Atom atom, SExp* list) {
  switch(length(rest(list))) {
    case 0:
      fnNoArgsError(atom);
    case 1:
      noBodyError(atom);
    default: {
      /* Process the lambda definition */
      SExp* args = first(list);
      SExp* body = rest(list);
      return processLambdaDefinition(atom, args, body);
    }
  }
}

AnnotAST* annotateList(SExp* list) {
  /* Note: If a list makes it this far (when called from 'annotate') it is
     guaranteed to be non-NULL, that is, to have at least one element. */
  SExp* first = first(list);
  if(listp(first)) {
    /* If the first element of a list is a list, then it's an expression that
       (Presumably) returns a function pointer */
    return new CallAST(annotate(first), annotate(sexpToVec(rest(list))));
  } else {
    /* The first element is an atom, which can be either a function, a variable,
       or a special form. We check the special forms first. If it's none of
       those, we just return a CallAST: Differentiating between variables and
       functions is done further ahead.

       This part of the process involves verifying the syntax of the special
       forms represented in the annotated AST: let, defn and lambda. */
    Atom atom = first->content.atom;
    if(atomeq(first, "let")) {
      return processLetForm(atom, rest(list));
    } else if(atomeq(first, "defn")) {
      return processFunctionForm(atom, rest(list));
    } else if(atomeq(first, "lambda")) {
      return processLambdaForm(atom, rest(list));
    } else {
      return new CallAST(annotate(first), annotate(sexpToVec(rest(list))));
    }
  }
}

AnnotAST* annotate(SExp* sexp) {
  if(sexp) {
    if(listp(sexp)) {
      return annotateList(sexp);
    } else {
      return new AtomAST(sexp->content.atom);
    }
  } else {
    /* A null pointer represents the null form, which designates the unit
       type. */
    return NULL;
  }
}
