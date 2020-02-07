/*
 * The Matrixinator v0.6 (alpha)
 * a simple utility i made for my assistant Joshu for his laboratory endeavours.
 * its purpose lies in fetching a dendrogram containing samples and the similarity
 * between them, along with a spreadsheet file to identify foreign samples from US samples.
 *
 * using the dendrogram provided in a .xml file, it creates a similarity matrix between all
 * the samples specified.
 * 
 * after the similarity matrix is generated, it's capable of inferring the following values:
 * BS22, BS15, BS3, BS8, BS27, BS84, BS18, BS278. these calculations are made based on the
 * generated similarity matrix.
 * it then outputs the inferred values (if applicable) into a .csv spreadsheet. it can either
 * overwrite the input spreadsheet file or create a new one, depending on the user's choice.
 *
 * -Leo, 5-Feb-2020
 */

#include <iostream>
#include <vector>
#include <string>
#include "mtx.hpp"
#include "utils.hpp"

int main()
{
    //============================ initialization ============================
    using namespace std;
    fmtx fc; //"functions"
    cout<<"The Matrixinator v0.6 (alpha)"; fc.nline();
    int nodes;

    //============================ data input ============================

    mtx matrix;
    fc.firstRun(&matrix.TreeFile, &matrix.SSinput); matrix.outputOptions();
    matrix.usaRead(); nodes = fc.nodeNumber(matrix.TreeFile);

    vector<SSheet> SS (matrix.numSamples); //schutzstaffel (^:
    vector<tree> acacia (nodes+1);

    //grow tree
    cout<<"Growing tree... ";
    fc.treeGrow(&matrix, acacia, nodes);
    cout<<"done."<<endl;

    //load spreadsheet to memory
    cout<<"Loading spreadsheet to memory... ";
    for (int i = 0; i < matrix.numSamples; ++i)
        SS[i].readSelf(matrix.SSinput, i);
    cout<<"done.\n====="<<endl;

    //============================ post-initialization ============================

    printf("Commencing post-initialization...\n");
    //allocate matrix here
    printf("- Allocating matrix... ");
    double** dat = new double*[matrix.numSamples];
    for (int i = 0; i < matrix.numSamples; ++i)
        dat[i] = new double[matrix.numSamples];
    matrix.data = dat; //and pass it to mtx object as pointer.
    printf("done.\n");

    //other subroutines
    fc.postInit(&matrix, acacia, nodes, SS);

    //============================ processing ============================

    printf("Sweeping matrix... ");
    fc.matrixSweep(&matrix, SS);
    printf("done.\n=====\n");

    //============================ data output ============================

    printf("Writing output spreadsheet... ");
    SS[0].writeSelf(matrix.SSoutput, true, true); //title first, content later
    for (int i = 0; i < matrix.numSamples; ++i)
        SS[i].writeSelf(matrix.SSoutput, false, false);
    printf("done.\n=====");

    printf("\nTerminating program execution.");
    sysexit(0);
}
