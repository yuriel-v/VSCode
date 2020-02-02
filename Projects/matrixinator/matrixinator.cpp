/*
 * The Matrixinator v0.4 (alpha)
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
 * -Leo, 2-Feb-2020
 */

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include "mtx.hpp"
#include "utils.hpp"

int main()
{
    //============================ initialization ============================
    using namespace std;
    cout<<"The Matrixinator v0.4 (alpha)"; nline();
    bool debug = false, SSdebug = false;
    int nodes;

    //============================ data input ============================

    mtx matrix;
    firstRun(&matrix.TreeFile, &matrix.SSinput); matrix.outputOptions();
    matrix.usaRead(matrix.SSinput);
    matrix.countSamples(matrix.SSinput); nodes = nodeNumber(matrix.TreeFile);

    vector<SSheet> SS (matrix.numSamples); //schutzstaffel (^:
    vector<tree> acacia (nodes+1);

    //grow tree
    cout<<"Growing tree... ";
    for (int i = 0; i <= nodes; ++i) {
        if (i == 0) {
            acacia[0].ID = 0;
            acacia[0].parentID = 0;
            acacia[0].similarity = 0;
            acacia[0].isSample = false;
            acacia[0].parentAddress = &acacia[0];
        }
        else
            acacia[i].readSelf(matrix.TreeFile, i-1);
    }
    for (int i = 1; i <= nodes; ++i)
        acacia[0].list.insert(i);
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

    printf("- Deorphanizing tree... ");
    //fill out the memory address of each node's parent.
    for (int i = 1; i <= nodes; ++i) {
        int id = acacia[i].parentID;
        acacia[i].parentAddress = &(acacia[id]);
    }
    printf("done.\n");
    
    printf("- Branching tree... ");
    //insert node #i's ID into its parent's list.
    for (int i = 1; i <= nodes; ++i)
        if (acacia[i].isSample)
            bulldozer(&acacia[i]);
    printf("done.\n");

    //associate samples with nodes
    printf("- Performing sample-node association... ");
    int sCount = 0;
    for (int i = 1; i <= nodes; ++i) {
        if (acacia[i].isSample) {
            SS[sCount].node = i;
            ++sCount;
        }
    }
    printf("done.\n");

    if (debug) {
        printf("Debug mode enabled. Diagnostics are as follows.\n"
            "We currently possess %d samples - %d of which are US samples.\n"
            "Press enter to continue.\n"
            "=====\n", matrix.numSamples-1, (int) matrix.usa.size());
        cin.get();
    }

    //generate matrix
    printf("- Generating similarity matrix - this might take a while.\n");
    for (int row = 0; row < matrix.numSamples; ++row) {
        for (int column = 0; column < matrix.numSamples; ++column) {
            if ( !XOR(matrix.isUS(row), matrix.isUS(column)) ) continue;
            
            if (row == column)
                matrix.data[row][column] = 100;
            else if (row > column)
                matrix.data[row][column] = matrix.data[column][row];
            else {
                int colid = SS[column].node, rowid = SS[row].node;
                //compare row to column
                //matrix.data[row][column] = simCompare(&acacia, rowid, colid);
                matrix.data[row][column] = bullSim(&acacia[rowid], colid);
            }
        }
        printf("\r-> Row %d done.", row);
    }
    printf("\n- Similarity matrix generated.\n=====\n");

    //============================ diagnostics ============================
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
    for (int column = 0; column < matrix.numSamples; ++column) {
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
    printf("done.\n=====\n");

    //============================ data output ============================

    printf("Writing output spreadsheet... ");
    for (int i = 0; i < matrix.numSamples; ++i) {
        if (i == 0) SS[i].writeSelf(matrix.SSoutput, true, false, true); //title first, content later
        SS[i].writeSelf(matrix.SSoutput, false, false, false);
    }
    printf("done.\n=====");

    printf("\nTerminating program execution.");
    sysexit(0);
}
