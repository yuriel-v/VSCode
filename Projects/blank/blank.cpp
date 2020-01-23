//standard library
#include <cstdio>
#include <iostream>
#include <vector>
//custom
#include "utils.hpp"

int main()
{
    std::vector<std::string> message {"hello", "WARUDO", "!!"};

    int i = 0;
    for (const std::string& word : message) {
        printf("%s ", word.c_str());

        ++i;
    }

    sysexit(0);
}