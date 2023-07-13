#ifndef _PARSE_ARGUMENTS_H
#define _PARSE_ARGUMENTS_H

#include "basis_pms.h"

struct Config
{
	std::string input_file;
	uint seed;

	Config() : seed(1) {}
};

bool parse_args(int argc, char *argv[], Config& cfg);
bool parse_arguments(int argc, char **argv);


#endif