#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
void reverse_string(char *, int);
void print_words(char *, int, int);

int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    int i = 0;
    while (i < len && user_str[i] != '\0')
    {
        buff[i] = user_str[i];
        i++;
    }
    while (i < len) {
        buff[i++] = '.';
    }
    
    return i; //for now just so the code compiles. 
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
int count = 0, in_word = 0;
    for (int i = 0; i < str_len && buff[i] != '.'; i++) {
        if (buff[i] != ' ' && !in_word) {
            in_word = 1;
            count++;
        } else if (buff[i] == ' ') {
            in_word = 0;
        }
    }    
    return count;
}
void reverse_string(char *buff, int str_len) {
    for (int i = 0, j = str_len - 1; i < j; i++, j--) {
        char temp = buff[i];
        buff[i] = buff[j];
        buff[j] = temp;
    }
}

void print_words(char *buff, int len, int str_len) {
    int i = 0, word_start = -1, word_len = 0, word_index = 1;

    while (i < str_len && buff[i] != '.') {
        if (buff[i] != ' ' && word_start == -1) {
            word_start = i;
        }
        if ((buff[i] == ' ' || i == str_len - 1) && word_start != -1) {
            word_len = (buff[i] == ' ') ? i - word_start : i - word_start + 1;
            printf("Word %d: %.*s (length: %d)\n", word_index++, word_len, buff + word_start, word_len);
            word_start = -1;
        }
        i++;
    }
}


int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
  buff = (char *)malloc(BUFFER_SZ * sizeof(char));
    if (!buff) {
        printf("Error: Memory allocation failed\n");
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
         case 'r':
            reverse_string(buff, user_str_len);
            printf("Reversed String: ");
            print_buff(buff, BUFFER_SZ);
            break;

        case 'w':
            print_words(buff, BUFFER_SZ, user_str_len);
            break;

        default:
            usage(argv[0]);
            free(buff);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes? 
//  
//          providing both the pointer and the length is good practice beacause
//          it helps the flexibitly of the code and the buffer is at a fixed size
//          in the program passing the length makes the functions more reusable 
//          for buffers of different sizes.It also reduces the amount of errors
//          the buffer overflows byt makeing sure the function does not pass a 
//          curtain lenth it is also very clear to read the buffer being at at
//          length that it is and very easy to change the code by it being that way