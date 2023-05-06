#include<stdio.h>

#ifndef X
#define X 20
#endif
struct A{
	int a;
	int b;
	char c;
};
typedef struct A B;

int main() {
	printf("%d",X);
}

void f(struct A b) {
	
}
