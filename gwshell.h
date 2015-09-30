#ifndef _GWSHELL_H
#define _GWSHELL_H

/* Size to string to tokenize at a time */
#define TOK_BUFSIZE 16

/* Characters to stop tokenizing on */
#define TOK_DELIM " \t\r\n\a"

/* Fallback prompt to show */
#define DEFAULT_PROMPT "gwshell> "

void
gwshell_print_prompt(void);

char *
gwshell_read_line(void);

char **
gwshell_tok_line(char *line);

int
gwshell_exec(char **args);

int
gwshell_run(char **args);

#endif
