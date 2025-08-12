#ifndef OPCODE_H
#define OPCODE_H

struct opcode {
    char instruction[20];
    char* operands[5];
};

#endif