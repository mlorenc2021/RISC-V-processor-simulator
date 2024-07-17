//***************************************************************************
//
//  Matthew Lorenc
//  z1904531
//  CSCI 463 Section 1
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//***************************************************************************

#include "cpu_single_hart.h"


/**
 * @brief Method to run the simulator until is is halted or 
 * instruction execution limit is reached.
 * 
 * @param exec_limit maximum number of instructions that can be
 * executed.
 */
void cpu_single_hart::run(uint64_t exec_limit)
{
    //set register x2 to mem size
    regs.set(2, mem.get_size());

    //if exec limit is zero
    if(exec_limit == 0)
    {
        //call tick() until is_halted() returns true
        while(is_halted() != true)
        {
            tick();
        }
    }

    if(exec_limit != 0)//if exec limit is not zero
    {
        //call tick() until is_halted() is true or until exec limit is reached
        while(is_halted() != true && get_insn_counter() != exec_limit)
        {
            tick();
        }
    }

    //if the hart becomes halted then print message indicating why
    if (is_halted())
    {
        std::cout << "Execution terminated. Reason: " << get_halt_reason() << std::endl;
    }

    //print number of instructions that have been executed
    std::cout << get_insn_counter() << " instructions executed" << std::endl;
}