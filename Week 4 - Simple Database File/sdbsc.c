#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> //c library for system call file routines
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

// database include files
#include "db.h"
#include "sdbsc.h"

int open_db(const char *dbFile, int should_truncate) {
    int flags = O_RDWR | O_CREAT;  // Read/write mode, create if missing
    if (should_truncate) {
        flags |= O_TRUNC;  // Clear file if needed
    }

    int fd = open(dbFile, flags, 0666);  // Open file with read/write permissions
    if (fd == -1) {
        printf(M_ERR_DB_OPEN);  // Print error message
        return ERR_DB_FILE;     // Return error code
    }

    return fd;  // Return valid file descriptor
}
int open_db(char *dbFile, bool should_truncate)
{
    // Set permissions: rw-rw----
    // see sys/stat.h for constants
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;

    // open the file if it exists for Read and Write,
    // create it if it does not exist
    int flags = O_RDWR | O_CREAT;

    if (should_truncate)
        flags += O_TRUNC;

    // Now open file
    int fd = open(dbFile, flags, mode);

    if (fd == -1)
    {
        // Handle the error
        printf(M_ERR_DB_OPEN);
        return ERR_DB_FILE;
    }

    return fd;
}

int get_student(int fd, int id, student_t *s) {
    // Validate student ID range
    if (id < 1 || id > 100000) {
        return SRCH_NOT_FOUND;
    }

    // Calculate byte offset
    int offset = id * STUDENT_RECORD_SIZE;

    // Seek to the correct position in the file
    if (lseek(fd, offset, SEEK_SET) == -1) {
        return ERR_DB_FILE;
    }

    // Read the student record
    ssize_t bytes_read = read(fd, s, sizeof(student_t));
    if (bytes_read != sizeof(student_t)) {
        return ERR_DB_FILE;  // Read error
    }

    // Check if the record is empty (id field == 0)
    if (s->id == 0) {
        return SRCH_NOT_FOUND;
    }

    return NO_ERROR;  // Student found successfully
}
int get_student(int fd, int id, student_t *s)
{
    // TODO
    return NOT_IMPLEMENTED_YET;
}

/int add_student(int fd, int id, const char *fname, const char *lname, int gpa) {
    // Validate student ID and GPA range
    if (id < 1 || id > 100000 || gpa < 0 || gpa > 400) {
        return ERR_DB_OP;
    }

    // Calculate the byte offset
    int offset = id * STUDENT_RECORD_SIZE;
    
    // Seek to the correct position
    if (lseek(fd, offset, SEEK_SET) == -1) {
        printf(M_ERR_DB_READ);
        return ERR_DB_FILE;
    }

    // Check if student already exists
    student_t existing_student;
    memset(&existing_student, 0, sizeof(student_t));  // Initialize with zeros
    if (read(fd, &existing_student, sizeof(student_t)) != sizeof(student_t)) {
        printf(M_ERR_DB_READ);
        return ERR_DB_FILE;
    }

    // If the record is not empty, student already exists
    if (existing_student.id != 0) {
        printf(M_ERR_DB_ADD_DUP);
        return ERR_DB_OP;
    }

    // Prepare new student record
    student_t new_student;
    memset(&new_student, 0, sizeof(student_t));  // Initialize to zero
    new_student.id = id;
    strncpy(new_student.fname, fname, sizeof(new_student.fname) - 1);
    strncpy(new_student.lname, lname, sizeof(new_student.lname) - 1);
    new_student.gpa = gpa;

    // Seek back to the correct position to write
    if (lseek(fd, offset, SEEK_SET) == -1) {
        printf(M_ERR_DB_WRITE);
        return ERR_DB_FILE;
    }

    // Write the new student record
    if (write(fd, &new_student, sizeof(student_t)) != sizeof(student_t)) {
        printf(M_ERR_DB_WRITE);
        return ERR_DB_FILE;
    }

    // Success
    printf(M_STD_ADDED);
    return NO_ERROR;
}
int add_student(int fd, int id, char *fname, char *lname, int gpa)
{
    // TODO
    printf(M_NOT_IMPL);
    return NOT_IMPLEMENTED_YET;
}

int del_student(int fd, int id) {
    student_t student;

    // Check if student exists
    int result = get_student(fd, id, &student);
    if (result == SRCH_NOT_FOUND) {
        printf(M_STD_NOT_FND_MSG);
        return ERR_DB_OP;  // Student not found
    }
    if (result == ERR_DB_FILE) {
        printf(M_ERR_DB_READ);
        return ERR_DB_FILE;  // File error
    }

    // Calculate byte offset
    int offset = id * STUDENT_RECORD_SIZE;

    // Seek to the correct position
    if (lseek(fd, offset, SEEK_SET) == -1) {
        printf(M_ERR_DB_WRITE);
        return ERR_DB_FILE;
    }

    // Write EMPTY_STUDENT_RECORD to delete the student
    if (write(fd, &EMPTY_STUDENT_RECORD, sizeof(student_t)) != sizeof(student_t)) {
        printf(M_ERR_DB_WRITE);
        return ERR_DB_FILE;
    }

    // Success
    printf(M_STD_DEL_MSG);
    return NO_ERROR;
}
int del_student(int fd, int id)
{
    // TODO
    printf(M_NOT_IMPL);
    return NOT_IMPLEMENTED_YET;
}

int count_db_records(int fd) {
    student_t student;
    int count = 0;

    // Move to the beginning of the file
    if (lseek(fd, 0, SEEK_SET) == -1) {
        printf(M_ERR_DB_READ);
        return ERR_DB_FILE;
    }

    // Read each record and check if it is empty
    while (read(fd, &student, sizeof(student_t)) == sizeof(student_t)) {
        if (memcmp(&student, &EMPTY_STUDENT_RECORD, sizeof(student_t)) != 0) {
            count++;  // Record is not empty, count it
        }
    }

    // Handle read error
    if (read(fd, &student, sizeof(student_t)) == -1) {
        printf(M_ERR_DB_READ);
        return ERR_DB_FILE;
    }

    // Print result
    if (count == 0) {
        printf(M_DB_EMPTY);
    } else {
        printf(M_DB_RECORD_CNT, count);
    }

    return count;
}

int count_db_records(int fd)
{
    // TODO
    printf(M_NOT_IMPL);
    return NOT_IMPLEMENTED_YET;
}

int print_db(int fd) {
    student_t student;
    int records_found = 0;

    // Move to the beginning of the file
    if (lseek(fd, 0, SEEK_SET) == -1) {
        printf(M_ERR_DB_READ);
        return ERR_DB_FILE;
    }

    // Read and print records
    while (read(fd, &student, sizeof(student_t)) == sizeof(student_t)) {
        // Skip empty records
        if (memcmp(&student, &EMPTY_STUDENT_RECORD, sizeof(student_t)) == 0) {
            continue;
        }

        // Print table header only once (before first record)
        if (records_found == 0) {
            printf(STUDENT_PRINT_HDR_STRING, "ID", "FIRST_NAME", "LAST_NAME", "GPA");
        }

        // Convert GPA from int to float
        float gpa = student.gpa / 100.0;

        // Print student record
        printf(STUDENT_PRINT_FMT_STRING, student.id, student.fname, student.lname, gpa);
        records_found++;
    }

    // Handle read error
    if (read(fd, &student, sizeof(student_t)) == -1) {
        printf(M_ERR_DB_READ);
        return ERR_DB_FILE;
    }

    // If no records were printed, print database empty message
    if (records_found == 0) {
        printf(M_DB_EMPTY);
    }

    return NO_ERROR;
}

int print_db(int fd)
{
    // TODO
    printf(M_NOT_IMPL);
    return NOT_IMPLEMENTED_YET;
}

    // Validate student pointer
    if (s == NULL || s->id == 0) {
        printf(M_ERR_STD_PRINT);
        return;
    }

    // Convert GPA from int to float
    float gpa = s->gpa / 100.0;

    // Print table header
    printf(STUDENT_PRINT_HDR_STRING, "ID", "FIRST NAME", "LAST NAME", "GPA");

    // Print student record
    printf(STUDENT_PRINT_FMT_STRING, s->id, s->fname, s->lname, gpa);

void print_student(student_t *s)
{
    // TODO
    printf(M_NOT_IMPL);
}

/*
 *  NOTE IMPLEMENTING THIS FUNCTION IS EXTRA CREDIT - student.db file
 *
 *  compress_db
 *      fd:     linux file descriptor
 *
 *  This assignment takes advantage of the way Linux handles sparse files
 *  on disk. Thus if there is a large hole between student records, Linux
 *  will not use any physical storage.  However, when a database record is
 *  deleted storage is used to write a blank - see EMPTY_STUDENT_RECORD from
 *  db.h - record.
 *
 *  Since Linux provides no way to delete data in the middle of a file, and
 *  deleted records take up physical storage, this function will compress the
 *  database by rewriting a new database file that only includes valid student
 *  records. There are a number of ways to do this, but since this is extra credit
 *  you need to figure this out on your own.
 *
 *  At a high level create a temporary database file then copy all valid students from
 *  the active database (passed in via fd) to the temporary file. When this is done
 *  rename the temporary database file to the name of the real database file. See
 *  the constants in db.h for required file names:
 *
 *         #define DB_FILE     "student.db"        //name of database file
 *         #define TMP_DB_FILE ".tmp_student.db"   //for extra credit
 *
 *  Note that you are passed in the fd of the database file to be compressed,
 *  it is very likely you will need to close it to overwrite it with the
 *  compressed version of the file.  To ensure the caller can work with the
 *  compressed file after you create it, it is a good design to return the fd
 *  of the new compressed file from this function
 *
 *  returns:  <number>       returns the fd of the compressed database file
 *            ERR_DB_FILE    database file I/O issue
 *
 *
 *  console:  M_DB_COMPRESSED_OK  on success, the db was successfully compressed.
 *            M_ERR_DB_OPEN    error when opening/creating temporary database file.
 *                             this error should also be returned after you
 *                             compressed the database file and if you are unable
 *                             to open it to pass the fd back to the caller
 *            M_ERR_DB_CREATE  error creating the db file. For instance the
 *                             inability to copy the temporary file back as
 *                             the primary database file.
 *            M_ERR_DB_READ    error reading or seeking the the db or tempdb file
 *            M_ERR_DB_WRITE   error writing to db or tempdb file (adding student)
 *
 */
int compress_db(int fd)
{
    // TODO
    printf(M_NOT_IMPL);
    return fd;
}

/*
 *  validate_range
 *      id:  proposed student id
 *      gpa: proposed gpa
 *
 *  This function validates that the id and gpa are in the allowable ranges
 *  as per the specifications.  It checks if the values are within the
 *  inclusive range using constents in db.h
 *
 *  returns:    NO_ERROR       on success, both ID and GPA are in range
 *              EXIT_FAIL_ARGS if either ID or GPA is out of range
 *
 *  console:  This function does not produce any output
 *
 */
int validate_range(int id, int gpa)
{

    if ((id < MIN_STD_ID) || (id > MAX_STD_ID))
        return EXIT_FAIL_ARGS;

    if ((gpa < MIN_STD_GPA) || (gpa > MAX_STD_GPA))
        return EXIT_FAIL_ARGS;

    return NO_ERROR;
}

/*
 *  usage
 *      exename:  the name of the executable from argv[0]
 *
 *  Prints this programs expected usage
 *
 *  returns:    nothing, this is a void function
 *
 *  console:  This function prints the usage information
 *
 */
void usage(char *exename)
{
    printf("usage: %s -[h|a|c|d|f|p|z] options.  Where:\n", exename);
    printf("\t-h:  prints help\n");
    printf("\t-a id first_name last_name gpa(as 3 digit int):  adds a student\n");
    printf("\t-c:  counts the records in the database\n");
    printf("\t-d id:  deletes a student\n");
    printf("\t-f id:  finds and prints a student in the database\n");
    printf("\t-p:  prints all records in the student database\n");
    printf("\t-x:  compress the database file [EXTRA CREDIT]\n");
    printf("\t-z:  zero db file (remove all records)\n");
}

// Welcome to main()
int main(int argc, char *argv[])
{
    char opt;      // user selected option
    int fd;        // file descriptor of database files
    int rc;        // return code from various operations
    int exit_code; // exit code to shell
    int id;        // userid from argv[2]
    int gpa;       // gpa from argv[5]

    // space for a student structure which we will get back from
    // some of the functions we will be writing such as get_student(),
    // and print_student().
    student_t student = {0};

    // This function must have at least one arg, and the arg must start
    // with a dash
    if ((argc < 2) || (*argv[1] != '-'))
    {
        usage(argv[0]);
        exit(1);
    }

    // The option is the first character after the dash for example
    //-h -a -c -d -f -p -x -z
    opt = (char)*(argv[1] + 1); // get the option flag

    // handle the help flag and then exit normally
    if (opt == 'h')
    {
        usage(argv[0]);
        exit(EXIT_OK);
    }

    // now lets open the file and continue if there is no error
    // note we are not truncating the file using the second
    // parameter
    fd = open_db(DB_FILE, false);
    if (fd < 0)
    {
        exit(EXIT_FAIL_DB);
    }

    // set rc to the return code of the operation to ensure the program
    // use that to determine the proper exit_code.  Look at the header
    // sdbsc.h for expected values.

    exit_code = EXIT_OK;
    switch (opt)
    {
    case 'a':
        //   arv[0] arv[1]  arv[2]      arv[3]    arv[4]  arv[5]
        // prog_name     -a      id  first_name last_name     gpa
        //-------------------------------------------------------
        // example:  prog_name -a 1 John Doe 341
        if (argc != 6)
        {
            usage(argv[0]);
            exit_code = EXIT_FAIL_ARGS;
            break;
        }

        // convert id and gpa to ints from argv.  For this assignment assume
        // they are valid numbers
        id = atoi(argv[2]);
        gpa = atoi(argv[5]);

        exit_code = validate_range(id, gpa);
        if (exit_code == EXIT_FAIL_ARGS)
        {
            printf(M_ERR_STD_RNG);
            break;
        }

        rc = add_student(fd, id, argv[3], argv[4], gpa);
        if (rc < 0)
            exit_code = EXIT_FAIL_DB;

        break;

    case 'c':
        //    arv[0] arv[1]
        // prog_name     -c
        //-----------------
        // example:  prog_name -c
        rc = count_db_records(fd);
        if (rc < 0)
            exit_code = EXIT_FAIL_DB;
        break;

    case 'd':
        //   arv[0]  arv[1]  arv[2]
        // prog_name     -d      id
        //-------------------------
        // example:  prog_name -d 100
        if (argc != 3)
        {
            usage(argv[0]);
            exit_code = EXIT_FAIL_ARGS;
            break;
        }
        id = atoi(argv[2]);
        rc = del_student(fd, id);
        if (rc < 0)
            exit_code = EXIT_FAIL_DB;

        break;

    case 'f':
        //    arv[0] arv[1]  arv[2]
        // prog_name     -f      id
        //-------------------------
        // example:  prog_name -f 100
        if (argc != 3)
        {
            usage(argv[0]);
            exit_code = EXIT_FAIL_ARGS;
            break;
        }
        id = atoi(argv[2]);
        rc = get_student(fd, id, &student);

        switch (rc)
        {
        case NO_ERROR:
            print_student(&student);
            break;
        case SRCH_NOT_FOUND:
            printf(M_STD_NOT_FND_MSG, id);
            exit_code = EXIT_FAIL_DB;
            break;
        default:
            printf(M_ERR_DB_READ);
            exit_code = EXIT_FAIL_DB;
            break;
        }
        break;

    case 'p':
        //    arv[0] arv[1]
        // prog_name     -p
        //-----------------
        // example:  prog_name -p
        rc = print_db(fd);
        if (rc < 0)
            exit_code = EXIT_FAIL_DB;
        break;

    case 'x':
        //    arv[0] arv[1]
        // prog_name     -x
        //-----------------
        // example:  prog_name -x

        // remember compress_db returns a fd of the compressed database.
        // we close it after this switch statement
        fd = compress_db(fd);
        if (fd < 0)
            exit_code = EXIT_FAIL_DB;
        break;

    case 'z':
        //    arv[0] arv[1]
        // prog_name     -x
        //-----------------
        // example:  prog_name -x
        // HINT:  close the db file, we already have fd
        //       and reopen db indicating truncate=true
        close(fd);
        fd = open_db(DB_FILE, true);
        if (fd < 0)
        {
            exit_code = EXIT_FAIL_DB;
            break;
        }
        printf(M_DB_ZERO_OK);
        exit_code = EXIT_OK;
        break;
    default:
        usage(argv[0]);
        exit_code = EXIT_FAIL_ARGS;
    }

    // dont forget to close the file before exiting, and setting the
    // proper exit code - see the header file for expected values
    close(fd);
    exit(exit_code);
}
