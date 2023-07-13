#include "settings.h"
#include "init.h"
#include "heuristic.h"

void default_algorithm_settings() {
    algorithm_settings_default();
}
void algorithm_settings_default() 
{
    init_assignment_ptr = init_assignment_false;
    //init_assignment_ptr = init_assignment_true;
    //init_assignment_ptr = init_assignment_random;
    
    flip_ptr = flip_with_neighbor;
    select_var_after_update_weight_ptr = select_var_after_update_weight_2;
}
