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


#include "memory.h"
#include "hex.h"
#include <vector>   //mem
#include <ctype.h>  //isprint()
#include <iostream>
#include <fstream>  //load_file

using std::cerr;
using std::cout;
using std::endl;
using std::hex;

/**
 * This constructor allocates s bytes in the mem vector and initializes every
 * byte/element to 0xa5.
 *
 * @param s size of the mem vector to be allocated.
 ********************************************************************************/
memory::memory(uint32_t s)
{
    //allocate bytes for vector and round the length up % 16
    s = (s + 15) & 0xfffffff0;

    //resize the vector with size and junk value
    mem.resize(s, 4);

    //initialize vector with 0xa5
    fill(mem.begin(), mem.end(), 0xa5);
}



/**
 * This destructor clears up space within the mem vector.
 ********************************************************************************/
memory::~memory()
{
    mem.clear();
}



/**
 * This function checks if the the given address is within the range of valid
 * addresses of the simulated memory. A warning message prints out if address is
 * not valid.
 *
 * @param addr unsigned 32 bit integer to be checked for validity.
 *
 * @return true if address is valid or false if address is not valid.
 ********************************************************************************/
bool memory::check_illegal(uint32_t addr) const
{
    if (!(addr < mem.size()))
    {
        cout << "WARNING: Address out of range: " << hex::to_hex0x32(addr) << endl;
        return true;
    }
    return false;
}




/**
 * This function returns the number of bytes within the simulated memory.
 *
 * @return mem vector size as unsigned 32 bit constant integer.
 ********************************************************************************/
uint32_t memory::get_size() const
{
    return mem.size();
}



/**
 * This function checks to see if the address parameter is valid. If the address
 * is valid then the value of the byte at that address is returned. If the
 * address is not valid then 0 is returned.
 *
 * @param addr unsigned 32 bit integer to be checked for validity and used to
 * return byte value at address.
 *
 * @return unsigned 8 bit integer.
 ********************************************************************************/
uint8_t memory::get8(uint32_t addr) const
{
    //if address is in range then return position in vector
    if (!check_illegal(addr))
    {
        return mem.at(addr);
    }
    return 0;
}


/**
 * This function calls get8() to retrieve two 8 bit values and combine them
 * into a 16 bit value in little endian order.
 *
 * @param addr unsigned 32 bit integer to be checked for validity and used to
 * return byte value at address.
 *
 * @return unsigned 16 bit integer representing little endian order of a value.
 *
 * @note I kept the previous code that I had in order to understand the logic
 * behind my return statement.
 ********************************************************************************/
uint16_t memory::get16(uint32_t addr) const
{
    /*                      0  1  2  3  4  5  6  7   8  9 10 11 12 13 14 15
        addr = 0x00000000: 68 65 6c 6c 6f 20 77 6f  72 6c 64 20 31 20 32 20
               0x00000000 = ^  ^  0x00000001 = 0x00000000 + 0x00000001
    */

    //get first and second bytes
    // uint8_t first = get8(addr);

    // uint8_t second = get8(addr + 0x00000001);


    /*  big endian    little endian
        0x6865      = 0x6568
        second << 8 = 0x6500 + 0x0068 = 0x6568
    */

    //combine bytes into little endian order to create 16 byte return value
    // uint16_t littleEndianOrder = get8(addr) + (get8(addr + 0x00000001) << 8);
    // cout << std::hex << "le: " << littleEndian << endl;

    return (get8(addr) + (get8(addr + 0x00000001) << 8));

}


/**
 * This function calls get16() to retrieve two 16 bit values and combine them
 * into a 32 bit value in little endian order.
 *
 * @param addr unsigned 32 bit integer to be checked for validity and used to
 * return byte value at address.
 *
 * @return unsigned 32 bit integer representing little endian order of a value.
 *
 * @note I kept the previous code that I had in order to understand the logic
 * behind my return statement.
 ********************************************************************************/
uint32_t memory::get32(uint32_t addr) const
{
    /*                      0  1  2  3  4  5  6  7   8  9 10 11 12 13 14 15
        addr = 0x00000000: 68 65 6c 6c 6f 20 77 6f  72 6c 64 20 31 20 32 20
               0x00000000 = ^    ^  0x00000002 = 0x00000000 + 0x00000002
    */ 

    //get first and second 16 bytes
    // uint16_t first = get16(addr);
 
    // uint16_t second = get16(addr + 0x00000002);

    /*  big endian    little endian
        0x68656c6c  = 0x6c6c6568
        second << 16 = 0x6c6c0000 + 0x00006568 = 0x6c6c6568
    */

    //combine bytes into little endian order to create 16 byte return value
    // uint32_t littleEndianOrder = get16(addr) + (get16(addr + 0x00000002) << 16);

    return (get16(addr) + (get16(addr + 0x00000002) << 16));

}



/**
 * This function calls get8() to then return the sign-extended value of the byte
 * as a 32-bit signed integer.
 *
 * @param addr unsigned 32 bit integer to be checked for validity and used to
 * return byte value at address.
 *
 * @return signed 32 bit integer sign extended.
 ********************************************************************************/
int32_t memory::get8_sx(uint32_t addr) const
{
    int32_t signExtended = get8(addr);
    return signExtended | (signExtended & 0x00000080 ? 0xffffff00 : 0);
}


/**
 * This function calls get16() to then return the sign-extended value of the byte
 * as a 32-bit signed integer.
 *
 * @param addr unsigned 32 bit integer to be checked for validity and used to
 * return byte value at address.
 *
 * @return signed 32 bit integer sign extended.
 ********************************************************************************/
int32_t memory::get16_sx(uint32_t addr) const
{
    int32_t signExtended = get16(addr);
    return signExtended | (signExtended & 0x00008000 ? 0xffff0000 : 0);
}


/**
 * This function calls get32() which returns the value as a 32-bit signed integer.
 *
 * @param addr unsigned 32 bit integer to be checked for validity and used to
 * return byte value at address.
 *
 * @return signed 32 bit integer sign extended.
 ********************************************************************************/
int32_t memory::get32_sx(uint32_t addr) const
{
    return get32(addr);
}



/**
 * This function checks to see if the address parameter is valid. If the address
 * is valid then the value of the byte at that address is set to val. If the
 * address is not valid then the function is returned to caller.
 *
 * @param addr unsigned 32 bit integer to be checked for validity and used to
 * return byte value at address.
 * @param val unsigned 8 bit integer to be set to the valid address location.
 ********************************************************************************/
void memory::set8(uint32_t addr, uint8_t val)
{
    //verify address and set byte in the memory
    //at that address to value else discard
    if (!check_illegal(addr))
    {
        mem.at(addr) = val;
    }
    return;
}


/**
 * This function stores the given val in little-endian order into the
 * simulated memory starting at the address given in the addr argument.
 *
 * @param addr unsigned 32 bit integer to be checked for validity and used to
 * return byte value at address.
 * @param val unsigned 16 bit integer to be set to the valid address location.
 ********************************************************************************/
void memory::set16(uint32_t addr, uint16_t val)
{
    //call set8() twice to store value in little endian
    //order into memory starting at address
    set8(addr, val);
    set8((addr + 0x00000001), (val >>= 8));
}


/**
 * This function stores the given val in little-endian order into the
 * simulated memory starting at the address given in the addr argument.
 *
 * @param addr unsigned 32 bit integer to be checked for validity and used to
 * return byte value at address.
 * @param val unsigned 32 bit integer to be set to the valid address location.
 ********************************************************************************/
void memory::set32(uint32_t addr, uint32_t val)
{
    //call set16() twice to store value in little endian
    //order into memory starting at address
    set16(addr, val);
    set16((addr + 0x00000002), (val >>= 16));
}



/**
 * This function dumps the entire contents of the simulated memory using proper
 * formatting and outputting an ASCII box that corresponds to each byte in 
 * the simulated memory.
 ********************************************************************************/
void memory::dump() const
{
    //print out contents of memory with proper formatting
    for (uint32_t addr = 0; addr < mem.size(); addr++)
    {
        switch(addr % 16)
        {
            case 0://print address and colon

                //print a newline after the last element in the row
                if (addr != 0)
                {
                    cout << endl;
                }

                //print memory address in 0x format before colon
                cout << hex::to_hex32(addr) << ": ";

                //print out first byte from memory
                cout << hex::to_hex8(mem.at(addr)) << " ";

                break;

            case 8://extra space between byte 7 & 8

                cout << " ";

                //print out byte from memory
                cout << hex::to_hex8(mem.at(addr)) << " ";

                break;

            case 15://ASCII BOX

                //print out byte from memory
                cout << hex::to_hex8(mem.at(addr)) << " ";

                //start of ASCII box
                cout << "*";

                //go through each byte in row and check if printable ASCII character
                for (uint32_t i = addr - 15; i <= addr; i++)
                {
                    //fetch byte
                    uint8_t ch = get8(i);

                    //check if printable ASCII character or not
                    ch = isprint(ch) ? ch : '.';

                    //print character
                    cout << ch;
                }

                //end of ASCII box
                cout << "*";

                break;

            default:
                //print out byte from memory
                cout << hex::to_hex8(mem.at(addr)) << " ";
        }

    }

    //newline for next row
    cout << endl;
}



/**
 * This function checks to see if a file can be opened. If it can be opened then
 * the contents of the file are read into simulated memory. If it cannot be opened
 * then the function returns to caller.
 *
 * @param fname name of file to be checked if it can be opened and read.
 *
 * @return true if the file could be opened or false if could not be opened.
 ********************************************************************************/
bool memory::load_file(const std::string &fname)
{
    //open file in binary mode
    std::ifstream infile(fname, std::ios::in|std::ios::binary);


    //check if file can be opened
    if(infile.is_open())
    {
        //value of current character in file
        uint8_t i;

        //do not skip white space while reading file
        infile >> std::noskipws;

        //read the file contents
        for (uint32_t addr = 0; infile >> i; ++addr)
        {
            //check that the file can fit into memory and return status
            if(check_illegal(addr))
            {
                //output error message
                cerr << "Program too big." << endl;

                //disassociate file from is_open() call by calling close
                infile.close();

                //return failed status
                return false;
            }

            //store value read from file into memory address
            mem.at(addr) = i;
        }

        //disassociate file from is_open() call by calling close
        infile.close();

        //return success status
        return true;
    }


    //if file cannot be opned then stderr name of file and return status
    cerr << "Can't open file '" << fname << "' for reading." << endl;

    //disassociate file from is_open() call by calling close
    infile.close();    

    //return failed status
    return false;

}
