#include <stdio.h>

#include <unistd.h> 

struct block{

    int size;

    int pad;

    void*start;

    struct block* prev;

    struct block* next;

};

//basically linked list structure


//gets the current block and current position for the malloc function to accurately add at a certain point.
struct block* currentBlock = 0;

void* currentPosition;




struct block* custommalloc(char bytes){

    if(bytes <= 0){
        return 0;
    }

    printf("creating space for %d bytes\n", bytes);

    int pad = (8 - (bytes % 8) % 8);

    void* fullPointer = sbrk(bytes + pad + sizeof(struct block
    )); //update current position, add space for full

    currentPosition = fullPointer;

    struct block* b = (struct block*)((char*)currentPosition + bytes + 1);

    printf("current position = %p\n", currentPosition);

    printf("Block info at %p\n", b);

    b->start = fullPointer; //adds to tail basically, note that start is the end of the previous block rather than the start of current block

    printf("Block start at %p\n", b->start);

    if(currentBlock != 0){
        currentBlock->next = b;
    }


    b->size = bytes + sizeof(struct block) + pad;

    
    printf("Block contains %d bytes\n", b->size);

    b->prev = currentBlock;

    b->next = 0;

    b->pad = pad;

    currentBlock = b;

    return b;
}



//shifts a block x bytes in any direction (moves block pointer by amount x)

struct block* shiftBlock(struct block* block, int bytes){

        printf("Block contains %d bytes\n", block->size);

        printf("Shifting block %d bytes\n", bytes);

        printf("Block starts at %p\n", block->start);

        printf("Block ends at %p\n", ((char*)block->start + block->size));

        int size = block->size;

        int pad = block->pad;

        struct block* next = block->next;

        struct block* prev = block->prev;

        void* start = block->start;
    
    for(char*  i = (char*)start + 1; i <= (char*)(start + size); i++){

        char* j = i + bytes;
        // printf("changing pointer location %p\n", i);

        (*j) = *(char*)i;


        // printf("going to pointer location %p\n", j);

        printf("value at location %p is %d\n", j, *j);
    }

    block->start = (void*)((char*)block->start + bytes);

    block->size = size;

    block->prev = prev;

    block->next = next;

    block->pad = pad;

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

    int size = b->size;

    b->size = 0;
    b->prev = 0;
    b->next = 0;
    b->start = 0;
    b->pad = 0;

    while (a != 0)
    {
        struct block* next = a->next;
       shiftBlock(a, -(size));
       a = next;
    }

    currentPosition = (void*)((char*)currentPosition - size);

    printf("Current position is %p\n", currentPosition);
}


//literally malloc, copy and then free the exact same block.

struct block* customrealloc(struct block* b, char newBytes){
    if(newBytes <= 0){
        customfree(b);
        return 0;
    }

    int oldbytes = b->size - sizeof(struct block) - b->pad;

    printf("The bytes of the old structure is %d\n", oldbytes);

    struct block* newBlock = custommalloc(newBytes);

    int i = 0;

    while(i < oldbytes && i < newBytes){

        printf("Character to copy is %d\n",*((char*)b->start + i + 1) );
        *((char*)newBlock->start + i + 1) = *((char*)b->start + i + 1);

        printf("Character at %p is %d\n", newBlock->start + i + 1, *((char*)newBlock->start + i + 1) );
        i++;
    }
    

    customfree(b);

    return newBlock;
}


int main(){

    struct block* first = custommalloc(5);

    struct block* second = custommalloc(7);

    char*s = (char*)second->start;

    printf("start address is %p\n", s);

    int x = 1;

    for(char*i = s + 1; i < s + second->size - sizeof(struct block); i++){
        *i = x;
        x++;

        printf("Value at location %p is %d\n", i, *i);
    }

    customfree(first);

    struct block* third = custommalloc(9);

    second = customrealloc(second, 10);
}
