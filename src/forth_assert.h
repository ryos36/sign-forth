#ifndef __FORTH_ASSERT_H__
#define __FORTH_ASSERT_H__

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

#define check_fetch() do { \
	if ( (*(dstackp - 1) * sizeof(forth_cell)) > sizeof(image) ) { \
		panic("pc out of range"); \
	} \
} while (0);

#define check_store() do { \
	if ( (*(dstackp - 1) * sizeof(forth_cell)) > sizeof(image) ) { \
		panic("tos out of range"); \
	} \
	if ( (*(dstackp - 2)) > 0xFFFF ) { \
		panic("nos out of range"); \
	} \
} while (0);

#endif /* __FORTH_ASSERT_H__ */
