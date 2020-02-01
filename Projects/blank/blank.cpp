#include <cstdio>
#include <iostream>
#include "utils.hpp"

#include <string>
#include <vector>
#include <tuple>
#include <functional>

/*
int main() {
	using namespace std;
	fstream fs ("C:\\Users\\Leo\\Desktop\\matrix.txt", fstream::in);
	if (!fs.is_open()) { cout<<"error opening file, terminating."; sysexit(1); }
	printf("printing values found in matrix.txt.\n");

	//string in;
	int mtx[5][5], num = 0;
	for (int i = 0; !fs.eof() && i < 5; ++i) {
		for (int j = 0; j <= i; ++j) {
			if (fs.eof()) break;
			else {
				if (!j) fs.ignore(20, ' ');
				fs >> mtx[i][j];
				//stringstream(in) >> mtx[i][j];
			}
		}
		fs.ignore(256, '\n'); ++num;
	}
	printf("printing generated matrix.\n");
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j <= i; ++j)
			printf("%i ", mtx[i][j]);
		printf("\n");
	}
	printf("%d rows were processed.", num);
	fs.close();
	sysexit(0);
}*/
/*
typedef struct {
	int i;
	float f;
	double d;
}data;

int main()
{
	data var[10];
	data* pt[10];

	for (int i = 0; i < 10; ++i)
		pt[i] = &var[i];

	for (int i = 0; i < 10; ++i) {
		pt[i]->i = i;
		pt[i]->f = i + 0.5;
		pt[i]->d = i + 0.25;
	}

	printf("var's contents:\n");
	for (int i = 0; i < 10; ++i) {
		printf("value = i: %d | f: %f | d: %lf\n", var[i].i, var[i].f, var[i].d);
		printf("reference = i: %d | f: %f | d: %lf\n--\n", pt[i]->i, pt[i]->f, pt[i]->d);
	}

	sysexit(0);
}*/

int main()
{
	using namespace std;
	string str;
	getline(cin, str);
	cout<<str;

	sysexit(0);
}
