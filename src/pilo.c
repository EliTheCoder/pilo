#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "pilo.h"

typedef enum {
    OP_PUSH,
    OP_POP,
    OP_ADD,
    OP_PUTS,
    OP_DUMP,
    OP_DUP
} Opcode;

struct State {
    int stack[0x100];
    int stack_pointer;
};

typedef struct {
    Opcode opcode;
    int operand;
} Instruction;

void remove_all_chars(char* str, char c) {
    char *pr = str, *pw = str;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != c);
    }
    *pw = '\0';
}

void interpret(struct State *state, Instruction *instruction) {
    Opcode op = instruction->opcode;
    int operand = instruction->operand;
    switch (op) {
        case OP_PUSH:
            state->stack[state->stack_pointer++] = operand;
            break;
        case OP_POP:
            if (state->stack_pointer == 0) {
                printf("Error: Stack underflow");
                exit(1);
            } else {
                state->stack_pointer--;
            }
            break;
        case OP_ADD:
            state->stack[state->stack_pointer - 2] += state->stack[state->stack_pointer - 1];
            state->stack_pointer--;
            break;
        case OP_PUTS:
            printf("%d\n", state->stack[state->stack_pointer - 1]);
            state->stack_pointer--;
            break;
        case OP_DUMP:
            for (int i = 0; i < state->stack_pointer; i++) {
                printf("%d ", state->stack[i]);
            }
            printf("\n");
            break;
        case OP_DUP:
            state->stack[state->stack_pointer] = state->stack[state->stack_pointer - 1];
            state->stack_pointer++;
            break;
        default:
            printf("Error: Unknown opcode");
            exit(1);
    }
}

void lex_and_interpret(struct State *state, char *code) {
    remove_all_chars(code, '\n');
    remove_all_chars(code, '\r');
    char *currentToken;
    currentToken = strtok(code, " ");
    Instruction instruction;
    while (currentToken != NULL) {
        remove_all_chars(currentToken, ' ');
        if (isdigit(currentToken[0])) {
            instruction.opcode = OP_PUSH;
            instruction.operand = atoi(currentToken);
        } else if (strcmp(currentToken, "+") == 0) {
            instruction.opcode = OP_ADD;
            instruction.operand = 0;
        } else if (strcmp(currentToken, "puts") == 0) {
            instruction.opcode = OP_PUTS;
            instruction.operand = 0;
        } else if (strcmp(currentToken, "dump") == 0) {
            instruction.opcode = OP_DUMP;
            instruction.operand = 0;
        } else if (strcmp(currentToken, "pop") == 0) {
            instruction.opcode = OP_POP;
            instruction.operand = 0;
        } else if (strcmp(currentToken, "dup") == 0) {
            instruction.opcode = OP_DUP;
            instruction.operand = 0;
        } else {
            printf("Error: Unknown token: %s\n", currentToken);
            exit(1);
        }
        interpret(state, &instruction);
        currentToken = strtok(NULL, " ");
    }
}

void shell(struct State *state) {
    int running = 1;
    char cmd[0x100]; 
    printf("Pilo v%s shell\nType \"exit\" to quit\n", VERSION);
    while (running) {
        printf(">> ");
        fgets(cmd, 0x100, stdin);
        if (strcmp(cmd, "exit\n") == 0) {
            running = 0;
        } else {
            lex_and_interpret(state, cmd);
        }
    }
}

void run(struct State *state, FILE *file) {
    char code[0x100];
    while (fgets(code, 0x100, file) != NULL) {
        lex_and_interpret(state, code);
    }
}

int main(int argc, char *argv[]) {

    struct State state = {0};

    if (argc < 2) {
        shell(&state);
    } else {
        run(&state, fopen(argv[1], "r"));
    }

    return 0;
}

