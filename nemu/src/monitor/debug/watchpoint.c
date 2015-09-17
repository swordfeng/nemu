#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include <stdlib.h>

#define NR_WP 32

struct watchpoint {
	int NO;
	struct watchpoint *next;
	char *exp;
	uint32_t result;
};

static WP wp_list[NR_WP];
static WP *head, *free_;

void init_wp_list() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_list[i].NO = i;
		wp_list[i].next = &wp_list[i + 1];
	}
	wp_list[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_list;
}

/* TODO: Implement the functionality of watchpoint */

WP* wp_new() {
	Assert(free_, "watchpoints reaches the limit");
	WP *wp = free_;
	free_ = wp->next;
	wp->next = head;
	wp->exp = NULL;
	head = wp;
	return wp;
}

void wp_free(WP *wp) {
	free(wp->exp);
	wp->exp = NULL;
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

void wp_set_expr(WP *wp, const char *exp) {
	size_t len = strlen(exp);
	free(wp->exp);
	wp->exp = malloc(len + 1);
	strcpy(wp->exp, exp);
	wp_eval(wp);
	return;
}

bool wp_eval(WP *wp) {
	Assert(wp->exp, "NULL EXPRESSION IN WATCHPOINT");
	bool success;
	wp->result = expr(wp->exp, &success);
	Assert(success, "WRONG EXPRESSION IN WATCHPOINT");
	return true;
}

uint32_t wp_get_result(WP *wp) {
	Assert(wp->exp, "NULL EXPRESSION IN WATCHPOINT");
	return wp->result;
}

int wp_get_no(WP *wp) {
	return wp->NO;
}
