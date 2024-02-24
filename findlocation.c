#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>

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

int my_print(const char *s) {
    return my_write(1, s, my_strlen(s));
}


int main(int argc, char **argv) {
    char *filename, *word;
    int fd;
    off_t res_lseek;
    size_t file_size;
    void *ptr;

    if(argc < 2){/*Check for number of arguments*/
         my_print("running without arguments\n");
         return 1;
    }
    /* Make sure we have enough arguments */
    if (argc < 3) {
        fprintf(stderr, "Error: not enough arguments\n");
        return 1;
    }
    filename = argv[1];
    word = argv[2];

    /* Open the file for reading */
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        my_print("Error at opening the file\n");
        return 1;
    }


    res_lseek = lseek(fd,  0, SEEK_END);
    if (res_lseek <  0) {
        my_print("Error at using lseek\n");
        return 1;
    }

    file_size = (size_t) res_lseek;

    /* Handle the special case of a file that is empty */
    if (file_size ==0 ) {
        close(fd);
        return 0;
    }

    /* Map the content of the file described by fd into memory */
    ptr = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        my_print("Error at mapping the file to memory\n");
        close(fd);
        return 1;    
    }


    if (perform_lookup(ptr, file_size, word) < 0) {
        my_print("Error at looking up in the file\n");
        munmap(ptr,file-size);
        close(fd);
        return 1;
    }

    /* Unmap the memory */
    if (munmap(ptr, file_size) < 0) {
        my_print("Error at unmapping the file\n");
        close(fd);
        return 1;
    }

    /* Close the file descriptor */
    if (close(fd) < 0) {
        my_print("Error at closing the file\n");
        return 1;
    }

    /* Signal success */
    return 0;
}