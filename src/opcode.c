#include <stdio.h>
#include "sign-forth.h"
#include "opcode.h"
#include "utils.h"
#include "stats.h"

#define IMAGE_SIZE 0x10000
#define CELL_SIZE (sizeof(forth_cell))

forth_cell image[IMAGE_SIZE / CELL_SIZE];
int32_t pc;

#define MAX_DSTACK_N 256
#define MAX_RSTACK_N 256
forth_int dstack[MAX_DSTACK_N];
forth_int *dstackp;
forth_int rstack[MAX_RSTACK_N];
forth_int *rstackp;


//----------------------------------------------------------------
#define check_can_push() do { \
	if ( &dstack[MAX_DSTACK_N] <= dstackp ) { \
		panic("dstack over flow"); \
	} \
} while (0);

#define check_depth(n) do { \
	if ( (dstackp - &dstack[0]) < n ) { \
		panic("dstack under flow"); \
	} \
} while (0);

#define rcheck_can_push() do { \
	if ( &rstack[MAX_DSTACK_N] <= rstackp ) { \
		panic("rstack over flow"); \
	} \
} while (0);

#define rcheck_depth(n) do { \
	if ( (rstackp - &rstack[0]) < n ) { \
		panic("rstack under flow"); \
	} \
} while (0);

//----------------------------------------------------------------
void
init_image()
{
	pc = 0;
	dstackp = &dstack[0];
	rstackp = &rstack[0];
}

//----------------------------------------------------------------

void
run_image()
{
	forth_cell opcode;
	forth_int tmpi;

_top_:
	opcode = image[pc];
	rec_stats(opcode);

	dump_step();

	switch( opcode ) {
	case VM_NOP:
		break;
	case VM_LIT16:
		check_can_push();
		++pc;
		*dstackp = (forth_cell_int)(image[pc]);
		++dstackp;
		break;

	case VM_LIT32:
		check_can_push();
		++pc;
		tmpi = image[pc];

		++pc;
		tmpi = ((image[pc]) << 16) | ( tmpi ); 

		*dstackp = tmpi;
		++dstackp;

		break;

	case VM_DUP:
		check_can_push();
		check_depth(1);
		*dstackp = *(dstackp - 1);
		++dstackp;
		break;

	case VM_DROP:
		check_depth(1);
		--dstackp;
		break;

	case VM_SWAP:
		check_depth(2);

		tmpi = *(dstackp - 1);
		*(dstackp - 1) = *(dstackp - 2);
		*(dstackp - 2) = tmpi;
		break;

	case VM_PUSH:
		check_depth(1);
		rcheck_can_push();

		*rstackp = *(dstackp - 1);
		++rstackp;
		--dstackp;
		break;

	case VM_POP:
		rcheck_depth(1);
		check_can_push();

		*dstackp = *(rstackp - 1);
		++dstackp;
		--rstackp;
		break;

	case VM_LOOP:
		check_depth(1);

		--(*(dstackp -1));
		++pc;
		if ((*(dstackp-1)) > 0) {
			pc = image[pc] - 1;
		} else {
			--dstackp;
		}
		
		break;
		
	case VM_DEBUG_NOP:
		dump_all();
		break;

	case VM_HALT:
		dump_all();
		panic("halt");
		break;
	default:
		panic("illegal opcode");
		break;
	}

	++pc;

	goto _top_;
}

