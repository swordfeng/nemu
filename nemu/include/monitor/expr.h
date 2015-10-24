#ifndef __EXPR_H__
#define __EXPR_H__

#include "common.h"

uint32_t expr(char *exp_str, bool *succ);
bool expr_prettify(char *exp_str, char **exp_out);

typedef uint32_t (*ExprFun)(bool *succ);
ExprFun new_expr_fun(const char *exp_str);
void free_expr_fun(ExprFun expr_fun);

#endif
