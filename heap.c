#include<stdio.h>
#include<stdlib.h>
#define HEAP_SIZE 2000

typedef struct meta_data
{
   int size;
   struct meta_data*next;
}block;

void* heap;
block* freeList;

void*init()
{
  block*heap=malloc(HEAP_SIZE);
  freeList=(block*)heap;
  freeList->size = HEAP_SIZE - sizeof(block); // Size of the block, excluding metadata
  freeList->next = NULL; // No other free blocks initially
}


void* alloc(int req)
{
   block *ptr, *prev;
   ptr = freeList;
   prev = NULL;

   //printf("Allocating %d bytes\n", req);

   while(ptr != NULL && ptr->size < req)
   {
      //printf("Current block size: %d, moving to next block\n", ptr->size);
      prev = ptr;
      ptr = ptr->next;
   }

   if(ptr != NULL && ptr->size == req)
   {
       //printf("Found exact size match, removing block from free list\n");
       if (prev != NULL)
       {
            prev->next = ptr->next;
       } 
       else 
       {
            freeList = ptr->next;
       }
   }
   else if(ptr != NULL && ptr->size > req)
   {
      //printf("Splitting block to allocate %d bytes\n", req);
      block* new_block;
      new_block = (block*)((char*)ptr + sizeof(block) + req);
      new_block->size = ptr->size - req - sizeof(block);
      new_block->next = ptr->next;

      // Update the size of the allocated block
      if(prev != NULL)
      {
         prev->next = new_block;
      }
      else
      {
         freeList = new_block;
      }      
      ptr->size = req;
   }
   else
   {
      printf("Memory is not allocated. Requested size: %d\n", req);
      return NULL;
   }

   printf("Memory allocated successfully: %d bytes\n", req);
   return ptr;
}

void mergeFree()
{
    block* ptr = freeList;
    while (ptr != NULL && ptr->next != NULL)
    {
        //printf("Current block: size = %d, address = %p\n", ptr->size, (void*)ptr);
        //printf("Next block: size = %d, address = %p\n", ptr->next->size, (void*)ptr->next);
        //printf("Merging calculation result: %p\n", (void*)((char*)ptr + sizeof(block) + ptr->size));

        // Calculate the end address of the current block
        void* endOfCurrentBlock = (void*)((char*)ptr + sizeof(block) + ptr->size);

        if (endOfCurrentBlock == (void*)ptr->next)
        {
            printf("Merging blocks\n");
            ptr->size = ptr->size + ptr->next->size;
            ptr->next = ptr->next->next;
        }
        else
        {
            //printf("Blocks not contiguous, moving to next block\n");
            ptr = ptr->next;
        }
    }

    // Additional check for merging with the freeList node itself
    if (freeList != NULL && freeList->next != NULL && (char*)freeList + sizeof(block) + freeList->size == (char*)freeList->next)
    {
        printf("Merging blocks with freeList\n");
        freeList->size = freeList->size + freeList->next->size;
        freeList->next = freeList->next->next;
    }
}




void dealloc(void*dptr)
{
   if(dptr==NULL)
   {
      printf("INVALID POINTER!!");
   }
   else
   {
      block *ptr,*prev;
      block*dealloc_block=(block*)((char*)dptr);
      ptr=freeList;
      prev=NULL;
      while(prev!=NULL&&ptr<dealloc_block)
      {
        prev=ptr;
        ptr=ptr->next;
      }
      // Check if the deallocated block is found in the free list
 
      if(prev!=NULL)
      {
        //printf("here\n");
        prev->next=dealloc_block;
      }
      else
      {
        //printf("else here\n");
        freeList=dealloc_block;
      }    
      dealloc_block->next =ptr;
      
      mergeFree();
      printf("Memory deallocated successfully\n");
      
   }
              
}

void display()
{
     block*ptr=freeList;
     printf("The available free blocks are:\n");
     printf("ADDRESS\t\t\tSIZE\n");
     while(ptr!=NULL)
     {
           printf("%p\t\t%d\n\n",ptr,ptr->size);
           ptr=ptr->next;
     }
}

int main() {
    // Initialize the heap
    heap = init();

    // Example allocations
    int* ptr1 = (int*)alloc(100*sizeof(int)); // Allocate memory for an integer
    printf("\nptr1 allocation going on....\n");
    display();
    char* ptr2 = (char*)alloc(10 * sizeof(char)); // Allocate memory for 10 characters
    printf("\nptr2 allocation going on....\n");
    display();
    float* ptr3 = (float*)alloc(100*sizeof(float)); // Allocate memory for a float
    printf("\nptr3 allocation going on....\n");
    display();
    dealloc(ptr3);
    printf("\nptr3 deallocation going on....\n");
    display();
    int* ptr4 = (int*)alloc(250*sizeof(int)); // Allocate memory for an integer after deallocating ptr3
    printf("\nptr4 allocation going on....\n");
    display();
    dealloc(ptr2);
    printf("\nptr2 deallocation going on....\n");
    display();
    float* ptr5 = (float*)alloc(100*sizeof(float)); // Allocate memory for a float after deallocating ptr2
    printf("\nptr5 allocation going on....\n");
    display();
    free(heap);
    return 0;
}

