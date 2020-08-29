/*************************************************************************************************************
 * Maggie Horton
 * CS-451L Summer 2020
 * lab__6.h: simple header file that holds constants and function signatures
*************************************************************************************************************/


#define PRODUCER_NO 5       //Number of producers
#define NUM_PRODUCED 2000   //Number of items to be produced

void *generator_function(void *v_param);
void *print_function(void);