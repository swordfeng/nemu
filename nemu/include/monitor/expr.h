#ifndef __EXPR_H__
#define __EXPR_H__

#include "common.h"

uint32_t expr(char *, bool *);
bool expr_prettify(char *, char **);

typedef uint32_t (*ExprFun)(bool *);
ExprFun new_expr_fun(const char *);
void free_expr_fun(ExprFun);

#endif
