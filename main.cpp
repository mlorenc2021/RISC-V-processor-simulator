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

#include <iostream>
#include <cstdint> //uint32_t
#include <unistd.h> //getopt
#include <sstream>	//istringstream iss


#include "hex.h"
#include "memory.h"
#include "rv32i_decode.h"
#include "rv32i_hart.h"
#include "cpu_single_hart.h"

using std::cerr;
using std::cout;
using std::endl;



/**
 * This function decodes and prints each instruction in a loop.
 *
 * @param mem constant memory object of mem vector 
 ********************************************************************************/
static void disassemble(const memory &mem)
{
	for (uint32_t addr = 0; addr < mem.get_size(); addr++)
	{
		//just print the pc and fetched instruction-word values
		if (addr % 4 == 0)
		{
			//render instruction by decoding it
			std::string instruction = rv32i_decode::decode(addr, mem.get32(addr));

			//print the memory address, instruction hex value, and the instruction mnemonic
			cout << hex::to_hex32(addr) << ": " << hex::to_hex32(mem.get32(addr)) << "  " << instruction << endl;
		}

	}
}

/**
 * This function displays a help message.
 *
 * This message consists of the correct command line usage 
 * for this program and includes a list of the correct command-line
 * arguments or options acceptable to said program. 
 ********************************************************************************/
static void usage()
{
	cerr << "Usage: rv32i [-d] [-i] [-r] [-z] [-l exec-limit] [-m hex-mem-size] infile" << endl;
	cerr << "    -d show disassembly before program execution" << endl;
	cerr << "    -i show instruction printing during execution" << endl;
	cerr << "    -l maximum number of instructions to exec" << endl;
	cerr << "    -m specify memory size (default = 0x100)" << endl;
	cerr << "    -r show register printing during execution" << endl;
	cerr << "    -z show a dump of the regs & memory after simulation" << endl;

	exit(1);
}

/**
 * This is the main function that take command line arguments and executes
 * various functions depending on the arguments provided.
 *
 * The main function takes command line arguments that dictate which files
 * to open and how large the memory size will be. It then executes several
 * functions that process the file data and then outputs the contents.
 *
 * @param argc integer that accepts an optional memory-size argument.
 * @param argv pointer to a character pointer accepts a filename argument.
 *
 * @return 0.
 ********************************************************************************/
int main(int argc, char **argv)
{
	uint32_t memory_limit = 0x100; // default memory size = 256 bytes
	int instruction_limit = 0;//max limit of instructions to execute

	//set of flags to set if argument is used in command line
	bool dashD = false;
	bool dashI = false;
	bool dashR = false;
	bool dashZ = false;

	int opt;

	while ((opt = getopt(argc, argv, "dirzl:m:")) != -1)
	{
		switch (opt)
		{
						
			case 'd':
				{
					dashD = true;
					break;
				}
										
			case 'i':
				{
					dashI = true;
					break;
				}

			case 'r':
				{
					dashR = true;
					break;
				}
										
			case 'z':
				{
					dashZ = true;
					break;
				}

			case 'l':
				{
					std::istringstream iss(optarg);
					iss >> instruction_limit;
					break;
				}

			case 'm':
				{
					std::istringstream iss(optarg);
					iss >> std::hex >> memory_limit;
					break;
				}

			default: /* ’?’ */
				usage();
		}
	}

	//construct the mem and hart before disassembly
	memory mem(memory_limit);
	rv32i_hart hart(mem);

	// fails to load file or missing filename
	if (!mem.load_file(argv[optind]) || optind >= argc)
	{
		usage(); 
	}


	//Show a disassembly of the entire memory before program simulation begins.
	//By default, do not disassemble the program memory.
	if (dashD)
	{
		disassemble(mem);

		//construct and reset the CPU
		cpu_single_hart cpu(mem);
		cpu.reset();
	}

	cpu_single_hart cpu(mem);

	//Show instruction printing during execution.
	//By default, do not print instructions during execution.
	if (dashI)
	{
		cpu.set_show_instructions(true);
	}
	
	//Show a dump of the hart (GP-registers and pc) status before each instruction is simulated.
	if (dashR)
	{
		cpu.set_show_registers(true);
	}

	cpu.run(instruction_limit);

	//Show a dump of the hart status and memory after the simulation has halted.
	if(dashZ)
	{
		cpu.dump();
		mem.dump();
	}

	return 0;
}