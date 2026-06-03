typedef struct{

    char size;

    void*start;

    block* prev;

    block* next;

} block;

//basically linked list structure


//gets the current block and current position for the malloc function to accurately add at a certain point.
block* currentBlock = 0;

void* currentPosition = 0;



block* custommalloc(char bytes){

    currentPosition = sbrk(bytes); //update current position

    block* b;

    b->start = currentPosition; //adds to tail basically, note that start is the end of the previous block rather than the start of current block

    if(currentBlock != 0){
        currentBlock->next = b;
    }

    b->size = bytes;

    b->prev = currentBlock;

    b->next = 0;

    currentBlock = b;

    return b;
}



//shifts a block x bytes in any direction (moves block pointer by amount x)

block* shiftBlock(block* block, char bytes){
    
    for(void*  i = block->start + block->size; i > block->start; i--){
        char* j = i + bytes;

        (*j) = *(char*)i;
    }

    block->start -= bytes;

    return block;
}

//shifts all blocks to the right back by the freed block's size essentially overwriting the data.

void customfree(block* b){

    if(b->next != 0){
        b->next->prev = b->prev;
    }
    if(b->prev != 0){
        b->prev->next = b->next;
    }

    block* a = b->next;

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

block* customrealloc(block* b, char newBytes){
    if(newBytes <= 0){
        customfree(b);
        return 0;
    }

    block* newBlock = custommalloc(newBytes);

    for(char i = newBytes; i > 0; i--){
        *((char*) b->start + newBytes) = *((char*) newBlock->start + newBytes);
    }
    customfree(b);

    return newBlock;
}


