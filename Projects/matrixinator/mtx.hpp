/*
 * classes header file for the almighty matrixinator
 * --------------------------------------------------------------------------------------------------
 * mtx: class 'matrix'
 * this class stores information about the 'triangle' data values (similarity) and number of samples,
 * along with methods for reading that data, reading the output spreadsheet for the US samples'
 * locations relative to the data matrix itself and identifying whether a provided index is a
 * US sample's index or not.
 *
 * please note, the data matrix has to be provided in the form of a double pointer to pointer, aka
 * a dynamically allocated matrix, declared elsewhere in the code. this is to circumvent the normal
 * restriction C++ imposes on static allocation matrices, since this program was made to work with
 * very large matrices (1000x1000).
 *
 * --------------------------------------------------------------------------------------------------
 * SSheet: class 'spreadsheet'
 * this class is meant simply for storage of the entire 'spreadsheet' .csv file, with methods to copy
 * the file to memory and write the memory's contents into a file. it may be used to create an entirely
 * new file or overwrite a currently existing spreadsheet (truncate).
 *
 * -Leo, 21-Jan-2020
*/

#ifndef SOURCE_MTX_HPP_
#define SOURCE_MTX_HPP_

#include <string>
#include <vector>

class mtx {
public:
	//attributes
	double **data;					//1000x1000 matrices are too large, so dynamic allocation is in play
	std::vector<int> usa;
	int numSamples = 0;
	std::string SSinput, Tri;

	//methods
	bool isUS(int);
	std::string firstRun(void);
	bool YN(void);

	//back-end attributes & methods
private:
	bool usaLoaded, dataLoaded;

	bool dataRead(std::string);
	bool usaRead(std::string);
	void manualLoad(void);
};

class SSheet {
public:
	//attributes
	double octagon[8];

	//methods
	void writeSelf(std::string, bool, bool, bool);
	void readSelf(std::string, int);
private:
	std::string Key, Location, CollectionDate, Company, FSGID, Farm, Age_days, SampleOrigin,
	SampleType, VMP, ibeA, traT, iutA, ompT, sitA, irp2, cvaC, tsh, iucC, iss;
	//BS22, BS15, BS3, BS8, BS27, BS84, BS18, BS278 = octagon values (for title purposes)
};

#endif /* SOURCE_MTX_HPP_ */
