#include <stdio.h>
#include "sign-forth.h"
#include "opcode.h"
#include "utils.h"

extern forth_cell image[];
extern int32_t pc;

extern forth_int dstack[];
extern forth_int *dstackp;
extern forth_int rstack[];
extern forth_int *rstackp;

//----------------------------------------------------------------
void
panic(char *msg)
{
	fprintf(stderr, "panic!!:%s\n", msg);
	exit(255);
}

//----------------------------------------------------------------
void dump_dstack()
{
	int depth;
	int i;
	depth = dstackp - &dstack[0];
	fprintf(stderr, "------------ \n", depth);
	fprintf(stderr, "dstack\n", depth);
	fprintf(stderr, "<%d>\n", depth);

	for( i = 0 ; i < depth ; ++i ) {
		forth_int v;
		v = dstack[depth - i - 1];
		fprintf(stderr, "0x%08x %d\n", v, v);
	}
}

//----------------------------------------------------------------
void dump_rstack()
{
	int depth;
	int i;
	depth = rstackp - &rstack[0];
	fprintf(stderr, "------------ \n", depth);
	fprintf(stderr, "rstack\n", depth);
	fprintf(stderr, "<%d>\n", depth);

	for( i = 0 ; i < depth ; ++i ) {
		forth_int v;
		v = rstack[depth - i - 1];
		fprintf(stderr, "0x%08x %d\n", v, v);
	}
}

//----------------------------------------------------------------
void dump_all()
{
	fprintf(stderr, "---------------------------- \n");
	dump_dstack();
	dump_rstack();
}

//----------------------------------------------------------------
void dump_step()
{
	fprintf(stderr, "opcode:0x%04x pc:0x%04x\n", image[pc], pc);
}

