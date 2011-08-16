#include <stdio.h>
#include "sign_forth.h"
#include "sign_forth_private.h"
#include "reg_stack.h"

#define IMAGE_SIZE 0x10000
#define CELL_SIZE (sizeof(forth_cell))

forth_cell image[IMAGE_SIZE / CELL_SIZE];
int32_t pc;

#define MAX_DSTACK_N 256
#define MAX_RSTACK_N 256
forth_int dstack[MAX_DSTACK_N];
int dstack_index;
forth_int rstack[MAX_RSTACK_N];
forth_int *rstackp;

struct register_cache {
	forth_int reg;
	int valid;
	int sync;
	int dstack_index;
};

#define INVALID_REG_INDEX -1
int reg_index[MAX_REG_N] = {
	INVALID_REG_INDEX, INVALID_REG_INDEX,
	INVALID_REG_INDEX, INVALID_REG_INDEX,
	INVALID_REG_INDEX, INVALID_REG_INDEX,
	INVALID_REG_INDEX, INVALID_REG_INDEX
};
int valid_reg_n;
int sync_reg_index = INVALID_REG_INDEX;

struct register_cache register_cache[MAX_REG_N];

enum {
	LOW_WATER_N = 4,
	HIGH_WATER_N = 6,
};

int do_sync_flag;
int do_restore_flag;

int
search_free_reg_index()
{
	struct register_cache *free_rc;
	int i;
	for( i = valid_reg_n + 1; i < MAX_REG_N; ++i ) {
		free_rc = &register_cache[i];
		if ( free_rc->valid == 0 ) {
			return i;
		}
	}
	for( i = 0 ; i <= valid_reg_n; ++i ) {
		free_rc = &register_cache[i];
		if ( free_rc->valid == 0 ) {
			return i;
		}
	}
	return INVALID_REG_INDEX;
}

void 
do_sync()
{
	struct register_cache *rc;
	int i;

	if ( sync_reg_index == INVALID_REG_INDEX ) {
		sync_reg_index = valid_reg_n - 1;
	}

	for( i = sync_reg_index ; 0 <= i ; --i ) {
		rc = &register_cache[reg_index[i]];
		if ( rc->valid && ( rc->sync == 0 )) {
			sync_reg_index = i;
			break;
		} else {
			rc = 0;
		}
	}
	if ( rc == 0 ) {
		do_sync_flag = 0;
		sync_reg_index = INVALID_REG_INDEX;
		return;
	}
	//printf("sync_reg %d %d\n", rc->dstack_index, sync_reg_index);
	dstack[rc->dstack_index] = rc->reg;
	rc->sync = 1;
}

void 
do_restore()
{
	struct register_cache *rc;
	int free_rc_index;
	struct register_cache *free_rc;

	if ( valid_reg_n == MAX_REG_N ) {
		do_restore_flag = 0;
		return;
	}

	if ( dstack_index == valid_reg_n ) {
		do_restore_flag = 0;
		return;
	}

	free_rc_index = search_free_reg_index();
	free_rc = &register_cache[free_rc_index];

	free_rc->valid = 1;
	free_rc->sync = 1;
	free_rc->dstack_index = dstack_index - valid_reg_n - 1;
	free_rc->reg = dstack[free_rc->dstack_index];

	reg_index[valid_reg_n] = free_rc_index;
	++valid_reg_n;
	if ( valid_reg_n == MAX_REG_N ) {
		do_restore_flag = 0;
	}
}

void 
do_sync_restore()
{
	if ( do_restore_flag ) {
		do_restore();
	} else if ( do_sync_flag ) {
		do_sync();
	}
}



int
is_occupied()
{
	struct register_cache *rc;
	if ((reg_index[MAX_REG_N - 1] == INVALID_REG_INDEX)) {
		return 0;
	}
	rc = &register_cache[reg_index[MAX_REG_N-1]];
	return (rc->sync == 0);
}

int
need_push_to_mem()
{
	return (reg_index[LOW_WATER_N] != INVALID_REG_INDEX);
}

int
need_pop_from_mem()
{
	//????
	return (reg_index[HIGH_WATER_N] != INVALID_REG_INDEX);
}

void
push_memory_to()
{
	int i;
	for( i = 0 ; i < MAX_REG_N ; ++i ) {
		struct register_cache *rc;
		rc = &register_cache[i];
		if ( rc->valid ) {
			if ( rc->sync == 0 ) {
				/*
				printf("dstack[%d] <- %x\n",
					rc->dstack_index, rc->reg);
				*/
				dstack[rc->dstack_index] = rc->reg;
				rc->sync = 1;
			}
		}
		
	}
}

void
reserve_register_cache()
{
	int i, free_rc_index;;
	struct register_cache *free_rc;
	int start_reg_n;

	free_rc_index = INVALID_REG_INDEX;
	if ( valid_reg_n == MAX_REG_N ) {
		free_rc_index = reg_index[MAX_REG_N - 1];
	} else {
		free_rc_index = search_free_reg_index();
	}

	if ( free_rc_index == INVALID_REG_INDEX ) {
		fprintf(stderr, "panic!!\n");
		exit(1);
	}
	free_rc = &register_cache[free_rc_index];

	if ( valid_reg_n == MAX_REG_N ) {
		start_reg_n = MAX_REG_N - 1;
	} else {
		++valid_reg_n;
		start_reg_n = valid_reg_n;
	}
	for(i = start_reg_n ; 1 <= i ; --i ) {
		struct register_cache *rc;
		//printf("mov %d <- %d\n", i, i - 1);
		reg_index[i] = reg_index[i-1];
		rc = &register_cache[reg_index[i]];
	}

	free_rc->valid = 0;
	free_rc->sync = 0;
	free_rc->dstack_index = dstack_index;
	++dstack_index;
	reg_index[0] = free_rc_index;
}

#define SHOW_STACK() do {\
	int i; \
	fprintf(stdout, "==========\n"); \
	fprintf(stdout, "<%d>\n", dstack_index); \
	for( i = dstack_index - 1 ; 0 <= i ; --i ) { \
		fprintf(stdout, "0x%08x\n", dstack[i]); \
	} \
	fprintf(stdout, "==========\n"); \
} while(0);

void
consume_register_cache()
{
	int i;
	struct register_cache *free_rc;
	int end_reg_n;
	int free_rc_index;

	free_rc_index = reg_index[0];
	free_rc = &register_cache[free_rc_index];
	end_reg_n = valid_reg_n - 1;
	for(i = 0; i < end_reg_n; ++i ) {
		reg_index[i] = reg_index[i+1];
	}

	reg_index[valid_reg_n - 1] = INVALID_REG_INDEX;

	/*
	for( i = 0 ; i < valid_reg_n; ++i ) {
		fprintf(stderr, "i %d %d\n", i, reg_index[i]);
	}
	*/

	--valid_reg_n;
	--dstack_index;

	if (( valid_reg_n <= HIGH_WATER_N ) && 
	    ( dstack_index != valid_reg_n )) {

		do_restore_flag = 1;
		do_sync_flag = 0;
	}
	    	/*
		free_rc->valid = 1;
		free_rc->sync = 1;
		free_rc->dstack_index = dstack_index - valid_reg_n - 1;
		free_rc->reg = dstack[free_rc->dstack_index];
		reg_index[valid_reg_n] = free_rc_index;
		++valid_reg_n;
	} else {
		*/
	{
		if ( do_sync_flag ) {
			sync_reg_index = valid_reg_n - 1;
		}
		free_rc->valid = 0;
		free_rc->sync = 0;
	}
}


void reserve_reg0()
{
	if( is_occupied() ) {
		push_memory_to();
	}
	if ( need_push_to_mem()) {
		do_sync_flag = 1;
		do_restore_flag = 0;
	}
	reserve_register_cache();
}

forth_int
get_reg(int reg_n)
{
	return register_cache[reg_index[reg_n]].reg;
}

void
set_reg(int reg_n, forth_int v)
{
	struct register_cache *rc;

	rc = &register_cache[reg_index[reg_n]];
	rc->reg = v;
	rc->sync = 0;
	rc->valid = 1;
}

void
drop_reg()
{
	consume_register_cache();
}

void
mov_reg(int reg_n0, int reg_n1)
{
	struct register_cache *rc0, *rc1;
	rc0 = &register_cache[reg_index[reg_n0]];
	rc1 = &register_cache[reg_index[reg_n1]];

	rc0->reg = rc1->reg;
	rc0->sync = 0;
	rc0->valid = 1;
}

void
show()
{
	struct register_cache *rc;
	int i;

	fprintf(stdout, "------------------------\n");
	for( i = 0 ; i < MAX_REG_N; ++i ) {
		rc = &register_cache[reg_index[i]];
		fprintf(stdout, "reg_index:%d ", i);
		if ( reg_index[i] == INVALID_REG_INDEX ) {
			fprintf(stdout, "- ");
		} else {
			fprintf(stdout, "%d ", reg_index[i]);
		}
		if ( rc->valid ) {
			fprintf(stdout, "0x%08x %c %2d\n", rc->reg,
				rc->sync?'S':'-',
				rc->dstack_index);
		} else {
			fprintf(stdout, "  XXXXXXXX X XX\n");
		}
	}
	fprintf(stdout, "----------\n");
	fprintf(stdout, "<%d>\n", dstack_index);
	for( i = dstack_index - 1 ; 0 <= i ; --i ) {
		fprintf(stdout, "0x%08x\n", dstack[i]);
	}
}

#define PUSH(__N__) do { \
	printf("push\n");\
	reserve_reg0(); \
	set_reg(0, __N__); \
	do_sync_restore(); \
	show(); \
} while(0);

#define NOP() do { \
	printf("nop\n");\
	do_sync_restore(); \
	show(); \
} while(0);

#define POP() do { \
	printf("pop\n");\
	drop_reg(); \
	do_sync_restore(); \
	show(); \
} while(0);

int
main(int argc, char **argv)
{
	PUSH(0x9999);
	PUSH(0x8888);
	PUSH(0x7777);
	PUSH(0x6666);
	PUSH(0x5555);
	PUSH(0x4444);
	PUSH(0x3333);
	PUSH(0x2222);
	PUSH(0x1111);
	PUSH(0x0001);
	PUSH(0x0002);
	NOP();
	NOP();

	POP();
	POP();

	NOP();
	NOP();

	POP();
	POP();

	NOP();
	NOP();

	POP();
	POP();
	POP();
	POP();
}
