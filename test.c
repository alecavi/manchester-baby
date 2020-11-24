#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "manchester_baby.h"

int main() {
    Baby baby;
    memset(baby.store, 0, sizeof(baby.store));
    baby.accumulator = 0;
    baby.current_instruction = 0;
    baby.present_instruction = 0;

    FILE *program = fopen("program.txt", "r");
    if(program == NULL) {
        fprintf(stderr, "ERROR: Program file not found\n");
        return 1;
    }

    for(int i = 0; i < 32; ++i) {
        char buffer[40] = {'\0'};
        char *err = fgets(buffer, 40, program);
        if(err == NULL) break;
        
        uint32_t line = 0;
        for(int j = 0; j < 32; ++j) {
            switch(buffer[j]) {
                case '0': 
                    //Do nothing
                    break;
                case '1':
                    line |= 1 << j;
                    break;
                case '\0':
                    fprintf(stderr, "ERROR: NUL character in program file found too early (line %d, column %d). Each line must have 32 bits\n", i, j);
                    return 1;
                default:
                    fprintf(stderr, "ERROR: invalid character in program file: \'%c\' (ASCII %d) at line %d, column %d\n", buffer[j], (int) buffer[j], i, j);
                    return 1;
            }
        }
        baby.store[i] = line;
    }
    fclose(program);

    print(&baby);
    printf("\n");

    int running = 1;
    while(running) {
        fetch(&baby);
        int opcode, line;
        decode(&baby, &opcode, &line);
        running = execute(&baby, opcode, line);
        print(&baby);
        printf("\n");
        getchar();
    }

    return 0;
}
