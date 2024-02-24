#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>


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

int my_print(const char *s) {
    return my_write(1, s, my_strlen(s));
}

void copy_lines(int source_fd, int dest_fd, int num_lines) {
    char buffer[100];
    char *lines[num_lines];
    int line_lengths[num_lines];
    int line_count = 0;

    // Initialize line lengths
    for (int i = 0; i < num_lines; i++) {
        line_lengths[i] = 0;
    }

    while (true) {
        int line_length = read_line(source_fd, buffer, sizeof(buffer));
        if (line_length == 0) {
            break;
        }

        if (line_count < num_lines) {
            lines[line_count] = malloc(line_length + 1);
            if (lines[line_count] == NULL) {
                perror("Error allocating memory");
                exit(EXIT_FAILURE);
            }
            // Copy characters manually
            for (int i = 0; i < line_length; i++) {
                lines[line_count][i] = buffer[i];
            }
            lines[line_count][line_length] = '\0';
            line_lengths[line_count] = line_length;
        } else {
            free(lines[line_count % num_lines]);
            lines[line_count % num_lines] = malloc(line_length + 1);
            if (lines[line_count % num_lines] == NULL) {
                perror("Error allocating memory");
                exit(EXIT_FAILURE);
            }
            // Copy characters manually
            for (int i = 0; i < line_length; i++) {
                lines[line_count % num_lines][i] = buffer[i];
            }
            lines[line_count % num_lines][line_length] = '\0';
            line_lengths[line_count % num_lines] = line_length;
        }

        line_count++;
    }

    // Print the last num_lines lines
    int start = (line_count > num_lines) ? (line_count % num_lines) : 0;
    int end = (line_count > num_lines) ? num_lines : line_count;
    for (int i = 0; i < end; i++) {
        // Write characters manually
        for (int j = 0; j < line_lengths[(start + i) % num_lines]; j++) {
            my_write(dest_fd, &lines[(start + i) % num_lines][j], 1);
        }
    }

    // Free allocated memory
    for (int i = 0; i < num_lines; i++) {
        free(lines[i]);
    }
}



int main(int argc, char **argv) {
    int fd;

    switch (argc) {
        case 1: {
            // Copy last 10 lines from standard input
            copy_lines(0, 1, 10);
            break;
        }
        case 2: {
            // Copy last 10 lines from file
            fd = open(argv[1], O_RDONLY);
            if (fd == -1) {
                my_print("Error: file not found\n");
                return 1;
            }
            copy_lines(fd, 1, 10);
            close(fd);
            break;
        }
        case 3: {
            if (my_strlen(argv[1]) != 2 || argv[1][0] != '-' || argv[1][1] != 'n') {
                my_print("Error: invalid argument\n");
                return 1;
            }
            // Copy last n lines from standard input
            int n = my_atoi(argv[2]);
            copy_lines(0, 1, n);
            break;
        }
        case 4: {
            if (argv[1][0] == '-' && argv[1][1] == 'n') {
                // Copy last n lines from file
                int n = my_atoi(argv[2]);
                fd = open(argv[3], O_RDONLY);
                if (fd == -1) {
                    my_print("Error: file not found\n");
                    return 1;
                }
                copy_lines(fd, 1, n);
                close(fd);
            } else {
                // Copy last 10 lines from file
                fd = open(argv[1], O_RDONLY);
                if (fd == -1) {
                    my_print("Error: file not found\n");
                    return 1;
                }
                int n = my_atoi(argv[3]);
                copy_lines(fd, 1, n);
                close(fd);
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
