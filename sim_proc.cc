#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "sim_proc.h"

/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim 256 32 4 gcc_trace.txt
    argc = 5
    argv[0] = "sim"
    argv[1] = "256"
    argv[2] = "32"
    ... and so on
*/


int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    proc_params params;       // look at sim_bp.h header file for the the definition of struct proc_params
   // int op_type, dest, src1, src2;  // Variables are read from trace file
    //uint64_t pc; // Variable holds the pc read from input file

    if (argc != 5)
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }

    params.rob_size     = strtoul(argv[1], NULL, 10);
    params.iq_size      = strtoul(argv[2], NULL, 10);
    params.width        = strtoul(argv[3], NULL, 10);
    trace_file          = argv[4];
    // printf("rob_size:%lu "
    //         "iq_size:%lu "
    //         "width:%lu "
    //         "tracefile:%s\n", params.rob_size, params.iq_size, params.width, trace_file);
    // Open trace_file in read mode
    FP = fopen(trace_file, "r");


    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }

    superScalar superScalar;

    superScalar.superScalarInitialise(params.rob_size, params.iq_size, params.width, FP);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // The following loop just tests reading the trace and echoing it back to the screen.
    //
    // Replace this loop with the "do { } while (Advance_Cycle());" loop indicated in the Project 3 spec.
    // Note: fscanf() calls -- to obtain a fetch bundle worth of instructions from the trace -- should be
    // inside the Fetch() function.
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    // while(fscanf(FP, "%lx %d %d %d %d", &pc, &op_type, &dest, &src1, &src2) != EOF) {
    //     printf("%lx %d %d %d %d\n", pc, op_type, dest, src1, src2); //Print to check if inputs have been read correctly

    do {
        superScalar.Retire();
        superScalar.Writeback();
        superScalar.Execute();
        superScalar.Issue();
        superScalar.Dispatch();
        superScalar.RegisterRead();
        superScalar.Rename();
        superScalar.Decode();
        superScalar.Fetch();
    }
    while(superScalar.Advance_Cycle());



    printf("# === Simulator Command =========\n");
    printf("# ./sim %s %s %s %s\n",argv[1],argv[2],argv[3],argv[4]);
    printf("# === Processor Configuration ===\n");
    printf("# ROB_SIZE = %s\n",argv[1]);
    printf("# IQ_SIZE  = %s\n",argv[2]);
    printf("# WIDTH    = %s\n",argv[3]);
    printf("# === Simulation Results ========\n");

    // Total number of retired instructions or the number of instructions fetched form the trace file or
    // total number of instructions executed by the 9 stagees of pipeline.
    // cout<<"# Dynamic Instruction Count    = "<<superScalar.currentInstructionCount<<endl;
    //
    // // Total number of cycles to retire all the instructions fetched
    // cout<<"# Cycles                       = "<<superScalar.cycleCount<<endl;

    //printf("# Instructions Per Cycle (IPC) = %.2f\n",((float)superScalar.currentInstructionCount)/((float)superScalar.cycleCount));
   // printf("# === Simulation Results ========\n");
    printf("# Dynamic Instruction Count    = %d\n",superScalar.currentInstructionCount);
    printf("# Cycles                       = %d\n",superScalar.cycleCount);
    printf("# Instructions Per Cycle (IPC) = %.2f\n",((float)superScalar.currentInstructionCount)/((float)superScalar.cycleCount));
    return 0;
}
