#include <stdint.h>
#include "opcode.h"

typedef uint16_t forth_cell;
typedef int32_t forth_int;
#define IMAGE_SIZE 0x10000
#define CELL_SIZE (sizeof(forth_cell))

forth_cell codes[IMAGE_SIZE / CELL_SIZE];
forth_cell *codep;

forth_int dstack[256];
forth_int *dstackp;
forth_int rstack[256];
forth_int *rstackp;

static
inline 
int32_t get_int(uint16_t acode)
{
	int32_t v;

	v = (acode & 0x8000) << 16;
	v |= ((acode & 0x7FFC) >> 2);

	return v;	
}

void
opecode()
{
	uint16_t acode;
	acode = *codep;

	switch( acode & 0x03 ) {
	case 0x00:
		*dstackp = get_int(acode);
		dstackp++;
		break;
	case 0x01:
		*rstackp = codep + 1;
		++rstackp;
		codep = (acode >> 2);
		break;
	case 0x02:
		codep = (acode >> 2);
		break;
	case 0x03:
		if (( acode & 0x4 ) == 0 ) {
			codep += ((acode & 0xFFF80) >> 3);
		} else {
			if (( acode & 0x8 ) == 0 ) {
			} else {
			}
		}
		break;
	}
}

