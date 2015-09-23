#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_LINE 40
#define ANIMATE 0

static int height = 0;

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
#if ANIMATE
    printf("\033[2J\033[1;1H");
    nanosleep((const struct timespec[]){{0, 1000L*1000L*1000L/10}}, NULL);
#endif
    int top_row = (MAX_LINE + height) * 2 + 1;
    while(top_row--) {
        if(output[top_row] != NULL) {
            puts(output[top_row]);
        }
    }
}

void draw_init(int floor, int column, char **output, int *row, int *small_column) {
    *row = floor * 2;
    *small_column = column * 4;
    allocate_row(*row, output);
    draw_trailing_space(*row, *small_column, output);
}

void draw_roof(int floor, int column, char **output, int shift);

void merge_roof(int floor, int column, char **output, int shift) {
    int row = floor * 2;
    int small_column = column * 4 + shift;

    memcpy(output[row] + small_column - 2, "/   \\", sizeof(char) * 5);
    memcpy(output[row - 1] + small_column - 1, "   ", sizeof(char) * 3);
    if(output[row - 2][small_column] == '/') {
        output[row - 2][small_column] = ' ';
    }

#if ANIMATE
                draw_house(output, height);
#endif
    draw_roof(floor + 1, column, output, shift-2);
}

void draw_roof(int floor, int column, char **output, int shift) {
    int row, small_column;
    draw_init(floor, column, output, &row, &small_column);
    small_column += shift;

    draw_trailing_space(row, small_column + 2, output);
    memcpy(output[row] + small_column + 2, "A", sizeof(char));
    draw_trailing_space(row - 1, small_column + 1, output);
    memcpy(output[row - 1] + small_column + 1, "/ \\", sizeof(char) * 3);
#if ANIMATE
                draw_house(output, height);
#endif
    if(small_column > 0 && output[row - 1][small_column - 1] == '\\') {
        merge_roof(floor, column, output, shift);
    }
}

void draw_box(int floor, int column, char **output) {
    int row, small_column;
    draw_init(floor, column, output, &row, &small_column);

    bool is_below = output[row][small_column + 1] == '-';
    bool has_left = small_column > 0 && output[row - 1][small_column] == '|';
    bool left_is_below = has_left && output[row][small_column] == '|';

    if(is_below) {
        memcpy(output[row] + small_column + 1, "   |", sizeof(char) * 4);
    } else {
        memcpy(output[row] + small_column + 1, "---+", sizeof(char) * 4);
    }
    memcpy(output[row - 1] + small_column + 1, "   |", sizeof(char) * 4);
    if(rand() % 2) {
            output[row - 1][small_column + 2] = 'o';
    }
    memcpy(output[row - 2] + small_column + 1, "---+", sizeof(char) * 4);

    if(has_left) {
        if(left_is_below && is_below) {
            output[row][small_column] = ' ';
        } else if(left_is_below || is_below){
            output[row][small_column] = '+';
        } else {
            output[row][small_column] = '-';
        }
        output[row - 1][small_column] = ' ';
        output[row - 2][small_column] = '-';
    } else {
        if(is_below) {
            output[row][small_column] = '|';
        } else {
            output[row][small_column] = '+';
        }
        output[row - 1][small_column] = '|';
        output[row - 2][small_column] = '+';
    }

    if(!is_below) {
        draw_roof(floor + 1, column, output, 0);
    }
}

int main() {
    char **output;
    char buffer[MAX_LINE];
    srand(time(NULL));

    height = atoi(get_line(buffer, MAX_LINE));

    output = calloc((MAX_LINE + height) * 2 + 1, sizeof(char*));

    printf("height: %d\n", height);

    int column = 0;
    for(int floor = height; floor > 0; floor--){
        char c;
        column = 0;
        while((c = getc(stdin)) != '\n' && c != EOF) {
            if(c == '*') {
                draw_box(floor, column, output);
#if ANIMATE
                draw_house(output, height);
#endif
            }
            column++;
        }
    }
    memcpy(output[1] + (rand() % column) * 4 + 1, "| |", sizeof(char) * 3);

    draw_house(output, height);
}