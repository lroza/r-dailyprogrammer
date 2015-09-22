#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE 40

char* get_line(char *buffer, int count) {
    size_t l;
    if(fgets(buffer, count, stdin) == NULL || buffer[l = strlen(buffer) - 1 ] != '\n') {
        fputs("Error reading line", stderr);
        return NULL;
    } else {
        buffer[l] = '\0';
        return buffer;
    }
}

void allocate_row(int row, char **output) {
    for(int i = 0; i < 3; i++) {
        if(output[row - i] == NULL) {
            output[row - i] = calloc(MAX_LINE * 4 + 1, sizeof(char));
        }
    }
}

void draw_trailing_space(int row, int column, char **output) {
    for(int i = 0; i < 3; i++) {
        int c = column;
        while(c--) {
            if(output[row - i][c] == '\0') {
                output[row - i][c] = ' ';
            }
        }
    }
}

void draw_house(char **output, int height) {
    int top_row = (MAX_LINE + height) * 2 + 1;
    while(top_row--) {
        if(output[top_row] != NULL) {
            puts(output[top_row]);
        }
    }
}

void draw_box(int floor, int column, char **output) {
    int row = floor * 2;
    int small_column = column * 4;
    allocate_row(row, output);
    draw_trailing_space(row, small_column, output);

    bool is_below = output[row][small_column + 1] == '-';
    bool has_left = small_column > 0 && output[row - 1][small_column] == '|';
    bool left_is_below = has_left && output[row][small_column] == '|';

    if(is_below) {
        if(left_is_below){
            memcpy(output[row] + small_column, "    |", sizeof(char) * 5);
        } else if (has_left) {
            memcpy(output[row] + small_column, "+   |", sizeof(char) * 5);
        } else {
            memcpy(output[row] + small_column, "|   |", sizeof(char) * 5);
        }
    } else if (has_left && !left_is_below) {
        memcpy(output[row] + small_column, "----+", sizeof(char) * 5);
    } else {
        memcpy(output[row] + small_column, "+---+", sizeof(char) * 5);
    }
    if(has_left) {
        memcpy(output[row - 1] + small_column, "    |", sizeof(char) * 5);
        memcpy(output[row - 2] + small_column, "----+", sizeof(char) * 5);
    } else {
        memcpy(output[row - 1] + small_column, "|   |", sizeof(char) * 5);
        memcpy(output[row - 2] + small_column, "+---+", sizeof(char) * 5);
    }
}

int main() {
    char **output;
    char buffer[MAX_LINE];
    int height;

    height = atoi(get_line(buffer, MAX_LINE));

    output = calloc((MAX_LINE + height) * 2 + 1, sizeof(char*));

    printf("height: %d\n", height);

    for(int floor = height; floor > 0; floor--){
        char c;
        int column = 0;
        while((c = getc(stdin)) != '\n' && c != EOF) {
            if(c == '*') {
                draw_box(floor, column, output);
                puts("[DEBUG]");
                draw_house(output, height);
            }
            column++;
        }
    }

    draw_house(output, height);
}