#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

static int binary_search(void *ptr, size_t size, char *word);
static int binary_search_helper(nanpa_entry *dict, size_t n, char *word);


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

//my_write function
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


static int binary_search(void *ptr, size_t size, char *word){
    size_t n = size / sizeof(nanpa_entry);
    if (size % sizeof(nanpa_entry) != 0) {
        fprintf(stderr, "Size is not a multiple of nanpa_entry\n");
        return -1;
    }
    return binary_search_helper((nanpa_entry *) ptr, n, word);

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
    return -1;
}




int main (int argc, char **argv) {
    int fd;
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <prefix>\n", argv[0]);
        return 1;
    }
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    off_t size = lseek(fd, 0, SEEK_END);
    if (size == -1) {
        perror("lseek");
        return 1;
    }
    lseek(fd, 0, SEEK_SET);
    void *ptr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    int result = binary_search(ptr, size, argv[2]);
    if (result == -1) {
        fprintf(stderr, "Prefix not found\n");
    }
    munmap(ptr, size);
    close(fd);
    return 0;
}