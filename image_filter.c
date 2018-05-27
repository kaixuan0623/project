#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "bitmap.h"


#define ERROR_MESSAGE "Warning: one or more filter had an error, so the output image may not be correct.\n"
#define SUCCESS_MESSAGE "Image transformed successfully!\n"


/*
 * Check whether the given command is a valid image filter, and if so,
 * run the process.
 *
 * We've given you this function to illustrate the expected command-line
 * arguments for image_filter. No further error-checking is required for
 * the child processes.
 */
void run_command(const char *cmd) {
    if (strcmp(cmd, "copy") == 0 || strcmp(cmd, "./copy") == 0 ||
        strcmp(cmd, "greyscale") == 0 || strcmp(cmd, "./greyscale") == 0 ||
        strcmp(cmd, "gaussian_blur") == 0 || strcmp(cmd, "./gaussian_blur") == 0 ||
        strcmp(cmd, "edge_detection") == 0 || strcmp(cmd, "./edge_detection") == 0) {
        execl(cmd, cmd, NULL);
    } else if (strncmp(cmd, "scale", 5) == 0) {
        // Note: the numeric argument starts at cmd[6]
        execl("scale", "scale", cmd + 6, NULL);
    } else if (strncmp(cmd, "./scale", 7) == 0) {
        // Note: the numeric argument starts at cmd[8]
        execl("./scale", "./scale", cmd + 8, NULL);
    } else {
        fprintf(stderr, "Invalid command '%s'\n", cmd);
        exit(1);
    }
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: image_filter input output [filter ...]\n");
        exit(1);
    }

    if ((argc == 3) || (argc == 4)) {
        // the case where there is only one or no input filter.
        // don't need to create pipe in this case.
        int n = fork();
        if (n == 0) {
            // open the input image for reading.
            // argv[1] is the image name, fd_in: the input file_descriptor.
            int fd_in = open(argv[1], O_RDONLY);
            if (fd_in < 0) {
                perror("open");
                exit(1);
            }

            // Reset stdin so when we read from stdin, it comes from the image file.
            // i.e. redirects the data from image file to stdin of 1st child.
            if ((dup2(fd_in, STDIN_FILENO)) == -1) {
                perror("dup2");
                exit(1);
            }

            // create an output image file.
            int fd_out = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
            if (fd_out == -1) {
                perror("open");
                exit(1);
            }

            // redirects output from stdout to the file.
            if (dup2(fd_out, STDOUT_FILENO) == -1) {
                perror("dup2");
                exit(1);
            }

            // We won't be using fd_in directly, so close it.
            if ((close(fd_in)) == -1) {
                perror("close");
                exit(1);
            }

            // We won't be using fd_out directly, so close it.
            if ((close(fd_out)) == -1) {
                perror("close");
                exit(1);
            }

            if (argc == 3) { // run copy.
                if (execl("./copy", "./copy", NULL) == -1) {
                    perror("execl");
                    exit(1);
                }
                exit(0);
            } else {
                run_command(argv[3]);
                exit(0);
            }
        }
    } else if (argc > 4) {
        // when there is more than one filters.
        int fds[argc - 4][2]; // array of file_descriptor array.
        for (int i = 0; i < argc - 4; i++) {
            // Create all the pipes before we fork().
            // is there are 'argc' arguments, means there are (argc - 3) filters.
            // means we need to create (argc - 3) children.
            // thus, we need (argc - 4) pipes to connect them.
            // iterates (argc - 4) times, creates (argc - 4) pipes.
            // not execute when argc = 4.
            pipe(fds[i]);
        }
        int result; // used for fork().
        for (int i = 0; i < argc - 3; i++) {
            // fork all the child processes.
            result = fork();
            if ((result > 0) && (i <= argc - 5)) {
                // parent process, just close the corresponding pipe write end
                // as soon as a new child is create
                // so that the parent process won't write thing into the pipe,
                // even if the parent read in data from the pipes.
                // remember to close the read at very last.
                // Also note that in the case there is only one input filter,
                // on pipe is created, so don't need to close any pipe.
                if ((close(fds[i][1])) == -1) {
                    perror("close");
                    exit(1);
                }
            }

            if ((result == 0)) { // child process.
                if (i == 0) { // the 1st child.
                    // 1. redirects input from the input image to stdin using dup2().
                    // 2. redirects output to pipe0 (fds[0][1])
                    // 3. close all the used pipe ends.

                    // open the input image for reading.
                    // argv[1] is the image name, fd_in: the input file_descriptor.
                    int fd_in = open(argv[1], O_RDONLY);
                    if (fd_in < 0) {
                        perror("open");
                        exit(1);
                    }

                    // Reset stdin so when we read from stdin, it comes from the image file.
                    // i.e. redirects the data from image file to stdin of 1st child.
                    if ((dup2(fd_in, STDIN_FILENO)) == -1) {
                        perror("dup2");
                        exit(1);
                    }

                    // Reset stdout so that when we write to stdout, it goes to the pipe
                    if ((dup2(fds[0][1], STDOUT_FILENO)) == -1) {
                        perror("dup2");
                        exit(1);
                    }

                    // Close the read end of 1st child, it won't be reading from pipe.
                    if ((close(fds[0][0])) == -1) {
                        perror("close");
                        exit(1);
                    }

                    // close fds[0][1] of 1st child, it won't be writing to pipe.
                    if ((close(fds[0][1])) == -1) {
                        perror("close");
                        exit(1);
                    }

                    // We won't be using fd_in directly, so close it.
                    if ((close(fd_in)) == -1) {
                        perror("close");
                        exit(1);
                    }

                    // close all remaining unused pipe ends.
                    for (int j = 1; j < argc - 4; j++) {
                        if ((close(fds[j][0])) == -1) {
                            perror("close");
                            exit(1);
                        }

                        if ((close(fds[j][1])) == -1) {
                            perror("close");
                            exit(1);
                        }
                    }

                    run_command(argv[3]);
                    exit(0);

                } else if (i == argc - 4) { // the last child.
                    // 1. close all the unused pipe read ends.
                    // 2. redirects input from fds[argc - 5] to stdin using dup2().
                    // 3. redirects output to a file.
                    // note that there are no pipe attach to this child.
                    // so after closing the fd_out, nothing more to close.

                    // close all the unused pipe read ends first.
                    for (int q = 0; q < i - 1; q++) {
                        if ((close(fds[q][0])) == -1) {
                            perror("close");
                            exit(1);
                        }
                    }

                    // Reset stdin so when we read from stdin, it comes from the pipe. (fds[argc - 5])
                    if ((dup2(fds[argc - 5][0], STDIN_FILENO)) == -1) {
                        perror("dup2");
                        exit(1);
                    }

                    // create an output image file.
                    int fd_out = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
                    if (fd_out == -1) {
                        perror("open");
                        exit(1);
                    }

                    // redirects output from stdout to the file.
                    if (dup2(fd_out, STDOUT_FILENO) == -1) {
                        perror("dup2");
                        exit(1);
                    }

                    // We won't be using fd_out directly, so close it.
                    if ((close(fd_out)) == -1) {
                        perror("close");
                        exit(1);
                    }

                    run_command(argv[argc - 1]);
                    exit(0);

                } else { // children in between.

                    // close all the unused pipe read ends first.
                    for (int q = 0; q < i - 1; q++) {
                        if ((close(fds[q][0])) == -1) {
                            perror("close");
                            exit(1);
                        }
                    }

                    // Reset stdin so when we read from stdin, it comes from the pipe.
                    if ((dup2(fds[i - 1][0], STDIN_FILENO)) == -1) {
                        perror("dup2");
                        exit(1);
                    }

                    // Reset stdout so that when we write to stdout, it goes to the pipe
                    if ((dup2(fds[i][1], STDOUT_FILENO)) == -1) {
                        perror("dup2");
                        exit(1);
                    }

                    // close all unused pipe ends.
                    for (int j = i; j < argc - 4; j++) {
                        if ((close(fds[j][0])) == -1) {
                            perror("close");
                            exit(1);
                        }

                        if ((close(fds[j][1])) == -1) {
                            perror("close");
                            exit(1);
                        }
                    }
                    run_command(argv[i + 3]);
                    exit(0);
                }
            }
        }
        // Parent process.
        // first close all the remaining read end of the pipes.
        // only execute when argc > 4.
        for (int r = 0; r < argc - 4; r++) {
            if ((close(fds[r][0])) == -1) {
                perror("close");
                exit(1);
            }
        }
    }

    // Parent process.
    for (int i = 0; i < argc - 3; i++) {
        int status;
        if (wait(&status) == -1) {
            perror("wait");
            exit(1);
        } else if (!WIFEXITED(status)) {
            printf(ERROR_MESSAGE);
            return -1;
        }
    }
    printf(SUCCESS_MESSAGE);
    return 0;
}
