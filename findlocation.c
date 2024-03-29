#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

typedef struct {
  char prefix[6];    // 6 digits
  char location[25]; // 25 characters
  char newline;      // '\n'
} nanpa_entry;

static int my_atoi(const char *s) {
    int result = 0;
    int counter = 0;
    while (s[counter] != '\0') {
        if (s[counter] < '0' || s[counter] > '9') {
            return -1;
        }
        result = result * 10 + s[counter] - '0';
        counter++;
    }
    return result;
}

static int my_write(int fd, const void *buf, size_t count) {
    size_t bytes_written = 0;
    size_t bytes_remaining = count;
    const char *buf_addr = buf;

    while (bytes_remaining > 0) {
        bytes_written = write(fd, buf_addr, bytes_remaining);
        if (bytes_written == -1) {
            perror("Error writing to file");
            return -1;
        }
        bytes_remaining -= bytes_written;
        buf_addr += bytes_written;
    }
    return 0;
}

static int my_strlen(const char *s) {
    int len = 0;
    while (*s != '\0') {
        s++;
        len++;
    }
    return len;
}

static int binary_search_helper(nanpa_entry *dict, size_t n, char *word)
{
    int low = 0;
    int high = n - 1;
    int mid;
    while (low <= high) {
        mid = (low + high) / 2;
        if (strcmp(dict[mid].prefix, word) == 0) {
            my_write(1, "The location of ",  my_strlen("The location of "));
            my_write(1, dict[mid].prefix, 6);
            my_write(1, " is ", my_strlen(" is "));
            my_write(1, dict[mid].location, 25);
            my_write(1, "\n", my_strlen("\n"));
            return 0;
        } else if (strcmp(dict[mid].prefix, word) < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -2;
}

static int binary_search(void *ptr, size_t size, char *word){
    size_t n = size / sizeof(nanpa_entry);
    if (size % sizeof(nanpa_entry) != 0) {
        fprintf(stderr, "Size is not a multiple of nanpa_entry\n");
        return -1;
    }
    return binary_search_helper((nanpa_entry *) ptr, n, word);
}

int main (int argc, char **argv) {
    int fd;
    switch(argc) {
        case 1: {
            my_write(STDERR_FILENO, "\nThis is not a well-formed call to findlocation", 
            my_strlen("\nThis is not a well-formed call to findlocation.\n"));
            return 1;
        }
        case 2: {
            fd = open("nanpa.txt", O_RDONLY);
            if (fd == -1) {
                my_write(STDERR_FILENO, "Error: file not found\n", my_strlen("Error: file not found\n"));
                return 1;
            }
            struct stat sb;
            if (fstat(fd, &sb) == -1) {
                perror("fstat");
                return 1;
            }
            void *ptr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
            if (ptr == MAP_FAILED) {
                perror("mmap");
                return 1;
            }
            if (binary_search(ptr, sb.st_size, argv[1]) == -2) {
                my_write(STDERR_FILENO, "Error: prefix not found\n", my_strlen("Error: prefix not found\n"));
                return 1;
            }
            if (munmap(ptr, sb.st_size) == -1) {
                perror("munmap");
                return 1;
            }
            close(fd);
            break;
        }
        case 3:{
            fd = open(argv[2], O_RDONLY);
            if (fd == -1) {
                my_write(STDERR_FILENO, "Error: file not found\n", my_strlen("Error: file not found\n"));
                return 1;
            }
            struct stat sb;
            if (fstat(fd, &sb) == -1) {
                perror("fstat");
                return 1;
            }
            void *ptr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
            if (ptr == MAP_FAILED) {
                perror("mmap");
                return 1;
            }
            if (binary_search(ptr, sb.st_size, argv[1]) == -2) {
                my_write(STDERR_FILENO, "Error: prefix not found\n", my_strlen("Error: prefix not found\n"));
                return 1;
            }
            if (munmap(ptr, sb.st_size) == -1) {
                perror("munmap");
                return 1;
            }
            close(fd);
            break;
        }
    }
}