#ifndef __REG_STACK_H__
#define __REG_STACK_H__

#include "sign_forth.h"

enum {
	MAX_REG_N = 8
};

void reserve_reg0();
forth_int get_reg(int reg_n);
void set_reg(int reg_n, forth_int v);
void mov_reg(int reg_n0, int reg_n1);
void drop_reg();
void swap_reg(int reg_n0, int reg_n1);

void add_reg_imm(int reg_n, int n);
void add_reg(int reg_n0, int reg_n1, int reg_n2);
void sub_reg(int reg_n0, int reg_n1, int reg_n2);
void mul_reg(int reg_n0, int reg_n1, int reg_n2);
void divmod_reg(int reg_n0, int reg_n1, int reg_n2, int reg_n3);
void and_reg(int reg_n0, int reg_n1, int reg_n2);
void or_reg(int reg_n0, int reg_n1, int reg_n2);
void xor_reg(int reg_n0, int reg_n1, int reg_n2);
void shl_reg(int reg_n0, int reg_n1, int reg_n2);
void shr_reg(int reg_n0, int reg_n1, int reg_n2);

void reserve_rreg0();
void set_rreg0(forth_int v);
forth_int get_rreg0();
void drop_rreg();

#endif /* __REG_STACK_H__ */
