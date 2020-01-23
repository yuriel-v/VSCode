/*
 * The Matrixinator v0.1 (alpha)
 * a simple utility i made for my assistant Joshu for his laboratory endeavours.
 * its purpose lies in fetching a data matrix, containing samples and the similarity
 * between them, along with a spreadsheet file to identify foreign samples from US samples.
 *
 * after all is said and done, it's capable of inferring the following values:
 * BS22, BS15, BS3, BS8, BS27, BS84, BS18, BS278.
 * then outputting the inferred values (if applicable) into a .csv spreadsheet. it can either
 * overwrite the input spreadsheet file or create a new one, depending on the user's choice.
 *
 * -Leo, 21-Jan-2020
 */

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include "mtx.hpp"
#include "utils.hpp"


void nline() {printf("\n==================================================\n");}

int main()
{
	using namespace std;
	cout<<"The Matrixinator v0.2 (alpha)"; nline();
	bool debug = false, SSdebug = false;
	mtx matrix;

	//allocate matrix here
	double** dat = new double*[1000];
		for (int i = 0; i < 1000; ++i)
		    dat[i] = new double[1000];
	matrix.data = dat; //then pass it to class as pointer

	//============================ data input ============================

	string SSinput = matrix.firstRun();

	//load spreadsheet to memory
	cout<<"Loading spreadsheet to memory... ";
	SSheet SS[matrix.numSamples]; //schutzstaffel (^:
	for (int i = 0; i < matrix.numSamples; ++i)
		SS[i].readSelf(SSinput, i);

	cout<<"done.\n====="<<endl;

	//============================ diagnostics ============================
	if (debug) printf("Debug mode enabled. Diagnostics are as follows.\n"
			"We currently possess %d samples - %d of which are US samples.\n"
			"=====\n", matrix.numSamples-1, (int) matrix.usa.size());
	if (SSdebug) {
		printf("\n=====\n"
				"Spreadsheet debug has been enabled.\n");
		fstream output("test.csv", fstream::out | fstream::trunc);
		output<<"Key,Location,CollectionDate,Company,FSGID,Farm,Age_days,SampleOrigin,SampleType,VMP,ibeA,traT,iutA,ompT,sitA,irp2,cvaC,tsh,iucC,iss"
			  <<",BS22,BS15,BS3,BS8,BS27,BS84,BS18,BS278\n";
		output.close();
		for (int i = 0; i < matrix.numSamples-1; ++i)
			SS[i].writeSelf("test.csv", false, true, true);

		printf("A copy of the spreadsheet in memory has been created, titled \"test.csv\".\n=====\n");
	}

	//============================ processing ============================

	printf("Sweeping matrix... ");

	//reminder: 'column' is the foreign sample being analyzed, 'row' is the US sample being compared against it
	int caseCount; vector<int> matches;
	for (int column = 0; column < matrix.numSamples - 1; ++column) {
		if (matrix.isUS(column)) continue;

		matches.clear();
		caseCount = 0;
		for (int row = column + 1; row < matrix.numSamples; ++row)
			if (matrix.isUS(row) && matrix.data[row][column] >= 80) {
				++caseCount;
				matches.insert(matches.end(), row);
			}

		switch (caseCount) {
		case 1:
			for (int i = 0; i < 8; ++i)
				SS[column].octagon[i] = SS[matches[0]].octagon[i];
		break;
		default:
			for (int i = 0; i < 8; ++i) {
				double foreignOctagon = 0, simSum = 0;
				for (int j = 0; j < (int)matches.size(); ++j) {
					foreignOctagon += (SS[matches[j]].octagon[i]) * (matrix.data[matches[j]][column]);
					simSum += matrix.data[matches[j]][column];
				}
				foreignOctagon /= simSum;
				SS[column].octagon[i] = foreignOctagon;
			}
		}
	}
	printf("done.\n=====");

	//============================ data output ============================
	bool overwrite = true;
	string fileName;
	printf("\nWould you like to overwrite the original spreadsheet with the new values?\n"
			"Type 0 or 'false' to keep the old spreadsheet, or anything else to overwrite it.\n>> ");
	cin>>overwrite; inflush();
	if (overwrite) { printf("Understood. Overwriting contents.\n"); fileName = SSinput; }
	else {
		printf("Understood. The new output file will be titled \"output.csv\" and placed in the same folder as the executable.\n"
				"Please note, if there is already another \"output.csv\" file in this folder, it'll be overwritten.\nPress enter to continue.");
		fileName = "output.csv"; getchar();
	}
	printf("\nWriting output spreadsheet...");
	for (int i = 0; i < matrix.numSamples-1; ++i) {
		if (!i) SS[0].writeSelf(fileName, true, false, true); //title first, content later
		SS[i].writeSelf(fileName, false, false, false);
	}
	printf("done.\n=====");

	printf("\nTerminating program execution.\n");
	sysexit(0);
}
