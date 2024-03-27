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

int read_line(int fd, char* buffer, size_t max_size) {
    int bytes_read = 0;
    char ch;

    while (bytes_read < max_size && read(fd, &ch, 1) > 0) {
        if (ch == '\n') {
            buffer[bytes_read++] = ch;
            break; // Stop reading if newline is encountered
        }
        buffer[bytes_read++] = ch;
    }

    buffer[bytes_read] = '\0'; // Null-terminate the string
    return bytes_read;
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

/*printf*/
int my_print(const char *s) {
    return my_write(1, s, my_strlen(s));
}

void copyLines(int source_fd, int dest_fd, int num_lines) {
    char buffer[100];
    int counter = 0;

    while (counter < num_lines) {
        int line_cnt = read_line(source_fd, buffer, sizeof(buffer));
        if (line_cnt == 0) {
            break;
        }
        my_write(dest_fd, buffer, line_cnt);
        counter++;
    }
}

int main(int argc, char **argv) {
    int fd; /* Add this part to report */

    switch (argc) {
        case 1: {
            copyLines(0, 1, 10);
            break;
        }
        case 2: {
            /* Copy first 10 lines of standard input */
            fd = open(argv[1] ,O_RDONLY);
            copyLines(fd, 1, 10);
            break;
        }
        case 3: {
            if (my_strlen(argv[1]) != 2 || argv[1][0] != '-' || argv[1][1] != 'n') {
                my_print("Error: invalid argument\n");
                return 1;
            }
            int n = my_atoi(argv[2]);
            copyLines(0, 1, n);
            break;
        }
        case 4: {
            if (argv[1][0] == '-' && argv[1][1] == 'n') {
                int n = my_atoi(argv[2]);
                fd = open(argv[3], O_RDONLY);
                if (fd == -1) {
                    my_print("Error: file not found");
                    return 1;
                }
                copyLines(fd, 1, n);
                if (close(fd) == -1) {
                    my_print("Error: file not closed");
                    return 1;
                }
            } else {
                fd = open(argv[1], O_RDONLY);
                if(fd == -1) {
                    my_print("Error: file not found");
                    return 1;
                }
                int n = my_atoi(argv[3]);
                copyLines(fd, 1, n);
                if (close(fd) == -1) {
                    my_print("Error: file not closed");
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
