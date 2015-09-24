#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <alloca.h>
#include <sys/mman.h>
#include <unistd.h>

#define ANIMATE 1

char *copy_input(char *name, off_t *in_size) {
    FILE *in = fopen(name, "rb");
    if(in != NULL) {
        fseek(in, 0L, SEEK_END);
        *in_size = ftello(in);
        rewind(in);
        char *buff = mmap(NULL, *in_size, PROT_READ, MAP_PRIVATE, fileno(in), 0);
        fclose(in);
        return buff;
    } else {
        fputs("Error opening file\n", stderr);
        return NULL;
    }
}

void scan(char *buff, int *width, int *height) {
    char c;
    *width = 0;
    *height = 1;
    int i = 0;

    while((c = *(buff++))) {
        if(c == '\n') {
            (*height)++;
            if(i > *width) {
                *width = i;
            }
            i = 0;
        } else {
            i++;
        }
    }
    if(i > *width) {
        *width = i;
    }
}

void copy(char *buff, char *dest, int line_width) {
    int i = 0;
    while(*buff && (*(dest) = *(buff++))) {
        if(*dest == '\n') {
            *dest = ' ';
            dest += line_width - i + 1;
            i = 0;
        } else {
            i++;
            dest++;
        }
    }
}

char *get(char *buff, int row, int col, int width) {
    return buff + row * (width + 1) + col;
}

void insert_0(char *dest, int width, int height) {
    for(unsigned i = 0; i < height; i++) {
        *get(dest, i, width, width) = '\0';
    }
}

void init_board(char **current, char **next, int *width, int *height, char *filename) {
    off_t in_size = 0;
    char *buff = copy_input(filename, &in_size);

    scan(buff, width, height);

    int size = *height * (*width + 1);
    *current = malloc(size);
    *next = malloc(size);
    memset(*current, ' ', size);

    copy(buff, *current, *width);
    munmap(buff, in_size);
    insert_0(*current, *width, *height);
    insert_0(*next, *width, *height);
}

void print(char *buff, int width, int height) {
    char *it = buff;
    while(it < buff + height * width + height) {
        puts(it);
        it += width + 1;
    }
}

void get_neighbours(char *neighbours, char *current, int row, int col, int width, int height) {
    char  c;
    if(row > 0) {
        if(col > 0 && (c = *get(current, row - 1, col - 1, width)) != ' ') {
            *(neighbours++) = c;
        }
        if((c = *get(current, row - 1, col, width)) != ' ') {
            *(neighbours++) = c;
        }
        if(col < width - 1 && (c = *get(current, row - 1, col + 1, width)) != ' ') {
            *(neighbours++) = c;
        }
    }
    if(col > 0 && (c = *get(current, row, col - 1, width)) != ' ') {
        *(neighbours++) = c;
    }
    if(col < width - 1 && (c = *get(current, row, col + 1, width)) != ' ') {
        *(neighbours++) = c;
    }
    if(row < height - 1) {
        if(col > 0 && (c = *get(current, row + 1, col - 1, width)) != ' ') {
            *(neighbours++) = c;
        }
        if((c = *get(current, row + 1, col, width)) != ' ') {
            *(neighbours++) = c;
        }
        if(col < width - 1 && (c = *get(current, row + 1, col + 1, width)) != ' ') {
            *(neighbours++) = c;
        }
    }
}

char get_next(char *current, int row, int col, int width, int height) {
    char *neighbours = alloca(9);
    memset(neighbours, '\0', 9);
    get_neighbours(neighbours, current, row, col, width, height);
//    printf("(%d, %d): ", row, col);
//    puts(neighbours);

    size_t n = strlen(neighbours);

    char cell = *get(current, row , col, width);

    if(n < 2 || n > 3) {
        return ' ';
    } else if(n == 3 && cell == ' ') {
        return neighbours[rand() % 3];
    } else {
        return cell;
    }
}

void live(char *current, char *next, int width, int height) {
    for(unsigned row = 0; row < height; row++) {
        for(unsigned col = 0; col < width; col++) {
            *get(next, row, col, width) = get_next(current, row, col, width, height);
        }
    }
}

int main(int argc, char **argv) {
    char *current, *next;
    int width, height;
    srand(time(NULL));

    if(argc < 2) {
        fputs("Please provide input file as argument.\n", stderr);
        return 1;
    }
    init_board(&current, &next, &width, &height, argv[1]);

//    print(current, width, height);
    live(current, next, width, height);
    print(next, width, height);
#if ANIMATE
    while(true) {
        char *tmp = next;
        next = current;
        current = tmp;

        usleep(100 * 1000);
        live(current, next, width, height);
        printf("\033[2J\033[1;1H");
        print(next, width, height);
    }
#endif
}