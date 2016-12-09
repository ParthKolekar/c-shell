#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define HARAKIRI 1143

char * readLine(char * buffer) {
    char * ch = buffer;
    while ((*ch = getchar()) != '\n')
        ch++;
    *ch = 0;
    return buffer;
}

void parse(char * line, char ** argv) {
    while (*line != '\0' && *line != '>' && *line != '<') {
        // trim leading spaces
        while (*line == ' ' || *line == '\t' || *line == '\n')
            *line++ = '\0';

        // modify argv to point to the start of each line
        *argv++ = line;

        while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n')
            line++;
    }

    *argv = 0;
}

int run_process(char * command, char ** args, int flags, char * inputFile, char * outputFile) {
    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        return HARAKIRI; // Because Kill Yourself. But there is no self to kill.... #existential_crisis
    }

    int status = 0;
    if (pid != 0) {

        // hold the parent and let it wait
        waitpid (pid , &status , WUNTRACED);

        // Maybe kill childs that return HARAKIRI, but they kill -TERM themselves anyways...
        return 0;
    } else {
        int fd_in = 0;
        int fd_out = 1;
        if (flags & 1) {
            if (access(inputFile, F_OK) == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                kill(getpid(), SIGTERM);
                return HARAKIRI; // Because Kill Yourself.
            }
            fd_in = open(inputFile, O_RDWR);
            if (fd_in == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                kill(getpid(), SIGTERM);
                return HARAKIRI; // Because Kill Yourself.
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }
        if ((flags & 2)) {
            fd_out = open(outputFile, O_RDWR | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IWGRP | S_IWOTH);
            if (fd_out == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                kill(getpid(), SIGTERM);
                return HARAKIRI; // Because Kill Yourself.
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }
        if ((flags & 4)) {
            fd_out = open(outputFile, O_RDWR | O_CREAT | O_APPEND , S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IWGRP | S_IWOTH);
            if (fd_out == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                kill(getpid() , SIGTERM);
                return HARAKIRI; // Because Kill Yourself.
            }
            dup2(fd_out, STDOUT_FILENO);
            close (fd_out);
        }
        if (execvp(command, args) == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            kill(getpid() , SIGTERM);
            return HARAKIRI;  // Because Kill Yourself.
        }
    }
    return 0;
}

void parseAndExecCommandLine(char * command) {
    char * args[100];
    parse(command, args);
    char * inputFile = NULL;
    char * outputFile = NULL;
    int flags = 0;

    // Flag conventions!!
    // 0x00 = no redirection
    // 0x01 = stdin redirection
    // 0x02 = stdout redirection (write mode)
    // 0x04 = stdout redirection (append mode)

    int len;

    for (len = 0 ; args[len] ; len++)
        ;

    int i;
    for (i = 0 ; args[i] ; i++) {

        if (args[i][0] == '<') {
            if (i + 1 <= len)
                inputFile = args[i + 1];
            else
                inputFile = NULL;
            flags |= 1;
            args[i] = NULL;
            i++;
        }

        if (!args[i]) {
            fprintf(stderr, "%s\n", "Syntax Error");
            return;
        }

        if (args[i][0] == '>') {
            flags |= 2;
            if (strlen(args[i]) == 2 && args[i][1] == '>') {
                flags |= 4;
                flags &= ~2;
            }

            if (i + 1 <= len)
                outputFile = args[i + 1];
            else
                outputFile = NULL;

            args[i] = NULL;
            i++;
        }
    }
    run_process(command, args, flags, inputFile, outputFile);
}

int main (const int argc, const char ** argv) {
    // command buffer. 
    // Here's to hoping no one types a command longer than that.
    char command[1000];

    while (1) {

        printf(">>  ");
        readLine(command);

        while (command[0] == '\0') {
            printf(">>  ");
            readLine(command);
        }

        // quita quite exita exiting are all valid death commands
        if (strncmp (command, "quit" , 4) == 0)
            return 0;

        if (strncmp (command, "exit" , 4) == 0)
            return 0;

        if (strncmp (command, "easter-egg" , 10) == 0) {
            fprintf(stdout ,
                    "\n"
                    "         ##\n"
                    "          ###\n"
                    "           ####\n"
                    "            #####\n"
                    "            #######\n"
                    "             #######\n"
                    "             ########\n"
                    "             ########\n"
                    "             #########\n"
                    "             ##########\n"
                    "            ############\n"
                    "          ##############\n"
                    "         ################\n"
                    "          ################                                                  #\n"
                    "            ##############                                                ###\n"
                    "             ##############                                              ####\n"
                    "             ##############                                           #####\n"
                    "              ##############                                      #######\n"
                    "              ##############                                 ###########\n"
                    "              ###############                              #############\n"
                    "              ################                           ##############\n"
                    "             #################        #                ################\n"
                    "             ##################      ##    #          #################\n"
                    "            ####################    ###   ##         #################\n"
                    "                 ################   ########         #################\n"
                    "                  ################  #######         ###################\n"
                    "                    #######################       #####################\n"
                    "                     #####################       ###################\n"
                    "                       ############################################\n"
                    "                        ###########################################\n"
                    "                        ##########################################\n"
                    "                         ########################################\n"
                    "                         ########################################\n"
                    "                          ######################################\n"
                    "                          ######################################\n"
                    "                           ##########################      #####\n"
                    "                           ###  ###################           ##\n"
                    "                           ##    ###############               #\n"
                    "                           #     ##  ##########\n"
                    "                                     ##    ###\n"
                    "                                           ###\n"
                    "                                           ##\n"
                    "                                           #\n"
                    "\n"
                    "   LONG   LIVE   THE   BAT.\n"
                    "\n");

            // die after easter-egg. The bat lives long, not c-shell.
            return 0;
        }
        parseAndExecCommandLine(command);
    }
    return 0;
}
