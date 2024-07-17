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

#include "rv32i_decode.h"
#include <sstream>      // std::ostringstream
#include <iomanip> //setw
#include <cassert>//assert()
#include <string>//string

/**
 * Decodes the passed instruction based on the instructions opcode. A series
 * of switch statements determine how to render the instruction.
 *
 * @param addr unsigned 32 bit integer that contains the memory address from 
 * which the instruction has been fetched. Used to calculate the PC relative
 * target address shown in the J-type and B-type instructions.
 * @param insn signed 32 bit integer that contains the instruction to be decoded.
 *
 * @return string containing the disassembled instruction text
 ********************************************************************************/
std::string rv32i_decode::decode(uint32_t addr, uint32_t insn)
{
    //get funct3
    uint32_t funct3 = get_funct3(insn);

    //get funct7
    uint32_t funct7 = get_funct7(insn);

    switch(get_opcode(insn))
    {
        default: return render_illegal_insn(insn);

        //U-TYPE INSTRUCTIONS
        case opcode_lui:  return render_lui(insn);
        case opcode_auipc: return render_auipc(insn);

        //J-TYPE INSTRUCTIONS
        case opcode_jal: return render_jal(addr, insn);

        //I-TYPE INSTRUCTIONS
        case opcode_jalr: return render_jalr(insn);  

        //B-TYPE INSTRUCTIONS
        case opcode_btype:
            switch (funct3)
            {
                default: return render_illegal_insn(insn);
                case funct3_beq: return render_btype(addr, insn, "beq");
                case funct3_bne: return render_btype(addr, insn, "bne");
                case funct3_blt: return render_btype(addr, insn, "blt");
                case funct3_bge: return render_btype(addr, insn, "bge"); 
                case funct3_bltu: return render_btype(addr, insn, "bltu");
                case funct3_bgeu: return render_btype(addr, insn, "bgeu");                    
            }

            assert(0 && "unrecognized funct3"); // impossible


        //I-TYPE INSTRUCTIONS
        case opcode_load_imm:
            switch (funct3)
            {
                default: return render_illegal_insn(insn);
                case funct3_lb: return render_itype_load(insn, "lb");
                case funct3_lh: return render_itype_load(insn, "lh");
                case funct3_lw: return render_itype_load(insn, "lw");
                case funct3_lbu: return render_itype_load(insn, "lbu"); 
                case funct3_lhu: return render_itype_load(insn, "lhu");
            }
            assert(0 && "unrecognized funct3"); // impossible   


        //S-TYPE INSTRUCTIONS
        case opcode_stype:
            switch (funct3)
            {
                default: return render_illegal_insn(insn);
                case funct3_sb: return render_stype(insn, "sb");
                case funct3_sh: return render_stype(insn, "sh");
                case funct3_sw: return render_stype(insn, "sw");
            }
            assert(0 && "unrecognized funct3"); // impossible   

        
        //I-TYPE INSTRUCTIONS
        case opcode_alu_imm:
            switch (funct3)
            {
                default: return render_illegal_insn(insn);
                case funct3_add: return render_itype_alu(insn, "addi", get_imm_i(insn));
                case funct3_sll: return render_itype_alu(insn, "slli", get_imm_i(insn)%XLEN);
                case funct3_slt: return render_itype_alu(insn, "slti", get_imm_i(insn));
                case funct3_sltu: return render_itype_alu(insn, "sltiu", get_imm_i(insn));
                case funct3_xor: return render_itype_alu(insn, "xori", get_imm_i(insn));
                case funct3_or: return render_itype_alu(insn, "ori", get_imm_i(insn));
                case funct3_and: return render_itype_alu(insn, "andi", get_imm_i(insn));  

                case funct3_srx:
                    switch(funct7)
                    {
                        default: return render_illegal_insn(insn);
                        case funct7_sra: return render_itype_alu(insn, "srai", get_imm_i(insn)%XLEN);
                        case funct7_srl: return render_itype_alu(insn, "srli", get_imm_i(insn)%XLEN);
                    }
                    assert(0 && "unrecognized funct7"); // impossible
            }
            assert(0 && "unrecognized funct3"); // impossible

        
        //R-TYPE INSTRUCTIONS
        case opcode_rtype:
            switch (funct3)
            {
                default: return render_illegal_insn(insn);
                case funct3_add: 
                        switch(funct7)
                        {
                            default: return render_illegal_insn(insn);
                            case funct7_add: return render_rtype(insn, "add");
                            case funct7_sub: return render_rtype(insn, "sub");
                        }
                        assert(0 && "unrecognized funct7"); // impossible


                case funct3_sll: return render_rtype(insn, "sll");
                case funct3_slt: return render_rtype(insn, "slt");
                case funct3_sltu: return render_rtype(insn, "sltu");
                case funct3_xor: return render_rtype(insn, "xor");
                case funct3_or: return render_rtype(insn, "or");
                case funct3_and: return render_rtype(insn, "and");  

                case funct3_srx:
                    switch(funct7)
                    {
                        default: return render_illegal_insn(insn);
                        case funct7_sra: return render_rtype(insn, "sra");
                        case funct7_srl: return render_rtype(insn, "srl");
                    }
                    assert(0 && "unrecognized funct7"); // impossible
            }
            assert(0 && "unrecognized funct3"); // impossible


        case opcode_system:
            //EBBREAK AND ECALL
            switch(insn)
            {
                case insn_ebreak: return render_ebreak(insn);
                case insn_ecall: return render_ecall(insn);
            }

            switch(funct3)
            {
                default: return render_illegal_insn(insn);

                //csrrx functions
                case funct3_csrrw: return render_csrrx(insn, "csrrw");
                case funct3_csrrs: return render_csrrx(insn, "csrrs");
                case funct3_csrrc: return render_csrrx(insn, "csrrc");

                //csrrxi functions
                case funct3_csrrwi: return render_csrrxi(insn, "csrrwi");
                case funct3_csrrsi: return render_csrrxi(insn, "csrrsi");
                case funct3_csrrci: return render_csrrxi(insn, "csrrci");
            }
            assert(0 && "unrecognized funct3"); // impossible      

    }
    assert(0 && "unrecognized opcode"); // It should be impossible to ever get here!

}//end decode



/*
    HELPER FUNCTIONS
*/


/**
 * Extracts and returns the opcode field from the given instruction.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to be decoded.
 *
 * @return unsigned integer opcode field
 ********************************************************************************/
uint32_t rv32i_decode::get_opcode(uint32_t insn)
{
    //extract 6->0 and no shifting to 0th bit required
    return (insn & 0x0000007f);
}

/**
 * Extracts the rd field from the given instruction.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to extract.
 *
 * @return unsigned integer rd field
 ********************************************************************************/
uint32_t rv32i_decode::get_rd(uint32_t insn)
{
    //extract 5 bits at 11->7 and shift to 0th bit
    return ((insn & 0x00000f80) >> (7-0));
}

/**
 * Extracts the funct3 field from the given instruction as an int from 0 to 7
 *
 * @param insn unsigned 32 bit integer that contains the instruction to extract.
 *
 * @return unsigned integer funct3 field
 ********************************************************************************/
uint32_t rv32i_decode::get_funct3(uint32_t insn)
{
    //extract 3 bits at 14->12 and shift to 0th bit 
    return ((insn & 0x00007000) >> (12-0));
}

/**
 * Extracts the rs1 field from the given instruction.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to extract.
 *
 * @return unsigned integer rs1 field
 ********************************************************************************/
uint32_t rv32i_decode::get_rs1(uint32_t insn)
{
    //extract 5 bits at 19->15 and shift to 0th bit
    return ((insn & 0x000f8000) >> (15-0));
}

/**
 * Extracts the rs2 field from the given instruction.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to extract.
 *
 * @return unsigned integer rs2 field
 ********************************************************************************/
uint32_t rv32i_decode::get_rs2(uint32_t insn)
{
    //extract 5 bits at 24->20 and shift to 0th bit
    return ((insn & 0x01f00000) >> (20-0));
}

/**
 * Extracts the funct7 field from the given instruction as an int from 0x00 to
 * 0x7f
 *
 * @param insn unsigned 32 bit integer that contains the instruction to extract.
 *
 * @return unsigned integer funct7 field
 ********************************************************************************/
uint32_t rv32i_decode::get_funct7(uint32_t insn)
{
    //extract 7 bits at 31->25 and shift to 0th bit 
    return ((insn & 0xfe000000) >> (25-0));
}

/**
 * Extracts the immediate value for i-type instructions.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to extract.
 *
 * @return signed integer immediate value for i-type instructions
 ********************************************************************************/
int32_t rv32i_decode::get_imm_i(uint32_t insn)
{
    //extract 12 bits at 31->20 and shift them to bits 11->0
    int32_t imm_i = (insn & 0xfff00000) >> (20-0);//from 20 to 0

    //if negative then sign extend based on remaining bits
    if (insn & 0x80000000)
    {
        //sign extend 31->12
        imm_i |= 0xfffff000;
    }

    return imm_i;
}

/**
 * Extracts the immediate value for u-type instructions.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to extract.
 *
 * @return signed integer immediate value for u-type instructions
 ********************************************************************************/
int32_t rv32i_decode::get_imm_u(uint32_t insn)
{
    //extract 20 bits from 31->12 and no shifting needed
    return (insn & 0xfffff000);
}

/**
 * Extracts the immediate value for b-type instructions.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to extract.
 *
 * @return signed integer immediate value for b-type instructions
 ********************************************************************************/
int32_t rv32i_decode::get_imm_b(uint32_t insn)
{
    //extract 1 bit from 31 and shift to bit 12
    int32_t imm_b = (insn & 0x80000000) >> (31-12);//from 31 to 12

    //extract 1 bit from 7 and shift to bit 11
    imm_b |= (insn & 0x00000080) << (11-7);//from 7 to 11

    //extract 6 bits from 30->25 and shift to bits 10->5
    imm_b |= (insn & 0x7e000000) >> (25-5);//from 25 to 5

    //extract 4 bits from 11->8 and shift to bits 4->1
    imm_b |= (insn & 0x00000f00) >> (8-1);//from 8 to 1

    //if negative then sign extend based on remaining bits
    if (insn & 0x80000000)
    {
        //sign extend 
        imm_b |= 0xfffff000;
    }

    return imm_b;
}

/**
 * Extracts the immediate value for s-type instructions.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to extract.
 *
 * @return signed integer immediate value for s-type instructions
 ********************************************************************************/
int32_t rv32i_decode::get_imm_s(uint32_t insn)
{
    //extract 7 bits at 31->25 and shift them to bits 11->5
    int32_t imm_s = (insn & 0xfe000000) >> (25-5);//from 25 to 5

    //extract 5 bits at 11->7 and shift them to bits 4->0
    imm_s |= (insn & 0x00000f80) >> (7-0);//from 7 to 0

    //if negative then sign extend based on remaining bits
    if (insn & 0x80000000)
    {
        //sign extend 31->12
        imm_s |= 0xfffff000;
    }

    return imm_s;
}

/**
 * Extracts the immediate value for j-type instructions.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to extract.
 *
 * @return signed integer immediate value for j-type instructions
 ********************************************************************************/
int32_t rv32i_decode::get_imm_j(uint32_t insn)
{
    //extract 1 bit at 31 and shift it to bit 20
    int32_t imm_j = (insn & 0x80000000) >> (31-20);//from 31 to 20

    //extract 8 bits at 19->12 and no shifting needed
    imm_j |= (insn & 0x000ff000);//correct position

    //extract 1 bits from 20 and shift it to bit 20
    imm_j |= (insn & 0x00100000) >> (20-11);//from 20 to 11

    //extract 10 bits from 30->21 and shift them to bits 10->1
    imm_j |= (insn & 0x7fe00000) >> (21-1);//from 21 to 1

    //if negative then sign extend based on remaining bits
    if (insn & 0x80000000)
    {
        //sign extend 31->21
        imm_j |= 0xfff00000;
    }

    return imm_j;
}


/*
    INSTRUCTION FUNCTIONS
*/

/**
 * Renders the message for illegal instructions.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to render.
 *
 * @return string message indicating error.
 ********************************************************************************/
std::string rv32i_decode::render_illegal_insn(uint32_t insn)
{
    return "ERROR: UNIMPLEMENTED INSTRUCTION";
}

/**
 * Renders the lui instruction.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to render.
 *
 * @return string message indicating error.
 ********************************************************************************/
std::string rv32i_decode::render_lui(uint32_t insn)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get immediate value for u-type instruction
    int32_t imm_u = get_imm_u(insn);

    //render the instruction using proper formatting
    std::ostringstream os;

    //      lui                         rd             ,                imm
    os << render_mnemonic("lui") << render_reg(rd) << "," << hex::to_hex0x20((imm_u >> 12) & 0x0fffff);


    return os.str();
}

/**
 * Renders the auipc instruction.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to render.
 *
 * @return string with rendered instruction.
 ********************************************************************************/
std::string rv32i_decode::render_auipc(uint32_t insn)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get immediate value for u-type instruction
    int32_t imm_u = get_imm_u(insn);

    //render the instruction with proper formatting
    std::ostringstream os;

    //      auipc                         rd             ,                imm
    os << render_mnemonic("auipc") << render_reg(rd) << "," << hex::to_hex0x20((imm_u >> 12) & 0x0fffff);

    return os.str();
}

/**
 * Renders the jal instruction.
 *
 * @param addr The memory address where the insn is stored.
 * @param insn unsigned 32 bit integer that contains the instruction to render.
 *
 * @return string with rendered instruction.
 ********************************************************************************/
std::string rv32i_decode::render_jal(uint32_t addr, uint32_t insn)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get immediate value for j-type instruction and add it
    //to the memory address where the instruction is stored
    int32_t imm_j = get_imm_j(insn);

    //render the instruction with proper formatting
    std::ostringstream os;

    //              jal                 rd             ,      pcrel_21
    os << render_mnemonic("jal") << render_reg(rd) << "," << hex::to_hex0x32((imm_j + addr) & 0xffffffff);

    return os.str();
}

/**
 * Renders the jalr instruction.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to render.
 *
 * @return string with rendered instruction.
 ********************************************************************************/
std::string rv32i_decode::render_jalr(uint32_t insn)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get immediate value for i-type instruction
    int32_t imm_i = get_imm_i(insn);

    //render the instruction with proper formatting
    std::ostringstream os;

    //                  jalr                    rd      ,           imm(rs1)
    os << render_mnemonic("jalr") << render_reg(rd) << "," << render_base_disp(rs1, imm_i);

    return os.str();
}

/**
 * Renders the b-type instructions.
 *
 * @param addr The memory address where the insn is stored.
 * @param insn unsigned 32 bit integer that contains the instruction to render.
 * @param mnemonic the mnemonic for that b-type instruction
 *
 * @return string with rendered instruction.
 ********************************************************************************/
std::string rv32i_decode::render_btype(uint32_t addr, uint32_t insn, const char *mnemonic)
{
    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get second source operand
    uint32_t rs2 = get_rs2(insn);

    //get immediate value for b-type instruction
    int32_t imm_b = get_imm_b(insn);

    //render the instruction with proper formatting
    std::ostringstream os;
    
    //          b-type mnemonic                rs1         ,                rs2      ,          pcrel_13 
    os << render_mnemonic(mnemonic) << render_reg(rs1) << "," << render_reg(rs2) << "," << hex::to_hex0x32((imm_b + addr) & 0xffffffff);

    return os.str();

}

/**
 * Renders the i-type instructions.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to render.
 * @param mnemonic the mnemonic for that i-type instruction
 *
 * @return string with rendered instruction.
 ********************************************************************************/
std::string rv32i_decode::render_itype_load(uint32_t insn, const char *mnemonic)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get immediate value for i-type instruction
    int32_t imm_i = get_imm_i(insn);

    //render the instruction with proper formatting
    std::ostringstream os;

    //      i-type mnemonic                     rd        ,         imm(rs1)
    os << render_mnemonic(mnemonic) << render_reg(rd) << "," << render_base_disp(rs1, imm_i);

    return os.str();    
}

/**
 * Renders the s-type instructions.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to render.
 * @param mnemonic the mnemonic for that s-type instruction
 *
 * @return string with rendered instruction.
 ********************************************************************************/
std::string rv32i_decode::render_stype(uint32_t insn, const char *mnemonic)
{
    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get second source operand
    uint32_t rs2 = get_rs2(insn);

    //get immediate value for s-type instruction
    int32_t imm_s = get_imm_s(insn);

    //render the instruction with proper formatting
    std::ostringstream os;

    //      s-type mnemonic                 rs2            ,                imm(rs1)
    os << render_mnemonic(mnemonic) << render_reg(rs2) << "," << render_base_disp(rs1, imm_s);

    return os.str();          
}

/**
 * Renders the i-type instructions.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to render.
 * @param mnemonic the mnemonic for that i-type instruction.
 * @param imm_i immediate value for the i-type instructions.
 *
 * @return string with rendered instruction.
 ********************************************************************************/
std::string rv32i_decode::render_itype_alu(uint32_t insn, const char *mnemonic, int32_t imm_i)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //render the instruction with proper formatting
    std::ostringstream os;

    //          i-type mnemonic                 rd        ,         rs1             ,      imm
    os << render_mnemonic(mnemonic) << render_reg(rd) << "," << render_reg(rs1) << "," << imm_i;

    return os.str();         
}

/**
 * Renders the r-type instructions.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to render.
 * @param mnemonic the mnemonic for that r-type instruction.
 *
 * @return string with rendered instruction.
 ********************************************************************************/
std::string rv32i_decode::render_rtype(uint32_t insn, const char *mnemonic)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get second source operand
    uint32_t rs2 = get_rs2(insn);

    //render the instruction with proper formatting
    std::ostringstream os;

    //          r-type mnemonic                 rd        ,         rs1             ,           rs2
    os << render_mnemonic(mnemonic) << render_reg(rd) << "," << render_reg(rs1) << "," << render_reg(rs2);

    return os.str();           
}

/**
 * Renders the ecall instruction.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to render.
 *
 * @return string with rendered instruction.
 ********************************************************************************/
std::string rv32i_decode::render_ecall(uint32_t insn)
{
    //render the instruction with proper formatting
    std::ostringstream os;
    os << "ecall";

    return os.str();       
}

/**
 * Renders the ebreak instruction.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to render.
 *
 * @return string with rendered instruction.
 ********************************************************************************/
std::string rv32i_decode::render_ebreak(uint32_t insn)
{
    //render the instruction with proper formatting
    std::ostringstream os;
    os << "ebreak";

    return os.str();     
}

/**
 * Renders the csrrx-type instructions.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to render.
 * @param mnemonic the mnemonic for that csrrx-type instruction.
 *
 * @return string with rendered instruction.
 ********************************************************************************/
std::string rv32i_decode::render_csrrx(uint32_t insn, const char *mnemonic)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get immediate value for i-type instruction
    uint32_t csr = get_imm_i(insn);

    //render the instruction with proper formatting
    std::ostringstream os;

    //          csrrx mnemonic                   rd       ,             csr                       ,         rs1
    os << render_mnemonic(mnemonic) << render_reg(rd) << "," << hex::to_hex0x12(csr & 0x0fff) << "," << render_reg(rs1);

    return os.str();      

}

/**
 * Renders the csrrxi-type instructions.
 *
 * @param insn unsigned 32 bit integer that contains the instruction to render.
 * @param mnemonic the mnemonic for that csrrxi-type instruction.
 *
 * @return string with rendered instruction.
 ********************************************************************************/
std::string rv32i_decode::render_csrrxi(uint32_t insn, const char *mnemonic)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get zimm register
    uint32_t zimm = get_rs1(insn);

    //get immediate value for i-type instruction
    uint32_t csr = get_imm_i(insn);

    //render the instruction with proper formatting
    std::ostringstream os;

    //          csrrxi mnemonic                   rd      ,             csr                       ,     zimm
    os << render_mnemonic(mnemonic) << render_reg(rd) << "," << hex::to_hex0x12(csr & 0x0fff) << "," << zimm;

    return os.str();          
}


/*
    INSTRUCTION HELPER FUNCTIONS
*/

/**
 * Render the name of the given register.
 *
 * @param r integer that holds the name of the register to be rendered.
 *
 * @return string with the name of the register.
 ********************************************************************************/
std::string rv32i_decode::render_reg(int r)
{
    //render register with proper format
    std::ostringstream os;
    os << "x" << r;
    return os.str();
}

/**
 * Render the operands in the format "disp(base)" for the s-type and i-type
 * instructions.
 *
 * @param base unsigned integer that holds the register value (rsx).
 * @param disp signed integer that holds the immediate value (imm_x).
 *
 * @return string with the format of disp(base).
 ********************************************************************************/
std::string rv32i_decode::render_base_disp(uint32_t base, int32_t disp)
{
    //render base displacement with proper format
    std::ostringstream os;
    os << disp << "(" << render_reg(base) << ")";
    return os.str();
}

/**
 * Render the instruction mnemonic with the proper format
 *
 * @param m const string that holds the given instruction mnemonic
 *
 * @return string with the mnemonic formatted properly.
 ********************************************************************************/
std::string rv32i_decode::render_mnemonic(const std::string &m)
{
    //render mnemonic with proper format
    std::ostringstream os;
    os << std::setw(mnemonic_width) << std::left << m;
    return os.str();
}