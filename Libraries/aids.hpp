/*  Automatic Integrated Data Sorting - AIDS: C++ version
*   ===================================================
*   Library I made in order to idiot-proof programs in regards to basic data types, such as
*   integers and floats. Mostly aimed at making sure the program properly assigns a float/int
*   to a variable in case the user (purposely or otherwise) decides to type in something else.
*   ==
*   Functions included:
*   - asort
*   - acatch
*   ==
*   That's all, folks.
*   - L, v1.2.1
*/
#ifndef AIDS_H_
#define AIDS_H_

#include <string>

//declaration block -------------------------------------------------------------------------
//sorting from strings/capturing data
extern int asort(std::string, std::string, void*);        //sorting invalid input from valid
extern void acatch(std::string, void*);              //catching numbers, making sure the correct input value is typed

#endif // AIDS_H_
