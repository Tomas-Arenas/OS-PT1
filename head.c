#include <stdio.h>
#include <unistd.h>


static int my_write(int fd, const void *buf, size_t count) {
    size_t bytes_written = 0;
    size_t bytes_remaining = count;

    const char *buf_addr = buf;

    while (bytes_remaining > 0) {
        bytes_written = write(fd, buf_addr, bytes_remaining);
        if (bytes_written == -1) {
            printf("Error writing to file\n"); 
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

int main (int argc, char **argv) {
   
    /* create diff cases for num of arg
    might wanna change this to a switch statement
    with all the different cases
    */
    printf("argc: %d\n", argc);
    if (argc == 1){
        /*copy first 10 lines of standard input*/
        char buffer[100];
        int i = 0;
        while (i < 10){
            int n = read(0, buffer, 100);
            if (n == 0){
                break;
            }
            my_write(1, buffer, n);
            i++;
        }
    } 
    return 0;
}

