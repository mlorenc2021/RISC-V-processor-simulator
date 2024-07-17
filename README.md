# RISC-V Instruction Simulator

## Overview

This project is a RISC-V instruction simulator designed to execute RISC-V instructions on a single hardware thread (hart). It supports various operations such as logical, arithmetic, and control flow instructions. The simulator also includes features for disassembling memory contents, displaying instructions during execution, and showing register statuses.

## Features

- Simulates the execution of RISC-V instructions
- Provides disassembly of memory contents
- Displays instructions and register statuses during execution
- Configurable through command-line arguments

## Getting Started

### Prerequisites

- C++ compiler (e.g., g++)

### Compilation

To compile the program, use the following command:

```sh
g++ -o rv32i_simulator main.cpp cpu_single_hart.cpp rv32i_hart.cpp rv32i_decode.cpp registerfile.cpp memory.cpp
```

## Usage
Run the simulator with the following command:
```sh
./rv32i_simulator [options] <infile>
```

## Command-Line Options
• -d : Show disassembly before program execution
• -i : Show instruction printing during execution
• -r : Show register status before each instruction
• -z : Dump memory and register status after execution
• -l <exec-limit> : Set the maximum number of instructions to execute
• -m <hex-mem-size> : Set the memory size in hexadecimal

## Example
To run the simulator with a memory size of 0x1000 and disassemble the input file before execution, use:
```sh
./rv32i_simulator -d -m 0x1000 infile.bin
```
