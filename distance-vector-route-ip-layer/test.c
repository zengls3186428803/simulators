#include <stdio.h>
static int dv[4][4];
static int flag_entity = 0;
void rtinit0() {
	printf("rtinit0\n");
	int i,j;
	for(i = 0; i < 4; ++i) {
		printf("i = %d ",i);
		if(i == flag_entity) {
			for(j = 0; j < 4; ++j) {
				dv[i][j] = i * j;
			}
		} else {
			for(j = 0; j < 4; ++j) {
				dv[i][j] = 999;
			}
		}
	}
	printf("****");
}

int main() {
	rtinit0();
}
