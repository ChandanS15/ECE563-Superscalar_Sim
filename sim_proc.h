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
    uint32_t decodeDuration;
    uint32_t renameCycleCount;
    uint32_t renameDuration;
    uint32_t registerReadCycleCount;
    uint32_t registerReadDuration;
    uint32_t dispatchCycleCount;
    uint32_t dispatchDuration;
    uint32_t issueCycleCount;
    uint32_t issueDuration;
    uint32_t executeCycleCount;
    uint32_t executeDuration;
    uint32_t writeBackCycleCount;
    uint32_t writeBackDuration;
    uint32_t retireCycleCount;
    uint32_t retireDuration;

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
    void DecodeCycleUpdate();

    void Rename();
    uint32_t checkRN();
    void RenameCycleUpdate();

    void RegisterRead();
    uint32_t checkRR();
    void RegisterReadCycleUpdate();


    void Dispatch();
    uint32_t checkDS();
    void DispatchCycleUpdate();


    void Issue();
    uint32_t checkIS();
    void IssueCycleUpdate();

    void Writeback();
    uint32_t checkWB();
    void WriteBackCycleUpdate();


    void Retire();
    uint32_t checkRE();
    void RetireCycleUpdate();

    void WakeUpInstruction(std::vector<executePipeline>::iterator execIt);


    void Execute();
    uint32_t checkEX();
    void ExecuteCycleUpdate();

    int32_t Advance_Cycle();

    void CalculateDuration(uint32_t head);

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

    if(feof(filePointer)) {

    //if(advanceCyleEnable ) {

        // After fetching the last instruction from the trace file

        // check if there are valid instructions in the pipeline
        auto dispatchIt = dispatchPipelineDS.begin();
      auto renameIt = renamePipelineDS.begin();
      auto registerReadIt = registerReadPipelineDS.begin();
      auto decodeIt = decodePipelineDS.begin();
        for (;   dispatchIt != dispatchPipelineDS.end();
             ++dispatchIt, ++renameIt, ++registerReadIt, ++decodeIt) {

            if (dispatchIt->instructionBundle.validBit == 1 ||
                renameIt->instructionBundle.validBit == 1 ||
                registerReadIt->instructionBundle.validBit == 1 ||
                decodeIt->instructionBundle.validBit == 1)
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
        auto executeIt = executePipelineDS.begin();
        auto writeBackIt = writeBackPipelineDS.begin();
        for (;
             executeIt != executePipelineDS.end() && writeBackIt != writeBackPipelineDS.end();
             ++executeIt, ++writeBackIt) {

            if (executeIt->instructionBundle.validBit == 1 || writeBackIt->instructionBundle.validBit == 1)
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
inline void superScalar::RetireCycleUpdate() {
    for(auto iterator = reorderBuffer.begin(); iterator != reorderBuffer.end(); iterator++) {

        if(iterator->validBit == 1 && iterator->readyBit == 1) {

            instructionStageCycleCounter[iterator->currentRank].retireCycleCount++;
        }
    }
}
inline void superScalar::Retire() {

    RetireCycleUpdate();


    if(checkRE()) {
        // here if the ROB entry pointed by the head is ready/valid
        // compare the destination register with RMT entry
        // unconditionally update the ARF
        // If the robTag in the RMT and the ROB entry are saem remove and restt the valid bit in RMT as it says that the commited value in
        // ARF is the final one and not thr RMT.
        for(uint32_t i=0; i< width; i++) {
            if(reorderBuffer[headPointer].readyBit == 1) {
                if(reorderBuffer[headPointer].validBit == 1) {
                    CalculateDuration(headPointer);

                    cout<< reorderBuffer[headPointer].currentRank
                    <<" fu{" <<reorderBuffer[headPointer].operationType<<"}"
                    <<" src{" <<reorderBuffer[headPointer].sourceRegister1<< "," <<reorderBuffer[headPointer].sourceRegister2<< "}"
                    <<" dst{" <<reorderBuffer[headPointer].destination<<"}"
                    <<" FE{"<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].fetchCycleCount<<",1}"
                    <<" DE{"<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].fetchCycleCount + 1<<","<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].decodeDuration<< "}"
                    <<" RN{"<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].decodeCycleCount + 1<<","<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].renameDuration << "}"
                    <<" RR{"<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].renameCycleCount + 1<<","<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].registerReadDuration << "}"
                    <<" DI{"<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].registerReadCycleCount + 1<<","<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].dispatchDuration << "}"
                    <<" IS{"<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].dispatchCycleCount + 1<<","<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].issueDuration<< "}"
                    <<" EX{"<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].issueCycleCount + 1<<","<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].executeDuration<< "}"
                    <<" WB{"<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].executeCycleCount + 1<<","<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].writeBackDuration<< "}"
                    <<" RT{"<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].writeBackCycleCount + 1<<","<< instructionStageCycleCounter[reorderBuffer[headPointer].currentRank].retireDuration<< "}"

                    <<endl;

                }

                reorderBuffer[headPointer].validBit = 0;

                // reset the rmt is the headpointer and hthe robtag entry are same.
                if((renameMapTable[reorderBuffer[headPointer].destination].robTag ==
                    headPointer) && reorderBuffer[headPointer].destination != -1) {
                    // reset the RMT entry

                    renameMapTable[reorderBuffer[headPointer].destination].validBit = 0;
                    renameMapTable[reorderBuffer[headPointer].destination].robTag = -1;

                }
                // AFter retiring increment the headPointer

                headPointer = (headPointer + 1) % robSize;
            }
        }
    }
}

inline void superScalar::CalculateDuration(uint32_t head) {

     instructionStageCycleCounter[reorderBuffer[head].currentRank].decodeDuration =  instructionStageCycleCounter[reorderBuffer[head].currentRank].decodeCycleCount -instructionStageCycleCounter[reorderBuffer[head].currentRank].fetchCycleCount ;
      instructionStageCycleCounter[reorderBuffer[head].currentRank].renameDuration =                instructionStageCycleCounter[reorderBuffer[head].currentRank].renameCycleCount -instructionStageCycleCounter[reorderBuffer[head].currentRank].decodeCycleCount;
      instructionStageCycleCounter[reorderBuffer[head].currentRank].registerReadDuration =                instructionStageCycleCounter[reorderBuffer[head].currentRank].registerReadCycleCount -instructionStageCycleCounter[reorderBuffer[head].currentRank].renameCycleCount;
       instructionStageCycleCounter[reorderBuffer[head].currentRank].dispatchDuration =               instructionStageCycleCounter[reorderBuffer[head].currentRank].dispatchCycleCount -instructionStageCycleCounter[reorderBuffer[head].currentRank].registerReadCycleCount;
       instructionStageCycleCounter[reorderBuffer[head].currentRank].issueDuration =               instructionStageCycleCounter[reorderBuffer[head].currentRank].issueCycleCount -instructionStageCycleCounter[reorderBuffer[head].currentRank].dispatchCycleCount;
       instructionStageCycleCounter[reorderBuffer[head].currentRank].executeDuration =               instructionStageCycleCounter[reorderBuffer[head].currentRank].executeCycleCount -instructionStageCycleCounter[reorderBuffer[head].currentRank].issueCycleCount;
       instructionStageCycleCounter[reorderBuffer[head].currentRank].writeBackDuration =              instructionStageCycleCounter[reorderBuffer[head].currentRank].writeBackCycleCount -instructionStageCycleCounter[reorderBuffer[head].currentRank].executeCycleCount;
       instructionStageCycleCounter[reorderBuffer[head].currentRank].retireDuration =               instructionStageCycleCounter[reorderBuffer[head].currentRank].retireCycleCount -instructionStageCycleCounter[reorderBuffer[head].currentRank].writeBackCycleCount;


    
}


inline void superScalar::WriteBackCycleUpdate() {
    for(auto iterator = writeBackPipelineDS.begin(); iterator != writeBackPipelineDS.end(); iterator++) {

        if(iterator->instructionBundle.validBit == 1) {

            instructionStageCycleCounter[iterator->instructionBundle.currentRank].writeBackCycleCount++;
            instructionStageCycleCounter[iterator->instructionBundle.currentRank].retireCycleCount = instructionStageCycleCounter[iterator->instructionBundle.currentRank].writeBackCycleCount;
        }
    }


}

inline void superScalar::Writeback() {
    WriteBackCycleUpdate();



    // In writeback the data bypass has to be maintained i.e.
    // The updates has to be maintained across issue queue, ROB and as a bypass to the execute stage so that the inflight instruction that requires the value can directly
    // have access to the latest value.
    if(checkWB()) {
        for (auto wbIt = writeBackPipelineDS.begin(); wbIt != writeBackPipelineDS.end(); ++wbIt) {
            if (wbIt->instructionBundle.validBit == 1) {

                // Update the Reorder Buffer (ROB) by setting the ready bit to 1
                for (auto robIt = reorderBuffer.begin(); robIt != reorderBuffer.end(); ++robIt) {

                    if (robIt->validBit == 1 &&
                        robIt->currentIndex == wbIt->destinationRegister &&
                        robIt->readyBit == 0) {

                        if (wbIt->destinationRegister != -1) {
                            // Set the ready bit in the ROB
                            robIt->readyBit = 1;

                            // Reset the valid bit in the write-back pipeline for the next instruction
                            wbIt->instructionBundle.validBit = 0;
                            break; // Exit ROB loop as the matching entry is updated
                        }
                        }
                }
            }
        }

    }
}


inline void superScalar::ExecuteCycleUpdate() {

    for(auto iterator = executePipelineDS.begin(); iterator != executePipelineDS.end(); iterator++) {

        if(iterator->instructionBundle.validBit == 1) {

            instructionStageCycleCounter[iterator->instructionBundle.currentRank].executeCycleCount++;
            instructionStageCycleCounter[iterator->instructionBundle.currentRank].writeBackCycleCount = instructionStageCycleCounter[iterator->instructionBundle.currentRank].executeCycleCount;
        }
    }
}

inline void superScalar::WakeUpInstruction(std::vector<executePipeline>::iterator execIt) {

    for (auto issueIt = issueQueueDS.begin(); issueIt != issueQueueDS.end(); ++issueIt) {
        if (issueIt->instructionBundle.validBit == 1) {
            if (issueIt->sourceRegister1 != -1 && execIt->destinationRegister == issueIt->sourceRegister1) {
                issueIt->sourceRegister1 = -1;
                issueIt->sourceRegister1Ready = true;
            }

            if (issueIt->sourceRegister2 != -1 && execIt->destinationRegister == issueIt->sourceRegister2) {
                issueIt->sourceRegister2 = -1;
                issueIt->sourceRegister2Ready = true;
            }
        }
    }

    for (auto dispIt = dispatchPipelineDS.begin(); dispIt != dispatchPipelineDS.end(); ++dispIt) {
        if (dispIt->instructionBundle.validBit == 1) {
            if (dispIt->sourceRegister1 != -1 && execIt->destinationRegister == dispIt->sourceRegister1)
                dispIt->sourceRegister1 = -1;

            if (dispIt->sourceRegister2 != -1 && execIt->destinationRegister == dispIt->sourceRegister2)
                dispIt->sourceRegister2 = -1;
        }
    }

    for (auto rrIt = registerReadPipelineDS.begin(); rrIt != registerReadPipelineDS.end(); ++rrIt) {
        if (rrIt->instructionBundle.validBit == 1) {
            if (rrIt->sourceRegister1 != -1 && execIt->destinationRegister == rrIt->sourceRegister1)
                rrIt->sourceRegister1 = -1;

            if (rrIt->sourceRegister2 != -1 && execIt->destinationRegister == rrIt->sourceRegister2)
                rrIt->sourceRegister2 = -1;
        }
    }
}


inline void superScalar::Execute() {
    ExecuteCycleUpdate();
    if(checkEX()) {

        // Iterate through the execute pipeline
        for (auto execIt = executePipelineDS.begin(); execIt != executePipelineDS.end(); ++execIt) {
            // Decrement the timer for every valid instruction in the execute stage
            if (execIt->instructionBundle.validBit == 1) {
                execIt->waitCycles--;

                // If the instruction is ready to move to the write-back stage
                if (execIt->waitCycles == 0) {

                    WakeUpInstruction(execIt);

                    // Resolve dependencies in the dispatch and register read pipelines


                    // Move the instruction to the write-back pipeline
                    for (auto wbIt = writeBackPipelineDS.begin(); wbIt != writeBackPipelineDS.end(); ++wbIt) {
                        if (wbIt->instructionBundle.validBit == 0) {

                            // Transfer data to write-back pipeline
                            wbIt->instructionBundle = execIt->instructionBundle;
                            wbIt->destinationRegister = execIt->destinationRegister;
                            wbIt->sourceRegister1 = execIt->sourceRegister1;
                            wbIt->sourceRegister2 = execIt->sourceRegister2;

                            // Mark the execute pipeline entry as invalid
                            execIt->instructionBundle.validBit = 0;
                            break;
                        }
                    }
                }
            }
        }
    }
}

inline void superScalar::IssueCycleUpdate() {

    for(auto iterator = issueQueueDS.begin(); iterator != issueQueueDS.end(); iterator++) {

        if(iterator->instructionBundle.validBit == 1) {

            instructionStageCycleCounter[iterator->instructionBundle.currentRank].issueCycleCount++;
            instructionStageCycleCounter[iterator->instructionBundle.currentRank].executeCycleCount = instructionStageCycleCounter[iterator->instructionBundle.currentRank].issueCycleCount;
        }
    }
}

inline void superScalar::Issue() {
    IssueCycleUpdate();


    // In the issue queue I should be issuing upto WIDTH oldest instructions from the issueQuquq.
    // In order to achieve this I will be using the rank that was given to each instruction while fetching from the trace file



    if(checkIS()) {

        // sorting the instructions in the Issue Queue based on their rank.

        for (auto i = issueQueueDS.begin(); i != issueQueueDS.end() - 1; ++i) {
            for (auto j = i + 1; j != issueQueueDS.end(); ++j) {
                // First condition: Sort by validBit in descending order
                if (i->instructionBundle.validBit < j->instructionBundle.validBit) {
                    std::swap(*i, *j);
                }
                // Second condition: Sort by currentRank if validBit is 1
                else if ((i->instructionBundle.validBit == 1) &&
                         (j->instructionBundle.validBit == 1) &&
                         (i->instructionBundle.currentRank > j->instructionBundle.currentRank) &&
                         (j->instructionBundle.currentRank != -1)) {
                    std::swap(*i, *j);
                         }
            }
        }



        auto issueIt = issueQueueDS.begin();
int issueCounter = 0;

        for (; issueIt != issueQueueDS.end(); ++issueIt) {
            if (issueIt->instructionBundle.validBit == 1 &&
                issueIt->sourceRegister1 == -1 &&
                issueIt->sourceRegister2 == -1) {

                auto executeIt = executePipelineDS.begin();
                for (; executeIt != executePipelineDS.end(); ++executeIt) {
                    if (executeIt->instructionBundle.validBit == 0) {
                        // Populate the execute pipeline with the instruction
                        executeIt->instructionBundle.validBit = 1;
                        executeIt->instructionBundle.destinationRegister        = issueIt->instructionBundle.destinationRegister;
                        executeIt->instructionBundle.sourceRegister1            = issueIt->instructionBundle.sourceRegister1;
                        executeIt->instructionBundle.sourceRegister2            = issueIt->instructionBundle.sourceRegister2;
                        executeIt->instructionBundle.programCounter             = issueIt->instructionBundle.programCounter;
                        executeIt->instructionBundle.currentRank                = issueIt->instructionBundle.currentRank;
                        executeIt->instructionBundle.operationType = issueIt->instructionBundle.operationType;

                        // Propagate renamed register values
                        executeIt->destinationRegister = issueIt->destinationRegister;
                        executeIt->sourceRegister1 = issueIt->sourceRegister1;
                        executeIt->sourceRegister2 = issueIt->sourceRegister2;

                        // Set wait cycles based on operation type
                        if (issueIt->instructionBundle.operationType == 0)
                            executeIt->waitCycles = 1;
                        else if (issueIt->instructionBundle.operationType == 1)
                            executeIt->waitCycles = 2;
                        else if (issueIt->instructionBundle.operationType == 2)
                            executeIt->waitCycles = 5;

                        // Mark the issue queue entry as invalid
                        issueIt->instructionBundle.validBit = 0;
                        issueIt->instructionBundle.currentRank = -1;
                        ++issueCounter;

                        break; // Move to the next issue queue entry
                    }
                }

                // Stop after issuing the maximum number of instructions for the width
                if (issueCounter == width)
                    return;
            }
        }
    }
}

inline void superScalar::DispatchCycleUpdate() {

    for(auto iterator = dispatchPipelineDS.begin(); iterator != dispatchPipelineDS.end(); iterator++) {

        if(iterator->instructionBundle.validBit == 1) {

            instructionStageCycleCounter[iterator->instructionBundle.currentRank].dispatchCycleCount++;
            instructionStageCycleCounter[iterator->instructionBundle.currentRank].issueCycleCount = instructionStageCycleCounter[iterator->instructionBundle.currentRank].dispatchCycleCount;
        }
    }
}

inline void superScalar::Dispatch() {
    DispatchCycleUpdate();



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

    }
}
inline void superScalar::RegisterReadCycleUpdate() {

    for(auto iterator = registerReadPipelineDS.begin(); iterator != registerReadPipelineDS.end(); iterator++) {

        if(iterator->instructionBundle.validBit == 1) {

            instructionStageCycleCounter[iterator->instructionBundle.currentRank].registerReadCycleCount++;
            instructionStageCycleCounter[iterator->instructionBundle.currentRank].dispatchCycleCount = instructionStageCycleCounter[iterator->instructionBundle.currentRank].registerReadCycleCount;
        }
    }
}

inline void superScalar::RegisterRead() {
    RegisterReadCycleUpdate() ;


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


}

inline void superScalar::RenameCycleUpdate() {

    for(auto iterator = renamePipelineDS.begin(); iterator != renamePipelineDS.end(); iterator++) {

        if(iterator->instructionBundle.validBit == 1) {

            instructionStageCycleCounter[iterator->instructionBundle.currentRank].renameCycleCount++;
            instructionStageCycleCounter[iterator->instructionBundle.currentRank].registerReadCycleCount = instructionStageCycleCounter[iterator->instructionBundle.currentRank].renameCycleCount;
        }
    }
}
inline void superScalar::Rename() {

    // Processing the rename bundle -
    // 1. Allocate an entry in the ROB for the instruction
    // 2. Rename its source register i.e now the source registers become the entry index of ROB
    // 3. If it has a destination register rename it as well.

    RenameCycleUpdate();



    // Check if ROB tailPointer != robSize and  RR pipeline is empty
    if(checkRN()) {

         auto renameIt = renamePipelineDS.begin();
    auto registerReadIt = registerReadPipelineDS.begin();

    for (; renameIt != renamePipelineDS.end() && registerReadIt != registerReadPipelineDS.end(); ++renameIt, ++registerReadIt) {

        if (renameIt->instructionBundle.validBit == 1) {
            // Preliminary check to see the validity of the instruction.

            // Rename Steps -
            // 1. Allocate free entry at tail of the ROB by replacing the destination specifier with unique identity
            // of ROB Tag.
            // 2. Rename the sources by checking the RMT valid state. i.e. if the RMT of the source register specifier is valid in RMT
            // replace it with the RMT robTag into the pipeline if not there are no inflight instruction producing the register value hence
            // use ARF.
            // 3. Update the RMT with current instruction destination identifier which is the robTag being pointed by tailpointer
            // and increment the tailPointer.

            reorderBuffer[tailPointer].validBit = 1;
            // Step 1 of renaming
            // Allocate the entry pointed by tail pointer and update the destination register specifier
            reorderBuffer[tailPointer].destination = renameIt->instructionBundle.destinationRegister;
            // copy the PC to ROB
            reorderBuffer[tailPointer].programCounter = renameIt->instructionBundle.programCounter;
            // copy the index of instr into the ROB
            // This rank is used to identify the instruction rank between width number of instructions being fetched.
            reorderBuffer[tailPointer].currentRank = renameIt->instructionBundle.currentRank;
            reorderBuffer[tailPointer].operationType = renameIt->instructionBundle.operationType;
            reorderBuffer[tailPointer].sourceRegister1 = renameIt->instructionBundle.sourceRegister1;
            reorderBuffer[tailPointer].sourceRegister2 = renameIt->instructionBundle.sourceRegister2;
            // The current tail pointer value serves as the ROB tag
            //reorderBuffer[tailPointer].currentIndex = tailPointer;
            // Reset ready bit as it is not yet executed.
            reorderBuffer[tailPointer].readyBit = 0;

            // Rename source registers

            // // Set the ROB entry to be valid
            // reorderBuffer[tailPointer].validBit = 1;

            // Now that ROB entry of destination register is done move onto the renaming the source registers.

            // In this stage check if the source register is being used in the current instruction,
            // If yes then check if it has an entry in RMT (which would servev as the latest version of that register as opposed to the
            // one in the ARF.

            // If the Valid Bit of RMT is 0 then use the ARF value which is latest.
            if (renameIt->instructionBundle.sourceRegister1 != -1 &&
                renameMapTable[renameIt->instructionBundle.sourceRegister1].validBit == 1) {
                // Because there already exist a latest version of the required register specifier use it by replacing the ROB Tag present in the RMT.

                renameIt->sourceRegister1 = renameMapTable[renameIt->instructionBundle.sourceRegister1].robTag;
            }
            // If the Valid Bit of RMT is 0 then use the ARF value which is latest.
            if (renameIt->instructionBundle.sourceRegister2 != -1 &&
                renameMapTable[renameIt->instructionBundle.sourceRegister2].validBit == 1) {
                // Because there already exist a latest version of the required register specifier use it by replacing the ROB Tag present in the RMT.

                renameIt->sourceRegister2 = renameMapTable[renameIt->instructionBundle.sourceRegister2].robTag;
            }

            // Rename destination register
            // After renaming the destination and source registers now, update the latest ROB tags in the RMT to reflect these renames.

            // Here setting the current ith instruction destination to the destination register specifier of the ROB entry pointed by the tailPointer.

            renameIt->destinationRegister = reorderBuffer[tailPointer].currentIndex;

            // If the destination register != -1 i.e not a branch instruction then the renamed sources must also be updated

            if (renameIt->destinationRegister != -1) {
                renameMapTable[renameIt->instructionBundle.destinationRegister].validBit = 1;
                renameMapTable[renameIt->instructionBundle.destinationRegister].robTag = reorderBuffer[tailPointer].currentIndex;
            }

            // After ROB entry increment tail pointer for next cycle updation.
            // check if ROB is full, as it is a circular buffer restart from the first entry.(oth index)
            tailPointer = (tailPointer + 1) % robSize;

            // After renaming the instruction bundles and updating RMT , ROB advance the instructions to next stage



            // Advance the renamed PipelineRegister values into Register read stage.
            // Advance to the next pipeline stage
            registerReadIt->instructionBundle.validBit = 1;
            registerReadIt->instructionBundle.programCounter = renameIt->instructionBundle.programCounter;
            registerReadIt->instructionBundle.destinationRegister = renameIt->destinationRegister;
            registerReadIt->instructionBundle.sourceRegister1 = renameIt->sourceRegister1;
            registerReadIt->instructionBundle.sourceRegister2 = renameIt->sourceRegister2;
            registerReadIt->instructionBundle.currentRank = renameIt->instructionBundle.currentRank;
            registerReadIt->instructionBundle.operationType = renameIt->instructionBundle.operationType;

            // Copy the renamed source and destination values.

            registerReadIt->destinationRegister = renameIt->destinationRegister;
            registerReadIt->sourceRegister1 = renameIt->sourceRegister1;
            registerReadIt->sourceRegister2 = renameIt->sourceRegister2;

            // Because of renaming set validBit to 0 so that right after width number of instructions are advanced next set of WIDTH instructions are extracted into fetch and decode.

            // Reset valid bit for next cycle
            renameIt->instructionBundle.validBit = 0;
        }
    }
    }

}


inline void superScalar::DecodeCycleUpdate() {

    for(auto iterator = decodePipelineDS.begin(); iterator != decodePipelineDS.end(); iterator++) {

        if(iterator->instructionBundle.validBit == 1) {

            instructionStageCycleCounter[iterator->instructionBundle.currentRank].decodeCycleCount++;
            instructionStageCycleCounter[iterator->instructionBundle.currentRank].renameCycleCount = instructionStageCycleCounter[iterator->instructionBundle.currentRank].decodeCycleCount;
        }
    }
}
inline void superScalar::Decode() {
    DecodeCycleUpdate();


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


    for (auto it = reorderBuffer.begin(); it != reorderBuffer.end(); ++it) {
        if (it->validBit == 1)
            return 1;
    }

    return 0;
}




inline uint32_t superScalar::checkWB() {


    for (auto it = writeBackPipelineDS.begin(); it != writeBackPipelineDS.end(); ++it) {
        if (it->instructionBundle.validBit == 1)
            return 1;
    }

    return 0;
}

inline uint32_t superScalar::checkEX() {

    for (auto it = executePipelineDS.begin(); it != executePipelineDS.end(); ++it) {
        if (it->instructionBundle.validBit == 1)
            return 1;
    }

    return 0;


}

inline uint32_t superScalar::checkIS() {



    for (auto issueQueueIt = issueQueueDS.begin(); issueQueueIt != issueQueueDS.end(); ++issueQueueIt) {
        if (issueQueueIt->instructionBundle.validBit == 1) {
            return 1;
        }
    }
    return 0;
}


inline uint32_t superScalar::checkDS() {
    // check dispatch piipeline registers validity
    // and if the issue queue has atleast width sizze of free entires
    uint32_t dispatchCounter = 0;
    uint32_t issueCounter = 0;

    // Loop over dispatchPipelineDS to count valid entries
    for (auto dispatchIt = dispatchPipelineDS.begin(); dispatchIt != dispatchPipelineDS.end(); ++dispatchIt) {
        if (dispatchIt->instructionBundle.validBit == 1) {
            dispatchCounter++;
        }
    }

    // Loop over issueQueueDS to check for empty entries
    for (auto issueQueueIt = issueQueueDS.begin(); issueQueueIt != issueQueueDS.end(); ++issueQueueIt) {
        if (issueQueueIt->instructionBundle.validBit == 0) {
            issueCounter++;
        }
    }


    if(dispatchCounter <= width && dispatchCounter >0) {

        // Check if issue queue has enough empty entries to accomodate all the dispatch bundles
        // if not do nothing
        if(issueCounter < dispatchCounter)
            return 0;
        else
            return 1;

    }
    return 0;

}

inline uint32_t superScalar::checkRR() {

    // If RR contains RR Bundle and DI is not empty do nothing

    uint32_t registerReadCounter = 0;
    uint32_t dispatchCounter = 0;

    //
    auto registerReadIt = registerReadPipelineDS.begin();
    auto dispatchIt = dispatchPipelineDS.begin();
    for (;
         registerReadIt != registerReadPipelineDS.end() && dispatchIt != dispatchPipelineDS.end();
         ++registerReadIt, ++dispatchIt) {

        // Check if registerRead pipeline registers are full
        if (registerReadIt->instructionBundle.validBit == 1) {
            registerReadCounter += 1;
        }

        // Check if dispatch pipeline registers are empty
        if (dispatchIt->instructionBundle.validBit == 0) {
            dispatchCounter += 1;
        }
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

    // Loop over the renamePipelineDS and registerReadPipelineDS vectors

    auto renameIt = renamePipelineDS.begin();
    auto registerReadIt = registerReadPipelineDS.begin();

    for (;
         renameIt != renamePipelineDS.end() && registerReadIt != registerReadPipelineDS.end();
         ++renameIt, ++registerReadIt) {

        // Check if rename stage contains instruction bundle
        if (renameIt->instructionBundle.validBit == 1) {
            renameCounter += 1;
        }

        // Check if register read stage contains instruction bundle
        if (registerReadIt->instructionBundle.validBit == 0) {
            registerReadCounter += 1;
        }
         }

    // Loop over the reorderBuffer
    for (auto reorderIt = reorderBuffer.begin(); reorderIt != reorderBuffer.end(); ++reorderIt) {

        // Check if reorderBuffer entry is invalid
        if (reorderIt->validBit == 0) {
            reorderBufferCounter++;
        }
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
    auto decodeIt = decodePipelineDS.begin();
    auto renameIt = renamePipelineDS.begin();
    for (;
     decodeIt != decodePipelineDS.end() && renameIt != renamePipelineDS.end();
     ++decodeIt, ++renameIt) {

        // Check if DE contains instruction bundle
        if (decodeIt->instructionBundle.validBit == 1) {
            decodeCounter += 1;
        }

        // Check if rename stage contains any instruction bundle
        if (renameIt->instructionBundle.validBit == 0) {
            renameCounter += 1;
        }
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
    for (auto decodeIt = decodePipelineDS.begin(); decodeIt != decodePipelineDS.end(); ++decodeIt) {
        if (decodeIt->instructionBundle.validBit == 0) {
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


    // Initialize issueQueueDS using iterators
for (auto& issueQueue : issueQueueDS) {
    issueQueue.instructionBundle.validBit = 0;
    issueQueue.instructionBundle.programCounter = 0;
    issueQueue.instructionBundle.destinationRegister = -1;
    issueQueue.instructionBundle.operationType = -1;
    issueQueue.instructionBundle.currentRank = -1;
    issueQueue.instructionBundle.sourceRegister1 = -1;
    issueQueue.instructionBundle.sourceRegister2 = -1;

    issueQueue.destinationRegister = -1;
    issueQueue.sourceRegister1 = -1;
    issueQueue.sourceRegister2 = -1;
}

// Initialize reorderBuffer using iterators
for (auto& reorderEntry : reorderBuffer) {
    reorderEntry.validBit = 0;
    reorderEntry.destination = -1;
    reorderEntry.readyBit = 0;
    reorderEntry.programCounter = 0;
    reorderEntry.currentIndex = &reorderEntry - &reorderBuffer[0];

    reorderEntry.sourceRegister1 = -1;
    reorderEntry.sourceRegister2 = -1;
    reorderEntry.operationType = -1;
}

// Initialize renameMapTable using iterators
for (auto& renameEntry : renameMapTable) {
    renameEntry.validBit = 0;
    renameEntry.robTag = -1;
}

// Initialize decodePipelineDS, renamePipelineDS, registerReadPipelineDS, and dispatchPipelineDS using iterators
for (auto& decodeEntry : decodePipelineDS) {
    decodeEntry.instructionBundle.validBit = 0;
    decodeEntry.instructionBundle.programCounter = 0;
    decodeEntry.instructionBundle.destinationRegister = -1;
    decodeEntry.instructionBundle.operationType = -1;
    decodeEntry.instructionBundle.currentRank = -1;
    decodeEntry.instructionBundle.sourceRegister1 = -1;
    decodeEntry.instructionBundle.sourceRegister2 = -1;
}

for (auto& renameEntry : renamePipelineDS) {
    renameEntry.instructionBundle.validBit = 0;
    renameEntry.instructionBundle.programCounter = 0;
    renameEntry.instructionBundle.destinationRegister = -1;
    renameEntry.instructionBundle.operationType = -1;
    renameEntry.instructionBundle.currentRank = -1;
    renameEntry.instructionBundle.sourceRegister1 = -1;
    renameEntry.instructionBundle.sourceRegister2 = -1;
    renameEntry.destinationRegister = -1;
    renameEntry.sourceRegister1 = -1;
    renameEntry.sourceRegister2 = -1;
}

for (auto& registerEntry : registerReadPipelineDS) {
    registerEntry.instructionBundle.validBit = 0;
    registerEntry.instructionBundle.programCounter = 0;
    registerEntry.instructionBundle.destinationRegister = -1;
    registerEntry.instructionBundle.operationType = -1;
    registerEntry.instructionBundle.currentRank = -1;
    registerEntry.instructionBundle.sourceRegister1 = -1;
    registerEntry.instructionBundle.sourceRegister2 = -1;
    registerEntry.destinationRegister = -1;
    registerEntry.sourceRegister1 = -1;
    registerEntry.sourceRegister2 = -1;
}

for (auto& dispatchEntry : dispatchPipelineDS) {
    dispatchEntry.instructionBundle.validBit = 0;
    dispatchEntry.instructionBundle.programCounter = 0;
    dispatchEntry.instructionBundle.destinationRegister = -1;
    dispatchEntry.instructionBundle.operationType = -1;
    dispatchEntry.instructionBundle.currentRank = -1;
    dispatchEntry.instructionBundle.sourceRegister1 = -1;
    dispatchEntry.instructionBundle.sourceRegister2 = -1;
    dispatchEntry.destinationRegister = -1;
    dispatchEntry.sourceRegister1 = -1;
    dispatchEntry.sourceRegister2 = -1;
}

// Initialize executePipelineDS and writeBackPipelineDS using iterators
for (auto& executeEntry : executePipelineDS) {
    executeEntry.instructionBundle.validBit = 0;
    executeEntry.instructionBundle.programCounter = 0;
    executeEntry.instructionBundle.destinationRegister = -1;
    executeEntry.instructionBundle.operationType = -1;
    executeEntry.instructionBundle.currentRank = -1;
    executeEntry.instructionBundle.sourceRegister1 = -1;
    executeEntry.instructionBundle.sourceRegister2 = -1;
    executeEntry.destinationRegister = -1;
    executeEntry.sourceRegister1 = -1;
    executeEntry.sourceRegister2 = -1;
}

for (auto& writeBackEntry : writeBackPipelineDS) {
    writeBackEntry.instructionBundle.validBit = 0;
    writeBackEntry.instructionBundle.programCounter = 0;
    writeBackEntry.instructionBundle.destinationRegister = -1;
    writeBackEntry.instructionBundle.operationType = -1;
    writeBackEntry.instructionBundle.currentRank = -1;
    writeBackEntry.instructionBundle.sourceRegister1 = -1;
    writeBackEntry.instructionBundle.sourceRegister2 = -1;
    writeBackEntry.destinationRegister = -1;
    writeBackEntry.sourceRegister1 = -1;
    writeBackEntry.sourceRegister2 = -1;
}



}

















#endif