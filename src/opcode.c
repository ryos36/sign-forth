#include <stdio.h>
#include "sign_forth.h"
#include "opcode.h"
#include "utils.h"
#include "stats.h"
#include "forth_assert.h"
#include "reg_stack.h"

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
		reserve_reg0();
		set_reg(0, (forth_cell_int)(image[pc]));
		break;

	case VM_LIT32:
		check_can_push();
		++pc;
		tmpi = image[pc];

		++pc;
		tmpi = ((image[pc] & 0xFFFF) << 16) | ( tmpi & 0xFFFF); 

		reserve_reg0();
		set_reg(0, tmpi);

		break;

	case VM_DUP:
		check_can_push();
		check_depth(1);

		reserve_reg0();
		mov_reg(0, 1);
		break;

	case VM_DROP:
		check_depth(1);
		drop_reg();
		break;

	case VM_SWAP:
		check_depth(2);

		swap_reg(0, 1);
		break;

	case VM_PUSH:
		check_depth(1);
		rcheck_can_push();

		tmpi = get_reg(0);
		reserve_rreg0();
		set_rreg0(tmpi);
		drop_reg();

		break;

	case VM_POP:
		rcheck_depth(1);
		check_can_push();

		tmpi = get_rreg0();
		reserve_reg0();
		set_reg(0, tmpi);
		drop_rreg();

		break;

	case VM_LOOP:
		check_depth(1);

		add_reg_imm(0, -1);
		++pc;
		if (get_reg(0) > 0) {
			pc = image[pc] - 1;
		} else {
			drop_reg();
		}
		
		break;

	case VM_JUMP:
		++pc;
		pc = image[pc] - 1;
		break;

	case VM_RETURN:
		rcheck_depth(1);

		pc = get_rreg0();
		drop_rreg();
		break;

	case VM_GT_JUMP:
		check_depth(2);

		++pc;
		if (get_reg(1) > get_reg(0)) {
			pc = image[pc] - 1;
		}
		drop_reg();
		drop_reg();
		break;

	case VM_LT_JUMP:
		check_depth(2);

		++pc;
		if (get_reg(1) < get_reg(0)) {
			pc = image[pc] - 1;
		}
		drop_reg();
		drop_reg();
		break;

	case VM_NE_JUMP:
		check_depth(2);

		++pc;
		if (get_reg(1) != get_reg(0)) {
			pc = image[pc] - 1;
		}
		drop_reg();
		drop_reg();
		break;

	case VM_EQ_JUMP:
		check_depth(2);

		++pc;
		if (get_reg(1) == get_reg(0)) {
			pc = image[pc] - 1;
		}
		drop_reg();
		drop_reg();
		break;

	case VM_FETCH:
		check_depth(1);
		check_fetch();

		set_reg(0, image[get_reg(0)]);
		break;

	case VM_STORE:
		check_depth(2);
		check_store();

		image[get_reg(0)] = get_reg(1);
		drop_reg();
		drop_reg();
		break;

	case VM_ADD:
		check_depth(2);
		add_reg(1, 1, 0);

		drop_reg();
		break;

	case VM_SUB:
		check_depth(2);
		sub_reg(1, 1, 0);

		drop_reg();
		break;

	case VM_MUL:
		check_depth(2);
		mul_reg(1, 1, 0);

		drop_reg();
		break;

	case VM_DIVMOD:
		check_depth(2);
		divmod_reg(0, 1, 1, 0);

		drop_reg();
		break;

	case VM_AND:
		check_depth(2);
		and_reg(0, 1, 0);

		drop_reg();
		break;

	case VM_OR:
		check_depth(2);
		or_reg(0, 1, 0);

		drop_reg();
		break;

	case VM_XOR:
		check_depth(2);
		xor_reg(0, 1, 0);

		drop_reg();
		break;

	case VM_SHL:
		check_depth(2);
		shl_reg(0, 1, 0);

		drop_reg();
		break;

	case VM_SHR:
		check_depth(2);
		shr_reg(0, 1, 0);

		drop_reg();
		break;

	case VM_ZERO_EXIT:
		break;

	case VM_INC:
		check_depth(1);

		add_reg_imm(0, 1);
		break;

	case VM_DEC:
		check_depth(1);

		add_reg_imm(0, -1);
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

