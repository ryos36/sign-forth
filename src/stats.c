#include <stdint.h>
#include "sign_forth.h"
#include "opcode.h"
#include "stats.h"

#ifdef DO_STATS


enum {
	STATS_VM_LIT14 = VM_HALT + 1,
	STATS_VM_CALL,
	STATS_VM_ILLEGAL_OPCODE,

	STATS_N = OPCODE_N + 1,
// + 1 for illegal opcode.
};

static uint32_t stats[STATS_N];
static uint32_t stats_mat[STATS_N][STATS_N][STATS_N];
static int cell_history[3] = {
	VM_NOP,
	VM_NOP,
	VM_NOP
};

void
rec_stats(forth_cell cell)
{
	int index;

	if ( cell <= VM_HALT ) {
		index = (int)cell;
	} else if ( cell & VM_CALL_FLAG ) {
		index = STATS_VM_CALL;
	} else if (( cell & 0xC000 ) == VM_LIT14_FLAG ) {
		index = STATS_VM_LIT14;
	} else {
		index = STATS_VM_ILLEGAL_OPCODE;
	}

	++stats[index];

	cell_history[0] = cell_history[1];
	cell_history[1] = cell_history[2];
	cell_history[2] = index;
	++stats_mat[cell_history[0]][cell_history[1]][cell_history[2]];
}

void
show_stats()
{
	int i;
	for( i = 0 ; i < STATS_N ; ++i ) {
		printf("%-12s:%3d\n", opcode_str(i), stats[i]);
	}
}

static int      top10[10];
static uint32_t id[10];

void
add_sort(int i, int j, int k)
{
	uint32_t aid, tmp_id;
	int v, tmpv;
	int index;

	v = stats_mat[i][j][k];
	if ( v == 0 ) {
		return;
	}

	aid = ( i << 16 ) | 
	      ( j <<  8 ) | 
	      ( k <<  0 );

	for( index = 0 ; index < 10; ++index ) {
		if ( v > top10[index] ) {
			tmpv = top10[index];
			top10[index] = v;

			tmp_id = id[index];
			id[index] = aid;

			++index;
			break;
		}
	}

	for( ; index < 10; ++index ) {
		int tmp_tmpv;
		uint32_t tmp_tmp_id;

		tmp_tmpv = top10[index];
		top10[index] = tmpv;
		tmpv = tmp_tmpv;

		tmp_tmp_id = id[index];
		id[index] = tmp_id;
		tmp_id = tmp_tmp_id;
	}
}

void
show_stats_mat(int top_n)
{
	int i, j, k;

	for(i = 0 ; i < 10; ++i ) {
		top10[i] = 0;
		id[i] = 0;
	}

	for(i = 0 ; i < STATS_N; ++i ) {
		for(j = 0 ; j < STATS_N; ++j ) {
			for(k = 0 ; k < STATS_N; ++k ) {
				add_sort(i, j, k);
			}
		}
	}

	for(i = 0 ; i < 10; ++i ) {
		if ( top10[i] == 0 ) {
			break;
		}
		printf("%-12s=> %-12s=> %12s %3d\n",
			opcode_str((id[i] & 0xFF0000) >> 16),
			opcode_str((id[i] & 0x00FF00) >>  8),
			opcode_str((id[i] & 0x0000FF) >>  0),
			top10[i]);
	}
}

#endif
