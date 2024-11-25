#ifndef SIM_PROC_H
#define SIM_PROC_H
#include <vector>
#include <cstdint>
#include <emmintrin.h>
#include <iostream>
#define NUMBER_OF_REGISTERS 67U

using namespace std;

typedef struct proc_params{
    unsigned long int rob_size;
    unsigned long int iq_size;
    unsigned long int width;
}proc_params;

// Put additional data structures here as per your requirement





typedef struct instructionBundle {

    uint8_t validBit;
    uint32_t programCounter;
    int32_t operationType;
    int32_t destinationRegister;
    int32_t sourceRegister1;
    int32_t sourceRegister2;

    uint32_t currentRank;
}instructionBundle;

typedef struct decodePipelineDS {

    // instruction Bundle has the original Instruction
    instructionBundle instructionBundle;
}decodePipelineDS;

typedef struct issueQueueDS {

    uint32_t validBit;
    instructionBundle instructionBundle;
    int32_t destinationTag;
    uint32_t sourceRegister1Ready;
    int32_t  sourceRegister1Tag;
    uint32_t sourceRegister2Ready;
    int32_t  sourceRegister2Tag;

}issueQueueDS;

typedef struct renamePipelineDS {

    // In the rename pipeline regs the renamed and original values are stored.
    instructionBundle instructionBundle;

    // The below registers are used to store renamed contents
    int32_t destinationRegister;
    int32_t sourceRegister1;
    int32_t sourceRegister2;
}renamePipelineDS;

typedef struct registerReadPipelineDS {

    // In the register Read pipeline regs the renamed and original values are stored.
    instructionBundle instructionBundle;

    // The below registers are used to store renamed contents
    int32_t destinationRegister;
    int32_t sourceRegister1;
    int32_t sourceRegister2;
}registerReadPipelineDS;

typedef struct dispatchPipelineDS {

    // In the register Read pipeline regs the renamed and original values are stored.
    instructionBundle instructionBundle;

    // The below registers are used to store renamed contents
    int32_t destinationRegister;
    int32_t sourceRegister1;
    int32_t sourceRegister2;
}dispatchPipelineDS;

typedef struct writeBackPipelineDS {

    // In the write Back  pipeline regs the renamed and original values are stored.
    instructionBundle instructionBundle;

    // The below registers are used to store renamed contents
    int32_t destinationRegister;
    int32_t sourceRegister1;
    int32_t sourceRegister2;
}writeBackPipelineDS;

typedef struct executePipelineDS {

    // In the register Read pipeline regs the renamed and original values are stored.
    instructionBundle instructionBundle;

    // The below registers are used to store renamed contents
    int32_t destinationRegister;
    int32_t sourceRegister1;
    int32_t sourceRegister2;
    uint32_t waitCycles;
}executePipelineDS;

typedef struct reorderBuffer {
    //int32_t index;
    int32_t validBit;   // Check for vlaidity of the instruction
    int32_t destination; // To store the original Destination register
    int32_t readyBit;   //
    int32_t execute;
    int32_t mispredict;
    uint32_t programCounter;
    uint32_t currentRank;
    uint32_t currentIndex; // Store current index of this current ROB entry
}reorderBuffer;

typedef struct renameMapTable {
    uint32_t validBit;
    int32_t robTag;
}renameMapTable;




class superScalar {

public :
    vector<uint32_t> architecturalRegisterFile;

    vector<renameMapTable> renameMapTable;
    vector<reorderBuffer> reorderBuffer;
    vector<issueQueueDS> issueQueueDS;


    vector<decodePipelineDS> decodePipelineDS;
    vector<renamePipelineDS> renamePipelineDS;
    vector<registerReadPipelineDS> registerReadPipelineDS;
    vector<dispatchPipelineDS> dispatchPipelineDS;
    vector<executePipelineDS> executePipelineDS;
    vector<writeBackPipelineDS> writeBackPipelineDS;


    uint32_t headPointer;
    uint32_t tailPointer;

    uint32_t robSize;
    uint32_t iqSize;
    uint32_t width;

    uint32_t currentInstructionCycle;
    uint32_t cycleCount;


    int op_type, dest, src1, src2;  // Variables are read from trace file
    uint64_t pc; // Variable holds the pc read from input file


    FILE *filePointer;

    superScalar(uint32_t robSize, uint32_t iqSize, uint32_t width, FILE *filePointer) {
        // Architectural register and rename Map Table must be of same size i.e 67
        architecturalRegisterFile.resize(NUMBER_OF_REGISTERS);
        renameMapTable.resize(NUMBER_OF_REGISTERS);
        reorderBuffer.resize(robSize);
        issueQueueDS.resize(iqSize);

        decodePipelineDS.resize(width);
        renamePipelineDS.resize(width);
        registerReadPipelineDS.resize(width);
        dispatchPipelineDS.resize(width);

        executePipelineDS.resize(width*5);
        writeBackPipelineDS.resize(width*5);

        headPointer = 0;    // Points to the oldest instruction in ROB
        tailPointer = 0;    // points to the youngest instruction in ROB

        currentInstructionCycle = 0;
        cycleCount = 0;

        this->filePointer = filePointer;

        this->robSize = robSize;
        this->iqSize = iqSize;
        this->width = width;

        InitialisePipelineDS();




}

    void InitialisePipelineDS();

    // Perfrom Fetch operations where fetch upto Width instructions from trace file into DE stage.
    void Fetch();
    uint32_t checkFE();

    void Decode();
    uint32_t checkDE();

    void Rename();
    uint32_t checkRN();

    void RegisterRead();
    uint32_t checkRR();


    void Dispatch();
    uint32_t checkDS();


    void Issue();
    uint32_t checkIS();

    void Writeback();
    uint32_t checkWB();


    void Retire();
    uint32_t checkRE();


    void Execute();
    uint32_t checkEX();

    int32_t Advance_Cycle();

};

inline int32_t superScalar::Advance_Cycle() {

    cycleCount++;

    if(feof(filePointer)) {

        for(uint32_t i=0; i< width; i++) {
            if(dispatchPipelineDS[i].instructionBundle.validBit == 1 || renamePipelineDS[i].instructionBundle.validBit == 1 || registerReadPipelineDS[i].instructionBundle.validBit == 1)
                return 1;
        }

        return 0;
    }

    return 1;
}

inline void superScalar::Issue() {

    uint32_t issueQueueCounter = 0;

    if(checkIS()) {
        for(uint32_t i=0; i< iqSize; i++) {

            if(issueQueueDS[i].instructionBundle.validBit == 1 && issueQueueDS[i].sourceRegister1Ready == 0 && issueQueueDS[i].sourceRegister2Ready == 0) {
                for(uint32_t j =0; j < width*5; j++) {

                    if(executePipelineDS[j].instructionBundle.validBit == 0) {

                        executePipelineDS[j].instructionBundle.validBit = 1;
                        executePipelineDS[j].instructionBundle.destinationRegister      = issueQueueDS[i].instructionBundle.destinationRegister;
                        executePipelineDS[j].instructionBundle.sourceRegister1          = issueQueueDS[i].instructionBundle.sourceRegister1;
                        executePipelineDS[j].instructionBundle.sourceRegister2          = issueQueueDS[i].instructionBundle.sourceRegister2;
                        executePipelineDS[j].instructionBundle.programCounter           = issueQueueDS[i].instructionBundle.programCounter;
                        executePipelineDS[j].instructionBundle.currentRank               = issueQueueDS[i].instructionBundle.currentRank;
                        executePipelineDS[j].instructionBundle.operationType             = issueQueueDS[i].instructionBundle.operationType;

                        executePipelineDS[j].destinationRegister = issueQueueDS[i].destinationTag;
                        executePipelineDS[j].sourceRegister1 = issueQueueDS[i].sourceRegister1Tag;
                        executePipelineDS[j].sourceRegister2 = issueQueueDS[i].sourceRegister2Tag;

                        if(issueQueueDS[i].instructionBundle.operationType == 0)
                            executePipelineDS[j].waitCycles = 1;
                        else if(issueQueueDS[i].instructionBundle.operationType == 1)
                            executePipelineDS[j].waitCycles = 2;
                        else if(issueQueueDS[i].instructionBundle.operationType == 2)
                            executePipelineDS[j].waitCycles = 5;

                        issueQueueDS[i].validBit = 0;
                        issueQueueDS[i].instructionBundle.currentRank = -1;
                        issueQueueCounter++;
                        break;
                    }


                }

                if(issueQueueCounter == width)
                    return;
            }
        } return;
    } return;
}


inline void superScalar::Dispatch() {

    if(checkDE()) {
        for(uint32_t i =0; i < width; i++) {

            if(dispatchPipelineDS[i].instructionBundle.validBit == 1) {

                // read the register values if thery are still identifiers i.e if the ready bit is set in ROB
                // and reset the renamed register values.
                if(dispatchPipelineDS[i].sourceRegister1 != -1 &&
                    reorderBuffer[dispatchPipelineDS[i].sourceRegister1].readyBit == 1)
                    dispatchPipelineDS[i].sourceRegister1 = -1;

                if(dispatchPipelineDS[i].sourceRegister2 != -1 &&
                    reorderBuffer[dispatchPipelineDS[i].sourceRegister2].readyBit == 1)
                    dispatchPipelineDS[i].sourceRegister2 = -1;

                for(uint32_t j =0; j < iqSize; j++) {

                    // Here keep checking for instruction in issue queue that has to be removed i.e it is sent to execution stage if found
                    if(issueQueueDS[j].validBit == 0) {

                        issueQueueDS[j].validBit = 1;
                        issueQueueDS[j].instructionBundle.validBit = 1;
                        issueQueueDS[j].instructionBundle.destinationRegister   = dispatchPipelineDS[i].instructionBundle.destinationRegister;
                        issueQueueDS[j].instructionBundle.sourceRegister1       = dispatchPipelineDS[i].instructionBundle.sourceRegister1;
                        issueQueueDS[j].instructionBundle.sourceRegister2       = dispatchPipelineDS[i].instructionBundle.sourceRegister2;
                        issueQueueDS[j].instructionBundle.programCounter        = dispatchPipelineDS[i].instructionBundle.programCounter;
                        issueQueueDS[j].instructionBundle.operationType         = dispatchPipelineDS[i].instructionBundle.operationType;
                        issueQueueDS[j].instructionBundle.currentRank           = dispatchPipelineDS[i].instructionBundle.currentRank;

                        // Propagate the renamed register values as well.
                        issueQueueDS[j].destinationTag = dispatchPipelineDS[i].destinationRegister;
                        issueQueueDS[j].sourceRegister1Tag = dispatchPipelineDS[i].sourceRegister1;
                        issueQueueDS[j].sourceRegister2Tag = dispatchPipelineDS[i].sourceRegister2;

                        // Reset/set the valid bit of the dispatch stage to read next set of WIDTH instructions.
                        dispatchPipelineDS[j].instructionBundle.validBit = 0;



                    }
                }
            }
        }
    } return;
}


inline uint32_t superScalar::checkDS() {
// check dispatch piipeline registers validity
    // and if the issue queue has atleast width sizze of free entires
    uint32_t dispatchCounter = 0;
    uint32_t issueCounter = 0;

    for (uint32_t i = 0; i < width; i++) {

        if(dispatchPipelineDS[i].instructionBundle.validBit == 1)
            dispatchCounter++;

    }

    // Check for emptiness in the issue queue
    for(uint32_t j=0; j< iqSize ; j++) {
        if(issueQueueDS[j].validBit == 0)
            issueCounter++;

    }

    if(dispatchCounter <= width && dispatchCounter >0) {

        // Check if issue queue has enough empty entries to accomodate all the dispatch bundles
        // if not do nothing
        if(issueCounter < dispatchCounter)
            return 0;
        else if (issueCounter >= dispatchCounter)
            return 1;

    }
    return 0;

}

inline void superScalar::RegisterRead() {

    if(checkRR()) {

        for(uint32_t i =0; i < width; i++) {
            if(registerReadPipelineDS[i].instructionBundle.validBit == 1) {

                // Check if the instructions in the register Read pipeline registers are valid.

                // Because this is the register read stage data dependency is resolved by renaming the source registers as well
                // Check if the source register has a valid value and if the register in use is ready in ROB
                if(registerReadPipelineDS[i].sourceRegister1 != -1 &&
                    reorderBuffer[registerReadPipelineDS[i].sourceRegister1].readyBit == 1) {
                    // If the register under observation in the ROB is ready set it to -1 for next cycle usage.
                    registerReadPipelineDS[i].sourceRegister1 = -1;

                    }

                if(registerReadPipelineDS[i].sourceRegister2 != -1 &&
                    reorderBuffer[registerReadPipelineDS[i].sourceRegister2].readyBit == 1) {
                    registerReadPipelineDS[i].sourceRegister2 = -1;

                    }


                // Advance the cycle to dispatch stage

                dispatchPipelineDS[i].instructionBundle.validBit = 1;
                dispatchPipelineDS[i].instructionBundle.destinationRegister = registerReadPipelineDS[i].instructionBundle.destinationRegister;
                dispatchPipelineDS[i].instructionBundle.sourceRegister1 = registerReadPipelineDS[i].instructionBundle.sourceRegister1;
                dispatchPipelineDS[i].instructionBundle.sourceRegister2 = registerReadPipelineDS[i].instructionBundle.sourceRegister2;
                dispatchPipelineDS[i].instructionBundle.programCounter = registerReadPipelineDS[i].instructionBundle.programCounter;
                dispatchPipelineDS[i].instructionBundle.operationType = registerReadPipelineDS[i].instructionBundle.operationType;


                // Copy the renamed registers to next pipeline Values as well.
                dispatchPipelineDS[i].destinationRegister= registerReadPipelineDS[i].destinationRegister;
                dispatchPipelineDS[i].sourceRegister1 = registerReadPipelineDS[i].sourceRegister1;
                dispatchPipelineDS[i].sourceRegister2 = registerReadPipelineDS[i].sourceRegister2;


                dispatchPipelineDS[i].instructionBundle.currentRank = registerReadPipelineDS[i].instructionBundle.currentRank;

                // Advance cycle for next set of WIDTH instructions.
                registerReadPipelineDS[i].instructionBundle.validBit = 0;

            }
        }

    }
    return;

}
inline void superScalar::Rename() {

    bool checkRMTDestOrSource = false;

    // Check if ROB tailPointer != robSize and  RR pipeline is empty
    if(checkRN()) {

        for(int i = 0; i < width; i++) {

            // Processing the rename bundle -
            // 1. Allocate an entry in the ROB for the instruction
            // 2. Rename its source register i.e now the source registers become the entry index of ROB
            // 3. If it has a destination register rename it as well.

            if(renamePipelineDS[i].instructionBundle.validBit == 1) {
                // Preliminary check to see the validity of the instruction.


                // Check if RMT entry for the particular destination register is valid
                // if valid then it is in use by another instruction thus use ReorderBuffer to avoid RAW and WAW hazards.

                // Bool Check to know if destination or source register has to be considered to use RMT\
                // This is due to absence of destination register in some instructions(oper type 0)
                checkRMTDestOrSource = (renamePipelineDS[i].instructionBundle.destinationRegister == -1) ? false : true;

                    // Make entry into ROB base on teh current TailPointer

                // copy the destination reg of the instr to ROB
                reorderBuffer[tailPointer].destination = renamePipelineDS[i].instructionBundle.destinationRegister;
                // copy the PC to ROB
                reorderBuffer[tailPointer].programCounter = renamePipelineDS[i].instructionBundle.programCounter;
                // copy the index of instr into the ROB
                reorderBuffer[tailPointer].currentRank = renamePipelineDS[i].instructionBundle.currentRank;
                // This index serves as the robTag
                reorderBuffer[tailPointer].currentIndex = tailPointer;
                reorderBuffer[tailPointer].readyBit = 0;
                reorderBuffer[tailPointer].execute = 0;
                reorderBuffer[tailPointer].mispredict = 0;
                // Set the ROB entry to be valid
                reorderBuffer[tailPointer].validBit = 1;

                // Now that ROB entry is done, update the RMT and the pipeline registers to reflect the renamed registers.

                // Check if the register already has a valid value != -1
                // and check if the RMT entry for the register of interest is valid
                if( (renamePipelineDS[i].instructionBundle.sourceRegister1 != -1) &&
                    (renameMapTable[renamePipelineDS[i].instructionBundle.sourceRegister1].validBit == 1))
                    // Now instead of the original source register value the pipline caries the ROB entry as its source and destination register value.
                        renamePipelineDS[i].sourceRegister1 = renameMapTable[renamePipelineDS[i].instructionBundle.sourceRegister1].robTag;

                // Check if the register already has a valid value != -1
                // and check if the RMT entry for the register of interest is valid

                if( (renamePipelineDS[i].instructionBundle.sourceRegister2 != -1) &&
                    (renameMapTable[renamePipelineDS[i].instructionBundle.sourceRegister2].validBit == 1))
                    // Now instead of the original source register value the pipline caries the ROB entry as its source and destination register value.
                    renamePipelineDS[i].sourceRegister2 = renameMapTable[renamePipelineDS[i].instructionBundle.sourceRegister2].robTag;

                // Similar to source registers perfrom the same validity checks and move the destinationReg field value into the pipeline.
                renamePipelineDS[i].destinationRegister = reorderBuffer[tailPointer].destination;
                if(renamePipelineDS[i].destinationRegister != -1) {
                    // After making sure the RMT reflects the renamed changes update the robTag filed and set the validBit.
                    renameMapTable[renamePipelineDS[i].instructionBundle.destinationRegister].validBit = 1;

                    // The RMT's robtag now has the index which is robtag of ROB entry.
                    renameMapTable[renamePipelineDS[i].instructionBundle.destinationRegister].robTag = reorderBuffer[tailPointer].currentIndex;
                }


                    // After ROB entry increment tail pointer for next cycle updation.
                tailPointer++;

                // check if ROB is full, as it is a circular buffer restart from the first entry.(oth index)
                if(tailPointer == robSize)
                    tailPointer = 0;


                // After renaming the instruction bundles and updating RMT , ROB advance the instructions to next stage

                // Because of renaming set validBit to 0 so that right after width number of instructions are advanced next set of WIDTH instructions are extracted into fetch and decode.
                renamePipelineDS[i].instructionBundle.validBit = 0;

                // Advance the renamed PipelineRegister values into Register read stage.
                registerReadPipelineDS[i].instructionBundle.validBit = 1;
                registerReadPipelineDS[i].instructionBundle.programCounter      = renamePipelineDS[i].instructionBundle.programCounter;
                registerReadPipelineDS[i].instructionBundle.destinationRegister = renamePipelineDS[i].instructionBundle.destinationRegister;
                registerReadPipelineDS[i].instructionBundle.sourceRegister1     = renamePipelineDS[i].instructionBundle.sourceRegister1;
                registerReadPipelineDS[i].instructionBundle.sourceRegister2     = renamePipelineDS[i].instructionBundle.sourceRegister2;
                registerReadPipelineDS[i].instructionBundle.currentRank         = renamePipelineDS[i].instructionBundle.currentRank;
                // Copy the renamed source and destination values.
                registerReadPipelineDS[i].destinationRegister                   = renamePipelineDS[i].destinationRegister;
                registerReadPipelineDS[i].sourceRegister1                       = renamePipelineDS[i].sourceRegister1;
                registerReadPipelineDS[i].sourceRegister2                       = renamePipelineDS[i].sourceRegister2;

            }

        }
    }
    return;
}
inline uint32_t superScalar::checkRR() {

    // If RR contains RR Bundle and DI is not empty do nothing

    uint32_t registerReadCounter = 0;
    uint32_t dispatchCounter = 0;

    for( uint32_t i = 0; i < width; i++ ) {

        // Check if registerRead pipeline registers are full
        if(registerReadPipelineDS[i].instructionBundle.validBit == 1)
            registerReadCounter += 1;

        // Check if dispatch pipeline registers are empty
        if(dispatchPipelineDS[i].instructionBundle.validBit == 0)
            dispatchCounter += 1;
    }

    if(registerReadCounter <= width && registerReadCounter > 0 && dispatchCounter == width)
        return 1;

    return 0;
}


void superScalar::Fetch() {

    // Check if fetch criteria is met

    if(checkFE()) {
        for(int i =0; i < width; i++) {

            if( (fscanf(filePointer, "%lx %d %d %d %d", &pc, &op_type, &dest, &src1, &src2)) != EOF) {

                decodePipelineDS[i].instructionBundle.validBit = 1;
                decodePipelineDS[i].instructionBundle.destinationRegister = dest;
                decodePipelineDS[i].instructionBundle.sourceRegister1 = src1;
                decodePipelineDS[i].instructionBundle.sourceRegister2 = src2;
                decodePipelineDS[i].instructionBundle.programCounter = pc;
                decodePipelineDS[i].instructionBundle.operationType = op_type;
                decodePipelineDS[i].instructionBundle.currentRank = currentInstructionCycle;

                currentInstructionCycle++;
            } else
                return;
        }
    } return;

}

inline uint32_t superScalar::checkFE() {


    uint32_t counter = 0;

    // Check if the decode Pipeline Registers are empty/ invalid
    for(uint32_t i = 0; i < width; i++) {
        if(decodePipelineDS[i].instructionBundle.validBit == 0) {
            counter += 1;
        }

    }

    // If the DE pipeline registers are empty and the trace file has traces left return 1,
    // to denote fetch
    if(counter == width && !feof(filePointer))
        return 1;


    return 0;

}

inline void superScalar::Decode() {

    if(checkDE()) {
        for(int i = 0; i < width; i++) {
            if(decodePipelineDS[i].instructionBundle.validBit == 1) {

                // Advance the stage by moving the instruction bundles to next stage.

                renamePipelineDS[i].instructionBundle.currentRank = decodePipelineDS[i].instructionBundle.currentRank;
                renamePipelineDS[i].instructionBundle.destinationRegister = decodePipelineDS[i].instructionBundle.destinationRegister;
                renamePipelineDS[i].instructionBundle.sourceRegister1 = decodePipelineDS[i].instructionBundle.sourceRegister1;
                renamePipelineDS[i].instructionBundle.sourceRegister2 = decodePipelineDS[i].instructionBundle.sourceRegister2;
                renamePipelineDS[i].instructionBundle.operationType = decodePipelineDS[i].instructionBundle.operationType;
                renamePipelineDS[i].instructionBundle.programCounter = decodePipelineDS[i].instructionBundle.programCounter;
                renamePipelineDS[i].instructionBundle.validBit = decodePipelineDS[i].instructionBundle.validBit;

                renamePipelineDS[i].destinationRegister = -1;
                renamePipelineDS[i].sourceRegister1 = -1;
                renamePipelineDS[i].sourceRegister2 = -1;
            }
        }
    }
    return;
}





inline uint32_t superScalar::checkDE() {


    uint32_t decodeCounter = 0;
    uint32_t renameCounter = 0;

    // Check if the decode Pipeline Registers are not empty i.e valid bit is 1
    // and rename is not empty i.e. valid bit is 1 do nothing
    for(uint32_t i = 0; i < width; i++) {

        // check if DE contains instruction bundle
        if(decodePipelineDS[i].instructionBundle.validBit == 1)
            decodeCounter += 1;

        // check if rename stage contain any instruction bundle
        if(renamePipelineDS[i].instructionBundle.validBit == 0)
            renameCounter += 1;
    }

    // If the DE pipeline registers are filled and RN is empty doi nothing
    // else if RN is empty then move decode pipeline reg values to RN.

    if(decodeCounter <= width && decodeCounter>0 && renameCounter == width) {
        return 1;
    }
    return 0;

}




inline uint32_t superScalar::checkRN() {

    uint32_t renameCounter = 0;
    uint32_t registerReadCounter = 0;

    // If RN contains rename bundle -
    // if RR is full or ROB is full i.e tailPointer of ROB is = robSize do nothing.


    // if RR is empty and tailPointer of ROB is !+ robSize then process the Rename bundle

    // Processing the rename bundle -
    // 1. Allocate an entry in the ROB for the instruction
    // 2. Rename its source register i.e now the source registers become the entry index of ROB
    // 3. If it has a destination register rename it as well.

    for(uint32_t i = 0; i < width; i++) {

        // check if DE contains instruction bundle
        if(renamePipelineDS[i].instructionBundle.validBit == 1)
            renameCounter += 1;

        // check if rename stage contain any instruction bundle
        if(registerReadPipelineDS[i].instructionBundle.validBit == 0)
            registerReadCounter += 1;
    }

    if(renameCounter <= width && renameCounter > 0 && registerReadCounter == width) {
        return 1;
    }
    return 0;
}


void superScalar::InitialisePipelineDS() {


    for(uint32_t i = 0; i < iqSize; i++) {

        issueQueueDS[i].validBit = 0;
        issueQueueDS[i].destinationTag = -1;
        issueQueueDS[i].sourceRegister1Tag = -1;
        issueQueueDS[i].sourceRegister1Ready = 0;
        issueQueueDS[i].sourceRegister2Tag = -1;
        issueQueueDS[i].sourceRegister2Ready = 0;

    }

    for(uint32_t i = 0; i < robSize; i++) {

        reorderBuffer[i].validBit = 0;
        reorderBuffer[i].destination = -1;
        reorderBuffer[i].readyBit = 0;
        reorderBuffer[i].execute = 0;
        reorderBuffer[i].mispredict = 0;
        reorderBuffer[i].programCounter = 0;
        reorderBuffer[i].currentRank = 0;
        reorderBuffer[i].currentIndex = i;

    }

    for(uint32_t i = 0; i < NUMBER_OF_REGISTERS; i++) {

        architecturalRegisterFile[i] = 0;
        renameMapTable[i].validBit = -1;
        renameMapTable[i].robTag = -1;


    }




    for(uint32_t i = 0; i < width; i++) {

        /// Initialise the decode pipeline registers.
        decodePipelineDS[i].instructionBundle.validBit = 0;
        decodePipelineDS[i].instructionBundle.programCounter = 0;
        decodePipelineDS[i].instructionBundle.destinationRegister = -1;
        decodePipelineDS[i].instructionBundle.operationType = -1;
        decodePipelineDS[i].instructionBundle.currentRank =-1;
        decodePipelineDS[i].instructionBundle.sourceRegister1 = -1;
        decodePipelineDS[i].instructionBundle.sourceRegister2 = -1;

        // Initialise the rename pipeline registers.
        renamePipelineDS[i].instructionBundle.validBit = 0;
        renamePipelineDS[i].instructionBundle.programCounter = 0;
        renamePipelineDS[i].instructionBundle.destinationRegister = -1;
        renamePipelineDS[i].instructionBundle.operationType = -1;
        renamePipelineDS[i].instructionBundle.currentRank =-1;
        renamePipelineDS[i].instructionBundle.sourceRegister1 = -1;
        renamePipelineDS[i].instructionBundle.sourceRegister2 = -1;
        renamePipelineDS[i].destinationRegister = -1;
        renamePipelineDS[i].sourceRegister1 = -1;
        renamePipelineDS[i].sourceRegister2 = -1;

        // Initialise the register Read pipeline registers.
        registerReadPipelineDS[i].instructionBundle.validBit = 0;
        registerReadPipelineDS[i].instructionBundle.programCounter = 0;
        registerReadPipelineDS[i].instructionBundle.destinationRegister = -1;
        registerReadPipelineDS[i].instructionBundle.operationType = -1;
        registerReadPipelineDS[i].instructionBundle.currentRank =-1;
        registerReadPipelineDS[i].instructionBundle.sourceRegister1 = -1;
        registerReadPipelineDS[i].instructionBundle.sourceRegister2 = -1;
        registerReadPipelineDS[i].destinationRegister = -1;
        registerReadPipelineDS[i].sourceRegister1 = -1;
        registerReadPipelineDS[i].sourceRegister2 = -1;

        // Initialise the dispatch pipeline registers.
        dispatchPipelineDS[i].instructionBundle.validBit = 0;

        dispatchPipelineDS[i].instructionBundle.programCounter = 0;
        dispatchPipelineDS[i].instructionBundle.destinationRegister = -1;
        dispatchPipelineDS[i].instructionBundle.operationType = -1;
        dispatchPipelineDS[i].instructionBundle.currentRank =-1;
        dispatchPipelineDS[i].instructionBundle.sourceRegister1 = -1;
        dispatchPipelineDS[i].instructionBundle.sourceRegister2 = -1;
        dispatchPipelineDS[i].destinationRegister = -1;
        dispatchPipelineDS[i].sourceRegister1 = -1;
        dispatchPipelineDS[i].sourceRegister2 = -1;
    }

    for(uint32_t i = 0; i < (width * 5); i++) {

        // Initialise the execute pipeline registers.
        executePipelineDS[i].instructionBundle.validBit = 0;
        executePipelineDS[i].instructionBundle.programCounter = 0;
        executePipelineDS[i].instructionBundle.destinationRegister = -1;
        executePipelineDS[i].instructionBundle.operationType = -1;
        executePipelineDS[i].instructionBundle.currentRank =-1;
        executePipelineDS[i].instructionBundle.sourceRegister1 = -1;
        executePipelineDS[i].instructionBundle.sourceRegister2 = -1;
        executePipelineDS[i].destinationRegister = -1;
        executePipelineDS[i].sourceRegister1 = -1;
        executePipelineDS[i].sourceRegister2 = -1;

        // Initialise the writeBack pipeline registers.
        writeBackPipelineDS[i].instructionBundle.validBit = 0;
        writeBackPipelineDS[i].instructionBundle.programCounter = 0;
        writeBackPipelineDS[i].instructionBundle.destinationRegister = -1;
        writeBackPipelineDS[i].instructionBundle.operationType = -1;
        writeBackPipelineDS[i].instructionBundle.currentRank =-1;
        writeBackPipelineDS[i].instructionBundle.sourceRegister1 = -1;
        writeBackPipelineDS[i].instructionBundle.sourceRegister2 = -1;
        writeBackPipelineDS[i].destinationRegister = -1;
        writeBackPipelineDS[i].sourceRegister1 = -1;
        writeBackPipelineDS[i].sourceRegister2 = -1;

    }


}

















#endif