#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include "manchester_baby.h"

//#define DEBUG

/*
    ON ENDIANNESS:

    The Manchester Baby is big-endian at a bit level. That is to say, of its 32-bit lines, the last bit is the most significant.
    Now, endianness in the modern world normally refers to byte-level endianness. That is to say, in a 2-byte integer, is the
    most significant bit in the first or in the second byte?

    "bit-level endianness" is more often referred to as "bit ordering". Now, modern processors don't have ways to address individual bits, so these
    are entirely irrelevant to most software (They sometimes come into play in network protocols and extremely low level things). So much so that
    C does not actually have anyway at all to say "I want the bits of my number to be ordered with the LSbit first". 

    We _could_ pretend we can, but we'd have to invert the bit order again to perform arithmetics or display output. That seems horribly wasteful.

    Because of that, I decided to write the code below ignoring bit ordering. C provides a nice model where, when using unsigned integers, you can pretend you're working in
    LSbyte first, LSbit first and C will take care of the details for you.

    Of course, we'll need to handle bit ordering _somewhere_. I decided to handle it in input/output: when reading from input, the first bit of a line will actually be stored in the
    last bit of the line, and when printing output, the first bit of a line will be displayed last.
*/
static void print_line(uint32_t line);

void fetch(Baby *baby) {
    baby->current_instruction += 1;
    baby->present_instruction = baby->store[baby->current_instruction];

    #ifdef DEBUG
        printf("fetch: current instruction after increase: %d. present instruction: %d\n", baby->current_instruction, baby->present_instruction);
    #endif

}

void decode(Baby *baby, int *opcode, int *line) {
    *line = baby->present_instruction & 63; //63 = 0b11111
    *opcode = (baby->present_instruction >> 13) & 7; //Move bits so that bit 15 is the last one, then drop all bits which aren't the last 3 (7 = 0b111)

    #ifdef DEBUG
        printf("decode: decoding ");
        print_line(baby->present_instruction);
        printf("\n");
        printf("decode: decoded opcode: %d, line: %d\n", *opcode, *line);
    #endif
}

int execute(Baby *baby, unsigned int opcode, unsigned int line) {
    //assert(condition && errorMessage) simply crashes with the error message if the condition is false. It's a nice debugging tool
    assert(opcode < 8 && "Opcode out of range");
    assert(line < LINE_COUNT && "Line out of range");
    
    #ifdef DEBUG
        printf("Executing opcode %d on line %d\n", opcode, line);
    #endif

    switch(opcode) {
        case 0: //JMP
            baby->current_instruction = baby->store[line];
            break;
        case 1: //JRP
            baby->current_instruction -= baby->store[line];
            break;
        case 2: //LDN
            baby->accumulator = negate(baby->store[line]);
            break;
        case 3: //STO
            baby->store[line] = baby->accumulator;
            break;
        case 4: //SUB
        case 5: //SUB by another name
            baby->accumulator -= baby->store[line];
            break;
        case 6: //CMP;
            if(baby->accumulator >> 31 == 1) baby->current_instruction++; //In two's complement, the first bit is the sign bit, and it's 1 for negative numbers
            break;
        case 7: //STP
            return STOP;
    }

    return CONTINUE;
}

void print(Baby *baby) {
    printf("store:\n");
    for(int i = 0; i < LINE_COUNT; ++i) {
        if(i < 10) printf("%d:  ", i); //Two spaces
        else printf("%d: ", i); //One space  
        print_line(baby->store[i]);
        printf("\n");
    }

    printf("accumulator: ");
    print_line(baby->accumulator);
    printf("\n");

    printf("current instruction: ");
    print_line(baby->current_instruction);
    printf("\n");
}

//A static function is only visible to the file it's defined in.
static void print_line(uint32_t line) {
    for(int i = 0; i < 32; ++i) {
        printf("%d", (line & 1 << i) >> i);
    }
}


//Two's complement negation
//When input is 0, ~input is the bit pattern of all 1s, which overflows when 1 is added to it. Since we're using unsigned integers, overflow is defined to wrap around, so the result is the bit pattern of all 0s.
//In other words, negate(0) = 0, which is nice because it means we don't need to special case it.
uint32_t negate(uint32_t input) {
    return ~input + 1;
}
