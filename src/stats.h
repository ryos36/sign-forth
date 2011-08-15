#ifndef __STATS_H__
#define __STATS_H__

#ifdef DO_STATS
void rec_stats(forth_cell);
#else
void rec_stats(forth_cell) {}
#endif

#endif /* __STATS_H__ */
