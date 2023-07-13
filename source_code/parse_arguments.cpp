#include "basis_pms.h"
#include "parse_arguments.h"
#include "cmdline.h"
#include "init.h"
#include "heuristic.h"
bool parse_args(int argc, char *argv[], Config &cfg)
{
    bool ret = false;
    do
    {
        if (argc < 3 || NULL == argv)
        {
            break;
        }

        cfg.input_file = argv[1];
        
        if (!StringUtil::String2Uint(argv[2], cfg.seed))
        {
            break;
        }

        ret = true;
    } while (false);

    return ret;
}

bool parse_arguments(int argc, char **argv)
{
    /*cmdline:: parser a;
    a.add<int>("init_assignment", 'i', "init assignment", false, 1, cmdline::range(0,10));
    */
    int i;
    for (i = 4; i < argc; i++)
    {
        if (0 == strcmp(argv[i], "-init_assign"))
        {
            i++;
            if (i >= argc)
                return false;
            if (0 == strcmp(argv[i], "1"))
            {
                init_assignment_ptr = init_assignment_random;
            }
            else if (0 == strcmp(argv[i], "2"))
            {
                init_assignment_ptr = init_assignment_true;
            }
            else if (0 == strcmp(argv[i], "3"))
            {
                init_assignment_ptr = init_assignment_false;
            }
        }
        else if (0 == strcmp(argv[i], "-best"))
        {
            i++;
            if (i >= argc)
                return false;
            sscanf(argv[i], "%lld", &best_known);
            cout << "c best_known: " << best_known << endl;
            if (best_known == -1)
            {
                cout << "c no feasible solution" << endl;
                exit(0);
            }
        }
        else if (0 == strcmp(argv[i], "-rwprob"))
        {
            i++;
            if (i >= argc)
                return false;
            sscanf(argv[i], "%f", &rwprob);
            cout << "c rwprob: " << rwprob << endl;
        }
        else if (0 == strcmp(argv[i], "-initsoftw"))
        {
            i++;
            if (i >= argc)
                return false;
            sscanf(argv[i], "%f", &initsoftw);
            cout << "c initsoftw: " << initsoftw << endl;
        }
        else if (0 == strcmp(argv[i], "-hvargreedy"))
        {
            i++;
            if (i >= argc)
                return false;
            if (0 == strcmp(argv[i], "0"))
            {
                hard_var_greedy_ptr = var_greedy_hscore;
            }
            else if (0 == strcmp(argv[i], "1"))
            {
                hard_var_greedy_ptr = var_greedy_sscore;
            }
            else if (0 == strcmp(argv[i], "2"))
            {
                hard_var_greedy_ptr = var_greedy_score;
            }
        }
        else if (0 == strcmp(argv[i], "-svargreedy"))
        {
            i++;
            if (i >= argc)
                return false;
            if (0 == strcmp(argv[i], "0"))
            {
                soft_var_greedy_ptr = var_greedy_hscore;
            }
            else if (0 == strcmp(argv[i], "1"))
            {
                soft_var_greedy_ptr = var_greedy_sscore;
            }
            else if (0 == strcmp(argv[i], "2"))
            {
                soft_var_greedy_ptr = var_greedy_score;
            }
        }
        else if (0 == strcmp(argv[i], "-rdprob"))
        {
            i++;
            if (i >= argc)
                return false;
            sscanf(argv[i], "%f", &rdprob);
            cout << "c rdprob: " << rdprob << endl;
        }
        else if (0 == strcmp(argv[i], "-hinc"))
        {
            i++;
            if (i >= argc)
                return false;
            sscanf(argv[i], "%d", &h_inc);
            cout << "c h_inc: " << h_inc << endl;
        }
        else if (0 == strcmp(argv[i], "-sinc"))
        {
            i++;
            if (i >= argc)
                return false;
            sscanf(argv[i], "%d", &s_inc);
            cout << "c s_inc: " << s_inc << endl;
        }
        else if (0 == strcmp(argv[i], "-bms"))
        {
            i++;
            if (i >= argc)
                return false;
            sscanf(argv[i], "%d", &hd_count_threshold);
            cout << "c hd_count_threshold: " << hd_count_threshold << endl;
        }
        else if (0 == strcmp(argv[i], "-afterupdate"))
        {
            i++;
            if (i >= argc)
                return false;
            if (0 == strcmp(argv[i], "1"))
            {
                select_var_after_update_weight_ptr = select_var_after_update_weight_1;
            }
            else if (0 == strcmp(argv[i], "2"))
            {
                select_var_after_update_weight_ptr = select_var_after_update_weight_2;
            }
        }
    }
    return true;
}
