/*
 * classes header file for the almighty matrixinator
 * --------------------------------------------------------------------------------------------------
 * mtx: class 'matrix'
 * this class is made to store a pointer to a variable size similarity matrix, generated by a dendrogram
 * stored in the tree class. it also includes auxiliary methods to check if a sample is from the USA,
 * as well as taking note of which samples are these.
 *
 * please note, the data matrix has to be provided in the form of a double pointer to pointer, aka
 * a dynamically allocated matrix, declared elsewhere in the code. this is to circumvent the normal
 * restriction C++ imposes on static allocation matrices, since this program was made to work with
 * very large matrices (1000x1000 and larger).
 *
 * --------------------------------------------------------------------------------------------------
 * SSheet: class 'spreadsheet'
 * this class is meant simply for storage of the entire 'spreadsheet' .csv file, with methods to copy
 * the file to memory and write the memory's contents into a file. it may be used to create an entirely
 * new file or overwrite a currently existing spreadsheet (truncate).
 * 
 * --------------------------------------------------------------------------------------------------
 * tree: class 'tree'
 * this class reads and stores a dendrogram read from a .xml file and generates a similarity matrix
 * based on it.
 * the similarity value between any two samples is the similarity value of the closest compound node that
 * contains both samples.
 *
 * -Leo, 1-Feb-2020
*/

#ifndef SOURCE_MTX_HPP_
#define SOURCE_MTX_HPP_

#include <string>
#include <vector>
#include <set>

class mtx {
public:
    //attributes
    double **data;    //for very large matrices
    std::vector<int> usa;
    int numSamples = 0;
    std::string SSinput, TreeFile, SSoutput;
    bool overwrite;

    //methods
    bool isUS(int);
    bool usaRead(void);
    void countSamples(std::string);
    void outputOptions(void);
};

class SSheet {
public:
    //attributes
    double octagon[8];
    int node;

    //methods
    void writeSelf(std::string, bool, bool);
    void readSelf(std::string, int);
private:
    std::string Key, Location, CollectionDate, Company, FSGID, Farm, Age_days, SampleOrigin,
    SampleType, VMP, ibeA, traT, iutA, ompT, sitA, irp2, cvaC, tsh, iucC, iss;
    //BS22, BS15, BS3, BS8, BS27, BS84, BS18, BS278 = octagon values (for title purposes)
};

class tree {
public:
    //attributes
    int ID, parentID;
    tree *parentAddress;
    double similarity;
    std::set<int> list;
    bool isSample;

    //methods
    void readSelf(std::string, int);
};


//class for methods alone
class fmtx {
public:
    //input
    void firstRun(std::string*, std::string*);
    void manualLoad(std::string*, std::string*);
    int nodeNumber(std::string);
    bool isLoaded(std::string);
    void nline();

    //post-init
    void bulldozer(tree*);  // new branching function
    double bullSim(tree*, int);  // new similarity finder
    void treeGrow(mtx*, std::vector<tree>&, int);
    void postInit(mtx*, std::vector<tree>&, int, std::vector<SSheet>&);

    //processing
    void matrixSweep(mtx*, std::vector<SSheet>&);
};

#endif /* SOURCE_MTX_HPP_ */
