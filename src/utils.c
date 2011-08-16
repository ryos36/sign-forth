#include <stdio.h>
#include "sign_forth.h"
#include "opcode.h"
#include "utils.h"
#include "stats.h"

#include "sign_forth_private.h"

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

//----------------------------------------------------------------
void
halt()
{
	dump_all();
	show_stats();
	show_stats_mat();
	fprintf(stderr, "HALT:0x%x\n", pc);
	exit(255);
}
