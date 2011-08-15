#include "reg_stack.h"
#include "sign_forth_private.h"

#define REG(__N__) *(dstackp - (__N__ + 1))
#define RETURN_REG(__N__) *(rstackp - (__N__ + 1))

struct register_cache {
	forth_int reg;
	int valid;
};

#define INVALID_REG_INDEX -1
int reg_index[MAX_REG_N] = {
	INVALID_REG_INDEX, INVALID_REG_INDEX,
	INVALID_REG_INDEX, INVALID_REG_INDEX,
	INVALID_REG_INDEX, INVALID_REG_INDEX,
	INVALID_REG_INDEX, INVALID_REG_INDEX
};

struct register_cache register_cache[MAX_REG_N];

void reserve_reg0()
{
	++dstackp;
}

forth_int
get_reg(int reg_n)
{
	return REG(reg_n);
}

void
set_reg(int reg_n, forth_int v)
{
	REG(reg_n) = v;
}

void
mov_reg(int reg_n0, int reg_n1)
{
	REG(reg_n0) = REG(reg_n1);
}

void
drop_reg()
{
	--dstackp;
}

void
swap_reg(int reg_n0, int reg_n1)
{
	forth_int tmpi;

	tmpi = REG(reg_n0);
	REG(reg_n0) = REG(reg_n1);
	REG(reg_n1) = tmpi;
}

void
add_reg_imm(int reg_n, int n)
{
	REG(reg_n) += n;
}

void
add_reg(int reg_n0, int reg_n1, int reg_n2)
{
	REG(reg_n0) = REG(reg_n1) + REG(reg_n2);
}

void
sub_reg(int reg_n0, int reg_n1, int reg_n2)
{
	REG(reg_n0) = REG(reg_n1) - REG(reg_n2);
}

void
mul_reg(int reg_n0, int reg_n1, int reg_n2)
{
	REG(reg_n0) = REG(reg_n1) * REG(reg_n2);
}

void
divmod_reg(int reg_n0, int reg_n1, int reg_n2, int reg_n3)
{
	forth_int tmpi_d, tmpi_m;

	tmpi_d = REG(reg_n2) / REG(reg_n3);
	tmpi_m = REG(reg_n2) % REG(reg_n3);
	REG(reg_n0) = tmpi_d;
	REG(reg_n1) = tmpi_m;

}

void
and_reg(int reg_n0, int reg_n1, int reg_n2)
{
	REG(reg_n0) = REG(reg_n1) & REG(reg_n2);
}

void
or_reg(int reg_n0, int reg_n1, int reg_n2)
{
	REG(reg_n0) = REG(reg_n1) | REG(reg_n2);
}

void
xor_reg(int reg_n0, int reg_n1, int reg_n2)
{
	REG(reg_n0) = REG(reg_n1) ^ REG(reg_n2);
}

void
shl_reg(int reg_n0, int reg_n1, int reg_n2)
{
	REG(reg_n0) = REG(reg_n1) << REG(reg_n2);
}

void
shr_reg(int reg_n0, int reg_n1, int reg_n2)
{
	REG(reg_n0) = REG(reg_n1) >> REG(reg_n2);
}

//------------------------------------------------------------------
void reserve_rreg0()
{
	++rstackp;
}

void set_rreg0(forth_int v)
{
	RETURN_REG(0) = v;
}

forth_int
get_rreg0()
{
	return RETURN_REG(0);
}

void drop_rreg()
{
	--rstackp;
}
