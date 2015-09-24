#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

#define ANIMATE 1

char *copy_input(char *name, off_t *in_size) {
    FILE *in = fopen(name, "rb");
    if(in == NULL) {
        perror("Error opening file");
        exit(1);
    }
    fseek(in, 0L, SEEK_END);
    *in_size = ftello(in);
    rewind(in);
    char *buff = mmap(NULL, *in_size, PROT_READ, MAP_PRIVATE, fileno(in), 0);
    if(buff == NULL) {
        perror("Error mapping file");
        exit(1);
    }
    fclose(in);
    return buff;
}

void scan(char *buff, unsigned *width, unsigned *height) {
    *width = 0;
    *height = 1;
    char *ptr;

    while((ptr = strchr(buff, '\n'))) {
        (*height)++;
        unsigned tmp = ptr - buff;
        *width = tmp > *width ? tmp : *width;
        buff = ptr + 1;
    }
    size_t tmp = strlen(buff);
    *width = tmp > *width ? tmp : *width;
}

void copy(char *buff, char *dest, unsigned line_width) {
    unsigned i = 0;
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

char *get(char *buff, unsigned row, unsigned col, unsigned width) {
    return buff + row * (width + 1) + col;
}

void insert_0(char *dest, unsigned width, unsigned height) {
    for(unsigned i = 0; i < height; i++) {
        *get(dest, i, width, width) = '\0';
    }
}

void *null_check_malloc(size_t size) {
    void *tmp = malloc(size);
    if(tmp == NULL) {
        perror("Error alocating memmory");
        exit(1);
    } else {
        return tmp;
    }
}

void init_board(char **current, char **next, unsigned *width, unsigned *height, char *filename) {
    off_t in_size = 0;
    char *buff = copy_input(filename, &in_size);

    scan(buff, width, height);

    size_t size = *height * (*width + 1);
    *current = null_check_malloc(size);
    *next = null_check_malloc(size);
    memset(*current, ' ', size);

    copy(buff, *current, *width);
    munmap(buff, in_size);
    insert_0(*current, *width, *height);
    insert_0(*next, *width, *height);
}

void print(char *buff, unsigned width, unsigned height) {
    char *it = buff;
    while(it < buff + height * width + height) {
        puts(it);
        it += width + 1;
    }
}

void get_neighbours(char *neighbours, char *current, unsigned row, unsigned col, unsigned width, unsigned height) {
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

char get_next(char *current, unsigned row, unsigned col, unsigned width, unsigned height) {
    char neighbours[9] = "\0\0\0\0\0\0\0\0\0";
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

void live(char *current, char *next, unsigned width, unsigned height) {
    for(unsigned row = 0; row < height; row++) {
        for(unsigned col = 0; col < width; col++) {
            *get(next, row, col, width) = get_next(current, row, col, width, height);
        }
    }
}

int main(int argc, char **argv) {
    char *current, *next;
    unsigned width, height;
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