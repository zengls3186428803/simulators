#include"tool.h"
#include<stdlib.h>

double jimsrand() {
	double mmm = 2147483647;   /* largest int  - MACHINE DEPENDENT!!!!!!!!   */
	double x;                   /* individual students may need to change mmm */ 
	int y = rand();
	x = y/mmm;            /* x should be uniform in [0,1] */
	return x;
}  
