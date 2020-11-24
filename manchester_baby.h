#include <stdint.h>

#define STOP 0;
#define CONTINUE 1;
#define LINE_HIGHEST_BIT (1 << 31)

typedef struct Baby {
    uint32_t store[32];
    uint32_t accumulator;
    uint32_t current_instruction;
    uint32_t present_instruction;
} Baby;

void fetch(Baby *baby);
void decode(Baby *baby, int *opcode, int *line);
int execute(Baby *baby, unsigned int opcode, unsigned int line);
void print(Baby *baby);
uint32_t negate(uint32_t input);
