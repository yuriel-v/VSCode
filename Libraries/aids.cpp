#include <iostream>         //cin and cout
#include <string>           //string type
#include <sstream>          //converting strings to numbers
#include <cctype>           //isalpha, ispunct
#include <limits>           //for flushing the max number of chars
#include <cstdlib>          //for LAUGHERS :pepeLaugh:
#include "aids.hpp"
using namespace std;        //so we don't have to type std:: before most functions

//implementation block ----------------------------------------------------------------------
//sorting: will return 1 and assign values if the input is correct. returns 0 and nothing else otherwise.
/*  syntax: asort("%dt", str, &var);
*   where %dt is the data type ("%d" or "%i" for int, for instance), str is the string literal to be converted into
*   a digit and &var is the target variable's address. best used when called by acatch.
*/
int asort(string dt, string str, void* p)
{
    //int block
    if (dt != "%f" && dt != "%lf" && dt != "%Lf") {
        for (int i = 0; str[i] != '\0'; i++) {
            if (isalpha(str[i]) != 0)
                return 0;
            else if (ispunct(str[i])) //this isn't floating point!
                return 0;
        }
        if (dt == "%d" || dt == "%i")
            stringstream(str)>> *(int*)p;
        else if (dt == "%u")
            stringstream(str)>> *(unsigned int*)p;
        else if (dt == "%hi")
            stringstream(str)>> *(short int*)p;
        else if (dt == "%hu")
            stringstream(str)>> *(unsigned short int*)p;
        else if (dt == "%li")
            stringstream(str)>> *(long int*)p;
        else if (dt == "%lu")
            stringstream(str)>> *(unsigned long int*)p;
        else if (dt == "%lli")
            stringstream(str)>> *(long long int*)p;
        else if (dt == "%llu")
            stringstream(str)>> *(unsigned long long int*)p;
        else {
            return 0;
        }
    return 1;
    }
    //float block
    else {
        short int fpoint = 0; //dot counter.
        for (int i = 0; str[i] != '\0'; i++) {
            if (isalpha(str[i]) != 0)
                return 0;
            else if (ispunct(str[i])) {
                if (str[i] != '.')
                    return 0;
                else
                    fpoint++;
            }
        }
        if (fpoint == 1 || fpoint == 0) {
            if (dt == "%f" || dt == "%F")
                stringstream(str)>> *(float*)p;
            else if (dt == "%lf" || dt == "%lF")
                stringstream(str)>> *(double*)p;
            else if (dt == "%Lf" || dt == "%LF")
                stringstream(str)>> *(long double*)p;
            return 1;
        }
        //if the input has two or more dots, reject.
        else return 0;
    }
}
//catching: will capture a text string and convert it to a number if the specified data type matches. asks again otherwise.
/*  syntax: acatch("%dt", &var);
*   where "%dt" is the data type ("%d" or "%i" for int, for instance) and &var is the target variable's address.
*/
void acatch(string dt, void* p)
{
    bool chk;
    string str;
    do {
        getline(cin, str);
        chk = asort(dt, str, p); //this is why asort returns something.
        if (!chk)
            cout<<"Invalid input. Try again: ";
    } while (!chk);
}