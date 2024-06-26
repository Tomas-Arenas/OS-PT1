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


//map file to memory function
static void *map_file(const char *filename, size_t *size) {
    int fd;
    off_t res_lseek;
    void *ptr;

    /* Open the file for reading */
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return NULL;
    }

    /* Get the size of the file */
    res_lseek = lseek(fd, 0, SEEK_END);
    if (res_lseek == -1) {
        perror("Error seeking file");
        if (close(fd) == -1) {
            perror("Error closing file");
        }
        return NULL;
    }
    *size = res_lseek;

    /* Map the file into memory */
    ptr = mmap(NULL, *size, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("Error mapping file");
        if (close(fd) == -1) {
            perror("Error closing file");
        }
        return NULL;
    }

    /* Close the file */
    if (close(fd) == -1) {
        perror("Error closing file");
        return NULL;
    }
    
    return ptr;
}

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
    size_t low = 0;
    size_t high = n - 1;
    size_t mid;
    while (low <= high) {
        mid = (low + high) / 2;
        if (strncmp(dict[mid].prefix, my_atoi(word), 6) == 0) {
            my_write(1, "The location of ",  my_strlen("The location of "));
            my_write(1, dict[mid].prefix, 6);
            my_write(1, " is ", my_strlen(" is "));
            my_write(1, dict[mid].location, 25);
            my_write(1, "\n", my_strlen("\n"));
            return 0;
        } else if (strncmp(dict[mid].prefix, my_atoi(word), 6) < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    printf("The word %s does not figure in the dictionary\n", word);
    return 0;
}

static int binary_search(void *ptr, size_t size, char *word){
   
    if (size % sizeof(nanpa_entry) != 0) {
        fprintf(stderr, "Size is not a multiple of nanpa_entry\n");
        return -1;
    }
    size_t n = size / sizeof(nanpa_entry);
    return binary_search_helper((nanpa_entry *) ptr, n, word);
}

int main (int argc, char **argv) {
    size_t file_size;
    char *filename, *word;
    void *ptr;

    switch(argc) {
        case 1: {
            my_write(STDERR_FILENO, "\nThis is not a well-formed call to findlocation", 
            my_strlen("\nThis is not a well-formed call to findlocation.\n"));
            return 1;
        }
        case 2: {
            ptr = map_file("nanpa.txt", &file_size);
            binary_search(ptr, file_size, argv[1]);
            if (munmap(ptr, file_size) == -1) {
                perror("munmap");
                return 1;
            }
            break;
            
        }
        case 3:{
            filename = argv[1];
            word = argv[2];
            ptr = map_file(filename, &file_size);
            if (ptr == NULL) {
                return 1;
            }
            binary_search(ptr, file_size, word);
            if (munmap(ptr, file_size) == -1) {
                perror("munmap");
                return 1;
            }
            break;
        }
            
    }
    return 0;
}