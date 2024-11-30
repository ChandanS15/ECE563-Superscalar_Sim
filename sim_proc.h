#ifndef SIM_PROC_H
#define SIM_PROC_H
#include <vector>
#include <cstdint>
#include <emmintrin.h>
#include <iostream>
#include <random>
#define NUMBER_OF_REGISTERS 67U

using namespace std;

typedef struct proc_params{
    unsigned long int rob_size;
    unsigned long int iq_size;
    unsigned long int width;
}proc_params;

// Put additional data structures here as per your requirement


typedef struct instructionStageCycleCounterDS {

    uint32_t fetchCycleCount;
    uint32_t decodeCycleCount;
    uint32_t renameCycleCount;
    uint32_t registerReadCycleCount;
    uint32_t dispatchCycleCount;
    uint32_t issueCycleCount;
    uint32_t executeCycleCount;
    uint32_t writeBackCycleCount;
    uint32_t retireCycleCount;

}instructionStageCycleCounterDS;


typedef struct instructionBundleDS {

    uint8_t validBit;
    uint32_t programCounter;
    int32_t operationType;
    int32_t destinationRegister;
    int32_t sourceRegister1;
    int32_t sourceRegister2;

    int32_t currentRank;
}instructionBundleDS;

typedef struct decodePipeline {

    // instruction Bundle has the original Instruction
    instructionBundleDS instructionBundle;
}decodePipeline;

typedef struct issueQueue {

    instructionBundleDS instructionBundle;
    int32_t destinationRegister;
    int32_t  sourceRegister1;
    uint32_t sourceRegister1Ready;
    int32_t  sourceRegister2;
    uint32_t sourceRegister2Ready;

}issueQueue;

typedef struct renamePipeline {

    // In the rename pipeline regs the renamed and original values are stored.
    instructionBundleDS instructionBundle;

    // The below registers are used to store renamed contents
    int32_t destinationRegister;
    int32_t sourceRegister1;
    int32_t sourceRegister2;
}renamePipeline;

typedef struct registerReadPipeline {

    // In the register Read pipeline regs the renamed and original values are stored.
    instructionBundleDS instructionBundle;

    // The below registers are used to store renamed contents
    int32_t destinationRegister;
    int32_t sourceRegister1;
    int32_t sourceRegister2;
}registerReadPipeline;

typedef struct dispatchPipeline {

    // In the register Read pipeline regs the renamed and original values are stored.
    instructionBundleDS instructionBundle;

    // The below registers are used to store renamed contents
    int32_t destinationRegister;
    int32_t sourceRegister1;
    int32_t sourceRegister2;
}dispatchPipeline;

typedef struct writeBackPipeline {

    // In the write Back  pipeline regs the renamed and original values are stored.
    instructionBundleDS instructionBundle;

    // The below registers are used to store renamed contents
    int32_t destinationRegister;
    int32_t sourceRegister1;
    int32_t sourceRegister2;
}writeBackPipeline;

typedef struct executePipeline {

    // In the register Read pipeline regs the renamed and original values are stored.
    instructionBundleDS instructionBundle;

    // The below registers are used to store renamed contents
    int32_t destinationRegister;
    int32_t sourceRegister1;
    int32_t sourceRegister2;
    uint32_t waitCycles;
}executePipeline;

typedef struct reorderBufferDS {
    //int32_t index;

    int32_t validBit;   // Check for vlaidity of the instruction
    int32_t destination; // To store the original Destination register
    int32_t sourceRegister1;
    int32_t sourceRegister2;
    int32_t currentRank;
    int32_t operationType;
    int32_t readyBit;   //
    uint32_t programCounter;
    int32_t currentIndex; // Store current index of this current ROB entry
}reorderBufferDS;

typedef struct renameMapTableDS {

    uint32_t validBit;
    int32_t robTag;

}renameMapTableDS;




class superScalar {
public:
    std::vector<uint32_t> architecturalRegisterFile;
    //std::vector<renameMapTableDS> renameMapTable;
    renameMapTableDS renameMapTable [67];
    std::vector<reorderBufferDS> reorderBuffer;
    std::vector<issueQueue> issueQueueDS;
    std::vector<decodePipeline> decodePipelineDS;
    std::vector<renamePipeline> renamePipelineDS;
    std::vector<registerReadPipeline> registerReadPipelineDS;
    std::vector<dispatchPipeline> dispatchPipelineDS;
    std::vector<executePipeline> executePipelineDS;
    std::vector<writeBackPipeline> writeBackPipelineDS;
    std::vector<instructionStageCycleCounterDS> instructionStageCycleCounter;

    int32_t headPointer;
    int32_t tailPointer;
    uint32_t robSize;
    uint32_t iqSize;
    uint32_t width;
    uint32_t currentInstructionCount;
    uint32_t cycleCount;
    bool advanceCyleEnable;

    int op_type, dest, src1, src2;
    uint64_t pc;

    FILE* filePointer;
    void superScalarInitialise(int32_t robSize, uint32_t iqSize, uint32_t width, FILE *filePointer);





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

inline void superScalar::superScalarInitialise(int32_t robSize, uint32_t iqSize, uint32_t width, FILE* filePointer) {


    //renameMapTable(NUMBER_OF_REGISTERS),
    reorderBuffer.resize( robSize);
    issueQueueDS.resize(iqSize);
    decodePipelineDS.resize(width);
    renamePipelineDS.resize(width);
    registerReadPipelineDS.resize(width);
    dispatchPipelineDS.resize(width);
    executePipelineDS.resize(width * 5);
    writeBackPipelineDS.resize(width * 5);
    headPointer = 0;
    tailPointer=(0);
    currentInstructionCount=(0);
    cycleCount=(0);
    this->filePointer=(filePointer);
    this->robSize=(robSize);
    this->iqSize=(iqSize);
    this->width=width;

    if (filePointer == nullptr) {
        std::cerr << "Error: File pointer is null." << std::endl;
        exit(EXIT_FAILURE);
    }
    InitialisePipelineDS();

}




inline int32_t superScalar::Advance_Cycle() {

    // cycle count to retire all the instructions that enter the pipeline
    cycleCount++;
    size_t bytesRead;

    if(feof(filePointer)) {

    //if(advanceCyleEnable ) {

        // After fetching the last instruction from the trace file

        // check if there are valid instructions in the pipeline
        for(uint32_t i=0; i< width; i++) {
            if(dispatchPipelineDS[i].instructionBundle.validBit == 1 || renamePipelineDS[i].instructionBundle.validBit == 1 || registerReadPipelineDS[i].instructionBundle.validBit == 1 ||
                decodePipelineDS[i].instructionBundle.validBit == 1)
                return 1;
        }

        // check if there are valid instructions in the issue Queue waiting for operations to be performed
        // or to be woken up by producer instructions.
        for(auto iterator = issueQueueDS.begin(); iterator != issueQueueDS.end(); iterator++) {

            if(iterator->instructionBundle.validBit == 1)
                return 1;
        }

        // check if there are valid instructions in the execution Pipeline waiting for operations to be performed
        // or to be woken up by producer instructions.

        for(uint32_t i=0; i< width * 5; i++) {
            if(executePipelineDS[i].instructionBundle.validBit == 1 || writeBackPipelineDS[i].instructionBundle.validBit == 1)
                return 1;
        }

        // Check if there are entries int he ROB woth speculative versions in the ROB.

        for(auto iterator = reorderBuffer.begin(); iterator != reorderBuffer.end(); iterator++) {

            if(iterator->validBit == 1)
                return 1;
        }
        // If none of the above conditions match i.e every instruction fetched from the trace file has been retired, then simulation is done.
        return 0;
    }

    return 1;
}

inline void superScalar::Retire() {


    for(auto iterator = reorderBuffer.begin(); iterator != reorderBuffer.end(); iterator++) {

        if(iterator->validBit == 1 && iterator->readyBit == 1) {

            instructionStageCycleCounter[iterator->currentRank].retireCycleCount++;
        }
    }

    if(checkRE()) {
        // here if the ROB entry pointed by the head is ready/valid
        // compare the destination register with RMT entry
        // unconditionally update the ARF
        // If the robTag in the RMT and the ROB entry are saem remove and restt the valid bit in RMT as it says that the commited value in
        // ARF is the final one and not thr RMT.
        for(uint32_t i=0; i< width; i++) {
            if(reorderBuffer[headPointer].readyBit == 1) {
                if(reorderBuffer[headPointer].validBit == 1) {

                    cout<< reorderBuffer[headPointer].currentRank
                    <<" fu{" <<reorderBuffer[headPointer].operationType<<"}"
                    <<" src{" <<reorderBuffer[headPointer].sourceRegister1<< "," <<reorderBuffer[headPointer].sourceRegister2<< "}"
                    <<" dst{" <<reorderBuffer[headPointer].destination<<"}"
                    <<" FE{"<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].fetchCycleCount<<",1}"
                    <<" DE{"<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].fetchCycleCount + 1<<","<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].decodeCycleCount -instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].fetchCycleCount << "}"
                    <<" RN{"<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].decodeCycleCount + 1<<","<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].renameCycleCount -instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].decodeCycleCount << "}"
                    <<" RR{"<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].renameCycleCount + 1<<","<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].registerReadCycleCount -instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].renameCycleCount << "}"
                    <<" DI{"<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].registerReadCycleCount + 1<<","<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].dispatchCycleCount -instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].registerReadCycleCount << "}"
                    <<" IS{"<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].dispatchCycleCount + 1<<","<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].issueCycleCount -instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].dispatchCycleCount << "}"
                    <<" EX{"<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].issueCycleCount + 1<<","<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].executeCycleCount -instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].issueCycleCount << "}"
                    <<" WB{"<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].executeCycleCount + 1<<","<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].writeBackCycleCount -instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].executeCycleCount << "}"
                    <<" RT{"<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].writeBackCycleCount + 1<<","<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].retireCycleCount -instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].writeBackCycleCount << "}"

                    <<endl;

                    //printf("%d ",reorderBuffer[headPointer].currentRank);

                }

                reorderBuffer[headPointer].validBit = 0;

                if((renameMapTable[reorderBuffer[headPointer].destination].robTag ==
                    headPointer) && reorderBuffer[headPointer].destination != -1) {

                    renameMapTable[reorderBuffer[headPointer].destination].validBit = 0;
                    renameMapTable[reorderBuffer[headPointer].destination].robTag = -1;

                }

                headPointer ++;
                if(headPointer == robSize)
                    headPointer = 0;
            } else
                return;
        }
    }
    return;
}



inline void superScalar::Writeback() {

    for(auto iterator = writeBackPipelineDS.begin(); iterator != writeBackPipelineDS.end(); iterator++) {

        if(iterator->instructionBundle.validBit == 1) {

            instructionStageCycleCounter[iterator->instructionBundle.currentRank].writeBackCycleCount++;
            instructionStageCycleCounter[iterator->instructionBundle.currentRank].retireCycleCount = instructionStageCycleCounter[iterator->instructionBundle.currentRank].writeBackCycleCount;
        }
    }

    // In writeback the data bypass has to be maintained i.e.
    // The updates has to be maintained across issue queue, ROB and as a bypass to the execute stage so that the inflight instruction that requires the value can directly
    // have access to the latest value.
    if(checkWB()) {
        for(uint32_t i=0; i< width * 5; i++) {

            if(writeBackPipelineDS[i].instructionBundle.validBit == 1) {

                // update the ROB wiby setting ready bit to 1

                for(uint32_t j =0; j< robSize; j++) {

                    if((reorderBuffer[j].validBit == 1) && (reorderBuffer[j].currentIndex == writeBackPipelineDS[i].destinationRegister) && reorderBuffer[j].readyBit == 0) {

                        if(writeBackPipelineDS[i].destinationRegister != -1) {
                            // at the writeback stage set the ready bit to one so that in the retire stage the instructions can be commited to ARF.
                            // After setting the ready bit to 1 in ORB reset the valid bit in writePipeline fo r next instruction to be received.
                            reorderBuffer[j].readyBit = 1;
                            writeBackPipelineDS[i].instructionBundle.validBit = 0;
                            break;

                        }


                    }

                }

            }
        }
    }
    return;
}





inline void superScalar::Execute() {


    for(auto iterator = executePipelineDS.begin(); iterator != executePipelineDS.end(); iterator++) {

        if(iterator->instructionBundle.validBit == 1) {

            instructionStageCycleCounter[iterator->instructionBundle.currentRank].executeCycleCount++;
            instructionStageCycleCounter[iterator->instructionBundle.currentRank].writeBackCycleCount = instructionStageCycleCounter[iterator->instructionBundle.currentRank].executeCycleCount;
        }
    }



    if(checkEX()) {
        for(uint32_t i=0; i< width*5; i++) {

            // At every execute stage decrement the timer i.e. to emulate the cycles are being passed.

            if(executePipelineDS[i].instructionBundle.validBit == 1) {

                // This wait cycles simulate the waiting time inside the functional unit depending on the operation type.
                executePipelineDS[i].waitCycles--;

                if(executePipelineDS[i].waitCycles == 0) {
                    // If the current instruction bundles waitCycle is 0 ,
                    // It is done with execution and has to be sent to the writeback stage.

                    for(uint32_t j=0; j< iqSize; j++) {

                        if(issueQueueDS[j].instructionBundle.validBit == 1) {

                            // In the execute stage check for dependency between the instr in previous stage

                            if(issueQueueDS[j].sourceRegister1 != -1 && executePipelineDS[i].destinationRegister == issueQueueDS[j].sourceRegister1)
                                issueQueueDS[j].sourceRegister1 = -1;


                            if(issueQueueDS[j].sourceRegister2 != -1 && executePipelineDS[i].destinationRegister == issueQueueDS[j].sourceRegister2)
                                issueQueueDS[j].sourceRegister2 = -1;

                        }
                    }



                    for(uint32_t j=0; j< width; j++) {

                        // Wake up dependent instructions and set their source operand ready flags in the IQ, DI and RR.
                        if(dispatchPipelineDS[j].instructionBundle.validBit == 1) {

                            if(dispatchPipelineDS[j].sourceRegister1 != -1 && executePipelineDS[i].destinationRegister == dispatchPipelineDS[j].sourceRegister1)
                                dispatchPipelineDS[j].sourceRegister1 = -1;


                            if(dispatchPipelineDS[j].sourceRegister2 != -1 && executePipelineDS[i].destinationRegister == dispatchPipelineDS[j].sourceRegister2)
                                dispatchPipelineDS[j].sourceRegister2 = -1;

                        }

                        if(registerReadPipelineDS[j].instructionBundle.validBit == 1) {

                            if(registerReadPipelineDS[j].sourceRegister1 != -1 && executePipelineDS[i].destinationRegister == registerReadPipelineDS[j].sourceRegister1)
                                registerReadPipelineDS[j].sourceRegister1 = -1;


                            if(registerReadPipelineDS[j].sourceRegister2 != -1 && executePipelineDS[i].destinationRegister == registerReadPipelineDS[j].sourceRegister2)
                                registerReadPipelineDS[j].sourceRegister2 = -1;

                        }


                    }

                    for(uint32_t j =0; j <width*5; j++) {
                        if(writeBackPipelineDS[j].instructionBundle.validBit == 0) {

                            // In the writeBack List if the valid bit is 0, advance the executed instruction to writeback

                            executePipelineDS[i].instructionBundle.validBit = 0;

                            writeBackPipelineDS[j].instructionBundle.validBit = 1;
                            writeBackPipelineDS[j].instructionBundle.destinationRegister      = executePipelineDS[i].instructionBundle.destinationRegister;
                            writeBackPipelineDS[j].instructionBundle.sourceRegister1          = executePipelineDS[i].instructionBundle.sourceRegister1;
                            writeBackPipelineDS[j].instructionBundle.sourceRegister2          = executePipelineDS[i].instructionBundle.sourceRegister2;
                            writeBackPipelineDS[j].instructionBundle.programCounter           = executePipelineDS[i].instructionBundle.programCounter;
                            writeBackPipelineDS[j].instructionBundle.currentRank               = executePipelineDS[i].instructionBundle.currentRank;
                            writeBackPipelineDS[j].instructionBundle.operationType             = executePipelineDS[i].instructionBundle.operationType;

                            writeBackPipelineDS[j].destinationRegister                          = executePipelineDS[i].destinationRegister;
                            writeBackPipelineDS[j].sourceRegister1                              = executePipelineDS[i].sourceRegister1;
                            writeBackPipelineDS[j].sourceRegister2                              = executePipelineDS[i].sourceRegister2;
                            break;
                        }
                    }

                }
            }

        }
    }
    return;
}

inline void superScalar::Issue() {

    for(auto iterator = issueQueueDS.begin(); iterator != issueQueueDS.end(); iterator++) {

        if(iterator->instructionBundle.validBit == 1) {

            instructionStageCycleCounter[iterator->instructionBundle.currentRank].issueCycleCount++;
            instructionStageCycleCounter[iterator->instructionBundle.currentRank].executeCycleCount = instructionStageCycleCounter[iterator->instructionBundle.currentRank].issueCycleCount;
        }
    }

    uint32_t issueCounter = 0;

    // In the issue queue I should be issuing upto WIDTH oldest instructions from the issueQuquq.
    // In order to achieve this I will be using the rank that was given to each instruction while fetching from the trace file



    if(checkIS()) {

        for (uint32_t i = 0; i < iqSize - 1; i++) {
            for (uint32_t j = i + 1; j < iqSize; j++) {
                // First condition: Sort by validBit in descending order
                if (issueQueueDS[i].instructionBundle.validBit < issueQueueDS[j].instructionBundle.validBit) {
                    std::swap(issueQueueDS[i], issueQueueDS[j]);
                }
                // Second condition: Sort by currentRank if validBit is 1
                else if ((issueQueueDS[i].instructionBundle.validBit == 1) &&
                         (issueQueueDS[j].instructionBundle.validBit == 1) &&
                         (issueQueueDS[i].instructionBundle.currentRank > issueQueueDS[j].instructionBundle.currentRank) &&
                         (issueQueueDS[j].instructionBundle.currentRank != -1)) {
                    std::swap(issueQueueDS[i], issueQueueDS[j]);
                         }
            }
        }






        for(uint32_t i=0; i< iqSize; i++) {

            if(issueQueueDS[i].instructionBundle.validBit == 1 && issueQueueDS[i].sourceRegister1 == -1 && issueQueueDS[i].sourceRegister2 == -1) {
                for(uint32_t j =0; j < width*5; j++) {

                    if(executePipelineDS[j].instructionBundle.validBit == 0) {

                        executePipelineDS[j].instructionBundle.validBit = 1;
                        executePipelineDS[j].instructionBundle.destinationRegister      = issueQueueDS[i].instructionBundle.destinationRegister;
                        executePipelineDS[j].instructionBundle.sourceRegister1          = issueQueueDS[i].instructionBundle.sourceRegister1;
                        executePipelineDS[j].instructionBundle.sourceRegister2          = issueQueueDS[i].instructionBundle.sourceRegister2;
                        executePipelineDS[j].instructionBundle.programCounter           = issueQueueDS[i].instructionBundle.programCounter;
                        executePipelineDS[j].instructionBundle.currentRank               = issueQueueDS[i].instructionBundle.currentRank;
                        executePipelineDS[j].instructionBundle.operationType             = issueQueueDS[i].instructionBundle.operationType;

                        executePipelineDS[j].destinationRegister                        = issueQueueDS[i].destinationRegister;
                        executePipelineDS[j].sourceRegister1                            = issueQueueDS[i].sourceRegister1;
                        executePipelineDS[j].sourceRegister2                            = issueQueueDS[i].sourceRegister2;

                        if(issueQueueDS[i].instructionBundle.operationType == 0)
                            executePipelineDS[j].waitCycles = 1;

                        else if(issueQueueDS[i].instructionBundle.operationType == 1)
                            executePipelineDS[j].waitCycles = 2;

                        else if(issueQueueDS[i].instructionBundle.operationType == 2)
                            executePipelineDS[j].waitCycles = 5;

                        issueQueueDS[i].instructionBundle.validBit = 0;
                        issueQueueDS[i].instructionBundle.currentRank = -1;
                        issueCounter++;
                        break;
                    }


                }

                if(issueCounter == width)
                    return;
            }
        } return;
    } return;
}


inline void superScalar::Dispatch() {


    for(auto iterator = dispatchPipelineDS.begin(); iterator != dispatchPipelineDS.end(); iterator++) {

        if(iterator->instructionBundle.validBit == 1) {

            instructionStageCycleCounter[iterator->instructionBundle.currentRank].dispatchCycleCount++;
            instructionStageCycleCounter[iterator->instructionBundle.currentRank].issueCycleCount = instructionStageCycleCounter[iterator->instructionBundle.currentRank].dispatchCycleCount;
        }
    }

    if(checkDS()) {
        auto dispatchIt = dispatchPipelineDS.begin();

for (; dispatchIt != dispatchPipelineDS.end(); ++dispatchIt) {
    if (dispatchIt->instructionBundle.validBit == 1) {
        // Check if sourceRegister1 is ready in the ROB and update if so.
        if (dispatchIt->sourceRegister1 != -1 &&
            reorderBuffer[dispatchIt->sourceRegister1].readyBit == 1) {
            dispatchIt->sourceRegister1 = -1;
        }

        // Check if sourceRegister2 is ready in the ROB and update if so.
        if (dispatchIt->sourceRegister2 != -1 &&
            reorderBuffer[dispatchIt->sourceRegister2].readyBit == 1) {
            dispatchIt->sourceRegister2 = -1;
        }

        // Push the instruction bundle to the Issue Queue.
        auto issueQueueIt = issueQueueDS.begin();

        for (; issueQueueIt != issueQueueDS.end(); ++issueQueueIt) {
            if (issueQueueIt->instructionBundle.validBit == 0) {
                // Found an empty slot in the Issue Queue.

                // Reset the valid bit in the dispatch stage for the next cycle.
                dispatchIt->instructionBundle.validBit = 0;

                // Populate the issue queue entry with the instruction from the dispatch stage.
                issueQueueIt->instructionBundle.validBit = 1;
                issueQueueIt->instructionBundle.destinationRegister = dispatchIt->instructionBundle.destinationRegister;
                issueQueueIt->instructionBundle.sourceRegister1 = dispatchIt->instructionBundle.sourceRegister1;
                issueQueueIt->instructionBundle.sourceRegister2 = dispatchIt->instructionBundle.sourceRegister2;
                issueQueueIt->instructionBundle.programCounter = dispatchIt->instructionBundle.programCounter;
                issueQueueIt->instructionBundle.operationType = dispatchIt->instructionBundle.operationType;
                issueQueueIt->instructionBundle.currentRank = dispatchIt->instructionBundle.currentRank;

                // Copy renamed register values to the issue queue.
                issueQueueIt->destinationRegister = dispatchIt->destinationRegister;
                issueQueueIt->sourceRegister1 = dispatchIt->sourceRegister1;
                issueQueueIt->sourceRegister2 = dispatchIt->sourceRegister2;

                // Break after pushing the instruction into the Issue Queue.
                break;
            }
        }
    }
}

    } return;
}


inline void superScalar::RegisterRead() {

    for(auto iterator = registerReadPipelineDS.begin(); iterator != registerReadPipelineDS.end(); iterator++) {

        if(iterator->instructionBundle.validBit == 1) {

            instructionStageCycleCounter[iterator->instructionBundle.currentRank].registerReadCycleCount++;
            instructionStageCycleCounter[iterator->instructionBundle.currentRank].dispatchCycleCount = instructionStageCycleCounter[iterator->instructionBundle.currentRank].registerReadCycleCount;
        }
    }

    if(checkRR()) {

        auto registerReadIt = registerReadPipelineDS.begin();
auto dispatchIt = dispatchPipelineDS.begin();

for (; registerReadIt != registerReadPipelineDS.end() && dispatchIt != dispatchPipelineDS.end(); ++registerReadIt, ++dispatchIt) {
    if (registerReadIt->instructionBundle.validBit == 1) {
        // Check if the instructions in the register read pipeline are valid.

        // Check readiness of sourceRegister1 and update if ready.
        if (registerReadIt->sourceRegister1 != -1 &&
            reorderBuffer[registerReadIt->sourceRegister1].readyBit == 1) {
            registerReadIt->sourceRegister1 = -1;
        }

        // Check readiness of sourceRegister2 and update if ready.
        if (registerReadIt->sourceRegister2 != -1 &&
            reorderBuffer[registerReadIt->sourceRegister2].readyBit == 1) {
            registerReadIt->sourceRegister2 = -1;
        }

        // Advance the instruction to the dispatch stage.
        dispatchIt->instructionBundle.validBit = 1;
        dispatchIt->instructionBundle.destinationRegister = registerReadIt->instructionBundle.destinationRegister;
        dispatchIt->instructionBundle.sourceRegister1 = registerReadIt->instructionBundle.sourceRegister1;
        dispatchIt->instructionBundle.sourceRegister2 = registerReadIt->instructionBundle.sourceRegister2;
        dispatchIt->instructionBundle.programCounter = registerReadIt->instructionBundle.programCounter;
        dispatchIt->instructionBundle.operationType = registerReadIt->instructionBundle.operationType;

        // Copy renamed registers to the dispatch pipeline.
        dispatchIt->destinationRegister = registerReadIt->destinationRegister;
        dispatchIt->sourceRegister1 = registerReadIt->sourceRegister1;
        dispatchIt->sourceRegister2 = registerReadIt->sourceRegister2;

        // Copy the rank of the instruction.
        dispatchIt->instructionBundle.currentRank = registerReadIt->instructionBundle.currentRank;

        // Reset the valid bit for the next cycle.
        registerReadIt->instructionBundle.validBit = 0;
    }
}

    }
    return;

}
inline void superScalar::Rename() {

    for(auto iterator = renamePipelineDS.begin(); iterator != renamePipelineDS.end(); iterator++) {

        if(iterator->instructionBundle.validBit == 1) {

            instructionStageCycleCounter[iterator->instructionBundle.currentRank].renameCycleCount++;
            instructionStageCycleCounter[iterator->instructionBundle.currentRank].registerReadCycleCount = instructionStageCycleCounter[iterator->instructionBundle.currentRank].renameCycleCount;
        }
    }



    // Check if ROB tailPointer != robSize and  RR pipeline is empty
    if(checkRN()) {

         auto renameIt = renamePipelineDS.begin();
    auto registerReadIt = registerReadPipelineDS.begin();

    for (; renameIt != renamePipelineDS.end() && registerReadIt != registerReadPipelineDS.end(); ++renameIt, ++registerReadIt) {
        if (renameIt->instructionBundle.validBit == 1) {
            // Allocate entry in ROB
            reorderBuffer[tailPointer].validBit = 1;
            reorderBuffer[tailPointer].destination = renameIt->instructionBundle.destinationRegister;
            reorderBuffer[tailPointer].programCounter = renameIt->instructionBundle.programCounter;
            reorderBuffer[tailPointer].currentRank = renameIt->instructionBundle.currentRank;
            reorderBuffer[tailPointer].operationType = renameIt->instructionBundle.operationType;
            reorderBuffer[tailPointer].sourceRegister1 = renameIt->instructionBundle.sourceRegister1;
            reorderBuffer[tailPointer].sourceRegister2 = renameIt->instructionBundle.sourceRegister2;
            reorderBuffer[tailPointer].readyBit = 0;

            // Rename source registers
            if (renameIt->instructionBundle.sourceRegister1 != -1 &&
                renameMapTable[renameIt->instructionBundle.sourceRegister1].validBit == 1) {
                renameIt->sourceRegister1 = renameMapTable[renameIt->instructionBundle.sourceRegister1].robTag;
            }

            if (renameIt->instructionBundle.sourceRegister2 != -1 &&
                renameMapTable[renameIt->instructionBundle.sourceRegister2].validBit == 1) {
                renameIt->sourceRegister2 = renameMapTable[renameIt->instructionBundle.sourceRegister2].robTag;
            }

            // Rename destination register
            renameIt->destinationRegister = reorderBuffer[tailPointer].currentIndex;

            if (renameIt->destinationRegister != -1) {
                renameMapTable[renameIt->instructionBundle.destinationRegister].validBit = 1;
                renameMapTable[renameIt->instructionBundle.destinationRegister].robTag = reorderBuffer[tailPointer].currentIndex;
            }

            // Increment tail pointer
            tailPointer = (tailPointer + 1) % robSize;

            // Advance to the next pipeline stage
            registerReadIt->instructionBundle.validBit = 1;
            registerReadIt->instructionBundle.programCounter = renameIt->instructionBundle.programCounter;
            registerReadIt->instructionBundle.destinationRegister = renameIt->destinationRegister;
            registerReadIt->instructionBundle.sourceRegister1 = renameIt->sourceRegister1;
            registerReadIt->instructionBundle.sourceRegister2 = renameIt->sourceRegister2;
            registerReadIt->instructionBundle.currentRank = renameIt->instructionBundle.currentRank;
            registerReadIt->instructionBundle.operationType = renameIt->instructionBundle.operationType;
            registerReadIt->destinationRegister = renameIt->destinationRegister;
            registerReadIt->sourceRegister1 = renameIt->sourceRegister1;
            registerReadIt->sourceRegister2 = renameIt->sourceRegister2;

            // Reset valid bit for next cycle
            renameIt->instructionBundle.validBit = 0;
        }
    }
    }
    return;
}



inline void superScalar::Decode() {


    for(auto iterator = decodePipelineDS.begin(); iterator != decodePipelineDS.end(); iterator++) {

        if(iterator->instructionBundle.validBit == 1) {

            instructionStageCycleCounter[iterator->instructionBundle.currentRank].decodeCycleCount++;
            instructionStageCycleCounter[iterator->instructionBundle.currentRank].renameCycleCount = instructionStageCycleCounter[iterator->instructionBundle.currentRank].decodeCycleCount;
        }
    }

    if(checkDE()) {
        auto decodeIt = decodePipelineDS.begin();
        auto renameIt = renamePipelineDS.begin();

        while (decodeIt != decodePipelineDS.end() && renameIt != renamePipelineDS.end()) {
            if (decodeIt->instructionBundle.validBit == 1) {
                // Advance the stage by moving the instruction bundles to next stage
                renameIt->instructionBundle = decodeIt->instructionBundle;
                decodeIt->instructionBundle.validBit = 0;

                renameIt->destinationRegister = -1;
                renameIt->sourceRegister1 = -1;
                renameIt->sourceRegister2 = -1;
            }

            ++decodeIt;
            ++renameIt;
        }
    }

    return;
}


void superScalar::Fetch() {

    // Check if fetch criteria is met

    if(checkFE()) {
        auto decodeIt = decodePipelineDS.begin();
        while (decodeIt != decodePipelineDS.end()) {
            if (fscanf(filePointer, "%lx %d %d %d %d", &pc, &op_type, &dest, &src1, &src2) == 5) {
                // Set the valid bit of the instruction being advanced to the decode stage
                decodeIt->instructionBundle.validBit = 1;
                decodeIt->instructionBundle.destinationRegister = dest;
                decodeIt->instructionBundle.sourceRegister1 = src1;
                decodeIt->instructionBundle.sourceRegister2 = src2;
                decodeIt->instructionBundle.programCounter = pc;
                decodeIt->instructionBundle.operationType = op_type;
                decodeIt->instructionBundle.currentRank = currentInstructionCount;

                // Insert the cycle count into the instructionStageCycleCounter
                instructionStageCycleCounter.insert(
                    instructionStageCycleCounter.begin() + currentInstructionCount,
                    {cycleCount, cycleCount, 0, 0, 0, 0, 0, 0, 0}
                );

                currentInstructionCount++;
            }
            ++decodeIt;
        }
    }
}







inline uint32_t superScalar::checkRE() {


    for(uint32_t i=0; i< robSize; i++) {
// Atleast 1 rob entry is ready to be retired
        if(reorderBuffer[i].validBit == 1)
            return 1;

    }
    return 0;
}




inline uint32_t superScalar::checkWB() {


    for(uint32_t i=0; i< width*5; i++) {

        if(writeBackPipelineDS[i].instructionBundle.validBit == 1)
            return 1;

    }
    return 0;
}

inline uint32_t superScalar::checkEX() {

    for(uint32_t i=0; i< width*5; i++) {

        if(executePipelineDS[i].instructionBundle.validBit == 1)
            return 1;

    }
    return 0;


}

inline uint32_t superScalar::checkIS() {



    for(uint32_t i=0; i< iqSize; i++) {

        if(issueQueueDS[i].instructionBundle.validBit == 1)
            return 1;


    }return 0;
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
        if(issueQueueDS[j].instructionBundle.validBit == 0)
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



inline uint32_t superScalar::checkRN() {

    uint32_t renameCounter = 0;
    uint32_t registerReadCounter = 0;
    uint32_t reorderBufferCounter = 0;

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

    for(uint32_t i =0; i< robSize; i++) {

        if(reorderBuffer[i].validBit == 0)
            reorderBufferCounter++;
    }

    if(renameCounter <= width && renameCounter > 0 && registerReadCounter == width && reorderBufferCounter >= renameCounter)
        return 1;

    return 0;
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

inline uint32_t superScalar::checkFE() {


    uint32_t decodeCounter = 0;

    // Check if the decode Pipeline Registers are empty/ invalid
    for(uint32_t i = 0; i < width; i++) {
        if(decodePipelineDS[i].instructionBundle.validBit == 0) {
            decodeCounter += 1;
        }

    }

    // If the DE pipeline registers are empty and the trace file has traces left return 1,
    // to denote fetch
    if(decodeCounter == width && !feof(filePointer))
        return 1;


    return 0;

}

void superScalar::InitialisePipelineDS() {


    for(uint32_t i = 0; i < iqSize; i++) {

        issueQueueDS[i].instructionBundle.validBit = 0;
        issueQueueDS[i].instructionBundle.validBit = 0;
        issueQueueDS[i].instructionBundle.programCounter = 0;
        issueQueueDS[i].instructionBundle.destinationRegister = -1;
        issueQueueDS[i].instructionBundle.operationType = -1;
        issueQueueDS[i].instructionBundle.currentRank =-1;
        issueQueueDS[i].instructionBundle.sourceRegister1 = -1;
        issueQueueDS[i].instructionBundle.sourceRegister2 = -1;



        issueQueueDS[i].destinationRegister= -1;
        issueQueueDS[i].sourceRegister1 = -1;
        issueQueueDS[i].sourceRegister2 = -1;

    }

    for(uint32_t i = 0; i < robSize; i++) {

        reorderBuffer[i].validBit = 0;
        reorderBuffer[i].destination = -1;
        reorderBuffer[i].readyBit = 0;
        reorderBuffer[i].programCounter = 0;
        reorderBuffer[i].currentIndex = i;

        reorderBuffer[i].destination = -1;
        reorderBuffer[i].sourceRegister1 = -1;
        reorderBuffer[i].sourceRegister2 = -1;
        reorderBuffer[i].operationType = -1;

    }

    for(uint32_t i = 0; i < NUMBER_OF_REGISTERS; i++) {

        renameMapTable[i].validBit = 0;
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