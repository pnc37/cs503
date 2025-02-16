#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */

int exec_local_cmd_loop() {
    char cmd_buff[SH_CMD_MAX];  // User input buffer
    cmd_buff_t cmd;             // Command structure
    int rc = OK;                // Return code for handling errors

    while (1) {
        printf("%s", SH_PROMPT);  // Prompt for user input
        fflush(stdout);

        // Read user input and handle errors
        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;  // Exit if fgets fails (EOF or error)
        }

        // Remove the trailing newline character from the input
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        // Parse the input into cmd_buff_t structure
        if (build_cmd_buff(cmd_buff, &cmd) != OK) {
            printf(CMD_WARN_NO_CMD);
            continue;  // Skip iteration if no command was parsed
        }

        // Handle built-in commands like exit and cd
        Built_In_Cmds cmd_type = match_command(cmd.argv[0]);
        switch (cmd_type) {
            case BI_CMD_EXIT:
                printf("Exiting shell...\n");
                return OK_EXIT;  // Exit the shell
            case BI_CMD_CD:
                rc = exec_built_in_cmd(&cmd);  // Handle cd command
                if (rc != OK) {
                    printf(CMD_ERR_EXECUTE);  // Error while executing cd
                }
                continue;  // Skip the rest of the loop to read the next command
            case BI_CMD_RC:
                printf("%d\n", rc);  // Return code for the last operation
                continue;
            case BI_NOT_BI:
                break;  // Proceed with external command if it's not built-in
            default:
                printf("Unknown built-in command\n");
                continue;
        }

        // Handle external commands using fork/exec
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            rc = ERR_MEMORY;
            continue;  // Skip iteration if fork fails
        } else if (pid == 0) {
            // In child process
            if (execvp(cmd.argv[0], cmd.argv) == -1) {
                perror(CMD_ERR_EXECUTE);  // Execute the command, print error if it fails
                exit(1);
            }
        } else {
            // In parent process
            int status;
            waitpid(pid, &status, 0);  // Wait for the child process to finish
            rc = WEXITSTATUS(status);  // Get the exit status of the child process
        }
    }

    return OK;  // Return 0 if loop exits normally
}
