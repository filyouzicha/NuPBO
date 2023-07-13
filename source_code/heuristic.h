#ifndef _HEURISTIC_H
#define _HEURISTIC_H
#include "basis_pms.h"

void init_score_multi();

extern void (*flip_ptr)(int flipvar);
void flip_with_neighbor(int flipvar);
void flip_no_neighbor(int flipvar);

void flip_update_score_multi(int flipvar);
void flip_update_score_no_neighbor_multi(int flipvar);

void update_weight_score_multi(int c);

extern int (*select_var_after_update_weight_ptr)();
int select_var_after_update_weight_1();
int select_var_after_update_weight_2();

extern double (*soft_var_greedy_ptr)(int v);
extern double (*hard_var_greedy_ptr)(int v);
double var_greedy_hscore(int v);
double var_greedy_sscore(int v);
double var_greedy_score(int v);

#endif