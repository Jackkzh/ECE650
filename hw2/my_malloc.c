#include "my_malloc.h"
#include <limits.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>





void * ts_malloc_lock(size_t size) {
    pthread_mutex_lock(&lock);
    bool isLock = true;
    void * curr = bf_malloc(size, free_block_head_lock, isLock);
    pthread_mutex_unlock(&lock);
    return curr;
}

void ts_free_lock(void * ptr) {
    pthread_mutex_lock(&lock);
    bf_free(ptr, free_block_head_lock);
    pthread_mutex_unlock(&lock);
}

void * ts_malloc_nolock(size_t size) {
    bool isLock = false;
    void * curr = bf_malloc(size, free_block_head_nolock, isLock);
    return curr;
}

void ts_free_nolock(void *ptr) {
    bf_free(ptr, free_block_head_nolock);
}



/*
    This function finds the best-fit memory block to use, returns its address
*/
meta_info * getBestBlock(meta_info * start, size_t size) {
    meta_info * curr = start;
    meta_info * best_address = NULL;
    size_t min_size;
    //printf("%ld", min_size);
    while (curr != NULL) {
        if (curr->size >= size && (best_address == NULL || curr->size < min_size)) {
            min_size = curr->size;
            best_address = curr;
        }
        if (curr->size == size && curr->size < size + sizeof(meta_info)) {
            // this is for speeding the program, otherwise runs out of time(? 
            return curr;
        } 
        curr = curr->next;
    }
    return best_address;
} 


/*
    This function would allocate memory block with size as input, using existing free block or 
    creating new memory using sbrk() depending on different conditions
*/
void * bf_malloc(size_t size, meta_info * free_block_head, bool isLock) {
    // print a message "its in malloc"

    meta_info * curr = NULL;
    meta_info * to_use = NULL;

    if (free_block_head != NULL) {

        curr = free_block_head;

        to_use = getBestBlock(curr, size);

        if (to_use != NULL) {
            to_use->isUsed = true;
            // 1. needs to split the block into two parts
            if (to_use->size > size + sizeof(meta_info)) {
                meta_info * splitted = splitBlock(to_use, size);
                removeBlock(to_use, free_block_head);
                addFreeBlock(splitted, free_block_head);
                //ree_data_segment_size -= (size + sizeof(meta_info));
            } else {
                removeBlock(to_use, free_block_head);
                //free_data_segment_size -= (to_use->size + sizeof(meta_info));
            }
            return (void *)to_use + sizeof(meta_info);
        } 
    } 
    if (free_block_head == NULL || to_use == NULL) {

        //printf("BF -- creates new blocks ");
        // 2. no usable free blocks or free-blocks list is empty
        curr = creatNewBlock(size, isLock);
        return curr;    
    }
}

// This function creates a new block of memory using sbrk(), 
// with size as input, and isLock as a flag, and returns the address of the data segment
meta_info * creatNewBlock(size_t size, bool isLock) {
    // printf("in creatNewBlock\n");
    meta_info * curr = NULL;
    // if islock is true, then use lock, otherwise use no lock when calling sbrk
    if (isLock == false) {
        pthread_mutex_lock(&lock);
        curr = (meta_info *) sbrk(size + sizeof(meta_info));
        pthread_mutex_unlock(&lock);
    } else {
        curr = (meta_info *) sbrk(size + sizeof(meta_info));
    }
    if (curr == (void *) -1) {
        return NULL;
    } else {
        curr->size = size;
        curr->isUsed = true;
        curr->next = NULL;
        curr->prev = NULL;
        //data_segment_size = data_segment_size + size + sizeof(meta_info);
        return (void *) curr + sizeof(meta_info);
    }
}


void bf_free(void *ptr, meta_info * free_block_head) {
    if (ptr == NULL) {
        return;
    }
    // curr points to the beginning of a meta_info struct
    meta_info * curr = (meta_info *) ((void *)ptr - sizeof(meta_info));
    // free_data_segment_size += (curr->size + sizeof(meta_info));
    addFreeBlock(curr, free_block_head);
    checkBlockConnected(curr, free_block_head);
    
}



/*
    This function checks if there exists adjacent free memory space, and 
    merge them when such situation occurs, returns the altered address 
*/
void checkBlockConnected(meta_info * curr, meta_info * free_block_head) {
    // curr is before curr->next 
    meta_info * new_address = NULL;
    if (curr->next != NULL && (void *)curr + sizeof(meta_info) + curr->size == (void *)curr->next) {
        new_address = curr;
        mergeBlock(curr, curr->next->size);
        removeBlock(curr->next, free_block_head);
    }
    // curr->prev is before curr
    if (curr->prev != NULL && (void *)curr->prev + sizeof(meta_info) + curr->prev->size == (void *)curr) {
        new_address = curr->prev;
        mergeBlock(curr->prev, curr->size);
        removeBlock(curr, free_block_head);
    }
}


/*
    This function merges two adjacent blocks in the physical address, deletes the latter one, keeps the meta_info of 
    the first block, and updates the free-blocks list 
*/
void mergeBlock(meta_info * new_address, size_t incre_size) {
    // ...q ------ first ------ second ------ p ...
    new_address->size = new_address->size + sizeof(meta_info) + incre_size; 
}


/*
    This function removes an element(of type meta_info *) from the free-blocks list
*/
void removeBlock(meta_info * curr, meta_info * free_block_head) {
    printf("in removeBlock\n");
    meta_info * p = free_block_head;
    while (p != NULL) {
        if (curr == p) {
            //printf("in removeBLock: size is %lu\n",p->size);
            if (p == free_block_head) {
                free_block_head = curr->next;
            }
            if (p->prev != NULL) {
                //printf("prev not null\n");
                p->prev->next = p->next;
                p->next->prev = p->prev;
            }

            p->next = NULL;
            p->prev = NULL;
            return;
        }
        p = p->next;
    }
}



void addFreeBlock(meta_info * curr, meta_info * free_block_head) {
    // add a newly freed block to the end of the "free-blocks" list
    curr->isUsed = false;
    // 1. if the free block list is empty
    if (free_block_head == NULL) {
        free_block_head = curr;
        curr->next = NULL;
        curr->prev = NULL;
    } else {
    // 2. add to the list according to the position of their physical addresses
        meta_info * toAdd = free_block_head;
        if (curr < toAdd) {
            curr->next = free_block_head;
            free_block_head->prev = curr;
            curr->prev = NULL;
            free_block_head = curr;
        } else {
            while (toAdd != NULL) {
                if (curr > toAdd) {
                    if ((toAdd->next != NULL && curr < toAdd->next) || toAdd->next == NULL) {
                        // add to bewteen toAdd and toAdd->next
                        curr->next = toAdd->next;
                        toAdd->next = curr;
                        curr->prev = toAdd;
                        if (curr->next != NULL) {
                            curr->next->prev = curr;
                        } 
                        break;
                    } 
                }
                toAdd = toAdd->next;
            }
        }
    }
}


/*
    size is the size needed to allocate a block, curr is the original block to be operated on,
    returns the address of the newly splited block
*/
meta_info * splitBlock(meta_info * curr, size_t size) {
    meta_info * new_block = (void *)curr + sizeof(meta_info) + size;
    new_block->size = curr->size - size - sizeof(meta_info);
    new_block->isUsed = false;
    new_block->next = NULL;
    new_block->prev = NULL;
    curr->size = size;
    return new_block;
}





// unsigned long get_data_segment_size() {
//     return data_segment_size;
// }

// unsigned long get_data_segment_free_space_size() {
//     return free_data_segment_size;
// }
