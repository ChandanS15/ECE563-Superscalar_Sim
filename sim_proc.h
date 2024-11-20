#ifndef SIM_PROC_H
#define SIM_PROC_H
#include <vector>
#include <cstdint>
#include <iostream>
#define NUMBER_OF_REGISTERS 67U

using namespace std;

typedef struct proc_params{
    unsigned long int rob_size;
    unsigned long int iq_size;
    unsigned long int width;
}proc_params;

// Put additional data structures here as per your requirement


typedef struct issueQueueDS {

    uint32_t validBit;
    uint32_t destinationTag;
    uint32_t sourceRegister1Ready;
    int32_t  sourceRegister1Tag;
    uint32_t sourceRegister2Ready;
    int32_t  sourceRegister2Tag;

}issueQueueDS;

typedef struct instructionBundle {
    uint8_t validBit;
    uint32_t programCounter;
    uint32_t operationType;
    int32_t destinationRegister;
    int32_t sourceRegister1;
    int32_t sourceRegister2;

    uint32_t currentRank;
}instructionBundle;

typedef struct reorderBuffer {
    int32_t index;
    int32_t validBit;
    int32_t destination;
    int32_t readyBit;
    int32_t execute;
    int32_t mispredict;
    uint32_t programCounter;
    uint32_t currentRank;
}reorderBuffer;

typedef struct renameMapTable {
    uint32_t validBit;
    uint32_t robTag;
}renameMapTable;




class superScalar {

public :
    vector<uint32_t> architecturalRegisterFile;

    vector<renameMapTable> renameMapTable;
    vector<reorderBuffer> reorderBuffer;
    vector<issueQueueDS> issueQueueDS;
    vector<instructionBundle> instructionBundle;

    uint32_t headPointer;
    uint32_t tailPointer;

    uint32_t robSize;
    uint32_t iqSize;
    uint32_t width;

    FILE *filePointer;

    superScalar(uint32_t robSize, uint32_t iqSize, uint32_t width, FILE *filePointer) {
        // Architectural register and rename Map Table must be of same size i.e 67
        architecturalRegisterFile.resize(NUMBER_OF_REGISTERS);
        renameMapTable.resize(NUMBER_OF_REGISTERS);
        reorderBuffer.resize(robSize);
        issueQueueDS.resize(iqSize);
        instructionBundle.resize(width);

        headPointer = 0;    // Points to the oldest instruction
        tailPointer = 0;    // points to youngest instruction

        this->filePointer = filePointer;

        this->robSize = robSize;
        this->iqSize = iqSize;
        this->width = width;


}

    // Perfrom Fetch operations where fetch upto Width instructions from trace file into DE stage.
    void fetchCycle();
    uint32_t checkFE();
};

void superScalar::fetchCycle() {

    // Check if fetch criteria is met

    if(checkFE()) {
        for(int i =0; i<width; i++) {
            



        }


    }

}

inline uint32_t superScalar::checkFE() {


    uint32_t counter = 0;

    for(uint32_t i = 0; i < width; i++) {
        if(instructionBundle[i].validBit == 0) {
            counter++;
        }
    }

    if(counter == width && feof(filePointer))
        return 1;


    return 0;

}




















#endif