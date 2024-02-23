#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

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

int read_line(int fd, char* buffer) {
    int bytes_read = 0;
    while (bytes_read < 32 && read(fd, buffer + bytes_read, 1) > 0) {
        bytes_read++;
    }
    return bytes_read;
}

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

/*printf*/
int my_print(const char *s) {
    return my_write(1, s, my_strlen(s));
}

int main(int argc, char **argv) {
    int fd; /* Add this part to report */

    switch (argc) {
        case 1: {
            char buffer[100];
            int counter = 0;
            while (counter < 10) {
                int line_cnt = read(0, buffer, 100);
                if (line_cnt == 0) {
                    break;
                }
                my_write(1, buffer, line_cnt);
                counter++;
            }
            break;
        }
        case 2: {
            /* Copy first 10 lines of standard input */
            int counter = 0;
            char buffer[100];
            while (counter < 10) {
                int line_cnt = read_line(0, buffer);
                if (line_cnt == 0) {
                    break;
                } else {
                    my_write(1, buffer, line_cnt);
                    counter++;
                }
            }
            break;
        }
        /* has to be something like 'head -n 10', cant be any other combo, only reading from standard input*/
        case 3: {
            if (my_strlen(argv[1]) != 2 || argv[1][0] != '-' || argv[1][1] != 'n') {
                my_print("Error: invalid argument\n");
                return 1;
            }
            int n = my_atoi(argv[2]);
            char buffer3[100];
            int counter = 0;
            while (counter < n) {
                int line_cnt = read_line(0, buffer3);
                if (line_cnt == 0) {
                    break;
                }
                my_write(1, buffer3, line_cnt);
                counter++;
            }
            break;
        }
        case 4: {
            if (argv[1] == "-n") {
                int n = my_atoi(argv[2]);
                fd = open(argv[3], O_RDONLY);
                if (fd == -1) {
                    my_print("Error: file not found\n");
                    return 1;
                }
                char buffer4[100];
                int counter = 0;
                while (counter < n) {
                    int line_cnt = read_line(fd, buffer4);
                    if (line_cnt == 0) {
                        break;
                    }
                    my_write(1, buffer4, line_cnt);
                    counter++;
                }
                if (close(fd) == -1) {
                    my_print("Error: file not closed\n");
                    return 1;
                }
            } else {
                fd = open(argv[1], O_RDONLY);
                if (fd == -1) {
                    my_print("Error: file not found\n");
                    return 1;
                }
                char buffer5[100];
                int counter = 0;
                int n = my_atoi(argv[3]);
                while (counter < n) {
                    int q = read_line(fd, buffer5);
                    if (q == 0) {
                        break;
                    }
                    my_write(1, buffer5, q);
                    counter++;
                }
                if (close(fd) == -1) {
                    my_print("Error: file not closed\n");
                    return 1;
                }
            }
            break;
        }
        default: {
            my_print("Error: too many arguments\n");
            break;
        }
    }
    return 0;
}
