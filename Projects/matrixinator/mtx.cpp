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

// =============================== tree ===============================

//compare curNode's similarity to compNode
double simCompare(int curNode, int compNode, tree* parent)
{
    for (std::set<int>::const_iterator it = parent->list.begin(); it != parent->list.end(); ++it) {
        if (*it == compNode)
            return parent->similarity;
    }
    //if all else fails, recursion
    return simCompare(curNode, compNode, parent->parentAddress);
}

void branch(tree *node, tree *parent)
{
    bool debug = false;
    if (parent->ID >= 1) {
        parent->list.insert(node->ID);
        if (!node->isSample)
            for (std::set<int>::const_iterator it = node->list.begin(); it != node->list.end(); ++it)
                parent->list.insert(*it);
        if (debug) printf("Successfully branched node %d. Moving to its parent, ID %d.\n", node->ID, parent->ID);
        //only call self again up until ID 1, for 1 -> 0, do nothing
        branch(parent, parent->parentAddress);
    }
}

int nodeNumber(std::string path)
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

void tree::readSelf(std::string filePath, int line)
{
    std::fstream xml (filePath, std::fstream::in);
    for (int i = 0; i < line; ++i)
        xml.ignore(2048, '/');
    xml.precision(2);
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

std::string tree::removeQuotes(std::string str)
{
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\"')
            str.erase(str.begin() + i);
    }
    return str;
}

// =============================== mtx ===============================

bool mtx::YN(void) {
    char input;
    printf(">> "); input = getchar(); inflush();

    input = toupper(input);
    //assures input is either Y or N
    while (input != 'Y' && input != 'N') {
        printf("(Y/N) >> ");
        input = getchar(); inflush();
        input = toupper(input);
    }

    return (input == 'Y')? true : false;

}

bool mtx::isUS(int index) {
    for (std::vector<int>::const_iterator it = usa.begin(); it != usa.end(); ++it)
        if (*it == index) return true;
    return false;
}

bool mtx::usaRead(std::string filePath) {
    std::fstream fs (filePath, std::fstream::in);

    if (!fs.is_open()) return false;
    else {
        fs.ignore(4096, '\n'); //ignore header
        int cnt = 0; //position counter
        std::string in;

        while (!fs.eof()) {
            fs.ignore(64, ','); //ignore key
            getline(fs, in, ',');

            if (in.compare("US") == 0 || in.compare("USA") == 0)
                usa.insert(usa.end(), cnt);
            ++cnt;

            fs.ignore(4096, '\n');
        }

        fs.close();
        return true;
    }
}

void mtx::countSamples(std::string path)
{
    std::fstream fs (path, std::fstream::in);
    fs.ignore(4096, '\n');
    std::string buff;
    while (!fs.eof()) {
        buff.clear();
        fs >> buff;
        if (buff.size() > 5)
            ++numSamples;
        fs.ignore(5000, '\n');
    }
    fs.close();
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

void SSheet::writeSelf(std::string filePath, bool truncate, bool debugmode, bool title) {
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

        if (!debugmode) { //for serious stuff
            for (int j = 0; j < 8; ++j)
                output << "," << octagon[j];
        }

        else if (Location == "US" || Location == "USA") { //premature printing
            for (int j = 0; j < 8; ++j)
                output << "," << octagon[j];
        }

        output << ",\n";
    }
    output.close();
}

void manualLoad(std::string* TreeFile, std::string* SSfile)
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
            printf("error loading the following files. Try again.\n");

            printf("Dendrogram (tree) file: ");
            (TreeLoaded)? printf("loaded.\n") : printf("failed.\n");

            printf("Spreadsheet file: ");
            (SSloaded)? printf("loaded.\n") : printf("failed.\n");
        }
    }
}

bool isLoaded(std::string path) {
    bool ret;
    std::fstream file (path, std::fstream::in);
    ret = file.is_open();
    file.close();
    return ret;
}

//fetch the files, reading them is done on other methods.
void firstRun(std::string* TreeFile, std::string* SSfile)
{
    using namespace std;
    bool TreeLoaded = false, SSloaded = false;
    fstream ini ("mtx.ini", fstream::in);

    if (ini.is_open()) {
        printf("Initialization file found. Do you wish to load?\n"
                "You will be asked for input files' folders otherwise. (Y/N)\n");
        if (mtx::YN) {
            printf("Understood. Loading configuration file... ");

            ini.ignore(256, '=');
            ini >> *TreeFile;
            ini.ignore(256, '=');
            ini >> *SSfile;

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
                    printf("done.\n");
                }
                else {
                    printf("error loading the following files. Try again. (if you wish to quit the program, simply type 'exit')\n");

                    printf("Dendrogram (tree) file: ");
                    (TreeLoaded)? printf("loaded.\n") : printf("failed.\n");

                    printf("Spreadsheet file: ");
                    (SSloaded)? printf("loaded.\n") : printf("failed.\n");

                    printf("\n=====\n");
                }
            } //end-while
        } //end-if (Y/N)

        else {
            printf("Understood. Commencing manual load.\n");
            ini.close(); manualLoad(TreeFile, SSfile);
        }
    }

    else manualLoad(TreeFile, SSfile);
}