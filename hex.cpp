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
#include <iomanip>//setfill and setw
#include <sstream>//ostringstream os
#include "hex.h"

/**
 * This function formats an 8 bit integer into a 2 character hex string with
 * a leading zero.
 *
 * @param i unsigned 8 bit integer to be formatted.
 *
 * @return string with 2 hex digits representing the 8 bit integer
 ********************************************************************************/
std::string hex::to_hex8(uint8_t i)
{
    std::ostringstream os;
    os << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(i);
    return os.str();
}

/**
 * This function formats an 32 bit integer into a 8 character hex string with
 * a leading zero.
 *
 * @param i unsigned 32 bit integer to be formatted.
 *
 * @return string with 8 hex digits representing the 32 bit integer
 ********************************************************************************/
std::string hex::to_hex32(uint32_t i)
{
    std::ostringstream os;
    os << std::hex << std::setfill('0') << std::setw(8) << static_cast<uint64_t>(i);
    return os.str();
}

/**
 * This function formats an 32 bit integer into a 8 character hex string with
 * leading 0x.
 *
 * @param i unsigned 32 bit integer to be formatted.
 *
 * @return string with "0x" string appended to string returned from to_hex32()
 * function call
 ********************************************************************************/
std::string hex::to_hex0x32(uint32_t i)
{
    return std::string("0x")+to_hex32(i);
}

/**
* This function returns a string starting with 0x, followed by the 5 hex digits
* representing the 20 least significant bits of the i argument.
*
* @param i unsigned 32 bit integer to be formatted.
*
* @return string with "0x" string appended 5 hex digits
********************************************************************************/ 
std::string hex::to_hex0x20(uint32_t i)
{
    std::ostringstream os;
    os << "0x" << std::hex << std::setfill('0') << std::setw(5) << i;
    return os.str();
}

/**
* This function returns a string starting with 0x, followed by the 3 hex digits
* representing the 12 least significant bits of the i argument.
*
* @param i unsigned 32 bit integer to be formatted.
*
* @return string with "0x" string appended 3 hex digits
********************************************************************************/     
std::string hex::to_hex0x12(uint32_t i)
{
    std::ostringstream os;
    os << "0x" << std::hex << std::setfill('0') << std::setw(3) << i;
    return os.str();
}