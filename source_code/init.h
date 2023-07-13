#ifndef _INIT_H
#define _INIT_H

#include "basis_pms.h"
extern void (*init_assignment_ptr)(vector<int> &init_solution);
void init_assignment_false(vector<int> &init_solution);
void init_assignment_true(vector<int> &init_solution);
void init_assignment_random(vector<int> &init_solution);

#endif