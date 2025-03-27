
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <fcntl.h>
#include "dshlib.h"
#include "rshlib.h"


/*
 * exec_client_requests(cli_socket)
 *      cli_socket:  The server-side socket that is connected to the client
 *   
 *  This function handles accepting remote client commands. The function will
 *  loop and continue to accept and execute client commands.  There are 2 ways
 *  that this ongoing loop accepting client commands ends:
 * 
 *      1.  When the client executes the `exit` command, this function returns
 *          to process_cli_requests() so that we can accept another client
 *          connection. 
 *      2.  When the client executes the `stop-server` command this function
 *          returns to process_cli_requests() with a return code of OK_EXIT
 *          indicating that the server should stop. 
 * 
 *  Note that this function largely follows the implementation of the
 *  exec_local_cmd_loop() function that you implemented in the last 
 *  shell program deliverable. The main difference is that the command will
 *  arrive over the recv() socket call rather than reading a string from the
 *  keyboard. 
 * 
 *  This function also must send the EOF character after a command is
 *  successfully executed to let the client know that the output from the
 *  command it sent is finished.  Use the send_message_eof() to accomplish 
 *  this. 
 * 
 *  Of final note, this function must allocate a buffer for storage to 
 *  store the data received by the client. For example:
 *     io_buff = malloc(RDSH_COMM_BUFF_SZ);
 *  And since it is allocating storage, it must also properly clean it up
 *  prior to exiting.
 * 
 *  Returns:
 * 
 *      OK:       The client sent the `exit` command.  Get ready to connect
 *                another client. 
 *      OK_EXIT:  The client sent `stop-server` command to terminate the server
 * 
 *      ERR_RDSH_COMMUNICATION:  A catch all for any socket() related send
 *                or receive errors. 
 */
int exec_client_requests(int cli_socket) {
    int io_size;
    command_list_t cmd_list;
    int rc;
    int cmd_rc;
    int last_rc;
    char *io_buff;

    io_buff = malloc(RDSH_COMM_BUFF_SZ);
    if (io_buff == NULL){
        return ERR_RDSH_SERVER;
    }

    while(1) {
        memset(io_buff, 0, RDSH_COMM_BUFF_SZ);

        io_size = recv(cli_socket, io_buff, RDSH_COMM_BUFF_SZ, 0);

        if (io_size < 0) {
            free(io_buff);
            return ERR_RDSH_COMMUNICATION;
        }

        if (io_size == 0) {
            printf(RCMD_MSG_CLIENT_EXITED);
            break;
        }

        rc = build_cmd_list((char *)io_buff, &cmd_list);
        switch (rc) {
            case ERR_MEMORY:
                sprintf(io_buff, CMD_ERR_RDSH_ITRNL, ERR_MEMORY);
                send_message_string(cli_socket, (char *)io_buff);
                continue;
            case WARN_NO_CMDS:
                sprintf(io_buff, CMD_ERR_RDSH_ITRNL, WARN_NO_CMDS);
                send_message_string(cli_socket, (char *)io_buff);
                continue;
            case ERR_CMD_OR_ARGS_TOO_BIG:
                sprintf(io_buff, CMD_ERR_PIPE_LIMIT, CMD_MAX);
                send_message_string(cli_socket, (char *)io_buff);
                continue;
            default:
                break;
        }

        // TODO BEN add the rest here 
        last_rc = cmd_rc;
        cmd_rc = rsh_execute_pipeline(cli_socket, &cmd_list);

        switch(cmd_rc){
            case RC_SC:
                sprintf(io_buff, RCMD_MSG_SVR_RC_CMD, last_rc);
                send_message_string(cli_socket, (char *)io_buff);
                continue;
            case EXIT_SC:
                printf(RCMD_MSG_CLIENT_EXITED);
                free(io_buff);
                close(cli_socket);
                return OK;
            case STOP_SERVER_SC:
                printf(RCMD_MSG_SVR_STOP_REQ);
                free(io_buff);
                close(cli_socket);
                return OK_EXIT;
            default:
                break;
        }

        rc = send_message_eof(cli_socket);
        if (rc != OK){
            printf(CMD_ERR_RDSH_COMM);
            free(io_buff);
            close(cli_socket);
            return ERR_RDSH_COMMUNICATION;
        }

        printf(RCMD_MSG_SVR_EXEC_REQ, io_buff);

    } // end while loop

    free(io_buff);
    close(cli_socket);
    return OK;
}
