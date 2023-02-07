#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

struct _meta_info {
    size_t size;
    bool isUsed; // false if not used, true if used
    struct _meta_info * next;
    struct _meta_info * prev;
};

typedef struct _meta_info meta_info;

meta_info * free_block_head = NULL; // a pointer to the first element of the free-blocks list
unsigned long data_segment_size = 0;
unsigned long free_data_segment_size = 0;


//First Fit malloc/free
void * ff_malloc(size_t size);
void ff_free(void *ptr);
//Best Fit malloc/free
void * bf_malloc(size_t size);
void bf_free(void *ptr);

meta_info * creatNewBlock(size_t size);
meta_info * splitBlock(meta_info * curr, size_t size);
void addFreeBlock(meta_info * curr);
void removeBlock(meta_info * curr);

void checkBlockConnected(meta_info * curr);
void mergeBlock(meta_info * new_address, size_t incre_size);
meta_info * getBestBlock(meta_info * start, size_t size);

unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();