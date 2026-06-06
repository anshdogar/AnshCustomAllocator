#include <stdio.h>

#include <unistd.h> 

typedef struct block{

    char size;

    void*start;

    struct block* prev;

    struct block* next;

} ;

//basically linked list structure


//gets the current block and current position for the malloc function to accurately add at a certain point.
struct block* currentBlock = 0;

void* currentPosition;




struct block* custommalloc(char bytes){

    if(bytes <= 0){
        return 0;
    }

    printf("creating space for %d bytes\n", bytes);

    void* fullPointer = sbrk(bytes + sizeof(struct block
    )); //update current position, add space for full

    currentPosition = fullPointer;

    struct block* b = (struct block*)((char*)currentPosition + bytes);

    printf("current position = %p\n", currentPosition);

    printf("Block info at %p\n", b);

    b->start = fullPointer; //adds to tail basically, note that start is the end of the previous block rather than the start of current block

    printf("Block start at %p\n", b->start);

    if(currentBlock != 0){
        currentBlock->next = b;
    }


    b->size = bytes + sizeof(struct block);

    
    printf("Block contains %d bytes\n", b->size);

    b->prev = currentBlock;

    b->next = 0;

    currentBlock = b;

    return b;
}



//shifts a block x bytes in any direction (moves block pointer by amount x)

struct block* shiftBlock(struct block* block, int bytes){

        printf("Block contains %d bytes\n", block->size);

        printf("Shifting block %d bytes\n", bytes);
    
    for(void*  i = block->start + block->size; i > block->start; i--){
        char* j = i + bytes;

        (*j) = *(char*)i;
    }

    block->start = (void*)((char*)block->start - bytes);

    printf("New block start is at %p\n", block->start);

    return block;
}

//shifts all blocks to the right back by the freed block's size essentially overwriting the data.

void customfree(struct block* b){

      printf("Freeing block starts at %p\n", b->start);

    if(b->next != 0){
        struct block* a = b->prev;
        b->next->prev = a;
    }
    if(b->prev != 0){
        struct block* a = b->next;
        b->prev->next = a;
    }

    struct block* a = b->next;

    while (a != 0)
    {
       shiftBlock(a, -(b->size));
       a = a->next;
    }

    currentPosition -= b->size;

    b->size = 0;
    b->prev = 0;
    b->next = 0;
    b->start = 0;
}


//literally malloc, copy and then free the exact same block.

struct block* customrealloc(struct block* b, char newBytes){
    if(newBytes <= 0){
        customfree(b);
        return 0;
    }

    struct block* newBlock = custommalloc(newBytes);

    for(char i = newBytes; i > 0; i--){
        *((char*) b->start + newBytes) = *((char*) newBlock->start + newBytes);
    }
    customfree(b);

    return newBlock;
}


int main(){

    struct block* first = custommalloc(5);

    struct block* second = custommalloc(7);

    customfree(first);

    struct block* third = custommalloc(9);
}
