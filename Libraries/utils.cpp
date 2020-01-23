//includes
#include <string>
#include <cstring>
#include <iostream>
#include <cstdio>
#include "utils.hpp"

//implementations
void sysexit(int a) {
	printf("\n\nProcess returned %d. Press enter to continue.\n", a);
	std::cin.get(); exit(a);
	//warning: this function does NOT pause the system if the input buffer isn't empty!
}

void inflush(void) { char c; while ((c = std::cin.get()) != '\n' && c != EOF); }

void flushfgets(char* a) { if ( a[strlen(a)-1] == '\n' ) a[strlen(a)-1] = 0; }

void ascanf(const char* c, void* p) { scanf(c, p); inflush(); }