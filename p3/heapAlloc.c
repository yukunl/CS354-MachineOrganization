///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2019 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
//
///////////////////////////////////////////////////////////////////////////////
// Main File:        heapAlloc.c
// This File:        heapAlloc.c
// Other Files:      NA
// Semester:         CS 354 Fall 2019
//
// Author:           Yukun Li
// Email:            yli757@wisc.edu
// CS Login:         yukunl
//
/////////////////////////// OTHER SOURCES OF HELP /////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          NA
//
//
// Online sources:   NA
//
//
///////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "heapAlloc.h"

/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footer for each free block but only containing size.
 */
typedef struct blockHeader {
        int size_status;
    /*
    * Size of the block is always a multiple of 8.
    * Size is stored in all block headers and free block footers.
    *
    * Status is stored only in headers using the two least significant bits.
    *   Bit0 => least significant bit, last bit
    *   Bit0 == 0 => free block
    *   Bit0 == 1 => allocated block
    *
    *   Bit1 => second last bit 
    *   Bit1 == 0 => previous block is free
    *   Bit1 == 1 => previous block is allocated
    * 
    * End Mark: 
    *  The end of the available memory is indicated using a size_status of 1.
    * 
    * Examples:
    * 
    * 1. Allocated block of size 24 bytes:
    *    Header:
    *      If the previous block is allocated, size_status should be 27
    *      If the previous block is free, size_status should be 25
    * 
    * 2. Free block of size 24 bytes:
    *    Header:
    *      If the previous block is allocated, size_status should be 26
    *      If the previous block is free, size_status should be 24
    *    Footer:
    *      size_status should be 24
    */
} blockHeader;         

/* Global variable - DO NOT CHANGE. It should always point to the first block,
 * i.e., the block at the lowest address.
 */

blockHeader *heapStart = NULL; // start of heap
blockHeader *current = NULL;// tracker of current
static int counter = 0;// counts nth time of the allocation trip
int heapsize = 0;// keeps track of available heap size


//=========== helper methods ===================== //  
int getblocksize( blockHeader* block);  
int blockused ( blockHeader* block);
int prevblockused( blockHeader* block);
void*  getnextblock (void* current);
void* getprevblock (void* current);
void makenewblock( blockHeader* currentblock, int size, int previousoccupy, int currentoccupy );
// ================================================//
/* 
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block on success.
 * Returns NULL on failure.
 * This function should:
 * - Check size - Return NULL if not positive or if larger than heap space.
 * - Determine block size rounding up to a multiple of 8 and possibly adding padding as a result.
 * - Use NEXT-FIT PLACEMENT POLICY to chose a free block
 * - Use SPLITTING to divide the chosen free block into two if it is too large.
 * - Update header(s) and footer as needed.
 * Tips: Be careful with pointer arithmetic and scale factors.
 */    
void* allocHeap(int size) {

   counter ++ ;  // track counter times of heap allocation called

   if(counter == 1){ // if this is first time allocating on heap
    current = heapStart;
    heapsize = getblocksize( heapStart );
     }    
  
   // calculate end of the heap
   blockHeader* endmark = (blockHeader*)((void*) heapStart + heapsize);
  
   // if size is not positive or bigger than heap space 
   if( size <= 0 ){
       return NULL;
   }
  
   // add header and padding 
   size +=  sizeof(blockHeader); 
   if((size % 8) !=0 ){ 
       size = size + 8 - (size % 8);  
     } 

   // check if size bigger than heap
   if( size > heapsize){
       return NULL;
     }

   blockHeader* nextfitPointer = NULL; // pointer to next 
   blockHeader* blockholder = current; // pointer to track
   int nextblocksize = 0;// size of nextblock
   int allocsuccess = 0;// if found space for allocation

  
   while ( blockholder -> size_status != 1){ // while tracker have not reach end of heap 
      int currentsize = getblocksize( blockholder);
      allocsuccess = currentsize - size; 
      // if current size is big enough to allocate new block and current block not used
      if( currentsize >= size && !blockused( blockholder)){
         //  nextblocksize = currentsize;// record size of nextblock 
	   current = blockholder;
	   nextfitPointer = current;
	   nextblocksize = currentsize;// record size of nextblock
	     break;
        }
      // if not at the end of heap
      if(endmark > (blockHeader*)((void*) blockholder + getblocksize(blockholder))){
		blockholder = getnextblock(blockholder);  
        } else { // switch to start of heap
              blockholder = heapStart;
	}
      // if loop around the heap and did not find allocation space  	
      if(current == blockholder){
            return NULL; 
        }
   }
  if( nextfitPointer == NULL){ // if there are not nextfit
	    return NULL; 
      }
  // make a new block, split the block and change the header of the next block 
  if(allocsuccess){
    makenewblock( nextfitPointer , size  , prevblockused(nextfitPointer) , 1 );
    makenewblock( (void*)nextfitPointer + size , nextblocksize - size  , 1 , 0 );
      }
  
   return (void*)((char*)nextfitPointer + sizeof(blockHeader)) ;
}



/*
 * This helper method gets the payload of a block
 * blockHeader* block - current block
 * returns size of payload
 * */
int getblocksize( blockHeader* block){
    int blocksize = block -> size_status; 
    blocksize = (blocksize >> 2) << 2; // shift bit to getrid of last two bit  
    return blocksize; 
}

/*
 * This helper method checks if current is used
 * block - current block
 * reutns 0 when block not used
 * returns 1  when block is used
 * */

int blockused ( blockHeader* block){
       int size = block -> size_status; 
       return ( (size & 1) == 1 );  
}

/*
 * This helper method checks if previous is used
 * block - current block
 * reutns 0 when prev not used
 * returns 1  when prev is used
 * */

int prevblockused( blockHeader* block){
       int size = block -> size_status;  
       size = size >> 1 ; // shift bit so last bit is previous allocation bit  
       return ( (size & 1) == 1 );	 
}

/*
 * This helper method returns the pointer to nextblock
 * block - current block
 * reutns pointer of next block
 * */

void*  getnextblock (void* current){
      void* p = (void*)current + getblocksize( current) ; 
      return p; 
}


/*
 * This helper method returns the pointer to the previous block
 * block - current block
 * reutns pointer of previous block
 * */

void* getprevblock (void* current){
     // if current is at the start of heap, no previous block 
     if(current <= (void*) heapStart) {
	      return NULL;
      } 
      void* prevfooter = (void*) current - sizeof(blockHeader); 
      void* prevheader = (void*) current - getblocksize(prevfooter); 
      return prevheader; 
      }

/*
 * This helper method makes a new block given pointer of start block, size, prev and current status
 * current block - pointer to current payload
 * int size - size of payload
 * int previousoccupy - p bit 
 * int currentoccupy - a bit
 * */

void makenewblock( blockHeader* currentblock, int size, int previousoccupy, int currentoccupy ){
     currentblock -> size_status = size + previousoccupy * 2 + currentoccupy; 
     // if current is not occupied
     if( currentoccupy == 0) {
     blockHeader* footerp = (void*) currentblock + size - sizeof(blockHeader);
     footerp -> size_status = currentblock -> size_status;
   }

}	



/* 
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - USE IMMEDIATE COALESCING if one or both of the adjacent neighbors are free.
 * - Update header(s) and footer as needed.
 */                    
int freeHeap(void *ptr) {         
     // return -1 if block is NULL, out of bound
     if(ptr == NULL || (int)ptr < (int)heapStart ||  (int)ptr > (int)(heapStart + heapsize)){
	      return -1;
       }
     blockHeader* current = (blockHeader*)((char*)ptr - sizeof(blockHeader));
     // return -1 if current block size  
     if(getblocksize(current) % 8 != 0  || (int)ptr % 8 != 0 ){
	      return -1;
       }
      // return -1 if ptr block already freed
     if(!blockused(current)){
	      return -1;
       }
     int size = getblocksize(current); 
     // check if nextblock is free
     blockHeader* nextblock = getnextblock(current);
     if( nextblock -> size_status != 1  && ! blockused(nextblock)){
            size = size + getblocksize(nextblock);
      }

     // check if previous block is free
     blockHeader* prevblock = getprevblock(current); 
     if( prevblock >= heapStart  && !blockused(prevblock)){
	    size = size + getblocksize(prevblock); 
	   current = prevblock;
     } 

     // make new block starting from current
     makenewblock(current, size, prevblockused(current), 0); 
     // change size status of next block
     nextblock = getnextblock(current);
     // coalasce 
     if( nextblock -> size_status != 1){
          makenewblock( nextblock, getblocksize(nextblock), 0 , blockused(nextblock) );
     }
    return 0;
}

/*
 * Function used to initialize the memory allocator.
 * Intended to be called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */                    
int initHeap(int sizeOfRegion) {         

    static int allocated_once = 0; //prevent multiple initHeap calls

    int pagesize;  // page size
    int padsize;   // size of padding when heap size not a multiple of page size
    int allocsize; // size of requested allocation including padding
    void* mmap_ptr; // pointer to memory mapped area
    int fd;

    blockHeader* endMark;
  
    if (0 != allocated_once) {
        fprintf(stderr, 
        "Error:mem.c: InitHeap has allocated space during a previous call\n");
        return -1;
    }
    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion 
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    allocsize = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    mmap_ptr = mmap(NULL, allocsize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (MAP_FAILED == mmap_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }
  
    allocated_once = 1;

    // for double word alignment and end mark
    allocsize -= 8;

    // Initially there is only one big free block in the heap.
    // Skip first 4 bytes for double word alignment requirement.
    heapStart = (blockHeader*) mmap_ptr + 1;

    // Set the end mark
    endMark = (blockHeader*)((void*)heapStart + allocsize);
    endMark->size_status = 1;

    // Set size in header
    heapStart->size_status = allocsize;

    // Set p-bit as allocated in header
    // note a-bit left at 0 for free
    heapStart->size_status += 2;

    // Set the footer
    blockHeader *footer = (blockHeader*) ((char*)heapStart + allocsize - 4);
    footer->size_status = allocsize;
  
    return 0;
}         
                 
/* 
 * Function to be used for DEBUGGING to help you visualize your heap structure.
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block 
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block as stored in the block header
 */                     
void DumpMem() {  

    int counter;
    char status[5];
    char p_status[5];
    char *t_begin = NULL;
    char *t_end   = NULL;
    int t_size;

    blockHeader *current = heapStart;
    counter = 1;

    int used_size = 0;
    int free_size = 0;
    int is_used   = -1;

    fprintf(stdout, "************************************Block list***\
                    ********************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, "-------------------------------------------------\
                    --------------------------------\n");
  
    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;
    
        if (t_size & 1) {
            // LSB = 1 => used block
            strcpy(status, "used");
            is_used = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "Free");
            is_used = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "used");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "Free");
        }

        if (is_used) 
            used_size += t_size;
        else 
            free_size += t_size;

        t_end = t_begin + t_size - 1;
    
        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n", counter, status, 
        p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);
    
        current = (blockHeader*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, "---------------------------------------------------\
                    ------------------------------\n");
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fprintf(stdout, "Total used size = %d\n", used_size);
    fprintf(stdout, "Total free size = %d\n", free_size);
    fprintf(stdout, "Total size = %d\n", used_size + free_size);
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fflush(stdout);

    return;  
}  
