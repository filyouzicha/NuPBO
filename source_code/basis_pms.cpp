#include "basis_pms.h"
#include "init.h"
#include "heuristic.h"

#include <cmath>
#include <sstream>
#include <algorithm>

int *temp_array = NULL;
temp_var *temp_unsat = NULL;
int *soft_clause_num_index = NULL;
int *hard_clause_num_index = NULL;
long long total_step;
int is_print;

// size of the instance
int num_vars = 0;	  // number of variables, var index from 1 to num_vars
int num_clauses = 0;  // number of clauses, clause index from 0 to num_clauses-1
int num_hclauses = 0; // number of hard clauses
int num_sclauses = 0; // number of soft clauses

// steps and time
int tries;
int max_tries;
unsigned int max_flips;
unsigned int max_non_improve_flip;
unsigned int step;

int cutoff_time = 300;
double opt_time;

/* literal arrays */
lit **var_lit;		   // var_lit[i][j] means the j'th literal of variable i
int *var_lit_count;	   // amount of literals of each variable
lit **clause_lit;	   // clause_lit[i][j] means the j'th literal of clause i.
int *clause_lit_count; // amount of literals in each clause
int *clause_true_lit_thres;
double *avg_clause_coe;

/* Information about the variables. */
double *hscore;
double *sscore;
long long *time_stamp;
int **var_neighbor;
int *var_neighbor_count;
int *neighbor_flag;
int *temp_neighbor;

/* Information about the clauses */
long long top_clause_weight;
long long *org_clause_weight;
double *tune_soft_clause_weight;
double *unit_weight;
double *tuned_degree_unit_weight = NULL;

int *sat_count;
int *sat_var;

// unsat clauses stack
int *hardunsat_stack;		   // store the falsified clause number
int *index_in_hardunsat_stack; // which position is a clause in the hardunsat_stack
int hardunsat_stack_fill_pointer;

int *softunsat_stack;		   // store the falsified clause number
int *index_in_softunsat_stack; // which position is a clause in the softunsat_stack
int softunsat_stack_fill_pointer;

// good decreasing variables (dscore>0)
int *goodvar_stack;
int goodvar_stack_fill_pointer;
int *already_in_goodvar_stack;

/* Information about solution */
int *cur_soln; // the current assignment, with 1's for True variables, and 0's for False variables
int *best_soln;
int best_soln_feasible; // when find a feasible solution, this is marked as 1.
int local_soln_feasible;
int hard_unsat_nb;
long long soft_unsat_weight;
long long opt_unsat_weight;
long long best_known;

// parameters used in algorithm
float rwprob;
float rdprob;
int hd_count_threshold;
int h_inc;
int s_inc;
float initsoftw = 0;

struct tms start_time;

bool compare(temp_var a, temp_var b)
{
	return a.weight > b.weight;
}

void settings()
{
	// steps
	total_step = 0;
	max_tries = 100000000;
	max_flips = 10000000;
	max_non_improve_flip = 10000000;

	rdprob = 0.01;
	rwprob = 0.1;
	hd_count_threshold = 15;
	h_inc = 1;
	s_inc = 1;

	best_known = -2;
	if (num_vars > 2000)
	{
		rdprob = 0.01;
		rwprob = 0.1;
		hd_count_threshold = 50;
	}
	hard_var_greedy_ptr = var_greedy_score;
	soft_var_greedy_ptr = var_greedy_score;
}

void allocate_memory()
{
	int malloc_var_length = num_vars + 10;
	int malloc_clause_length = num_clauses + 10;

	temp_unsat = new temp_var[malloc_var_length];
	temp_array = new int[malloc_clause_length];

	var_lit = new lit *[malloc_var_length];
	var_lit_count = new int[malloc_var_length]();
	clause_lit = new lit *[malloc_clause_length];
	clause_lit_count = new int[malloc_clause_length]();
	clause_true_lit_thres = new int[malloc_clause_length];
	avg_clause_coe = new double[malloc_clause_length]();

	hscore = new double[malloc_var_length];
	sscore = new double[malloc_var_length];
	var_neighbor = new int *[malloc_var_length];
	var_neighbor_count = new int[malloc_var_length];
	time_stamp = new long long[malloc_var_length];
	neighbor_flag = new int[malloc_var_length];
	temp_neighbor = new int[malloc_var_length];

	soft_clause_num_index = new int[malloc_clause_length];
	hard_clause_num_index = new int[malloc_clause_length];
	org_clause_weight = new long long[malloc_clause_length];
	tune_soft_clause_weight = new double[malloc_clause_length];
	unit_weight = new double[malloc_clause_length];
	tuned_degree_unit_weight = new double[malloc_clause_length];
	sat_count = new int[malloc_clause_length];
	sat_var = new int[malloc_clause_length];

	hardunsat_stack = new int[malloc_clause_length];
	index_in_hardunsat_stack = new int[malloc_clause_length];
	softunsat_stack = new int[malloc_clause_length];
	index_in_softunsat_stack = new int[malloc_clause_length];

	goodvar_stack = new int[malloc_var_length];
	already_in_goodvar_stack = new int[malloc_var_length];

	cur_soln = new int[malloc_var_length];
	best_soln = new int[malloc_var_length];

}

void free_memory()
{
	int i;
	for (i = 0; i < num_clauses; i++)
		delete[] clause_lit[i];

	for (i = 1; i <= num_vars; ++i)
	{
		delete[] var_lit[i];
		delete[] var_neighbor[i];
	}
	delete[] temp_array;
	delete[] temp_unsat;
	delete[] var_lit;
	delete[] var_lit_count;
	delete[] clause_lit;
	delete[] clause_lit_count;
	delete[] clause_true_lit_thres;
	delete[] avg_clause_coe;

	delete[] hscore;
	delete[] sscore;
	delete[] var_neighbor;
	delete[] var_neighbor_count;
	delete[] time_stamp;
	delete[] neighbor_flag;
	delete[] temp_neighbor;

	delete[] soft_clause_num_index;
	delete[] hard_clause_num_index;
	delete[] org_clause_weight;
	delete[] tune_soft_clause_weight;
	delete[] unit_weight;
	delete[] tuned_degree_unit_weight;
	delete[] sat_count;
	delete[] sat_var;

	delete[] hardunsat_stack;
	delete[] index_in_hardunsat_stack;
	delete[] softunsat_stack;
	delete[] index_in_softunsat_stack;

	delete[] goodvar_stack;
	delete[] already_in_goodvar_stack;

	delete[] cur_soln;
	delete[] best_soln;

}

void build_neighbor_relation()
{
	//cout << "c start build neighbor" << endl;
	int i, j, count;
	int v, c, n;
	int temp_neighbor_count;

	for (v = 1; v <= num_vars; ++v)
	{
		neighbor_flag[v] = 1;
		temp_neighbor_count = 0;

		for (i = 0; i < var_lit_count[v]; ++i)
		{
			c = var_lit[v][i].clause_num;
			for (j = 0; j < clause_lit_count[c]; ++j)
			{
				n = clause_lit[c][j].var_num;
				if (neighbor_flag[n] != 1)
				{
					neighbor_flag[n] = 1;
					temp_neighbor[temp_neighbor_count++] = n;
				}
			}
		}

		neighbor_flag[v] = 0;
		var_neighbor[v] = new int[temp_neighbor_count];
		var_neighbor_count[v] = temp_neighbor_count;

		count = 0;
		for (i = 0; i < temp_neighbor_count; i++)
		{
			var_neighbor[v][count++] = temp_neighbor[i];
			neighbor_flag[temp_neighbor[i]] = 0;
		}
	}
	//cout << "c end build neighbor" << endl;
}

void build_instance(const char *filename)
{
	istringstream iss;
	char line[1024];
	string line2;
	char tempstr1[10];
	char tempstr2[10];
	int cur_lit;
	int i, v, c, j;
	ifstream infile(filename);
	if (!infile)
	{
		cout << "c the input filename " << filename << " is invalid, please input the correct filename." << endl;
		exit(-1);
	}

	/*** build problem data structures of the instance ***/

	getline(infile, line2);

	while (line2[0] != 'p')
	{
		getline(infile, line2);
	}
	for (i = 0; i < 1024; i++)
	{
		line[i] = line2[i];
	}
	int read_items;
	read_items = sscanf(line, "%s %s %d %d %lld", tempstr1, tempstr2, &num_vars, &num_clauses, &top_clause_weight);

	allocate_memory();

	num_hclauses = num_sclauses = 0;
	// Now, read clauses, one at a time.
	int lit_redundent, clause_redundent;
	temp_var *temp_weight = new temp_var[num_vars];
	int cur_weight;

	c = 0;
	while (getline(infile, line2))
	{
		if (line2[0] == 'c')
			continue;
		else
		{
			iss.clear();
			iss.str(line2);
			iss.seekg(0, ios::beg);
		}
		clause_redundent = 0;
		clause_lit_count[c] = 0;

		iss >> org_clause_weight[c];
		iss >> clause_true_lit_thres[c];
		if (clause_true_lit_thres[c] <= 0)
		{
			num_clauses--;
			continue;
		}

		if (org_clause_weight[c] != top_clause_weight)
		{
			soft_clause_num_index[num_sclauses++] = c;
		}
		else
		{
			hard_clause_num_index[num_hclauses++] = c;
		}

		iss >> cur_weight;
		iss >> cur_lit;
		while (cur_weight != 0)
		{
			temp_weight[clause_lit_count[c]].weight = cur_weight;
			temp_weight[clause_lit_count[c]].var_num = cur_lit;
			clause_lit_count[c]++;
			iss >> cur_weight;
			iss >> cur_lit;
		}
		sort(temp_weight, temp_weight + clause_lit_count[c], compare);
		clause_lit[c] = new lit[clause_lit_count[c] + 1];

		for (i = 0; i < clause_lit_count[c]; ++i)
		{
			clause_lit[c][i].clause_num = c;
			clause_lit[c][i].var_num = abs(temp_weight[i].var_num);
			clause_lit[c][i].weight = temp_weight[i].weight;
			avg_clause_coe[c] += double(clause_lit[c][i].weight);

			if (temp_weight[i].var_num > 0)
				clause_lit[c][i].sense = 1;
			else
				clause_lit[c][i].sense = 0;

			var_lit_count[clause_lit[c][i].var_num]++;
		}

		// round
		avg_clause_coe[c] = round(double(avg_clause_coe[c] / (double)clause_lit_count[c]));
		if (avg_clause_coe[c] < 1)
			avg_clause_coe[c] = 1;
		
		clause_lit[c][i].var_num = 0;
		clause_lit[c][i].clause_num = -1;
		clause_lit[c][i].weight = 0;

		c++;
	}
	infile.close();
	delete[] temp_weight;

	// creat var literal arrays
	long long tmp_lit_num = 0;
	double avg_neighbor_lit = 0;
	for (v = 1; v <= num_vars; ++v)
	{
		var_lit[v] = new lit[var_lit_count[v] + 1];
		tmp_lit_num += var_lit_count[v];
		var_lit_count[v] = 0;
	}
	avg_neighbor_lit = double(tmp_lit_num -num_sclauses )/ (num_vars-num_sclauses+1);
	//cout << "c avg_neighbor_lit: " << avg_neighbor_lit<< endl;

	// scan all clauses to build up var literal arrays
	for (c = 0; c < num_clauses; ++c)
	{
		for (i = 0; i < clause_lit_count[c]; ++i)
		{
			v = clause_lit[c][i].var_num;
			var_lit[v][var_lit_count[v]++] = clause_lit[c][i];
		}
		
	}
	
	if (avg_neighbor_lit < 1e+7)
	{
		build_neighbor_relation();
		flip_ptr = flip_with_neighbor;
	}
	else
	{
		flip_ptr = flip_no_neighbor;
	}
	best_soln_feasible = 0;
	opt_unsat_weight = top_clause_weight;
}

void init_local_search(vector<int> &init_solution)
{
	int v, c;
	int i, j;

	local_soln_feasible = 0;

	// Initialize clause information
	for (i = 0; i < num_hclauses; i++)
	{
		c = hard_clause_num_index[i];
		unit_weight[c] = 1;
		tuned_degree_unit_weight[c] = double(unit_weight[c]) / avg_clause_coe[c];
	}
	// round
	double tmp_avg_soft_clause_weight = 0.0;
	tmp_avg_soft_clause_weight = round(double(top_clause_weight - 1) / num_sclauses);
	if (tmp_avg_soft_clause_weight < 1)
	 	tmp_avg_soft_clause_weight = 1;
	for (i = 0; i < num_sclauses; i++)
	{
		c = soft_clause_num_index[i];
		tune_soft_clause_weight[c] = double(org_clause_weight[c] / tmp_avg_soft_clause_weight);
		unit_weight[c] = initsoftw;
	}

	// init solution
	init_assignment_ptr(init_solution);

	// init stacks
	hard_unsat_nb = 0;
	hardunsat_stack_fill_pointer = 0;
	softunsat_stack_fill_pointer = 0;

	/* figure out sat_count, sat_var, soft_unsat_weight and init unsat_stack */
	soft_unsat_weight = 0;

	for (i = 0; i < num_hclauses; i++)
	{
		c = hard_clause_num_index[i];
		sat_count[c] = 0;
		for (j = 0; j < clause_lit_count[c]; ++j)
		{
			if (cur_soln[clause_lit[c][j].var_num] == clause_lit[c][j].sense)
			{
				sat_count[c] += clause_lit[c][j].weight;
				sat_var[c] = clause_lit[c][j].var_num;
			}
		}
		if (sat_count[c] < clause_true_lit_thres[c])
		{
			unsat(c);
		}
	}
	for (i = 0; i < num_sclauses; i++)
	{
		c = soft_clause_num_index[i];
		sat_count[c] = 0;

		if (cur_soln[clause_lit[c][0].var_num] == clause_lit[c][0].sense)
		{
			sat_count[c] += clause_lit[c][0].weight;
			sat_var[c] = clause_lit[c][0].var_num;
		}
		else
		{
			soft_unsat_weight += (clause_true_lit_thres[c] - sat_count[c]) * org_clause_weight[c];
			unsat(c);
		}
	}

	/*figure out score*/
	init_score_multi();
	
	// init goodvars stack
	goodvar_stack_fill_pointer = 0;
	for (v = 1; v <= num_vars; v++)
	{
		if (hscore[v] + sscore[v] > 0)
		{
			already_in_goodvar_stack[v] = goodvar_stack_fill_pointer;
			mypush(v, goodvar_stack);
		}
		else
			already_in_goodvar_stack[v] = -1;
	}
}

int pick_var()
{
	int i, v, r, c, l, w;
	int best_var;
	lit *p;

	if (goodvar_stack_fill_pointer > 0)
	{
		if ((rand() % MY_RAND_MAX_INT) * BASIC_SCALE < rdprob)
			return goodvar_stack[rand() % goodvar_stack_fill_pointer];

		if (goodvar_stack_fill_pointer < hd_count_threshold)
		{
			best_var = goodvar_stack[0];
			for (i = 1; i < goodvar_stack_fill_pointer; ++i)
			{
				v = goodvar_stack[i];
				if (hscore[v] + sscore[v] > hscore[best_var] + sscore[best_var])
					best_var = v;
				else if (hscore[v] + sscore[v] == hscore[best_var] + sscore[best_var])
				{
					if (time_stamp[v] < time_stamp[best_var])
						best_var = v;
				}
			}
			return best_var;
		}
		else
		{
			r = rand() % goodvar_stack_fill_pointer;
			best_var = goodvar_stack[r];

			for (i = 1; i < hd_count_threshold; ++i)
			{
				r = rand() % goodvar_stack_fill_pointer;
				v = goodvar_stack[r];
				if (hscore[v] + sscore[v] > hscore[best_var] + sscore[best_var])
					best_var = v;
				else if (hscore[v] + sscore[v] == hscore[best_var] + sscore[best_var])
				{
					if (time_stamp[v] < time_stamp[best_var])
						best_var = v;
				}
			}
			return best_var;
		}
	}
	update_clause_weights();

	return select_var_after_update_weight_ptr();
}

void update_goodvarstack(int flipvar)
{
	int v;

	// remove the variables no longer goodvar in goodvar_stack
	for (int index = goodvar_stack_fill_pointer - 1; index >= 0; index--)
	{
		v = goodvar_stack[index];
		if (hscore[v] + sscore[v] <= 0)
		{
			int top_v = mypop(goodvar_stack);
			goodvar_stack[index] = top_v;
			already_in_goodvar_stack[top_v] = index;
			already_in_goodvar_stack[v] = -1;
		}
	}

	// add goodvar
	for (int i = 0; i < var_neighbor_count[flipvar]; ++i)
	{
		v = var_neighbor[flipvar][i];
		if (hscore[v] + sscore[v] > 0)
		{
			if (already_in_goodvar_stack[v] == -1)
			{
				already_in_goodvar_stack[v] = goodvar_stack_fill_pointer;
				mypush(v, goodvar_stack);
			}
		}
	}
}

void flip_with_neighbor(int flipvar)
{
	int i, v, c;
	int index;
	lit *clause_c;
	int weight;
	int gap;

	double org_flipvar_score = hscore[flipvar];
	double org_sscore = sscore[flipvar];
	cur_soln[flipvar] = 1 - cur_soln[flipvar];

	flip_update_score_multi(flipvar);

	// update information of flipvar
	hscore[flipvar] = -org_flipvar_score;
	sscore[flipvar] = -org_sscore;
	update_goodvarstack(flipvar);
}

void flip_no_neighbor(int flipvar)
{
	int i, v, c;
	int index;
	lit *clause_c;
	int weight;
	int gap;

	double org_flipvar_score = hscore[flipvar];
	double org_sscore = sscore[flipvar];
	cur_soln[flipvar] = 1 - cur_soln[flipvar];

	flip_update_score_no_neighbor_multi(flipvar);

	// update information of flipvar
	hscore[flipvar] = -org_flipvar_score;
	sscore[flipvar] = -org_sscore;
	if (already_in_goodvar_stack[flipvar] != -1)
	{
		int top_v = mypop(goodvar_stack);
		goodvar_stack[already_in_goodvar_stack[flipvar]] = top_v;
		already_in_goodvar_stack[top_v] = already_in_goodvar_stack[flipvar];
		already_in_goodvar_stack[flipvar] = -1;
	}
	return;
}

void print_best_solution()
{
	// cout << "c total_step: " << total_step << " " << tries << endl;
	if (0 == is_print)
	{
		is_print = 1;
		if (best_soln_feasible == 1)
		{
			if (verify_sol())
			{
				cout << "s ";
				for (int i = 1; i <= num_vars; i++)
				{
					cout << best_soln[i] << " ";
				}
				cout << endl;
				// cout << opt_unsat_weight << '\t' << opt_time << '\t' << tries << '\t' << step << endl;
			}
			else
				cout << "got an incorrect solution" << endl;
		}
		else
			cout << "no feasible solution found" << endl;
	}
}

void local_search(vector<int> &init_solution, const char *inputfile)
{
	for (tries = 1; tries < max_tries; ++tries)
	{
		init_local_search(init_solution);
		max_flips = 10000000;
		for (step = 1; step < max_flips; ++step)
		{
			total_step++;
			if (hard_unsat_nb == 0)
			{
				local_soln_feasible = 1;
				best_soln_feasible = 1;

				if (soft_unsat_weight < opt_unsat_weight)
				{
					opt_unsat_weight = soft_unsat_weight;
					cout << "o " << soft_unsat_weight << endl;// << " " << total_step << endl;
					opt_time = get_runtime();
					for (int v = 1; v <= num_vars; ++v)
						best_soln[v] = cur_soln[v];
					if (opt_unsat_weight == 0 || opt_unsat_weight <= best_known)
					{
						return;
					}
					max_flips = step + 10000000;
				}
			}
			
			int flipvar = pick_var();

			flip_ptr(flipvar);
			// check_new_score();
			time_stamp[flipvar] = step;
		}
	}
}

void check_softunsat_weight()
{
	int c, j, flag;
	long long verify_unsat_weight = 0;

	for (c = 0; c < num_clauses; ++c)
	{
		flag = 0;
		int tem_clause_true_lit_count = 0;
		for (j = 0; j < clause_lit_count[c]; ++j)
		{
			if (cur_soln[clause_lit[c][j].var_num] == clause_lit[c][j].sense)
			{
				tem_clause_true_lit_count++;
			}
		}
		if (tem_clause_true_lit_count >= clause_true_lit_thres[c])
			flag = 1;

		if (flag == 0)
		{
			if (org_clause_weight[c] == top_clause_weight) // verify hard clauses
			{
				continue;
			}
			else
			{
				verify_unsat_weight += org_clause_weight[c] * (clause_true_lit_thres[c] - tem_clause_true_lit_count);
			}
		}
	}

	if (verify_unsat_weight != soft_unsat_weight)
	{
		cout << step << endl;
		cout << "verify unsat weight is" << verify_unsat_weight << " and soft unsat weight is " << soft_unsat_weight << endl;
	}
	// return 0;
}

bool verify_sol()
{
	cout << "c start verification" << endl;
	int c, j, flag;
	long long verify_unsat_weight = 0;
	long long real_min_weight = 0;

	for (c = 0; c < num_clauses; ++c)
	{
		if (org_clause_weight[c] != top_clause_weight)
		{
			if (clause_lit[c][0].sense == 0)
			{
				real_min_weight += org_clause_weight[c] * best_soln[clause_lit[c][0].var_num];
			}
			else
			{
				real_min_weight -= org_clause_weight[c] * best_soln[clause_lit[c][0].var_num];
			}
		}
		flag = 0;
		int tem_clause_true_lit_count = 0;
		for (j = 0; j < clause_lit_count[c]; ++j)
		{
			if (best_soln[clause_lit[c][j].var_num] == clause_lit[c][j].sense)
			{
				tem_clause_true_lit_count += clause_lit[c][j].weight;
			}
		}
		if (tem_clause_true_lit_count >= clause_true_lit_thres[c])
			flag = 1;

		if (flag == 0)
		{
			if (org_clause_weight[c] == top_clause_weight) // verify hard clauses
			{
				// output the falsified clause under the assignment
				cout << "c Error: hard clause " << c << " is falsified" << endl;

				cout << "c ";
				for (j = 0; j < clause_lit_count[c]; ++j)
				{
					if (clause_lit[c][j].sense == 0)
						cout << "-";
					cout << clause_lit[c][j].var_num << " ";
				}
				cout << endl;
				cout << "c ";
				for (j = 0; j < clause_lit_count[c]; ++j)
					cout << best_soln[clause_lit[c][j].var_num] << " ";
				cout << endl;
				return 0;
			}
			else
			{
				verify_unsat_weight += org_clause_weight[c] * (clause_true_lit_thres[c] - tem_clause_true_lit_count);

			}
		}
	}

	if (verify_unsat_weight == opt_unsat_weight)
	{
		cout << "c realmin: " << real_min_weight << endl;
		return 1;
	}
	else
	{
		cout << "c Error: find opt=" << opt_unsat_weight << ", but verified opt=" << verify_unsat_weight << endl;
	}
	return 0;
}

void simple_print()
{
	if (best_soln_feasible == 1)
	{
		if (verify_sol() == 1)
			cout << opt_unsat_weight << '\t' << opt_time << endl;
		else
			cout << "solution is wrong " << endl;
	}
	else
		cout << -1 << '\t' << -1 << endl;
}

void increase_weights()
{
	int i, c, v;
	int weight;
	int flag = 0;

	for (i = 0; i < hardunsat_stack_fill_pointer; ++i)
	{
		flag = 1;
		c = hardunsat_stack[i];
		unit_weight[c] += h_inc;
		tuned_degree_unit_weight[c] = double(unit_weight[c]) / avg_clause_coe[c];
		update_weight_score_multi(c);
	}

	// increase all soft clause weights
	if (0 == hard_unsat_nb)
	{
		for (i = 0; i < num_sclauses; i++)
		{
			c = soft_clause_num_index[i];
			unit_weight[c] += s_inc;
			v = clause_lit[c][0].var_num;

			if (clause_lit[c][0].sense != cur_soln[v])
			{
				sscore[v] += s_inc * tune_soft_clause_weight[c];
				if (hscore[v] + sscore[v] > 0 && already_in_goodvar_stack[v] == -1)
				{
					already_in_goodvar_stack[v] = goodvar_stack_fill_pointer;
					mypush(v, goodvar_stack);
				}
			}
			else
			{
				sscore[v] -= s_inc * tune_soft_clause_weight[c];
				if (already_in_goodvar_stack[v] != -1 && hscore[v] + sscore[v] <= 0)
				{
					int top_v = mypop(goodvar_stack);
					goodvar_stack[already_in_goodvar_stack[v]] = top_v;
					already_in_goodvar_stack[top_v] = already_in_goodvar_stack[v];
					already_in_goodvar_stack[v] = -1;
				}
			}
		}
	}
}

void update_clause_weights()
{
	increase_weights();
}

void unsat(int clause)
{
	if (org_clause_weight[clause] == top_clause_weight) // hard
	{
		index_in_hardunsat_stack[clause] = hardunsat_stack_fill_pointer;
		mypush(clause, hardunsat_stack);
		hard_unsat_nb++;
	}
	else // soft
	{
		index_in_softunsat_stack[clause] = softunsat_stack_fill_pointer;
		mypush(clause, softunsat_stack);
		// soft_unsat_weight += org_clause_weight[clause];
	}
}

void sat(int clause)
{
	int index, last_unsat_clause;

	if (org_clause_weight[clause] == top_clause_weight)
	{

		last_unsat_clause = mypop(hardunsat_stack);
		index = index_in_hardunsat_stack[clause];
		hardunsat_stack[index] = last_unsat_clause;
		index_in_hardunsat_stack[last_unsat_clause] = index;

		hard_unsat_nb--;
	}
	else
	{
		last_unsat_clause = mypop(softunsat_stack);
		index = index_in_softunsat_stack[clause];
		softunsat_stack[index] = last_unsat_clause;
		index_in_softunsat_stack[last_unsat_clause] = index;

		// soft_unsat_weight -= org_clause_weight[clause];
	}
}

void check_new_score()
{
	long long tem_score = 0;
	long long tem_sscore = 0;
	int sense, c, v, i;
	int weight;
	for (v = 1; v <= num_vars; v++)
	{
		tem_score = 0;
		tem_sscore = 0;
		for (i = 0; i < var_lit_count[v]; ++i)
		{
			c = var_lit[v][i].clause_num;
			sense = var_lit[v][i].sense;
			weight = var_lit[v][i].weight;
			if (org_clause_weight[c] == top_clause_weight)
			{
				if (sat_count[c] < clause_true_lit_thres[c])
				{
					if (sense != cur_soln[v])
					{
						tem_score += unit_weight[c] * min(clause_true_lit_thres[c] - sat_count[c], weight);
					}
					else
						tem_score -= unit_weight[c] * weight;
				}
				else if (sat_count[c] >= clause_true_lit_thres[c])
				{
					if (sense == cur_soln[v])
					{
						tem_score -= unit_weight[c] * max(0, clause_true_lit_thres[c] - sat_count[c] + weight);
					}
				}
			}
			else
			{
				if (sat_count[c] < clause_true_lit_thres[c])
				{
					if (sense != cur_soln[v])
					{
						tem_sscore += unit_weight[c] * min(clause_true_lit_thres[c] - sat_count[c], weight);
					}
					else
						tem_sscore -= unit_weight[c] * weight;
				}
				else if (sat_count[c] >= clause_true_lit_thres[c])
				{
					if (sense == cur_soln[v])
					{
						tem_sscore -= unit_weight[c] * max(0, clause_true_lit_thres[c] - sat_count[c] + weight);
					}
				}
			}
		}
		if (tem_score != hscore[v] || tem_sscore != sscore[v])
		{

			cout << "score is worng in variable " << v << endl;
			cout << "tem_score is " << tem_score << endl;
			cout << "score function is " << hscore[v] << endl;
			cout << "flip num is " << step << endl;

			for (i = 0; i < var_lit_count[v]; ++i)
			{
				c = var_lit[v][i].clause_num;
				sense = var_lit[v][i].sense;
				weight = var_lit[v][i].weight;
				cout << c << " ";
			}
			cout << endl;
			exit(0);
			break;
		}
	}

	int tem_unsat_softweight = 0;
	for (int i = 0; i < num_clauses; ++i)
	{
		if (org_clause_weight[i] == top_clause_weight)
			continue;
		if (sat_count[i] < clause_true_lit_thres[i])
		{
			tem_unsat_softweight += (clause_true_lit_thres[i] - sat_count[i]);
		}
	}
	if (tem_unsat_softweight != soft_unsat_weight)
	{
		cout << "verify softunsat weight wrong " << endl;
		exit(0);
	}
}

void start_timing()
{
	times(&start_time);
}

double get_runtime()
{
	struct tms stop;
	times(&stop);
	return (double)(stop.tms_utime - start_time.tms_utime + stop.tms_stime - start_time.tms_stime) / sysconf(_SC_CLK_TCK);
}
