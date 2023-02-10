#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>






struct _meta_info {
    size_t size;
    bool isUsed; // false if not used, true if used
    struct _meta_info * next;
    struct _meta_info * prev;
};

typedef struct _meta_info meta_info;

// a global variable to store the head of the free-blocks list
meta_info * free_block_head_lock = NULL;
// a global variable to store the head of the free-blocks list
__thread meta_info * free_block_head_nolock = NULL;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// version 1 : locking version
void * ts_malloc_lock(size_t size);
void ts_free_lock(void *ptr);

// version 2 : non-locking version
void * ts_malloc_nolock(size_t size);
void ts_free_nolock(void *ptr);



//Best Fit malloc/free
void * bf_malloc(size_t size, meta_info ** free_block_head, bool isLock);
void bf_free(void *ptr, meta_info ** free_block_head);

meta_info * creatNewBlock(size_t size, bool isLock);
meta_info * splitBlock(meta_info * curr, size_t size);
void addFreeBlock(meta_info * curr, meta_info ** free_block_head);
void removeBlock(meta_info * curr, meta_info ** free_block_head);

void checkBlockConnected(meta_info * curr, meta_info ** free_block_head);
void mergeBlock(meta_info * new_address, size_t incre_size);
meta_info * getBestBlock(meta_info * start, size_t size);