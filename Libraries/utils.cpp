//includes
#include <string>
#include <cstring>
#include <iostream>
#include <cstdio>
#include "utils.hpp"

//implementations
void sysexit(int a) {
	std::cout<< "\n\nProcess returned "<< a <<". Press enter to continue." << std::flush;
	//std::cin.clear(); std::cin.ignore(INT_MAX, '\n');
	std::cin.get(); exit(a);
}

void inflush(void) { char c; while ((c = std::cin.get()) != '\n' && c != EOF); }

void flushfgets(char* a) { if ( a[strlen(a)-1] == '\n' ) a[strlen(a)-1] = 0; }

void ascanf(const char* c, void* p) { scanf(c, p); inflush(); }