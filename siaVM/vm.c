#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define REGS 16
#define MAXMEM 10000
/**
 * Author: Josh Horowitz
 * This file is the virtual machine for SIA, it reads the bytecode given and executes them within the console
 * Please note this is for educational purposes only while learning about Computer Architecture in college
 */
unsigned char* mainMem[MAXMEM]; // Main memory for store, load, and iterateover
unsigned char* r[REGS]; // Registers
unsigned char* op1, op2, result; // Result operation registers
unsigned char opcode; //Numerical identifier for the instruction
int PC; // Program Counter
int iterfinalReg; // Register to be stored after offset is added

/**
 * This function stores the information from the given bytecode file into an array to be read by the
 * program counter
 * Parameters: *src - The contents of the given bytecode file
 * 				bytes - The array to hold the instructions to be read
 * Returns programLimit - The number of instructions contained in the file
 */
int memLoad(FILE *src, unsigned char* bytes) {
    int programLimit = 0;
	int i = 0;
     char c = fgetc(src);
        for (i = 0; c != EOF; i++) {
            bytes[i] = c;
            c = fgetc(src);
            programLimit++;
        }
    return programLimit;
}

/**
 * This function uses the program counter to determine the Opcode to determine which instruction is currently being read
 * Then, assigns either 2 or 4 bytes (depending on the instruction) into an array which will be used to
 * execute the instruction shortly
 * Parameters:  bytes - The array of instructions in byte form
 * 				currInstruct - The array to hold the current instruction in bytes
 * 				PC - program counter
 * Returns: PC - program counter
 */
int fetch(unsigned char* bytes, unsigned char* currInstruct, int PC) { // Creates  the current memory buffer
    int i;
    int max;
    int hexBase = 16;
    opcode = bytes[PC] / hexBase;
        if (opcode == 0x0A || opcode == 0x0B || opcode == 0x0C || opcode == 0x0D) { // If the instruction is 4 bytes
            max = 4;
        } else {
            max = 2;
        }
    for (i = 0; i < max; i++) {
        currInstruct[i] = bytes[PC];
        PC++;
    }
    if (max == 4) { // Resets program counter as all 4 byte instructions can change the counter
        PC-=4;
    }
    return PC;
}

/**
 * Using the opcode determined from fetch, this function sets up the result operation registers for execution
 * Parameters:	currInstruct - The array containing the current instruction in bytes
 * Returns: 0 or addMemOrFinReg - This variable contains either the offset from jump, load, store, iterateover, etc.
 * OR it contains the third register, as seen in, add, subtract, etc.
 * OR it contains a numerical value, as seen in addimmediate.
 */
int dispatch(unsigned char* currInstruct) {
    int addMemOrFinReg;
    if (opcode == 0) { // halt command, no more checking or assigning needed
        return 0;
    }
    if (opcode != 8 && opcode != 0x0C) { // Second argument is register
        op1 = r[currInstruct[0] % 0x10];
        if (opcode != 9 && opcode != 0x0D && opcode != 7) { // Third argument is register
            op2 = r[currInstruct[1] / 0x10];
            if (opcode != 0x0A && opcode != 0x0B && opcode != 0x0E && opcode != 0x0F) { // Fourth argument is register
                addMemOrFinReg = currInstruct[1] % 0x10;
                return addMemOrFinReg;
            } else if (opcode == 0x0A || opcode == 0x0B) {
                addMemOrFinReg = ((currInstruct[1] % 0x10) << 16) | (currInstruct[2] << 8) | currInstruct[3];
                return addMemOrFinReg;
            } else if (opcode == 0x0F) {
                addMemOrFinReg = currInstruct[1] % 0x10;
                return addMemOrFinReg;
            } else if (opcode == 0x0E) {
                op1 = currInstruct[0] % 0x10;
                addMemOrFinReg = currInstruct[1] % 0x10;
                return addMemOrFinReg;
            }
        } else if (opcode == 9) {
            op2 = currInstruct[1];
            addMemOrFinReg = currInstruct[0] % 0x10;
            return addMemOrFinReg;
        } else if (opcode == 0x0D) {
            op2 = currInstruct[1];
            iterfinalReg = currInstruct[0] % 0x10;
            addMemOrFinReg = (currInstruct[2] << 8) | currInstruct[3];
            return addMemOrFinReg;
        } else if (opcode == 7) {
            op2 = currInstruct[1];
            addMemOrFinReg = currInstruct[0] % 0x10;
            return addMemOrFinReg;
        }
    } else if (opcode == 8){
        op1 = ((currInstruct[0] % 0x10) << 8) | currInstruct[1];
    } else if (opcode == 0x0C) {
        op1 = ((currInstruct[0] % 0x10) << 24) | (currInstruct[1] << 16) | (currInstruct[2] << 8) | currInstruct[3];
    }
    return 0;
}

/**
 * Using the result operation registers and addMemOrFinReg, the instruction is executed
 * Parameters: addMemOrFinReg - See description in dispatch above
 * Returns: -2 or -1 which are flags to be used in store
 * OR		addMemOrFinReg - which is now a flag to be used in store
 */
int execute(int addMemOrFinReg) {
    switch (opcode) {
        case 0:
            return -2;
            break;
        case 1:
            result = (int) op1 + op2;
            return addMemOrFinReg;
            break;
        case 2:
            result = (int) op1 & op2;
            return addMemOrFinReg;
            break;
        case 3:
            result = (int) op1 / op2;
            return addMemOrFinReg;
            break;
        case 4:
            result = (int) op1 * op2;
            return addMemOrFinReg;
            break;
        case 5:
            result = (int) op1 - op2;
            return addMemOrFinReg;
            break;
        case 6:
            result = (int) op1 | op2;
            return addMemOrFinReg;
            break;
        case 7:
            if ((op2 & 0x20) == 0x20) { // Right shift condition
                result = (int) op1 >> op2;
            } else { // Left shift
                result = (int) op1 << op2;
            }
            return addMemOrFinReg;
            break;
        case 8:
            if (op1 == 0) { // Readies all registers  to be printed
                int j = 0;
                while (j < REGS) {
                    if (r[j] == NULL) {
                        r[j] = 0;
                    }
                    j++;
                }
                printf("R0 = %d\nR1 = %d\nR2 = %d\nR3 = %d\nR4 = %d\nR5 = %d\nR6 = %d\nR7 = %d\nR8 = %d\nR9 = %d\nR10 = %d\nR11 = %d\nR12 = %d \nR13 = %d\nR14 = %d\nR15 = %d\n", r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7], r[8], r[9], r[10], r[11], r[12], r[13], r[14], r[15]);
            }
            if (op1 == 1) { // Prints all memory that was assigned
                printf("Current Memory:\n");
                int i = 0;
                while (i < MAXMEM) {
                    if (mainMem[i] != NULL) {
                        printf("Address %d: %d\n", i, mainMem[i]);
                    }
                    i++;
                }
            }
            break;
        case 9:
            result = (int) op1 + op2;
            return addMemOrFinReg; // Returns the original register
            break;
        case 0x0A:
            if ((int)op1 == op2) { // If equal, branches to the offset
                PC = PC + addMemOrFinReg;
            } else { // Otherwise keep going in program
                PC += 4;
            }
            break;
        case 0x0B:
            if ((int)op1 < op2) { // If less, branches to the offset
                PC = PC + addMemOrFinReg;
            } else { // Otherwise keep going in program
                PC += 4;
            }
            break;
        case 0x0C:
            PC = op1; // Sets program counter to the jump offset
            break;
        case 0x0D:
            result = (int)op1 + op2; // Makes the new address after iteration
            if (mainMem[result] == 0) { // If null, keep going in program
                PC += 4;
            } else { // Otherwise, keep iterating
                PC = PC - addMemOrFinReg;
            }
            return result;
            break;
        case 0x0E:
            result = mainMem[op2]; //Set the loaded register value from memory
        	return addMemOrFinReg;
            break;
        case 0x0F:
            result = (int) op1; // Set the stored register value to be put into memory
            return addMemOrFinReg;
            break;
    }
    return -1;
}

/**
 * This function does the last step of instruction execution, either storing values into main memory
 * or storing values from result operation register into one of the 16 registers
 * Parameters: addMemorFinReg
 */
void store(int addMemOrFinReg) {
    if (opcode == 0x0F) {
        mainMem[op2] = result; //Stores the memory from the register
        int i = 0;
        int j = (int) ++op2;
        while (i < addMemOrFinReg) { // If there's any added bytes, inputs them as '256' here
            mainMem[j] = 256; // Number to indicate bytes allocated, this can't be reached in other instructions
            i++;
            j = (int) ++op2;
        }
    } else if (opcode == 0x0E) { // Sets the loaded register
        r[(int)op1] = result;
    } else if (opcode == 0x0D) { // Sets the iterated register
        r[iterfinalReg] = result;
    } else { // Otherwise set the final register
    r[addMemOrFinReg] = result;
    }
}

int main(int argc, char **argv) {
	FILE *src = fopen(argv[1],"rb");
	if (src == NULL) {
        printf("File not found");
        exit(0);
	}
	unsigned char bytes[10000];
	int programLimit = memLoad(src, bytes); // Loads memory
    unsigned char currInstruct[4]; // Current Instruction Buffer
    PC = 0;
    int addMemOrFinReg; // This takes in either the result register number or the memory offset of an instruction
    int flag = 0;
        while (flag != -2) { // Run loop
        	if (PC >= programLimit) {
        		break;
        	}
            PC = fetch(bytes, currInstruct, PC); // Sets program counter
            addMemOrFinReg = dispatch(currInstruct);
            addMemOrFinReg = execute(addMemOrFinReg);
            if (addMemOrFinReg != -1 && addMemOrFinReg != -2) { // Only stores if necessary
                store(addMemOrFinReg);
            } else if (addMemOrFinReg == -2) { // Halt condition
                flag = addMemOrFinReg;
            }
            for(int i = 0; i < 4; i++) {
            	currInstruct[i] = NULL;
            }
        }
    printf("End program\n");
	fclose(src);
	return 0;
}



