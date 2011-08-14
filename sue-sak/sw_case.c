enum {
	L01,
	L02,
	L03
};
int code_table[10];
int pc;

int count;

void display(char *str)
{
	static volatile int *ip, iv;
	if ( ip == 0 ) {
		ip = &iv;
	}
	*ip = 0;
#ifdef DISPLAY
	printf(str);
#endif
}

void
threaded()
{

START:
	/*
	printf("pc %d %d\n", pc, code_table[pc]);
	sleep(1);
	*/
	switch( code_table[pc++] ) {
	case L01:
		display("L01\n");
		break;
	case L02:
		display("L02\n");
		break;
	case L03:
		display("L03\n");
		++count;
		if ( count == 100000000 ) {
			return;
		}
		pc = 0;
		break;
	}
	goto START;
}

int 
main(int argc, char **argv)
{
	code_table[0] = L01;
	code_table[1] = L02;
	code_table[2] = L01;
	code_table[3] = L02;
	code_table[4] = L03;
	pc = 0;

	threaded();
}
