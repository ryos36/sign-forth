void *code_table[10];
int pc;

static void *label_table[4];

void
threaded()
{
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

	goto *code_table[pc++];

L01:
	printf("L01\n");
	goto *code_table[pc++];

L02:
	printf("L02\n");
	goto *code_table[pc++];

L03:
	printf("L03\n");
	return;
}

void
threaded2()
{
	static const void *labels[] = {
		&&L04,
	};

	if ( label_table[3] == 0 ) {
		label_table[3] = labels[0];
		return;
	}

	goto *code_table[pc++];

L04:
	printf("L04 %d\n", pc);
	sleep(1);
	goto *code_table[pc++];

}

int 
main(int argc, char **argv)
{
	threaded();
	threaded2();

	code_table[0] = label_table[0];
	code_table[1] = label_table[1];
	code_table[2] = label_table[0];
	code_table[3] = label_table[1];
	code_table[4] = label_table[3];
	code_table[5] = label_table[2];
	code_table[4] = label_table[2];
	pc = 0;

	threaded();
}
