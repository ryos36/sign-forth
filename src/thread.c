void *code_table[10];
int pc;

static void *label_table[4];
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
	int my_pc;
	static const void *labels[] = {
		&&L01,
		&&L02,
		&&L03,
	};

	if ( label_table[0] == 0 ) {
		int i;
		for( i = 0; i < sizeof(labels)/sizeof(void *); ++i ) {
			label_table[i] = (void *)labels[i];
		}

		return;
	}
	my_pc = pc;

	goto *code_table[my_pc++];

L01:
	display("L01\n");
	goto *code_table[my_pc++];

L02:
	display("L02\n");
	goto *code_table[my_pc++];

L03:
	display("L03\n");
	pc = my_pc;
	++count;
	if ( count == 100000000 ) {
		return;
	}
	my_pc = 0;
	goto *code_table[my_pc++];
}

int 
main(int argc, char **argv)
{
	threaded();

	code_table[0] = label_table[0];
	code_table[1] = label_table[1];
	code_table[2] = label_table[0];
	code_table[3] = label_table[1];
	code_table[4] = label_table[2];
	pc = 0;

	threaded();
}
