#define _GNU_SOURCE

#include <libgen.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "gwshell.h"

int
main()
{
        char *line;
        char **args;
        int status;

        do {
                /* Give the user a prompt */
                gwshell_print_prompt();

                /* Read and parse the input */
                line = gwshell_read_line();
                args = gwshell_tok_line(line);

               /* Run the command */
                status = gwshell_exec(args);

                /* Cleanup memory */
                free(line);
                free(args);
        } while (status >= 0);

        return EXIT_SUCCESS;
}

void
gwshell_print_prompt(void)
{
        printf("%s", DEFAULT_PROMPT);
}

char *
gwshell_read_line(void)
{
        char *line = NULL;
        size_t size = 0;

        /* Read a line from the user */
        getline(&line, &size, stdin);

        return line;
}

char **
gwshell_tok_line(char *line)
{
        int buffer = TOK_BUFSIZE;
        int position = 0;
        char *token;
        char **tokens;
        char **tokens_backup;

        tokens = malloc(buffer * sizeof(char *));
        if (!tokens) {
                fprintf(stderr, "psh: Allocation failure :(\n");
                exit(EXIT_FAILURE);
        }

        /* Tokenize the input string
         * Can't handle spaces inside quoted strings :(  */
        token = strtok(line, TOK_DELIM);
        while (token) {
                tokens[position++] = token;

                if (position >= buffer) {
                        /* If the new string is greater than our buffer, increase the size
                         * Make sure to backup the old version, in case realloc fails */
                        buffer += TOK_BUFSIZE;
                        tokens_backup = tokens;
                        tokens = realloc(tokens, buffer * sizeof(char *));
                        if (!tokens) {
                                /* Free the pre-realloc array and exit with failure */
                                free(tokens_backup);
                                fprintf(stderr, "psh: Allocation failure :(\n");
                                exit(EXIT_FAILURE);
                        }
                }
                token = strtok(NULL, TOK_DELIM);
        }
        tokens[position] = NULL;
        return tokens;
}

/*
 * Used to run a command, first checking for null commands
 * You could also check for built in commands here
 */
int
gwshell_exec(char **args)
{
        int i;

        /* Check for empty command */
        if (args[0] == NULL) return 0;

        /* Simple fork/exec */
        return gwshell_run(args);
}

/*
 * Forks this process and runs what it has been passed
 */
int
gwshell_run(char **args)
{
        pid_t pid;
        int status;
        int i;

        /* Fork the process and execute stuff! */

        pid = fork();
        if (pid < 0) {
                /* Can't fork :( */
        } else if (pid == 0) {
                /* Child process - exec the given command */
                execvp(args[0], args);
        } else {
                do {
                        waitpid(pid, &status, WUNTRACED);
                } while(!WIFEXITED(status) && !WIFSIGNALED(status));
        }

        return 0;
}
