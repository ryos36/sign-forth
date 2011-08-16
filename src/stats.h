#ifndef __STATS_H__
#define __STATS_H__

#ifdef DO_STATS
void rec_stats(forth_cell);
void show_stats();
char *opcode_str(int index);
#else
void rec_stats(forth_cell) {}
void show_stats() {}
char *opcode_str(int index) { return ""; }
#endif

#endif /* __STATS_H__ */
