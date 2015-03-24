#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define HARAKIRI 1143

char * readLine ( char *buffer )
{
    char *ch = buffer;
    while ((*ch = getchar()) != '\n')
        ch++;
    *ch = 0;
    return buffer;
}

void parse ( char * line, char ** argv )
{
    while (*line != '\0' && *line != '>' && *line != '<')
    {
        while (*line == ' ' || *line == '\t' || *line == '\n')
            *line++ = '\0';
        *argv++ = line;
        while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n')
            line++;
    }
    *argv = '\0';
}

int run_process ( char *command , char ** args  , int files , char * inputFile , char * outputFile )
{
    int pid = fork();
    int status = 0;
    if (pid != 0)
    {
        waitpid (pid , &status , WUNTRACED);
        return 0;
    }
    else
    {
        int fd_in = 0;
        int fd_out = 1;
        switch (files & 1)
        {
            case 0:
                break;
            case 1:
                if (inputFile == NULL)
                {
                    fprintf(stderr,"INVALID FILE!!\n");
                    kill (getpid() , SIGTERM);
                    return HARAKIRI; // Because Kill Yourself.
                }
                if (access ( inputFile , F_OK ))
                {
                    fprintf(stderr,"INVALID FILE!!\n");
                    kill (getpid() , SIGTERM);
                    return HARAKIRI; // Because Kill Yourself.
                }
                fd_in = open ( inputFile , O_RDWR );
                if (fd_in == -1)
                {
                    fprintf(stderr, "PERMISSIONS DENIED!!\n");
                    kill (getpid() , SIGTERM);
                    return HARAKIRI; // Because Kill Yourself.
                }
                dup2(fd_in , STDIN_FILENO);
                close (fd_in);
                break;
        }
        switch ((files & 2) >> 1)
        {
            case 0:
                break;
            case 1:
                if (outputFile == NULL)
                {
                    fprintf(stderr,"INVALID FILE!!\n");
                    kill (getpid() , SIGTERM);
                    return HARAKIRI; // Because Kill Yourself.
                }
                fd_out = open ( outputFile , O_RDWR | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IWGRP | S_IWOTH );
                if (fd_out == -1)
                {
                    fprintf(stderr , "PERMISSIONS DENIED!!\n");
                    kill (getpid() , SIGTERM);
                    return HARAKIRI; // Because Kill Yourself.
                }
                dup2(fd_out , STDOUT_FILENO);
                close (fd_out);
                break;
        }
        switch ((files & 4) >> 2)
        {
            case 0:
                break;
            case 1:
                if (outputFile == NULL)
                {
                    fprintf(stderr , "INVALID FILE!!\n");
                    kill (getpid() , SIGTERM);
                    return HARAKIRI; // Because Kill Yourself.
                }
                fd_out = open ( outputFile , O_RDWR | O_CREAT | O_APPEND , S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IWGRP | S_IWOTH );
                if (fd_out == -1)
                {
                    fprintf(stderr , "PERMISSIONS DENIED!!\n");
                    kill (getpid() , SIGTERM);
                    return HARAKIRI; // Because Kill Yourself.
                }
                dup2(fd_out , STDOUT_FILENO);
                close (fd_out);
                break;
        }
        if (execvp ( command , args ) < 0)
		{
            fprintf ( stderr , "COULD NOT FIND COMMAND : %s\n" , command );
		    kill (getpid() , SIGTERM);
		    return HARAKIRI;  // Because Kill Yourself.
		}
    }
    return 0;
}

void parseCommandLine ( char *command )
{
    char *args[100];
    parse(command , args);
    char * inputFile = NULL;
    char * outputFile = NULL;
    int flags = 0;
    int len;
    for (len = 0 ; args[len] ; len++)
        ;
    int i;
    for (i = 0 ; args[i] ; i++)
    {
        if (args[i][0] == '<')
        {
            if (i + 1 < len)
                inputFile = args[i + 1];
            else
                inputFile = NULL;
            flags |= 1;
            args[i] = NULL;
            i++;
        }
        if (args[i][0] == '>')
        {
            flags |= 2;
            if (strlen(args[i]) == 2 && args[i][1] == '>')
            {
                flags |= 4;
                flags &= ~2;
            }
            if (i + 1 < len)
                outputFile = args[i + 1];
            else
                outputFile = NULL;
            args[i] = NULL;
            i++;
        }
    }
    run_process ( command , args , flags , inputFile , outputFile );
}

int main ( const int argc , const char ** argv )
{
    char command[1000];
    while (1)
    {
        printf(">>  ");
        readLine(command);
        while (command[0] == '\0')
        {
            printf(">>  ");
            readLine(command);
        }
        if (strncmp (command , "quit" , 4) == 0)
            return 0;
        if (strncmp (command , "exit" , 4) == 0)
            return 0;
        if (strncmp (command , "easter-egg" , 10) == 0)
        {
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
            return 0;
        }
        parseCommandLine(command);
    }
    return 0;
}


