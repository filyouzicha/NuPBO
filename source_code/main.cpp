#include "basis_pms.h"

#include "settings.h"
#include "cmdline.h"
#include "parse_arguments.h"

#include <sstream>
#include <string>

//extern long long total_step;

void interrupt(int sig)
{
	print_best_solution();
	//free_memory();
	exit(10);
}

int main(int argc, char *argv[])
{
	default_algorithm_settings();
	Config cfg;
	if(!parse_args(argc, argv, cfg))
	{
		cout << "parse args failed." << endl;
		return -1;
	}
	
	//srand((unsigned)time(NULL));
	srand(cfg.seed);
	start_timing();
	signal(SIGTERM, interrupt);
	
	is_print = 0;
	vector<int> init_solution;
	build_instance(cfg.input_file.c_str());
	settings();
	parse_arguments(argc, argv);
	
	local_search(init_solution, cfg.input_file.c_str());

	//s.simple_print();
	print_best_solution();
	free_memory();

	return 0;
}
