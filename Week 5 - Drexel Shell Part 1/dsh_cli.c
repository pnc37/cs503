#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dshlib.h"

/*
 * Implement your main function by building a loop that prompts the
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.  Since we want fgets to also handle
 * end of file so we can run this headless for testing we need to check
 * the return code of fgets.  I have provided an example below of how
 * to do this assuming you are storing user input inside of the cmd_buff
 * variable.
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
 *
 *   Expected output:
 *
 *      CMD_OK_HEADER      if the command parses properly. You will
 *                         follow this by the command details
 *
 *      CMD_WARN_NO_CMD    if the user entered a blank command
 *      CMD_ERR_PIPE_LIMIT if the user entered too many commands using
 *                         the pipe feature, e.g., cmd1 | cmd2 | ... |
 *
 *  See the provided test cases for output expectations.
 */
int main()
{
    char cmd_buff[ARG_MAX];
    int rc = 0;
    command_list_t clist;

    while (1) {
        // Print the shell prompt
        printf("%s", SH_PROMPT);

        // Read user input
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL) {
            // Find error in input
            printf("\n");
            break;
        }

        // Remove the trailing newline character from cmd_buff
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        // Check if the user entered the "exit" command
        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            // If "exit" command is typed, print a message and exit the shell
            printf("Exiting shell...\n");
            break;  // Exit the loop and terminate the shell
        }
        // Handle the "dragon" extra credit command
        if (strcmp(cmd_buff, "dragon") == 0) {
            // Print the Drexel dragon ASCII art
            printf("                                                                        @%%%%\n");
            printf("                                                                     %%%%%%\n");
            printf("                                                                    %%%%%%\n");
            printf("                                                                 % %%%%%%%           @\n");
            printf("                                                                %%%%%%%%%%        %%%%%%%\n");
            printf("                                       %%%%%%%  %%%%@         %%%%%%%%%%%%@    %%%%%%  @%%%%\n");
            printf("                                  %%%%%%%%%%%%%%%%%%%%%%      %%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
            printf("                                %%%%%%%%%%%%%%%%%%%%%%%%%%   %%%%%%%%%%%% %%%%%%%%%%%%%%%\n");
            printf("                               %%%%%%%%%%%%%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%     %%%\n");
            printf("                             %%%%%%%%%%%%%%%%%%%%%%%%%%%%@ @%%%%%%%%%%%%%%%%%%        %%\n");
            printf("                            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%%%%\n");
            printf("                            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
            printf("                            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%@\n");
            printf("      %%%%%%%%@           %%%%%%%%%%%%%%%%        %%%%%%%%%%%%%%%%%%%%%%%%%%      %%\n");
            printf("    %%%%%%%%%%%%%         %%@%%%%%%%%%%%%           %%%%%%%%%%% %%%%%%%%%%%%      @%\n");
            printf("  %%%%%%%%%%   %%%        %%%%%%%%%%%%%%            %%%%%%%%%%%%%%%%%%%%%%%%\n");
            printf(" %%%%%%%%%       %         %%%%%%%%%%%%%             %%%%%%%%%%%%@%%%%%%%%%%%\n");
            printf("%%%%%%%%%@                % %%%%%%%%%%%%%            @%%%%%%%%%%%%%%%%%%%%%%%%%\n");
            printf("%%%%%%%%@                 %%@%%%%%%%%%%%%            @%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
            printf("%%%%%%%@                   %%%%%%%%%%%%%%%           %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
            printf("%%%%%%%%%%                  %%%%%%%%%%%%%%%          %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%      %%%%\n");
            printf("%%%%%%%%%@                   @%%%%%%%%%%%%%%         %%%%%%%%%%%%@ %%%% %%%%%%%%%%%%%%%%%   %%%%%%%%\n");
            printf("%%%%%%%%%%                  %%%%%%%%%%%%%%%%%        %%%%%%%%%%%%%      %%%%%%%%%%%%%%%%%% %%%%%%%%%\n");
            printf("%%%%%%%%%@%%@                %%%%%%%%%%%%%%%%@       %%%%%%%%%%%%%%     %%%%%%%%%%%%%%%%%%%%%%%%  %%\n");
            printf(" %%%%%%%%%%                  % %%%%%%%%%%%%%%@        %%%%%%%%%%%%%%   %%%%%%%%%%%%%%%%%%%%%%%%%% %%\n");
            printf("  %%%%%%%%%%%%  @           %%%%%%%%%%%%%%%%%%        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  %%%\n");
            printf("   %%%%%%%%%%%%% %%  %  %@ %%%%%%%%%%%%%%%%%%          %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    %%%\n");
            printf("    %%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%%%%           @%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    %%%%%%%\n");
            printf("     %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%              %%%%%%%%%%%%%%%%%%%%%%%%%%%%        %%%\n");
            printf("      @%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                  %%%%%%%%%%%%%%%%%%%%%%%%%\n");
            printf("        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                      %%%%%%%%%%%%%%%%%%%  %%%%%%%\n");
            printf("           %%%%%%%%%%%%%%%%%%%%%%%%%%                           %%%%%%%%%%%%%%%  @%%%%%%%%%\n");
            printf("              %%%%%%%%%%%%%%%%%%%%           @%@%                  @%%%%%%%%%%%%%%%%%%   %%%\n");
            printf("                  %%%%%%%%%%%%%%%        %%%%%%%%%%                    %%%%%%%%%%%%%%%    %\n");
            printf("                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                      %%%%%%%%%%%%%%\n");
            printf("                %%%%%%%%%%%%%%%%%%%%%%%%%%  %%%% %%%                      %%%%%%%%%%  %%%@\n");
            printf("                     %%%%%%%%%%%%%%%%%%% %%%%%% %%                          %%%%%%%%%%%%%@\n");
            printf("                                                                                 %%%%%%%@\n");
            continue;  // Skip further processing and prompt again
        }
        // Handle empty command input
        if (cmd_buff[0] == '\0') {
            printf("%s\n", CMD_WARN_NO_CMD);  // Warning for empty command
            continue;
        }

        // Build the command list from the input
        rc = build_cmd_list(cmd_buff, &clist);

        // Check for errors or success
        if (rc == ERR_TOO_MANY_COMMANDS) {
            printf("%s\n", CMD_ERR_PIPE_LIMIT);  // Too many pipes in the command
        } else if (rc == OK) {
            // Successfully parsed the command line
            printf("%s\n", CMD_OK_HEADER);
            for (int i = 0; i < clist.num_commands; i++) {
                printf("<%d> %s\n", i + 1, clist.commands[i].command_name);
                for (int j = 0; j < clist.commands[i].argc; j++) {
                    printf("\targ[%d]: %s\n", j + 1, clist.commands[i].argv[j]);
                }
            }
        } else {
            // Handle other errors here (e.g., ERR_CMD_OR_ARGS_TOO_BIG)
            printf("Error: Invalid command input.\n");
        }
    }

    // Exit the shell cleanly
    return 0;
}