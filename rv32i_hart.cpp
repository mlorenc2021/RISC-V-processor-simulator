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

#include "rv32i_hart.h"
#include "rv32i_decode.h"

using std::cout;
using std::endl;


/**
 * @brief Method that tells the simulator to execute an instruction.
 * 
 * @param hdr string that holds the header that will be printed on the
 * left of any output that is displayed as a result of calling this method.
 */
void rv32i_hart::tick(const std::string& hdr)
{
    //dump the state of the hart if show_registers is true
    if(show_registers)
    {
        dump(hdr);
    }

    //set flags and return from function if pc reg is not multiple of 4
    if ((pc & 3) != 0)
    {
        halt = true;
        halt_reason = "PC alignment error";
        return;
    }

    //increment the instruction counter
    insn_counter++;

    //fetch an instruction from the memory at the address in the pc register
    uint32_t insn = mem.get32(pc);

    //print and execute the instruction
    if(show_instructions)
    {
        //print the hdr, pc register, and fetched instructions
        cout << hdr << to_hex32(pc) << ": " << to_hex32(insn) << "  ";

        //execute and render the instruction and simulation details
        exec(insn, &std::cout);

        cout << endl;
    }
    else
    {
        //execute the instruction without rendering anything
        exec(insn, nullptr);
    }
}

/**
 * @brief Method that dumps the entire state of the hart.
 * 
 * @param hdr string that holds the header that will be printed on the
 * left of any output that is displayed as a result of calling this method.
 */
void rv32i_hart::dump(const std::string& hdr) const
{
    //dump the entire state of the hart prefixed with hdr string
    regs.dump(hdr);

    //dump the pc register
    cout << hdr << " pc " << to_hex32(pc) << endl;

}

/**
 * @brief Method that resets the rv32i object and the register file.
 * 
 */
void rv32i_hart::reset()
{
    //reset a hart
    pc = 0;
    regs.reset();
    insn_counter = 0;
    halt = false;
    halt_reason = "none";
}

/**
 * @brief Method will execute the given RV32I instruction by
 * making use of the get_xxx() methods to extract the needed
 * instruction fields to decode the instruction and invoke the
 * associated exec_xxx() helper function.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos flag to determine instructions need to print.
 */
void rv32i_hart::exec(uint32_t insn, std::ostream* pos)
{
    //get funct3
    uint32_t funct3 = get_funct3(insn);

    //get funct7
    uint32_t funct7 = get_funct7(insn);

    switch(get_opcode(insn))
    {
        default:  exec_illegal_insn(insn, pos); return;

        //U-TYPE INSTRUCTIONS
        case opcode_lui:   exec_lui(insn, pos); return;
        case opcode_auipc:  exec_auipc(insn, pos); return;

        //J-TYPE INSTRUCTIONS
        // case opcode_jal:  exec_jal(pc, insn, pos); return;
        case opcode_jal:  exec_jal(insn, pos); return;


        //I-TYPE INSTRUCTIONS
        case opcode_jalr:  exec_jalr(insn, pos);  return;

        //B-TYPE INSTRUCTIONS
        case opcode_btype:
            switch (funct3)
            {
                default:  exec_illegal_insn(insn, pos);
                case funct3_beq:  exec_beq(insn, pos); return;
                case funct3_bne:  exec_bne(insn, pos); return;
                case funct3_blt:  exec_blt(insn, pos); return;
                case funct3_bge:  exec_bge(insn, pos);  return;
                case funct3_bltu:  exec_bltu(insn, pos); return;
                case funct3_bgeu:  exec_bgeu(insn, pos); return;                    
            }

            assert(0 && "unrecognized funct3"); // impossible


        //I-TYPE INSTRUCTIONS
        case opcode_load_imm:
            switch (funct3)
            {
                default:  exec_illegal_insn(insn, pos); return;
                case funct3_lb:  exec_lb(insn, pos); return;
                case funct3_lh:  exec_lh(insn, pos); return;
                case funct3_lw:  exec_lw(insn, pos); return;
                case funct3_lbu:  exec_lbu(insn, pos); return; 
                case funct3_lhu:  exec_lhu(insn, pos); return;
            }
            assert(0 && "unrecognized funct3"); // impossible   


        //S-TYPE INSTRUCTIONS
        case opcode_stype:
            switch (funct3)
            {
                default:  exec_illegal_insn(insn, pos); return;
                case funct3_sb:  exec_sb(insn, pos); return;
                case funct3_sh:  exec_sh(insn, pos); return;
                case funct3_sw:  exec_sw(insn, pos); return;
            }
            assert(0 && "unrecognized funct3"); // impossible   

        
        //I-TYPE INSTRUCTIONS
        case opcode_alu_imm:
            switch (funct3)
            {
                default:  exec_illegal_insn(insn, pos); return;
                case funct3_add:  exec_addi(insn, pos); return;
                case funct3_sll:  exec_slli(insn, pos); return;
                case funct3_slt:  exec_slti(insn, pos); return;
                case funct3_sltu:  exec_sltiu(insn, pos); return;
                case funct3_xor:  exec_xori(insn, pos); return;
                case funct3_or:  exec_ori(insn, pos); return;
                case funct3_and:  exec_andi(insn, pos); return;

                case funct3_srx:
                    switch(funct7)
                    {
                        default:  exec_illegal_insn(insn, pos); return;
                        case funct7_srl:  exec_srli(insn, pos); return;
                        case funct7_sra:  exec_srai(insn, pos); return;
                    }
                    assert(0 && "unrecognized funct7"); // impossible
            }
            assert(0 && "unrecognized funct3"); // impossible

        
        //R-TYPE INSTRUCTIONS
        case opcode_rtype:
            switch (funct3)
            {
                default:  exec_illegal_insn(insn, pos); return;
                case funct3_add: 
                        switch(funct7)
                        {
                            default:  exec_illegal_insn(insn, pos); return;
                            case funct7_add:  exec_add(insn, pos); return;
                            case funct7_sub:  exec_sub(insn, pos); return;
                        }
                        assert(0 && "unrecognized funct7"); // impossible


                case funct3_sll:  exec_sll(insn, pos); return;
                case funct3_slt:  exec_slt(insn, pos); return;
                case funct3_sltu:  exec_sltu(insn, pos); return;
                case funct3_xor:  exec_xor(insn, pos); return;
                case funct3_or:  exec_or(insn, pos);  return;
                case funct3_and:  exec_and(insn, pos); return;

                case funct3_srx:
                    switch(funct7)
                    {
                        default:  exec_illegal_insn(insn, pos); return;
                        case funct7_sra:  exec_sra(insn, pos); return;
                        case funct7_srl:  exec_srl(insn, pos); return;
                    }
                    assert(0 && "unrecognized funct7"); // impossible
            }
            assert(0 && "unrecognized funct3"); // impossible


        case opcode_system:
            //EBBREAK
            switch(insn)
            {
                case insn_ebreak:  exec_ebreak(insn, pos); return;
            }

            switch(funct3)
            {
                default:  exec_illegal_insn(insn, pos); return;
                case funct3_csrrs:  exec_csrrs(insn, pos); return;
            }
            assert(0 && "unrecognized funct3"); // impossible      
    }
    assert(0 && "unrecognized opcode"); // It should be impossible to ever get here!
}

/**
 * @brief Method to halt execution when encountering an illegal
 * instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos flag that determines when to render error message.
 */
void rv32i_hart::exec_illegal_insn(uint32_t insn, std::ostream* pos)
{
    (void)insn;

    //render proper error message by writing it to the ostream
    if(pos)
    {
        *pos << render_illegal_insn(insn);
    }

    //set flag to true to halt execution
    halt = true;

    //set string to explain why execution halted
    halt_reason = "Illegal instruction";
}

/**
 * @brief Method to execute the lui instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */
void rv32i_hart::exec_lui(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get immediate value for u-type instruction
    int32_t imm_u = get_imm_u(insn);

    //render the simulation summary comment that shows the values of all the registers
    //involved before and after the instruction simulation (rd ← imm u, pc ← pc+4)
    if(pos)
    {
        std::string s = render_lui(insn);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                          rd       =          immu
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(imm_u);
    }

    //set rd register to immediate value of instruction (rd <- imm_u)
    regs.set(rd, imm_u);

    //increment the pc register (pc <- pc + 4)
    pc += 4;
}

/**
 * @brief Method to execute the auipc instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */
void rv32i_hart::exec_auipc(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get immediate value for u-type instruction
    int32_t imm_u = get_imm_u(insn);

    //calculate value for this instruction's pc register
    int32_t sum = pc + imm_u;

    //render the simulation summary comment that shows the values of all the registers
    //involved before and after the instruction simulation (rd ← pc + imm u, pc ← pc+4)
    if(pos)
    {
        std::string s = render_auipc(insn);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                      rd           =              pc          +               imm_u         =         val
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(pc) << " + " << to_hex0x32(imm_u) << " = " << to_hex0x32(sum);
    }

    //store the calculated result for this instruction into rd (rd ← pc + imm u)
    regs.set(rd, sum);

    //increment the pc register (pc <- pc + 4)
    pc += 4;
}

/**
 * @brief Method to execute the jal instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */       
// void rv32i_hart::exec_jal(uint32_t addr, uint32_t insn, std::ostream* pos)
void rv32i_hart::exec_jal(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get immediate value for j-type instruction
    int32_t imm_j = get_imm_j(insn);

    //calculate address of the next instruction
    int32_t next_insn = pc + 0x00000004;

    //calculate the target address
    int32_t target_addr = pc + imm_j;


    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← pc+4, pc ← pc+imm j)
    if(pos)
    {
        std::string s = render_jal(pc, insn);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                  rd              =           next instruction      , pc =                pc          +           imm_j             =             target_addr
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(next_insn) << ",  pc = " << to_hex0x32(pc) << " + " << to_hex0x32(imm_j) << " = " << to_hex0x32(target_addr);
    }

    //set register rd to the address of the next instruction (rd ← pc+4)
    regs.set(rd, next_insn);

    //jump to target address (pc ← pc+imm j)
    pc = target_addr;
}

/**
 * @brief Method to execute the jalr instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */    
void rv32i_hart::exec_jalr(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get immediate value for i-type instruction
    int32_t imm_i = get_imm_i(insn);

    //calculate address of the next instruction
    int32_t next_insn = pc + 0x00000004;

    //get the signed register source operand
    int32_t s_rs1 = regs.get(rs1);

    //calculate the target address and set the LSB to zero
    int32_t target_addr = (imm_i + s_rs1) & 0xfffffffe;

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← pc+4, pc ← (rs1+imm i)&~1)
    if(pos)
    {
        std::string s = render_jalr(insn);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                  rd               =                  pc+4          ,  pc = (              imm_i         +                   rs1    ) & 0xfffffffe =             target address
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(next_insn) << ",  pc = (" << to_hex0x32(imm_i) << " + " << to_hex0x32(s_rs1) << ") & 0xfffffffe = " << to_hex0x32(target_addr);
    }

    //set register rd to the address of the next instruction (rd ← pc+4)
    regs.set(rd, next_insn);

    //jump to target address ((rs1+imm i)&~1)
    pc = target_addr;
}

/**
 * @brief Method to execute the beq instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */
void rv32i_hart::exec_beq(uint32_t insn, std::ostream* pos)
{
    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get second source operand
    uint32_t rs2 = get_rs2(insn);

    //get immediate value for b-type instruction
    int32_t imm_b = get_imm_b(insn);

    //get signed source operands for calculation
    int32_t s_rs1 = regs.get(rs1);
    int32_t s_rs2 = regs.get(rs2);

    //set current instruction address
    uint32_t addr = pc;

    //If rs1 is equal to rs2 then add imm_b to the pc register. Else add 4.
    pc += ((s_rs1 == s_rs2) ? imm_b : 4);

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (pc ← pc + ((rs1==rs2) ? imm b : 4))
    if(pos)
    {
        std::string s = render_btype(addr, insn, "beq");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //           pc +=               (( rs1         ==                  rs2)       ?                  imm_b       : 4) =           pc
        *pos << "// pc += (" << to_hex0x32(s_rs1) << " == " << to_hex0x32(s_rs2) << " ? " << to_hex0x32(imm_b) << " : 4) = " << to_hex0x32(pc);
    }
}

/**
 * @brief Method to execute the bne instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */
void rv32i_hart::exec_bne(uint32_t insn, std::ostream* pos)
{
    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get second source operand
    uint32_t rs2 = get_rs2(insn);

    //get immediate value for b-type instruction
    int32_t imm_b = get_imm_b(insn);

    //get signed source operands for calculation
    int32_t s_rs1 = regs.get(rs1);
    int32_t s_rs2 = regs.get(rs2);

    //set current instruction address
    uint32_t addr = pc;

    //If rs1 is not equal to rs2 then add imm_b to the pc register. Else add 4.
    pc += ((s_rs1 != s_rs2) ? imm_b : 4);

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (pc ← pc + ((rs1!=rs2) ? imm b : 4))
    if(pos)
    {
        std::string s = render_btype(addr, insn, "bne");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //          pc    +=                (( rs1         !=                 rs2)       ?                  imm_b       : 4) =           pc
        *pos << "// pc += (" << to_hex0x32(s_rs1) << " != " << to_hex0x32(s_rs2) << " ? " << to_hex0x32(imm_b) << " : 4) = " << to_hex0x32(pc);
    }
}

/**
 * @brief Method to execute the blt instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */        
void rv32i_hart::exec_blt(uint32_t insn, std::ostream* pos)
{
    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get second source operand
    uint32_t rs2 = get_rs2(insn);

    //get immediate value for b-type instruction
    int32_t imm_b = get_imm_b(insn);

    //get signed source operands for calculation
    int32_t s_rs1 = regs.get(rs1);
    int32_t s_rs2 = regs.get(rs2);

    //set current instruction address
    uint32_t addr = pc;

    //If the signed value in rs1 is less than the signed
    //value in rs2 then add imm_b to the pc register. Else add 4.
    pc += ((s_rs1 < s_rs2) ? imm_b : 4);

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (pc ← pc + ((rs1<rs2) ? imm b : 4))
    if(pos)
    {
        std::string s = render_btype(addr, insn, "blt");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //          pc +=               (( rs1         <                  rs2)       ?                  imm_b       : 4) =           pc
        *pos << "// pc += (" << to_hex0x32(s_rs1) << " < " << to_hex0x32(s_rs2) << " ? " << to_hex0x32(imm_b) << " : 4) = " << to_hex0x32(pc);
    }
}

/**
 * @brief Method to execute the bge instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */                
void rv32i_hart::exec_bge(uint32_t insn, std::ostream* pos)
{
    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get second source operand
    uint32_t rs2 = get_rs2(insn);

    //get immediate value for b-type instruction
    int32_t imm_b = get_imm_b(insn);

    //get signed source operands for calculation
    int32_t s_rs1 = regs.get(rs1);
    int32_t s_rs2 = regs.get(rs2);

    //set current instruction address
    uint32_t addr = pc;

    //If the signed value in rs1 is greater than or equal 
    //to the signed value in rs2 then add imm_b to the pc register. Else add 4.
    pc += ((s_rs1 >= s_rs2) ? imm_b : 4);

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (pc ← pc + ((rs1>=rs2) ? imm b : 4))
    if(pos)
    {
        std::string s = render_btype(addr, insn, "bge");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //          pc +=               (( rs1         >=                  rs2)       ?                  imm_b       : 4) =           pc
        *pos << "// pc += (" << to_hex0x32(s_rs1) << " >= " << to_hex0x32(s_rs2) << " ? " << to_hex0x32(imm_b) << " : 4) = " << to_hex0x32(pc);
    }
}

/**
 * @brief Method to execute the bltu instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */                
void rv32i_hart::exec_bltu(uint32_t insn, std::ostream* pos)
{
    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get second source operand
    uint32_t rs2 = get_rs2(insn);

    //get immediate value for b-type instruction
    int32_t imm_b = get_imm_b(insn);

    //get the unsigned register source operand
    uint32_t u_rs1 = regs.get(rs1);
    uint32_t u_rs2 = regs.get(rs2);

    //set current instruction address
    uint32_t addr = pc;

    //If the unsigned value in rs1 is less than the
    //unsigned value in rs2 then add imm_b to the pc1795 register. Else add 4.
    pc += ((u_rs1 < u_rs2) ? imm_b : 4);

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (pc ← pc + ((rs1<rs2) ? imm b : 4))
    if(pos)
    {
        std::string s = render_btype(addr, insn, "bltu");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //          pc +=               (( rs1         <U                  rs2)       ?                  imm_b       : 4) =           pc
        *pos << "// pc += (" << to_hex0x32(u_rs1) << " <U " << to_hex0x32(u_rs2) << " ? " << to_hex0x32(imm_b) << " : 4) = " << to_hex0x32(pc);
    }
}

/**
 * @brief Method to execute the bgeu instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */   
void rv32i_hart::exec_bgeu(uint32_t insn, std::ostream* pos)
{
    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get second source operand
    uint32_t rs2 = get_rs2(insn);

    //get immediate value for b-type instruction
    int32_t imm_b = get_imm_b(insn);

    //get the unsigned register source operand
    uint32_t u_rs1 = regs.get(rs1);
    uint32_t u_rs2 = regs.get(rs2);

    //set current instruction address
    uint32_t addr = pc;

    //If the unsigned value in rs1 is greater than or equal
    //to the unsigned value in rs2 then add imm_b to the pc register. Else add 4.
    pc += ((u_rs1 >= u_rs2) ? imm_b : 4);

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (pc ← pc + ((rs1>=rs2) ? imm b : 4))
    if(pos)
    {
        std::string s = render_btype(addr, insn, "bgeu");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //           pc +=               (( rs1         >=U                  rs2)       ?                  imm_b       : 4) =           pc
        *pos << "// pc += (" << to_hex0x32(u_rs1) << " >=U " << to_hex0x32(u_rs2) << " ? " << to_hex0x32(imm_b) << " : 4) = " << to_hex0x32(pc);
    }
}

/**
 * @brief Method to execute the lb instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */  
void rv32i_hart::exec_lb(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get immediate value for i-type instruction
    int32_t imm_i = get_imm_i(insn);

    //get signed source operand
    int32_t s_rs1 = regs.get(rs1);

    //calculate the sum of rs1 and the immediate value
    int32_t sum = s_rs1 + imm_i;

    //fetch the byte from memory
    int32_t byte = mem.get8_sx(sum);

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← sx(m8(rs1+imm i)), pc ← pc+4)
    if(pos)
    {
        std::string s = render_itype_load(insn, "lb");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                          rd      = sx(m8(                rs1          +                 imm_i     )) =          byte
        *pos << "// " << render_reg(rd) << " = sx(m8(" << to_hex0x32(s_rs1) << " + " << to_hex0x32(imm_i) << ")) = " << to_hex0x32(byte);
    }

    //set register rd to the value of the sign extended byte fetched from the
    //memory address given by the sum of rs1 and imm_i
    regs.set(rd, byte);

    //increment the pc register (pc <- pc + 4)
    pc += 4;    
}

/**
 * @brief Method to execute the lh instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */         
void rv32i_hart::exec_lh(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get immediate value for i-type instruction
    int32_t imm_i = get_imm_i(insn);

    //get signed source operand
    int32_t s_rs1 = regs.get(rs1);

    //calculate the sum of rs1 and the immediate value
    int32_t sum = s_rs1 + imm_i;

    //fetch the 16 bit value from memory
    int32_t halfword = mem.get16_sx(sum);

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← sx(m16(rs1+imm i)), pc ← pc+4)
    if(pos)
    {
        std::string s = render_itype_load(insn, "lh");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                          rd      = sx(m16(                rs1          +                  imm_i     )) =          halfword
        *pos << "// " << render_reg(rd) << " = sx(m16(" << to_hex0x32(s_rs1) << " + " << to_hex0x32(imm_i) << ")) = " << to_hex0x32(halfword);
    }

    //set register rd to the value of the sign extended 16 bit little endian
    //half-word value fetched from the memory address given by the sum of rs1 and imm_i
    regs.set(rd, halfword);

    //increment the pc register (pc <- pc + 4)
    pc += 4;        
}

/**
 * @brief Method to execute the lw instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */          
void rv32i_hart::exec_lw(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get immediate value for i-type instruction
    int32_t imm_i = get_imm_i(insn);

    //get signed source operand
    int32_t s_rs1 = regs.get(rs1);

    //calculate the sum of rs1 and the immediate value
    int32_t sum = s_rs1 + imm_i;

    //fetch the 32 bit value from memory
    int32_t word = mem.get32_sx(sum);

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← sx(m32(rs1+imm i)), pc ← pc+4)
    if(pos)
    {
        std::string s = render_itype_load(insn, "lw");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                          rd      = sx(m32(                 rs1          +                imm_i     )) =          word
        *pos << "// " << render_reg(rd) << " = sx(m32(" << to_hex0x32(s_rs1) << " + " << to_hex0x32(imm_i) << ")) = " << to_hex0x32(word);
    }

    //set register rd to the value of the sign extended 32 bit little endian
    //word value fetched from the memory address given by the sum of rs1 and imm_i
    regs.set(rd, word);

    //increment the pc register (pc <- pc + 4)
    pc += 4;      
}

/**
 * @brief Method to execute the lbu instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */ 
void rv32i_hart::exec_lbu(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get immediate value for i-type instruction
    int32_t imm_i = get_imm_i(insn);

    //get the unsigned register source operand
    uint32_t u_rs1 = regs.get(rs1);

    //calculate the sum of rs1 and the immediate value
    uint32_t sum = u_rs1 + imm_i;

    //fetch the byte from memory
    uint32_t byte = mem.get8(sum);


    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← zx(m8(rs1+imm i)), pc ← pc+4)
    if(pos)
    {
        std::string s = render_itype_load(insn, "lbu");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                          rd      = zx(m8(                        rs1          +                  imm_i     )) =          byte
        *pos << "// " << render_reg(rd) << " = zx(m8(" << to_hex0x32(u_rs1) << " + " << to_hex0x32(imm_i) << ")) = " << to_hex0x32(byte);
    }

    //set register rd to the value of the zero extended byte fetched from the
    //memory address given by the sum of rs1 and imm_i
    regs.set(rd, byte);

    //increment the pc register (pc <- pc + 4)
    pc += 4;     
}

/**
 * @brief Method to execute the lhu instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */ 
void rv32i_hart::exec_lhu(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get immediate value for i-type instruction
    int32_t imm_i = get_imm_i(insn);

    //get the unsigned register source operand
    uint32_t u_rs1 = regs.get(rs1);

    //calculate the sum of rs1 and the immediate value
    uint32_t sum = u_rs1 + imm_i;

    //fetch the byte from memory
    uint32_t halfword = mem.get16(sum);


    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← zx(m16(rs1+imm i)), pc ← pc+4)
    if(pos)
    {
        std::string s = render_itype_load(insn, "lhu");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                          rd      = zx(m16(                        rs1          +                  imm_i     )) =          halfword
        *pos << "// " << render_reg(rd) << " = zx(m16(" << to_hex0x32(u_rs1) << " + " << to_hex0x32(imm_i) << ")) = " << to_hex0x32(halfword);
    }

    //set register rd to the value of the zero extended halfword fetched from the
    //memory address given by the sum of rs1 and imm_i
    regs.set(rd, halfword);

    //increment the pc register (pc <- pc + 4)
    pc += 4;     
}

/**
 * @brief Method to execute the sb instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */         
void rv32i_hart::exec_sb(uint32_t insn, std::ostream* pos)
{
    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get second source operand
    uint32_t rs2 = get_rs2(insn);

    //get immediate value for s-type instruction
    uint32_t imm_s = get_imm_s(insn);

    //get the unsigned register source operands
    uint32_t u_rs1 = regs.get(rs1);
    uint32_t u_rs2 = regs.get(rs2);

    //calculate the sum of rs1 and the immediate value
    uint32_t sum = u_rs1 + imm_s;

    //extract bits 7->0 from the rs2
    uint32_t byte = u_rs2 & 0x000000ff;

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (m8(rs1+imm s) ← rs2[7:0], pc ← pc+4)
    if(pos)
    {
        std::string s = render_stype(insn, "sb");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //          m8(                  rs1        +                imm_s     )) =          rs2[7:0]
        *pos << "// m8(" << to_hex0x32(u_rs1) << " + " << to_hex0x32(imm_s) << ") = " << to_hex0x32(byte);
    }

    //set the byte of memory at the address given by the sum of rs1 and imm_s
    mem.set8(sum, byte);

    //increment the pc register
    pc += 4;
}

/**
 * @brief Method to execute the sh instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */         
void rv32i_hart::exec_sh(uint32_t insn, std::ostream* pos)
{
    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get second source operand
    uint32_t rs2 = get_rs2(insn);

    //get immediate value for s-type instruction
    uint32_t imm_s = get_imm_s(insn);

    //get the unsigned register source operands
    uint32_t u_rs1 = regs.get(rs1);
    uint32_t u_rs2 = regs.get(rs2);

    //calculate the sum of rs1 and the immediate value
    uint32_t sum = u_rs1 + imm_s;

    //extract bits 15->0 from the rs2
    uint32_t halfword = u_rs2 & 0x0000ffff;

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (m16(rs1+imm s) ← rs2[15:0], pc ← pc+4)
    if(pos)
    {
        std::string s = render_stype(insn, "sh");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //          m16(                  rs1        +                imm_s     )) =          rs2[15:0]
        *pos << "// m16(" << to_hex0x32(u_rs1) << " + " << to_hex0x32(imm_s) << ") = " << to_hex0x32(halfword);
    }

    //set the halfword of memory at the address given by the sum of rs1 and imm_s
    mem.set16(sum, halfword);

    //increment the pc register
    pc += 4;
}

/**
 * @brief Method to execute the sw instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */         
void rv32i_hart::exec_sw(uint32_t insn, std::ostream* pos)
{

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get second source operand
    uint32_t rs2 = get_rs2(insn);

    //get immediate value for s-type instruction
    uint32_t imm_s = get_imm_s(insn);

    //get the unsigned register source operands
    uint32_t u_rs1 = regs.get(rs1);
    uint32_t u_rs2 = regs.get(rs2);

    //calculate the sum of rs1 and the immediate value
    uint32_t sum = u_rs1 + imm_s;

    //extract bits 32->0 from the rs2
    uint32_t word = u_rs2;

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (m32(rs1+imm s) ← rs2[31:0], pc ← pc+4)
    if(pos)
    {
        std::string s = render_stype(insn, "sw");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //          m32(                  rs1        +                imm_s     )) =          rs2[31:0]
        *pos << "// m32(" << to_hex0x32(u_rs1) << " + " << to_hex0x32(imm_s) << ") = " << to_hex0x32(word);
    }

    //set the word of memory at the address given by the sum of rs1 and imm_s
    mem.set32(sum, word);

    //increment the pc register
    pc += 4;
}

/**
 * @brief Method to execute the addi instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */
void rv32i_hart::exec_addi(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get immediate value for i-type instruction
    int32_t imm_i = get_imm_i(insn);

    //get the signed register source operand
    int32_t s_rs1 = regs.get(rs1);

    //calculate the sum of rs1 and the immediate value
    int32_t sum = s_rs1 + imm_i;

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← rs1 + imm i, pc ← pc+4)
    if(pos)
    {
        std::string s = render_itype_alu(insn, "addi", imm_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                         rd        =                rs1          +              imm_i          =            sum
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(s_rs1) << " + " << to_hex0x32(imm_i) << " = " << to_hex0x32(sum);
    }

    //set the rd to rs1 + imm_i
    regs.set(rd, sum);

    //increment the pc register
    pc += 4;    
}

/**
 * @brief Method to execute the slli instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */
void rv32i_hart::exec_slli(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get immediate value for i-type instruction
    int32_t imm_i = get_imm_i(insn);

    //get the signed register source operand
    int32_t s_rs1 = regs.get(rs1);

    //Shift rs1 left by the number of bits specified in shamt_i
    uint32_t shift = s_rs1 << imm_i;

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← rs1 << shamt i, pc ← pc+4)
    if(pos)
    {
        std::string s = render_itype_alu(insn, "slli", imm_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                         rd        =                rs1          <<      imm_i      =            shift
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(s_rs1) << " << " << imm_i << " = " << to_hex0x32(shift);
    }

    //set rd to the result of the shift
    regs.set(rd, shift);

    //increment the pc register
    pc += 4;       
}

/**
 * @brief Method to execute the slti instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */
void rv32i_hart::exec_slti(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get immediate value for i-type instruction
    int32_t imm_i = get_imm_i(insn);

    //get the signed register source operand
    int32_t s_rs1 = regs.get(rs1);

    //If the signed integer value in rs1 is less than the signed integer value in imm_i then set rd to 1. Otherwise, set rd to 0.
    int32_t val = ((s_rs1 < imm_i) ? 1 : 0);

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← (rs1 < imm i) ? 1 : 0, pc ← pc+4)
    if(pos)
    {
        std::string s = render_itype_alu(insn, "slti", imm_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                         rd        =                rs1          <       imm_i          =            val
        *pos << "// " << render_reg(rd) << " = (" << to_hex0x32(s_rs1) << " < " << imm_i << ") ? 1 : 0 = " << to_hex0x32(val);
    }

    //set rd to the result of the shift
    regs.set(rd, val);

    //increment the pc register
    pc += 4;   
}

/**
 * @brief Method to execute sltiu instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column. 
 */
void rv32i_hart::exec_sltiu(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get immediate value for i-type instruction
    uint32_t imm_i = get_imm_i(insn);

    //get the unsigned register source operand
    uint32_t u_rs1 = regs.get(rs1);

    //If the unsigned integer value in rs1 is less than the signed integer value in imm_i then set rd to 1. Otherwise, set rd to 0.
    uint32_t val = ((u_rs1 < imm_i) ? 1 : 0);

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← (rs1 < imm i) ? 1 : 0, pc ← pc+4)
    if(pos)
    {
        std::string s = render_itype_alu(insn, "sltiu", imm_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                         rd        =                rs1          <U              imm_i          =            val
        *pos << "// " << render_reg(rd) << " = (" << to_hex0x32(u_rs1) << " <U " << imm_i << ") ? 1 : 0 = " << to_hex0x32(val);
    }

    //set rd to the designated value
    regs.set(rd, val);

    //increment the pc register
    pc += 4;       
}

/**
 * @brief Method to execute the xori instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */
void rv32i_hart::exec_xori(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get immediate value for i-type instruction
    int32_t imm_i = get_imm_i(insn);

    //get the signed register source operand
    int32_t s_rs1 = regs.get(rs1);

    //Set register rd to the bitwise xor of rs1 and imm_i.
    int32_t val = s_rs1 ^ imm_i;

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← rs1 ^ imm i, pc ← pc+4)
    if(pos)
    {
        std::string s = render_itype_alu(insn, "xori", imm_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                         rd        =                rs1          ^              imm_i          =            val
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(s_rs1) << " ^ " << to_hex0x32(imm_i) << " = " << to_hex0x32(val);
    }

    //set rd to the result of the bitwise operation
    regs.set(rd, val);

    //increment the pc register
    pc += 4;    
}

/**
 * @brief Method to execute the ori instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */
void rv32i_hart::exec_ori(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get immediate value for i-type instruction
    int32_t imm_i = get_imm_i(insn);

    //get the signed register source operand
    int32_t s_rs1 = regs.get(rs1);

    //Set register rd to the bitwise or of rs1 and imm_i.
    uint32_t val = s_rs1 | imm_i;

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← rs1 ^ imm i, pc ← pc+4)
    if(pos)
    {
        std::string s = render_itype_alu(insn, "ori", imm_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                         rd        =                rs1          |              imm_i          =            val
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(s_rs1) << " | " << to_hex0x32(imm_i) << " = " << to_hex0x32(val);
    }

    //set rd to the result of the bitwise operation
    regs.set(rd, val);

    //increment the pc register
    pc += 4;      
}

/**
 * @brief Method to execute the andi instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */
void rv32i_hart::exec_andi(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get immediate value for i-type instruction
    int32_t imm_i = get_imm_i(insn);

    //get the signed register source operand
    int32_t s_rs1 = regs.get(rs1);

    //Set register rd to the bitwise and of rs1 and imm_i.
    uint32_t val = s_rs1 & imm_i;

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← rs1 ^ imm i, pc ← pc+4)
    if(pos)
    {
        std::string s = render_itype_alu(insn, "andi", imm_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                         rd        =                rs1          &              imm_i          =            val
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(s_rs1) << " & " << to_hex0x32(imm_i) << " = " << to_hex0x32(val);
    }

    //set rd to the result of the bitwise operation
    regs.set(rd, val);

    //increment the pc register
    pc += 4;
}

/**
 * @brief Method to execute the srli instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column. 
 */
void rv32i_hart::exec_srli(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get immediate value for i-type instruction
    uint32_t imm_i = get_imm_i(insn);

    //get the signed register source operand
    uint32_t u_rs1 = regs.get(rs1);

    //Shift rs1 right by the number of bits specified in shamt_i
    uint32_t shift = u_rs1 >> (imm_i & 0x0000001f);

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← rs1 >> shamt i, pc ← pc+4)
    if(pos)
    {
        std::string s = render_itype_alu(insn, "srli", imm_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                         rd        =                rs1          >>       imm_i          =            shift
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(u_rs1) << " >> " << imm_i << " = " << to_hex0x32(shift);
    }

    //set rd to the result of the shift
    regs.set(rd, shift);

    //increment the pc register
    pc += 4;   
}

/**
 * @brief Method to execute the srai instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column. 
 */
void rv32i_hart::exec_srai(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get immediate value for i-type instruction
    uint32_t imm_i = get_imm_i(insn);

    //get the signed register source operand
    int32_t s_rs1 = regs.get(rs1);

    //extract shamt_i value from imm_i
    int32_t shamt_i = (imm_i & 0x0000001f);

    //Shift rs1 right by the number of bits specified in shamt_i
    int32_t shift = s_rs1 >> shamt_i;

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← rs1 >> shamt i, pc ← pc+4)
    if(pos)
    {
        std::string s = render_itype_alu(insn, "srai", shamt_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                         rd        =                rs1          >>        shamt_i     =            shift
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(s_rs1) << " >> " << shamt_i << " = " << to_hex0x32(shift);
    }

    //set rd to the result of the shift
    regs.set(rd, shift);

    //increment the pc register
    pc += 4;
}

/**
 * @brief Method to execute the add instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */
void rv32i_hart::exec_add(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source register
    uint32_t rs1 = get_rs1(insn);

    //get second source register
    uint32_t rs2 = get_rs2(insn);

    //calculate sum of rs1 + rs2
    int32_t val = regs.get(rs1) + regs.get(rs2);

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← rs1 + rs2, pc ← pc+4)
    if(pos)
    {
        std::string s = render_rtype(insn, "add");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                         rd        =                rs1                  +                     rs2           =            val
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(regs.get(rs1)) << " + " << to_hex0x32(regs.get(rs2)) << " = " << to_hex0x32(val);
    }

    //Set register rd to rs1 + rs2.
    regs.set(rd, val);

    //increment pc register
    pc += 4;

}

/**
 * @brief Method to execute the sub instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */
void rv32i_hart::exec_sub(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source register
    uint32_t rs1 = get_rs1(insn);

    //get second source register
    uint32_t rs2 = get_rs2(insn);

    //calculate difference of rs1 + rs2
    int32_t val = regs.get(rs1) - regs.get(rs2);

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← rs1 - rs2, pc ← pc+4)
    if(pos)
    {
        std::string s = render_rtype(insn, "sub");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                         rd        =                rs1                  -                     rs2           =            val
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(regs.get(rs1)) << " - " << to_hex0x32(regs.get(rs2)) << " = " << to_hex0x32(val);
    }

    //Set register rd to rs1 - rs2.
    regs.set(rd, val);

    //increment pc register
    pc += 4;

}

/**
 * @brief Method to execute the sll instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */
void rv32i_hart::exec_sll(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source register
    uint32_t rs1 = get_rs1(insn);

    //get second source register
    uint32_t rs2 = get_rs2(insn);

    //extract the 5 LSB of rs2
    int32_t shift = (regs.get(rs2) & 0x0000001f);

    //shift left rs1 by the extracted 5 LSB of rs2
    int32_t val = regs.get(rs1) << shift;

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← rs1 << (rs2%XLEN), pc ← pc+4)
    if(pos)
    {
        std::string s = render_rtype(insn, "sll");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                         rd        =                rs1                  <<      shift     =            val
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(regs.get(rs1)) << " << " << shift << " = " << to_hex0x32(val);
    }

    //Set register rd to rs1 << (rs2%XLEN)
    regs.set(rd, val);

    //increment pc register
    pc += 4;
}

/**
 * @brief Method to execute the slt instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column. 
 */
void rv32i_hart::exec_slt(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source register
    uint32_t rs1 = get_rs1(insn);

    //get second source register
    uint32_t rs2 = get_rs2(insn);

    //If the signed integer value in rs1 is less than the signed integer value in rs2 then set rd to 1. Otherwise, set rd to 0.
    int32_t val = (regs.get(rs1) < regs.get(rs2) ? 1 : 0);

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← (rs1 < rs2) ? 1 : 0, pc ← pc+4)
    if(pos)
    {
        std::string s = render_rtype(insn, "slt");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                         rd        =                (rs1                   <                     rs2          ) ? 1 : 0 =            val
        *pos << "// " << render_reg(rd) << " = (" << to_hex0x32(regs.get(rs1)) << " < " << to_hex0x32(regs.get(rs2)) << ") ? 1 : 0 = " << to_hex0x32(val);
    }

    //set rd to value
    regs.set(rd, val);

    //increment pc register
    pc += 4;
}

/**
 * @brief Method to execute the sltu instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column. 
 */
void rv32i_hart::exec_sltu(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source register
    uint32_t rs1 = get_rs1(insn);

    //get second source register
    uint32_t rs2 = get_rs2(insn);

    //get the unsigned source operands
    uint32_t u_rs1 = regs.get(rs1);
    uint32_t u_rs2 = regs.get(rs2);

    //if the unsigned integer value in rs1 is less than the unsigned integer value in rs2 then set rd to 1. Otherwise, set rd to 0.
    uint32_t val = (u_rs1 < u_rs2 ? 1 : 0);

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← (rs1 < rs2) ? 1 : 0, pc ← pc+4)
    if(pos)
    {
        std::string s = render_rtype(insn, "sltu");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                         rd        =                (rs1          <U                rs2          ) ? 1 : 0 =            val
        *pos << "// " << render_reg(rd) << " = (" << to_hex0x32(u_rs1) << " <U " << to_hex0x32(u_rs2) << ") ? 1 : 0 = " << to_hex0x32(val);
    }

    //set rd to value
    regs.set(rd, val);

    //increment pc register
    pc += 4;
}

/**
 * @brief Method to execute the xor instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column. 
 */
void rv32i_hart::exec_xor(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get second source operand
    uint32_t rs2 = get_rs2(insn);

    //Set register rd to the bitwise XOR of rs1 and rs2.
    int32_t val = regs.get(rs1) ^ regs.get(rs2);

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← rs1 ^ rs2, pc ← pc+4)
    if(pos)
    {
        std::string s = render_rtype(insn, "xor");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                         rd        =                rs1          ^              rs2          =            val
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(regs.get(rs1)) << " ^ " << to_hex0x32(regs.get(rs2)) << " = " << to_hex0x32(val);
    }

    //set rd to the result of the bitwise operation
    regs.set(rd, val);

    //increment the pc register
    pc += 4;    

}

/**
 * @brief Method to execute the or instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column. 
 */
void rv32i_hart::exec_or(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get second source operand
    uint32_t rs2 = get_rs2(insn);

    //Set register rd to the bitwise OR of rs1 and rs2.
    uint32_t val = regs.get(rs1) | regs.get(rs2);

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← rs1 | rs2, pc ← pc+4)
    if(pos)
    {
        std::string s = render_rtype(insn, "or");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                         rd        =                rs1                  |                        rs2          =            val
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(regs.get(rs1)) << " | " << to_hex0x32(regs.get(rs2)) << " = " << to_hex0x32(val);
    }

    //set rd to the result of the bitwise operation
    regs.set(rd, val);

    //increment the pc register
    pc += 4;       
}

/**
 * @brief Method to execute the and instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */
void rv32i_hart::exec_and(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get second source operand
    uint32_t rs2 = get_rs2(insn);

    //Set register rd to the bitwise AND of rs1 and rs2.
    uint32_t val = regs.get(rs1) & regs.get(rs2);

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← rs1 & rs2, pc ← pc+4)
    if(pos)
    {
        std::string s = render_rtype(insn, "and");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                         rd        =                rs1                  &                        rs2          =            val
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(regs.get(rs1)) << " & " << to_hex0x32(regs.get(rs2)) << " = " << to_hex0x32(val);
    }

    //set rd to the result of the bitwise operation
    regs.set(rd, val);

    //increment the pc register
    pc += 4;
}

/**
 * @brief Method to execute the sra instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */
void rv32i_hart::exec_sra(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get second source operand
    uint32_t rs2 = get_rs2(insn);

    //extract the 5 LSB of rs2
    int32_t shift = (regs.get(rs2) & 0x0000001f);

    //shift right arithmetically rs1 by the extracted 5 LSB of rs2
    int32_t val = regs.get(rs1) >> shift;

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← rs1 >> (rs2%XLEN), pc ← pc+4)
    if(pos)
    {
        std::string s = render_rtype(insn, "sra");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                         rd        =                (rs1                   >>     shift      =            val
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(regs.get(rs1)) << " >> " << shift << " = " << to_hex0x32(val);
    }

    //set rd to the result of the bitwise operation
    regs.set(rd, val);

    //increment the pc register
    pc += 4;
}

/**
 * @brief Method to execute the srl instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */
void rv32i_hart::exec_srl(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get second source operand
    uint32_t rs2 = get_rs2(insn);

    //extract the 5 LSB of rs2
    uint32_t shift = (regs.get(rs2) & 0x0000001f);

    //shift right arithmetically rs1 by the extracted 5 LSB of rs2
    uint32_t val = (uint32_t) regs.get(rs1) >> shift;

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation (rd ← rs1 >> (rs2%XLEN), pc ← pc+4)
    if(pos)
    {
        std::string s = render_rtype(insn, "srl");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                         rd        =                (rs1                   >>     shift      =            val
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(regs.get(rs1)) << " >> " << shift << " = " << to_hex0x32(val);
    }

    //set rd to the result of the bitwise operation
    regs.set(rd, val);

    //increment the pc register
    pc += 4;    
}


/**
 * @brief Method to execute the EBREAK instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */
void rv32i_hart::exec_ebreak(uint32_t insn, std::ostream* pos)
{
    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation
    if(pos)
    {
        std::string s = render_ebreak(insn);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        
        *pos << "// HALT";
    }
    halt = true;
    halt_reason = "EBREAK instruction";
}


/**
 * @brief Method to execute the csrrs instruction.
 * 
 * @param insn RV32I instruction to be executed.
 * @param pos ostream used to print comment column.
 */
void rv32i_hart::exec_csrrs(uint32_t insn, std::ostream* pos)
{
    //get register destination
    uint32_t rd = get_rd(insn);

    //get first source operand
    uint32_t rs1 = get_rs1(insn);

    //get immediate value for i-type instruction and extract
    int32_t csr = get_imm_i(insn) & 0x00000fff;

    //halt execution if illegal CSR in csrrs instruction or rs1 is not x0
    if(csr != 0xf14 || rs1 != 0)
    {
        halt = true;
        halt_reason = "Illegal CSR in CSRRS instruction";
        return;
    }

    //render the simulation summary comment that shows the values of all the registers 
    //involved before and after the instruction simulation
    if(pos)
    {
        std::string s = render_csrrx(insn, "csrrs");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;

        //                      rd           =      mhartid
        *pos << "// " << render_reg(rd) << " = " << mhartid;
    }

    //set rd to mhartid
    regs.set(rd, mhartid);

    //increment the pc register
    pc += 4;
}