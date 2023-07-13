#ifndef _BASIS_PMS_H_
#define _BASIS_PMS_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>
#include <signal.h>
#include <algorithm>
#include "string_util.h"

using namespace std;

#define mypop(stack) stack[--stack##_fill_pointer]
#define mypush(item, stack) stack[stack##_fill_pointer++] = item

const float MY_RAND_MAX_FLOAT = 10000000.0;
const int MY_RAND_MAX_INT = 10000000;
const float BASIC_SCALE = 0.0000001; // 1.0f/MY_RAND_MAX_FLOAT;

// Define a data structure for a literal.
struct lit
{
	int clause_num; // clause num, begin with 0
	int var_num;	// variable num, begin with 1
	int weight;
	bool sense;		// 1 for true literals, 0 for false literals.
};
struct temp_var
{
	int var_num;
	int weight;
};

extern int* temp_array;

extern temp_var* temp_unsat;

extern struct tms start_time;

void build_instance(const char *filename);
void local_search(vector<int> &init_solution, const char *inputfile);
void simple_print();
void print_best_solution();
void free_memory();
void check_new_score();
void check_softunsat_weight();
void start_timing();
double get_runtime();

extern int* soft_clause_num_index;
extern int* hard_clause_num_index;

extern int is_print;

// size of the instance
extern int num_vars;	// var index from 1 to num_vars
extern int num_clauses; // clause index from 0 to num_clauses-1
extern int num_hclauses;
extern int num_sclauses;

// steps and time
extern int tries;
extern int max_tries;
extern unsigned int max_flips;
extern unsigned int max_non_improve_flip;
extern unsigned int step;

extern int cutoff_time;
extern double opt_time;

/* literal arrays */
extern lit **var_lit;		  // var_lit[i][j] means the j'th literal of variable i.
extern int *var_lit_count;	  // amount of literals of each variable
extern lit **clause_lit;	  // clause_lit[i][j] means the j'th literal of clause i.
extern int *clause_lit_count; // amount of literals in each clause
extern int *clause_true_lit_thres;
extern double *avg_clause_coe;

/* Information about the variables. */
extern double *hscore;
extern double *sscore;
extern long long *time_stamp;
extern int **var_neighbor;
extern int *var_neighbor_count;
extern int *neighbor_flag;
extern int *temp_neighbor;

/* Information about the clauses */
extern long long top_clause_weight;
extern long long *org_clause_weight;
extern double *tune_soft_clause_weight;
extern double *unit_weight;
extern double *tuned_degree_unit_weight;

extern int *sat_count;
extern int *sat_var;

// unsat clauses stack
extern int *hardunsat_stack;		  // store the falsified clause number
extern int *index_in_hardunsat_stack; // which position is a clause in the unsat_stack
extern int hardunsat_stack_fill_pointer;

extern int *softunsat_stack;		  // store the falsified clause number
extern int *index_in_softunsat_stack; // which position is a clause in the unsat_stack
extern int softunsat_stack_fill_pointer;

// good decreasing variables (dscore>0)
extern int *goodvar_stack;
extern int goodvar_stack_fill_pointer;
extern int *already_in_goodvar_stack;

/* Information about solution */
extern int *cur_soln; // the current assignment, with 1's for True variables, and 0's for False variables
extern int *best_soln;
extern int best_soln_feasible; // when find a feasible solution, this is marked as 1.
extern int local_soln_feasible;
extern int hard_unsat_nb;
extern long long soft_unsat_weight;
extern long long opt_unsat_weight;
extern long long best_known;

// parameters used in algorithm
extern float rwprob;
extern float rdprob;
extern int hd_count_threshold;
extern int h_inc;
extern int s_inc;
extern float initsoftw;

// function used in algorithm
void build_neighbor_relation();
void allocate_memory();
bool verify_sol();
void increase_weights();
void update_clause_weights();
void unsat(int clause);
void sat(int clause);
void init_local_search(vector<int> &init_solution);
void flip(int flipvar);
void update_goodvarstack(int flipvar);
int pick_var();
void settings();



#endif
