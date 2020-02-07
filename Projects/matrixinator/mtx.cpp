/*
 * method implementation file: the matrixinator
 * for more details, check the header file.
 * 
 * -Leo, 1-Feb-2020
 * 
*/

#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <cstddef>
#include <set>
#include "utils.hpp"
#include "mtx.hpp"

// =============================== fmtx ===============================

void fmtx::postInit(mtx* matrix, std::vector<tree>& acacia, int nodes, std::vector<SSheet>& SS)
{
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

    //generate matrix
    printf("- Generating similarity matrix - this might take a while.\n");
    for (int row = 0; row < matrix->numSamples; ++row) {
        for (int column = 0; column < matrix->numSamples; ++column) {
            
            if (row == column)
                matrix->data[row][column] = 100;
            
            else if (row > column)
                matrix->data[row][column] = matrix->data[column][row];
                //similarity matrices are mirrored

            else {
                int colid = SS[column].node, rowid = SS[row].node;
                matrix->data[row][column] = bullSim(&acacia[rowid], colid);
            }
        }
        printf("\r-> Row %d done.", row);
    }
    printf("\n- Similarity matrix generated.\n=====\n");
}

void fmtx::treeGrow(mtx* matrix, std::vector<tree>& acacia, int nodes)
{
    acacia[0].ID = 0;
    acacia[0].parentID = 0;
    acacia[0].similarity = 0;
    acacia[0].isSample = false;
    acacia[0].parentAddress = &acacia[0];
    for (int i = 1; i <= nodes; ++i)
            acacia[i].readSelf(matrix->TreeFile, i-1);
    
    for (int i = 1; i <= nodes; ++i)
        acacia[0].list.insert(i);
}

void fmtx::matrixSweep(mtx* matrix, std::vector<SSheet> &SS) {
    using namespace std;
    for (int foreign = 0; foreign < matrix->numSamples; ++foreign) {
        if (matrix->isUS(foreign)) continue; //do not process US samples!

        int caseCount = 0; vector<int> matches;
        for (vector<int>::const_iterator it = matrix->usa.begin(); it != matrix->usa.end(); ++it)
            if (matrix->data[foreign][*it] >= 80) {
                ++caseCount;
                matches.push_back(*it);
            }
        
        switch (caseCount) {
        //case 0, do nothing 
        case 1:
            for (int iteration = 0; iteration < 8; ++iteration)
                SS[foreign].octagon[iteration] = SS[matches[0]].octagon[iteration];
        break;
        default:
            //multiply value by the similarity of that particular usa sample to the foreign sample
            double tempOct[matches.size()][8];
            int i = 0;
            for (vector<int>::iterator it = matches.begin(); it != matches.end(); ++it) {
                double curSim = matrix->data[foreign][*it];
                for (int j = 0; j < 8; ++j)   //octagon value #j
                    tempOct[i][j] = SS[*it].octagon[j] * curSim;
                ++i;
            }

            //add up all the similarities for every usa sample matched
            double tempSim = 0;
            for (vector<int>::iterator it = matches.begin(); it != matches.end(); ++it)
                tempSim += matrix->data[foreign][*it];
            
            //add up all of the octagon values for every usa sample
            double foreignOctagon[8] = {0, 0, 0, 0, 0, 0, 0, 0};
            for (int i = 0; i < (int)matches.size(); ++i)
                for (int j = 0; j < 8; ++j)
                    foreignOctagon[j] += tempOct[i][j];
            
            //divide by the total similarity + print to octagon for foreign sample
            for (int i = 0; i < 8; ++i) {
                foreignOctagon[i] /= tempSim;
                SS[foreign].octagon[i] = foreignOctagon[i];
            }
        }
    }
}
void fmtx::nline() {printf("\n==================================================\n");}

//compare node's similarity to compID
double fmtx::bullSim(tree* node, int compID) {
    if (!node->isSample) return 0;

    tree* parent = node->parentAddress;

    while (parent->ID >= 1) {
        //if the parent's similarity is below 80%, it's no longer a possible match
        if (node->similarity < 80)
            return 0;

        //sweep parent node's list for the ID being compared
        for (std::set<int>::const_iterator it = (parent->list).begin(); it != (parent->list).end(); ++it)
            if (*it == compID)
                return parent->similarity;
        //if parent doesn't contain compNode, node becomes parent, parent becomes grandparent.
        node = parent;
        parent = parent->parentAddress;
    }
    //if all else fails, return the similarity of node 1
    return node->similarity;
}

//carry all the node IDs from sample to root, adding them to the nodes' lists along the way
void fmtx::bulldozer(tree *node)
{
    if (!node->isSample) return;

    std::vector<int> changeList;
    int parID = (node->parentAddress)->ID;

    while (parID >= 1) {
        changeList.push_back(node->ID);
        node = node->parentAddress;
        parID = (node->parentAddress)->ID;

        //add the change list to the parent's list
        for (std::vector<int>::const_iterator it = changeList.begin(); it != changeList.end(); ++it)
            (node->list).insert(*it);
    }
    //we're at node 1 at this point, parent ID 0
    for (std::vector<int>::const_iterator it = changeList.begin(); it != changeList.end(); ++it)
        (node->list).insert(*it);
}

//fetch the number of nodes within a .xml file
int fmtx::nodeNumber(std::string path)
{
    std::fstream file (path, std::fstream::in);
    if (!file.is_open()) return -1;
    file.seekg(-1, std::ios_base::end);

    int count = 0;
    while (count < 3) {
        file.seekg(-1, std::ios_base::cur);
        if (file.peek() == '=') ++count;
    }
    file.ignore(5, '\"');
    std::string buf; getline(file, buf);
    std::stringstream(buf) >> count; 
    return count;
}

void fmtx::manualLoad(std::string* TreeFile, std::string* SSfile)
{
    bool TreeLoaded = false, SSloaded = false;
    while (!TreeLoaded || !SSloaded) {
        printf("Warning: Use forward slashes (/) or double back slashes (\\\\) for file path!\n"
            "Additionally, at either prompt, type 'exit' (case-sensitive) to terminate the program.\n-\n");
        
        if (!TreeLoaded) {
            printf("Please input the Dendrogram (tree) file's location.\n>> ");
            getline(std::cin, *TreeFile);
            if (TreeFile->compare("exit") == 0) { printf("Understood. Exiting program."); sysexit(0); }
        }

        if (!SSloaded) {
            printf("Please input the Spreadsheet (csv) file's location.\n>> ");
            getline(std::cin, *SSfile);
            if (SSfile->compare("exit") == 0) { printf("Understood. Exiting program."); sysexit(0); }
        }
        printf("\nLoading files to memory... ");
        TreeLoaded = isLoaded(*TreeFile);
        SSloaded = isLoaded(*SSfile);

        if (TreeLoaded && SSloaded) printf("done.\n");
        else {
            printf("error loading the following files. Try again.\n-> If you wish to quit the program, simply type 'exit')\n-\n");

            printf("Dendrogram (tree) file: ");
            (TreeLoaded)? printf("loaded.\n") : printf("failed.\n");

            printf("Spreadsheet file: ");
            (SSloaded)? printf("loaded.\n-\n") : printf("failed.\n-\n");
        }
    }

    //.ini stuff
    printf("Would you like to save the current files' paths into an initialization file? (Y/N)\n");
    if (YN()) {
        printf("Understood. Saving current file paths to \"mtx.ini\", which will be placed in the\n"
               "same folder as the executable.\n");

        std::size_t SSI = SSfile->find_last_of("/\\"), TreeI = TreeFile->find_last_of("/\\");

        std::fstream ini ("mtx.ini", std::fstream::out | std::fstream::trunc);
        ini << "[Matrixinator Configuration File]\n";
        ini << "SpreadSheetPath=" << SSfile->substr(0, SSI+1) << '\n';
        ini << "TrianglePath=" << TreeFile->substr(0, TreeI+1) << '\n';
    }
    else
        printf("Understood. Resuming program execution.\n");
}

bool fmtx::isLoaded(std::string path) {
    bool ret;
    std::fstream file (path, std::fstream::in);
    ret = file.is_open();
    file.close();
    return ret;
}

//fetch the files, reading them is done on other methods.
void fmtx::firstRun(std::string* TreeFile, std::string* SSfile)
{
    using namespace std;
    bool TreeLoaded = false, SSloaded = false;
    fstream ini ("mtx.ini", fstream::in);

    if (ini.is_open()) {
        printf("Initialization file found. Do you wish to load?\n"
                "You will be asked for input files' folders otherwise. (Y/N)\n");
        if (YN()) {
            printf("Understood. Loading configuration file... ");

            ini.ignore(256, '=');
            std::getline(ini, *SSfile, '\n');
            ini.ignore(256, '=');
            std::getline(ini, *TreeFile, '\n');

            printf("done.\n\n");

            while (!TreeLoaded || !SSloaded) {
                printf("To quit the program, type 'exit' (case-sensitive) at any prompt.\n-\n");
                string TreeName, SSname;
                TreeName.clear(); SSname.clear();

                if (!TreeLoaded) {
                    printf("Please input the Dendrogram (tree) file's name: ");
                    getline(cin, TreeName);
                    if (TreeName.compare("exit") == 0) { printf("Understood. Exiting program."); sysexit(0); }
                }

                if (!SSloaded) {
                    printf("Please input the Spreadsheet (csv) file's name: ");
                    getline(cin, SSname);
                    if (SSname.compare("exit") == 0) { printf("Understood. Exiting program."); sysexit(0); }
                }

                printf("\nLoading files to memory... ");
                TreeLoaded = isLoaded((*TreeFile) + TreeName);
                SSloaded = isLoaded((*SSfile) + SSname);

                if (TreeLoaded && SSloaded) {
                    *TreeFile += TreeName;
                    *SSfile += SSname;
                    cout<<"done."<<endl;
                }
                else {
                    printf("error loading the following files. Try again.\n-> If you wish to quit the program, simply type 'exit')\n-\n");

                    printf("Dendrogram (tree) file: ");
                    (TreeLoaded)? printf("loaded.\n") : printf("failed.\n");

                    printf("Spreadsheet file: ");
                    (SSloaded)? printf("loaded.\n-\n") : printf("failed.\n-\n");

                    printf("\n=====\n");
                }
            } //end-while
        } //end-if (Y/N)

        else {
            printf("Understood. Commencing manual load.\n-\n");
            ini.close(); manualLoad(TreeFile, SSfile);
        }
    }

    else manualLoad(TreeFile, SSfile);
}

// =============================== tree ===============================

//read one line to memory (iterable)
void tree::readSelf(std::string filePath, int line)
{
    std::fstream xml (filePath, std::fstream::in);
    for (int i = 0; i < line; ++i)
        xml.ignore(2048, '/');
    xml.precision(3);
    std::string buffer;

    for (int i = 0; i < 3; ++i) {
        xml.ignore(1000, '\"');
        std::getline(xml, buffer, '\"');
        switch (i) {
        case 0:
            std::stringstream(buffer)>> ID; break;
        case 1:
            std::stringstream(buffer)>> parentID; break;
        case 2:
            std::stringstream(buffer)>> similarity; break;
        }

        char c = xml.peek();
        isSample = (c == '/')? false : true;
    }
    xml.close();
}

// =============================== mtx ===============================

//check if a sample is foreign
bool mtx::isUS(int index) {
    for (std::vector<int>::const_iterator it = usa.begin(); it != usa.end(); ++it)
        if (*it == index) return true;
    return false;
}

//identify the USA samples within a file and count the samples in one go
bool mtx::usaRead(void) {
    std::fstream fs (SSinput, std::fstream::in);

    if (!fs.is_open()) return false;
    else {
        fs.ignore(INT_MAX, '\n'); //ignore header
        int cnt = 0; //position counter
        std::string in;

        while (!fs.eof()) {
            fs.ignore(64, ','); //ignore key
            getline(fs, in, ',');

            if (in.compare("US") == 0 || in.compare("USA") == 0)
                usa.insert(usa.end(), cnt);
            ++cnt;

            fs.ignore(INT_MAX, '\n');
            ++numSamples;
        }

        fs.close();
        return true;
    }
}

//count the total amount of samples within a file
void mtx::countSamples(std::string path)
{
    std::fstream fs (path, std::fstream::in);
    fs.seekg(1, std::ios_base::beg);
    fs.ignore(INT_MAX, '\n');
    std::string buff;
    while (!fs.eof()) {
        buff.clear();
        std::getline(fs, buff, ',');
        if (buff.size() > 5)
            ++numSamples;
        fs.ignore(INT_MAX, '\n');
    }
    fs.close();
}

void mtx::outputOptions(void) {
    std::cout<<"\nWould you like to overwrite the original spreadsheet with the new values? (Y/N)\n";
    overwrite = YN();

    if (overwrite) {
        SSoutput = SSinput;
        printf("Understood. Overwriting contents.\nPress enter to continue.\n");
        getchar(); return;
    }
    else {
        SSoutput = "output.csv";
        std::cout<<"Understood. The new output file will be titled \"output.csv\" and placed in the same folder as the executable.\n"
            <<"Please note, if there is already another \"output.csv\" file in this folder, it'll be overwritten.\nPress enter to continue."<< std::endl;
        getchar(); return;
    }
}

// =============================== SSheet ===============================

void SSheet::readSelf(std::string filePath, int line) {
    std::fstream sprSheet (filePath, std::fstream::in);
    for (int i = 0; i <= line; ++i)
        sprSheet.ignore(2048, '\n');

    sprSheet.precision(14);

    std::getline(sprSheet, Key, ',');
    std::getline(sprSheet, Location, ',');
    std::getline(sprSheet, CollectionDate, ',');
    std::getline(sprSheet, Company, ',');
    std::getline(sprSheet, FSGID, ',');
    std::getline(sprSheet, Farm, ',');
    std::getline(sprSheet, Age_days, ',');
    std::getline(sprSheet, SampleOrigin, ',');
    std::getline(sprSheet, SampleType, ',');
    std::getline(sprSheet, VMP, ',');
    std::getline(sprSheet, ibeA, ',');
    std::getline(sprSheet, traT, ',');
    std::getline(sprSheet, iutA, ',');
    std::getline(sprSheet, ompT, ',');
    std::getline(sprSheet, sitA, ',');
    std::getline(sprSheet, irp2, ',');
    std::getline(sprSheet, cvaC, ',');
    std::getline(sprSheet, tsh, ',');
    std::getline(sprSheet, iucC, ',');
    std::getline(sprSheet, iss, ',');

    //get octagon values from US samples only
    if (Location == "US" || Location == "USA") {
        std::string buffer;
        for (int j = 0; j < 8; ++j) {
            std::getline(sprSheet, buffer, ',');
            std::stringstream(buffer) >> octagon[j];
        }
    }
    sprSheet.close();
}

void SSheet::writeSelf(std::string filePath, bool truncate, bool title) {
    std::fstream output;
    (truncate)?
        output.open(filePath, std::fstream::out | std::fstream::trunc):
        output.open(filePath, std::fstream::out | std::fstream::app);
    output.precision(14);

    if (!output.is_open()) {
        printf("\nError writing to file!\n");
        return;
    }

    if (title) output<<"Key,Location,CollectionDate,Company,FSGID,Farm,Age_days,SampleOrigin,SampleType,VMP,ibeA,traT,iutA,ompT,sitA,irp2,cvaC,tsh,iucC,iss"
                  <<",BS22,BS15,BS3,BS8,BS27,BS84,BS18,BS278\n";
    else {
        output << Key <<","
               << Location <<","
               << CollectionDate <<","
               << Company <<","
               << FSGID <<","
               << Farm <<","
               << Age_days <<","
               << SampleOrigin <<","
               << SampleType <<","
               << VMP <<","
               << ibeA <<","
               << traT <<","
               << iutA <<","
               << ompT <<","
               << sitA <<","
               << irp2 <<","
               << cvaC <<","
               << tsh <<","
               << iucC <<","
               << iss;

            for (int j = 0; j < 8; ++j)
                output << "," << octagon[j];

        output << ",\n";
    }
    output.close();
}