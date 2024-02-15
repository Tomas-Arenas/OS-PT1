#include <stdio.h>
#include <unistd.h>


static int my_write(int fd, const void *buf, size_t count) {
    size_t bytes_written = 0;
    size_t bytes_remaining = count;

    const char *buf_addr = buf;

    while (bytes_remaining > 0) {
        bytes_written = write(fd, buf_addr, bytes_remaining);
        if (bytes_written == -1) {
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
    my_print("Hello, World!\n");
    return 0;
}

