
#ifndef COMPILER_GENERATOR
#define COMPILER_GENERATOR

#include "ast.h"
#include "symtable.h"

Expression * Lvalue(Expression * n);
Expression * Rvalue(Expression * n);

#endif
