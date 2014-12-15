/* 8M */
#define MAX_SIZE (1024*1024*8)
#define WARMUP 100
#define LOOP 1000

extern double wtime(void);
extern void print_items(void);
extern void print_results(unsigned int, double);
extern void init_buf(char[], int num);
