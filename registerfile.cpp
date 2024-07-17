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

#include "registerfile.h"

/**
 * @brief Construct a new registerfile object
 * 
 */
registerfile::registerfile()
{
    reset();
}

/**
 * @brief Method to reset the registers.
 * 
 */
void registerfile::reset()
{
    //set regs vector to 32 for number of GP registers
    regs.resize(32);

    //initialize register x0 to zero
    set(regs.at(0), 0);

    //initialize all other registers to 0xf0f0f0
    for (size_t regr = 1; regr < regs.size(); regr++)
    {
        set(regr, 0xf0f0f0f0);
    }
}

/**
 * @brief Method to set the register to a value.
 * 
 * @param r register number.
 * @param val value to set the register to.
 */
void registerfile::set(uint32_t r, int32_t val)
{
    //set register to value if not zero else do nothing
    if (r != 0)
    {
        regs.at(r) = val;
    }
}

/**
 * @brief Method to return the value in register r.
 * 
 * @param r register number.
 * @return int32_t value in register.
 */
int32_t registerfile::get(uint32_t r) const
{
    //Return the value of register r. If r is zero then return zero.
    return ((r != 0) ? regs.at(r) : 0);
}

/**
 * @brief Method to dump the registers.
 * 
 * @param hdr string that must be printed at beginning line.
 */
void registerfile::dump(const std::string &hdr) const
{
    for (size_t regr = 0; regr < regs.size(); regr++)
    {
        // //only print out every 8 registers
        switch(regr % 8)
        {
            case 0:
            {
                //format string for row header
                std::string x = "x" + std::to_string(regr);

                //print out header with proper formatting
                std::cout << hdr;
                std::cout << std::setw(3) << std::setfill(' ') << std::right << x;

                //print register value in current row
                std::cout << " " << to_hex32(regs.at(regr));

                break;
            }
            case 4:
                //print 4th register value in current row with extra space
                std::cout << "  " << to_hex32(regs.at(regr));

                break;
            case 7:
                //print last register value in current row
                std::cout << " " << to_hex32(regs.at(regr)) << std::endl;

                break;
            default:
                //print register value in current row
                std::cout << " " << to_hex32(regs.at(regr));

                break;
        }
    }
}