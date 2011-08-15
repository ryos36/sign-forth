#ifndef __SIGN_FORTH_H__
#define __SIGN_FORTH_H__

#include <stdint.h>

typedef uint16_t forth_cell;
typedef int16_t forth_cell_int;
typedef int32_t forth_int;

void init_image();
void run_image();

#endif /* __SIGN_FORTH_H__ */
