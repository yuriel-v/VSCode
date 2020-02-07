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

void inflush(void) {
    using namespace std;
    string temp;
    getline(cin, temp);
}

void flushfgets(char* a) {
	if ( a[strlen(a)-1] == '\n' ) a[strlen(a)-1] = 0;
}

void ascanf(const char* c, void* p) {
	scanf(c, p); inflush();
}

bool XOR(bool a, bool b) {
	return (a + b) % 2;
}

bool YN(void) {
    char input;
    std::cout<<">> "; std::cin.get(input); inflush();

    input = toupper(input);
    //assures input is either Y or N
    while (input != 'Y' && input != 'N') {
        printf("(Y/N) >> ");
        std::cin.get(input); inflush();
        input = toupper(input);
    }
    return (input == 'Y')? true : false;
}