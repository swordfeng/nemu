#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

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

WP* new_wp() {
	Assert(free_, "watchpoints reaches the limit");
	WP *wp = free_;
	free_ = wp->next;
	wp->next = head;
	head = wp;
	return wp;
}

void free_wp(WP *wp) {
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
