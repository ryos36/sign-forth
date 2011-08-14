#include <stdint.h>
#include "opcode.h"

uint16_t codes[0x10000/sizeof(uint16_t)];
int16_t *codep;
int32_t dstack[256];
int32_t *dstackp;
int32_t rstack[256];
int32_t *rstackp;

static int32_t get_int(uint16_t acode)
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

