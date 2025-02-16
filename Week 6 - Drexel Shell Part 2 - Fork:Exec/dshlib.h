#ifndef __DSHLIB_H__
    #define __DSHLIB_H__

// Constants for command structure sizes
#define EXE_MAX 64
#define ARG_MAX 256
#define CMD_MAX 8
#define CMD_ARGV_MAX (CMD_MAX + 1)  // Max number of arguments
#define SH_CMD_MAX EXE_MAX + ARG_MAX // Max length of command

// Command buffer structure for parsing
typedef struct cmd_buff {
    int argc;
    char *argv[CMD_ARGV_MAX];
    char *_cmd_buffer;
} cmd_buff_t;

// Special character definitions
#define SPACE_CHAR  ' '
#define PIPE_CHAR   '|'
#define PIPE_STRING "|"

#define SH_PROMPT "dsh2> "
#define EXIT_CMD "exit"

// Standard Return Codes
#define OK                       0
#define WARN_NO_CMDS            -1
#define ERR_TOO_MANY_COMMANDS   -2
#define ERR_CMD_OR_ARGS_TOO_BIG -3
#define ERR_CMD_ARGS_BAD        -4      // For extra credit
#define ERR_MEMORY              -5
#define ERR_EXEC_CMD            -6
#define OK_EXIT                 -7

// Built-in command enum
typedef enum {
    BI_CMD_EXIT,
    BI_CMD_CD,
    BI_CMD_RC,  // Add rc (return code) for extra credit
    BI_NOT_BI,
    BI_EXECUTED,
} Built_In_Cmds;

// Function Prototypes
int alloc_cmd_buff(cmd_buff_t *cmd_buff);
int free_cmd_buff(cmd_buff_t *cmd_buff);
int clear_cmd_buff(cmd_buff_t *cmd_buff);
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff);

int cd(cmd_buff_t *cmd); // Function to handle cd command
void parse_input(char *input, cmd_buff_t *cmd); // Command parsing
void execute_command(cmd_buff_t *cmd);  // Fork/exec pattern for external commands
int exec_local_cmd_loop(); // Main command loop
int exec_cmd(cmd_buff_t *cmd); // Execute commands, built-in or external

// Output constants
#define CMD_OK_HEADER       "PARSED COMMAND LINE - TOTAL COMMANDS %d\n"
#define CMD_WARN_NO_CMD     "warning: no commands provided\n"
#define CMD_ERR_PIPE_LIMIT  "error: piping limited to %d commands\n"

#endif
