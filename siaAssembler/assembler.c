#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
/**
 * Author: Josh Horowitz
 * The assembler is designed to parse a text file of SIA (Simple Instruction Architecture) commands into
 * their bytecode equivalents as instructed byt the SIA documentation. Please note this is for educational
 * purposes only while I was learning Computer Architecture in college
 */

/**
 * This is to remove any extra whitespace or tab characters in the line currently being parsed
 * Parameters: *s - Contents of a character pointer, which would be the line being read
 * Return: The character pointer ready to be parsed
 */
char *ltrim(char *s) {
	while (*s == ' ' || *s == '\t') s++;
	return s;
}

/**
 * This function helps to parse the register(s) in the line, so that the assembler knows which register
 * needs to be used for the command in the virtual machine
 * Parameters: *text - Contents of a character pointer, for example: "r1", "r2", etc.
 * Returns The integer value of which register was given, for example: "1", "2", etc.
 */
char getRegister(char *text) {
	if (*text == 'r' || *text=='R') text++;
	return atoi(text);
}

/**
 * This function parses the line currently being read into bytecode, to be written to a bytecode file
 * for the virtual machine
 * Parameters: *text - The line currently being read
 * 				bytes - An array of type character pointer to hold the bytecode version of the line
 * Returns - the number of bytes needed for the command, either 2 or 4 based on the instruction
 */
int assembleLine(char *text, unsigned char* bytes) {
	text = ltrim(text);
	char *keyWord = strtok(text," ");
	int num;
	if (strcmp("add",keyWord) == 0) {
		bytes[0] = 0x10;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 | getRegister(strtok(NULL," "));
		return 2;
	}
	if (strcmp("addimmediate",keyWord) == 0) {
        bytes[0] = 0x90;
        bytes[0] |= getRegister(strtok(NULL, " "));
        bytes[1] = atoi(strtok(NULL, " "));
        return 2;
	}
	if (strcmp("interrupt",keyWord) == 0) {
        bytes[0] = 0x80;
        num = atoi(strtok(NULL, " "));
        bytes[0] |= (num & 0xFF0) >> 8;
        bytes[1] = (num & 0xFF);
        return 2;
	}
	if (strcmp("subtract",keyWord) == 0) {
        bytes[0] = 0x50;
        bytes[0] |= getRegister(strtok(NULL, " "));
        bytes[1] = getRegister(strtok(NULL, " ")) << 4 | getRegister(strtok(NULL," "));
        return 2;
	}
	if (strcmp("multiply",keyWord) == 0) {
        bytes[0] = 0x40;
        bytes[0] |= getRegister(strtok(NULL, " "));
        bytes[1] = getRegister(strtok(NULL, " ")) << 4 | getRegister(strtok(NULL, " "));
        return 2;
	}
	if (strcmp("divide",keyWord) == 0) {
        bytes[0] = 0x30;
        bytes[0] |= getRegister(strtok(NULL, " "));
        bytes[1] = getRegister(strtok(NULL, " ")) << 4 | getRegister(strtok(NULL, " "));
        return 2;
	}
	if (strcmp("and",keyWord) == 0) {
        bytes[0] = 0x20;
        bytes[0] |= getRegister(strtok(NULL, " "));
        bytes[1] = getRegister(strtok(NULL, " ")) << 4 | getRegister(strtok(NULL, " "));
        return 2;
	}
	if (strcmp("or", keyWord) == 0) {
        bytes[0] = 0x60;
        bytes[0] |= getRegister(strtok(NULL, " "));
        bytes[1] = getRegister(strtok(NULL, " ")) << 4 | getRegister(strtok(NULL, " "));
        return 2;
	}
	if (strcmp("leftshift",keyWord) == 0) {
        bytes[0] = 0x70;
        bytes[0] |= getRegister(strtok(NULL, " "));
        bytes[1] = atoi(strtok(NULL, " "));
        return 2;
	}
	if (strcmp("rightshift",keyWord) == 0) {
        bytes[0] = 0x70;
        bytes[0] |= getRegister(strtok(NULL, " "));
        bytes[1] = 0x20;
        bytes[1] |= atoi(strtok(NULL, " "));
        return 2;
	}
	if (strcmp("load",keyWord) == 0) {
        bytes[0] = 0xE0;
        bytes[0] |= getRegister(strtok(NULL, " "));
        bytes[1] = getRegister(strtok(NULL, " ")) << 4 | atoi(strtok(NULL, " "));
        return 2;
	}
	if (strcmp("store",keyWord) == 0) {
        bytes[0] = 0xF0;
        bytes[0] |= getRegister(strtok(NULL, " "));
        bytes[1] = getRegister(strtok(NULL, " ")) << 4 | atoi(strtok(NULL, " "));
        return 2;
	}
	if (strcmp("halt\r\n",keyWord) == 0) {
        bytes[0] = 0x00;
        bytes[1] = 0x00;
        return 2;
	}
	if (strcmp("jump",keyWord) == 0) {
        bytes[0] = 0xC0;
        num = atoi(strtok(NULL, " "));
        bytes[0] |= (num & 0xF000000) >> 24;
        bytes[1] = (num & 0xFF0000) >> 16;
        bytes[2] = (num & 0xFF00) >> 8;
        bytes[3] = (num & 0xFF);
        return 4;
	}
	if (strcmp("branchifequal",keyWord) == 0) {
        bytes[0] = 0xA0;
        bytes[0] |= getRegister(strtok(NULL, " "));
        bytes[1] = getRegister(strtok(NULL, " ")) << 4;
        num = atoi(strtok(NULL, " "));
        bytes[1] |= (num & 0xF0000) >> 16;
        bytes[2] = (num & 0xFF00) >> 8;
        bytes[3] = (num & 0xFF);
        return 4;
	}
	if (strcmp("branchifless",keyWord) == 0) {
        bytes[0] = 0xB0;
        bytes[0] |= getRegister(strtok(NULL, " "));
        bytes[1] = getRegister(strtok(NULL, " ")) << 4;
        num = atoi(strtok(NULL, " "));
        bytes[1] |= (num & 0xF0000) >> 16;
        bytes[2] = (num & 0xFF00) >> 8;
        bytes[3] = (num & 0xFF);
        return 4;
	}
	if (strcmp("iterateover",keyWord) == 0) {
        bytes[0] = 0xD0;
        bytes[0] |= getRegister(strtok(NULL, " "));
        bytes[1] = atoi(strtok(NULL, " "));
        num = atoi(strtok(NULL, " "));
        bytes[2] = (num & 0xFF00) >> 8;
        bytes[3] = (num & 0xFF);
        return 4;
	}
}

/**
 * Prepares the text file of commands to be read, writes to the bytecode file to be used by the virtual machine
 * later
 * Parameters: *src - The text file to be read
 */
int main(int argc, char **argv) {
	FILE *src = fopen(argv[1],"r");
	FILE *dst = fopen(argv[2],"w");
	while (!feof(src)) {
		unsigned char bytes[4];
		for(int i = 0; i < 4; i++) {
			bytes[i] = NULL;
		}
		char line[1000];
		printf ("\nabout to read\n");
		if (NULL != fgets(line, 1000, src)) {
			printf ("%s",line);
			int byteCount = assembleLine(line,bytes);
			fwrite(bytes,byteCount,1,dst);
		}
    }
	printf("\ndone");
	fclose(src);
	fclose(dst);
	return 0;
}
