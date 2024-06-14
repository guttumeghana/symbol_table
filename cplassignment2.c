//CPL ASSIGNMENT-2
//With displaying the no.of bytes are free or allocated

#include <stdio.h>
#include <stddef.h>//used because of size_t

char memory[20000];//used as memory pool for dynamic allocation

// The structure definition to contain metadata of each block allocated or deallocated
struct meta_block
{
    size_t size;             //the size of the block described by it and size_t is generally used in meory related operations
    int free;                //if free, it is set to 1. Otherwise 0     
    struct meta_block *next; //Pointer to the next metadata block
};

// Pointing to the main block of memory which is initially free (no storage allocation yet)
struct meta_block *freeList = (void *)memory;//initially entire memory is one free block

// function prototypes
void initialize();
void split(struct meta_block *fitting_slot, size_t size);
void *My_Alloc(size_t noOfBytes);
void merge();
void My_Free(void *ptr);

// Initializing the block of memory
void initialize()
{
    freeList->size = 20000 - sizeof(struct meta_block);
    freeList->free = 1;
    freeList->next = NULL;
}

//Making way for a new block allocation by splitting a free block -- (Assume first fit algorithm)
void split(struct meta_block *fitting_slot, size_t size)
{
    struct meta_block *new = (void *)(((void *)fitting_slot) + size + sizeof(struct meta_block));
    new->size = (fitting_slot->size) - size - sizeof(struct meta_block);
    new->free = 1;
    new->next = fitting_slot->next;
    fitting_slot->size = size;
    fitting_slot->free = 0;
    fitting_slot->next = new;
}

//My_Alloc function(allocation)
void *My_Alloc(size_t noOfBytes)
{
    struct meta_block *curr, *prev; //We need metadata block pointers to traverse through the freeList.
    void *result; //This result pointer is to return the starting address of the allocated chunk of memory.
    if (!(freeList->size))//checks if size is zero
    {
        initialize();//if free list is empty then it initialies the memory.
        printf("Memory initialized\n");
    }
    curr = freeList; //Now we make the temporary pointer curr to point to the start of the metadata block list.
    while (((curr->size < noOfBytes) || (curr->free == 0)) && (curr->next != NULL))
    {
        prev = curr;
        curr = curr->next;
        printf("One meta_block checked\n");
    }
    if ((curr->size) == noOfBytes)
    {
        curr->free = 0;
        result = (void *)(++curr);
        printf("Exact fitting meta_block allocated\n");
        return result;
    }
    else if (curr->size > (noOfBytes + sizeof(struct meta_block)))
    {
        split(curr, noOfBytes);
        result = (void *)(++curr);
        printf("Fitting meta_block allocated with a split\n");
        return result;
    }
    //Else it means that there is no sufficient memory to allocate so you should return a NULL pointer.
    else
    {
        result = NULL;
        printf("Sorry. No sufficient memory to allocate\n");
        return result;
    }
}

//This is to merge the consecutive free blocks by removing the metadata block in the middle. This will save space.
void merge()
{
    struct meta_block *curr, *prev;//both pointers are pointing to free list
    curr = freeList;//curr iterates through the free list and prev keeps track of prev block
    while ((curr->next) != NULL)
    {
        if ((curr->free) && (curr->next->free))
        {
            curr->size += (curr->next->size) + sizeof(struct meta_block);
            curr->next = curr->next->next;
        }
        prev = curr;
        curr = curr->next;
    }
}

void My_Free(void *ptr)
{
    if (((void *)memory <= ptr) && (ptr <= (void *)(memory + 20000)))
    {
        struct meta_block *curr = ptr;
        --curr;
        curr->free = 1;
        merge();
    }
    else
        printf("Please provide a valid pointer allocated by My_Alloc\n");
        //If a valid pointer is not provided, the above message will be printed.
}

void printBlocks()
{
    struct meta_block *curr;
    curr=freeList;
    int count=0;
    while(curr)
    {
        printf("Block %d : %lu bytes,%s\n" , ++count, curr->size ,curr->free ?"Free": "Allocated");
        curr=curr->next;
    }
}

int main()
{
    int *p = (int *)My_Alloc(150 * sizeof(int));
    char *q = (char *)My_Alloc(200 * sizeof(char));
    int *r = (int *)My_Alloc(1000 * sizeof(int));
    My_Free(p);
    char *w = (char *)My_Alloc(700);
    My_Free(r);
    int *k = (int *)My_Alloc(500 * sizeof(int));
    printf("Allocation and deallocation is done successfully!\n");
    printBlocks();
    return 0;
}