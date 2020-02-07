/*
 * 	utils.h: useless tools i love seeing
 *	obviously, it's a list of utilities i made to make my life easier.
 *	check the prototypes for descriptions and functions.
 *
 *  Created on: 11/03/2019
 *  Version: 0.1 (11/04/2019)
 *  Author: Leo
 */

#ifndef UTILS_H_
#define UTILS_H_

//prototypes
void sysexit(int a);				//system("pause") made shittier/safer
void inflush(void);				    //flushes input buffer, pauses if it's already empty
void flushfgets(char*);			    //removes \n from string captured with fgets
void ascanf(const char*, void*);	//scanf with inflush after
bool XOR(bool, bool);               //exclusive OR operation between two booleans
bool YN(void);                      //yes or no?

#endif /* UTILS_H_ */
