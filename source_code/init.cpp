#include "init.h"
void (*init_assignment_ptr)(vector<int> &init_solution);
void init_assignment_false(vector<int> &init_solution)
{
	int v = 0;
	for (v = 1; v <= num_vars; v++)
	{
		cur_soln[v] = 0;
		time_stamp[v] = 0;
	}
	return;
}
void init_assignment_true(vector<int> &init_solution)
{
	int v = 0;
	if (init_solution.size() == 0)
	{
		for (v = 1; v <= num_vars; v++)
		{
			cur_soln[v] = 1;
			time_stamp[v] = 0;
		}
	}
	else
	{
		for (v = 1; v <= num_vars; v++)
		{
			cur_soln[v] = init_solution[v];
			if (cur_soln[v] == 2)
				cur_soln[v] = rand() % 2;
			time_stamp[v] = 0;
		}
	}
	return;
}
void init_assignment_random(vector<int> &init_solution)
{
	int v = 0;
	if (init_solution.size() == 0)
	{
		for (v = 1; v <= num_vars; v++)
		{
			cur_soln[v] = rand() % 2;
			time_stamp[v] = 0;
		}
	}
	else
	{
		for (v = 1; v <= num_vars; v++)
		{
			cur_soln[v] = init_solution[v];
			if (cur_soln[v] == 2)
				cur_soln[v] = rand() % 2;
			time_stamp[v] = 0;
		}
	}
	return;
}