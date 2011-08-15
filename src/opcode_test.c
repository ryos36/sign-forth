#include <stdio.h>
#include "sign-forth.h"
#include "opcode.h"

extern forth_cell image[];
void init_image();
void run_image();

#define OPCODE(__opcode__) do { \
	image[n++] = __opcode__; \
} while(0);

int
main(int argc, char **argv)
{
	init_image();
	int n;
	n = 0;

	OPCODE(VM_LIT16);
	OPCODE(0xFFFF);
	OPCODE(VM_DUP);
	OPCODE(VM_NOP);
	OPCODE(VM_LIT32);
	OPCODE(0xFFFE);
	OPCODE(0xFFFD);
	OPCODE(VM_DROP);
	OPCODE(VM_LIT16);
	OPCODE(0x1209);
	OPCODE(VM_SWAP);
	OPCODE(VM_DEBUG_NOP);
	OPCODE(VM_PUSH);
	OPCODE(VM_DEBUG_NOP);
	OPCODE(VM_POP);
	OPCODE(VM_LIT16);
	OPCODE(2);
	OPCODE(VM_NOP);
	OPCODE(VM_LOOP);
	OPCODE(17);
	OPCODE(VM_HALT);

	run_image();
}
