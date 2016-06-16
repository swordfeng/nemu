#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include <stdlib.h>

#ifndef PERFORMANCE

#define NR_WP 32

struct watchpoint {
    int NO;
    bool cond;
    struct watchpoint *next;
    char *exp;
    ExprFun expr_fun;
    uint32_t result;
};

static WP wp_list[NR_WP];
static WP *head, *free_;
static int wp_no_count;

void init_wp_list() {
    int i;
    for(i = 0; i < NR_WP; i ++) {
        wp_list[i].NO = i;
        wp_list[i].next = &wp_list[i + 1];
        wp_list[i].expr_fun = NULL;
    }
    wp_list[NR_WP - 1].next = NULL;

    head = NULL;
    free_ = wp_list;
    wp_no_count = 0;
}

WP* wp_new(bool cond) {
    Assert(free_, "Watchpoint pool excceed");
    WP *wp = free_;
    free_ = wp->next;
    wp->next = head;
    wp->exp = NULL;
    head = wp;
    wp->NO = wp_no_count++;
    wp->cond = cond;
    return wp;
}

void wp_free(WP *wp) {
    free(wp->exp);
    wp->exp = NULL;
    if (wp->expr_fun) {
        free_expr_fun(wp->expr_fun);
        wp->expr_fun = NULL;
    }
    // if wp is the first in the list
    if (head == wp) {
        head = wp->next;
        wp->next = free_;
        free_ = wp;
        return;
    } else {
        WP *prev = head;
        Assert(prev, "watchpoint is not active");
        while (prev->next != wp) {
            Assert(prev->next, "watchpoint is not active");
            prev = prev->next;
        }
        prev->next = wp->next;
        wp->next = free_;
        free_ = wp;
        return;
    }
}

WP *wp_find(int no) {
    WP *wp = head;
    while (wp) {
        if (wp->NO == no) return wp;
        wp = wp->next;
    }
    return NULL;
}

void wp_set_expr(WP *wp, const char *exp) {
    size_t len = strlen(exp);
    free(wp->exp);
    wp->exp = malloc(len + 1);
    strcpy(wp->exp, exp);
    wp->expr_fun = new_expr_fun(exp);
    wp_eval(wp);
    return;
}

bool wp_eval(WP *wp) {
    Assert(wp->exp, "NULL EXPRESSION IN WATCHPOINT");
    bool success;
    wp->result = wp->expr_fun(&success); //expr(wp->exp, &success);
    Assert(success, "WRONG EXPRESSION IN WATCHPOINT");
    return true;
}

uint32_t wp_get_result(WP *wp) {
    Assert(wp->exp, "NULL EXPRESSION IN WATCHPOINT");
    return wp->result;
}

const char *wp_get_expr(WP *wp) {
    Assert(wp->exp, "NULL EXPRESSION IN WATCHPOINT");
    return wp->exp;
}

int wp_get_no(WP *wp) {
    return wp->NO;
}

bool wp_watch(WP **pwp, uint32_t *old_result, uint32_t *new_result) {
    WP *exam = head;
    while (exam) {
        uint32_t o_res = wp_get_result(exam);
        wp_eval(exam);
        uint32_t n_res = wp_get_result(exam);
        if (exam->cond) {
            if (pwp) *pwp = exam;
            if (old_result) *old_result = o_res;
            if (new_result) *new_result = n_res;
            return !!n_res;
        } else if (o_res != n_res) {
            if (pwp) *pwp = exam;
            if (old_result) *old_result = o_res;
            if (new_result) *new_result = n_res;
            return true;
        }
        exam = exam->next;
    }
    return false;
}

void wp_print_info() {
    WP *wp = head;
    if (!wp) {
        printf("No active watchpoints.\n");
        return;
    }
    printf("Watchpoint      Last result     Expression\n");
    for (; wp != NULL; wp = wp->next) {
        printf("%-16d%-16d%s\n", wp->NO, wp->result, wp->exp);
    }
}
#endif
