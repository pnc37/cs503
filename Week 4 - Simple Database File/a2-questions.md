## Assignment 2 Questions

#### Directions
Please answer the following questions and submit in your repo for the second assignment.  Please keep the answers as short and concise as possible.

1. In this assignment I asked you provide an implementation for the `get_student(...)` function because I think it improves the overall design of the database application.   After you implemented your solution do you agree that externalizing `get_student(...)` into it's own function is a good design strategy?  Briefly describe why or why not.

    > **Answer**:  Yes, externalizing get_student() into its own function is a good design strategy. It improves code organization by encapsulating the logic for fetching a student, making the code more modular, easier to maintain, and reusable. if changes are needed in how students are fetched from the database, updating a single function is more efficient than modifying multiple occurrences throughout the codebase. This approach aligns with best practices in software design by keeping functions focused on specific tasks and improving overall application structure.

2. Another interesting aspect of the `get_student(...)` function is how its function prototype requires the caller to provide the storage for the `student_t` structure:

    ```c
    int get_student(int fd, int id, student_t *s);
    ```

    Notice that the last parameter is a pointer to storage **provided by the caller** to be used by this function to populate information about the desired student that is queried from the database file. This is a common convention (called pass-by-reference) in the `C` programming language. 

    In other programming languages an approach like the one shown below would be more idiomatic for creating a function like `get_student()` (specifically the storage is provided by the `get_student(...)` function itself):

    ```c
    //Lookup student from the database
    // IF FOUND: return pointer to student data
    // IF NOT FOUND: return NULL
    student_t *get_student(int fd, int id){
        student_t student;
        bool student_found = false;
        
        //code that looks for the student and if
        //found populates the student structure
        //The found_student variable will be set
        //to true if the student is in the database
        //or false otherwise.

        if (student_found)
            return &student;
        else
            return NULL;
    }
    ```
    Can you think of any reason why the above implementation would be a **very bad idea** using the C programming language?  Specifically, address why the above code introduces a subtle bug that could be hard to identify at runtime? 

    > **ANSWER:** it returns a pointer to a local variable (&student), which is a stack-allocated variable. When the function exits, the local variable student goes out of scope, causing the pointer to become a dangling pointer. Accessing this memory after the function returns leads to undefined behavior, which can cause crashes or unpredictable results at runtime. The issue is particularly dangerous because the program may appear to work correctly in some cases but fail unpredictably due to other functions overwriting the same stack space. A safer approach would be to allocate memory dynamically using malloc() and require the caller to free it when no longer needed or, as done in the original function prototype, have the caller provide storage for the student structure.

3. Another way the `get_student(...)` function could be implemented is as follows:

    ```c
    //Lookup student from the database
    // IF FOUND: return pointer to student data
    // IF NOT FOUND or memory allocation error: return NULL
    student_t *get_student(int fd, int id){
        student_t *pstudent;
        bool student_found = false;

        pstudent = malloc(sizeof(student_t));
        if (pstudent == NULL)
            return NULL;
        
        //code that looks for the student and if
        //found populates the student structure
        //The found_student variable will be set
        //to true if the student is in the database
        //or false otherwise.

        if (student_found){
            return pstudent;
        }
        else {
            free(pstudent);
            return NULL;
        }
    }
    ```
    In this implementation the storage for the student record is allocated on the heap using `malloc()` and passed back to the caller when the function returns. What do you think about this alternative implementation of `get_student(...)`?  Address in your answer why it work work, but also think about any potential problems it could cause.  
    
    > **ANSWER:** This implementation of get_student(...) works in the sense that it dynamically allocates memory for the student structure on the heap using malloc(), which ensures that the returned pointer remains valid after the function exits, unlike the previous approach where the pointer was to a local variable. It introduces potential issues related to memory management. Since the function allocates memory, the caller is responsible for freeing it to prevent memory leaks. If the caller forgets to call free(), the program will slowly consume more memory over time, leading to resource exhaustion. Frequent memory allocations and deallocations can lead to memory fragmentation, which can degrade performance in long-running programs. A more efficient and safer approach is to let the caller provide storage, as seen in the original function prototype, ensuring better control over memory management.


4. Lets take a look at how storage is managed for our simple database. Recall that all student records are stored on disk using the layout of the `student_t` structure (which has a size of 64 bytes).  Lets start with a fresh database by deleting the `student.db` file using the command `rm ./student.db`.  Now that we have an empty database lets add a few students and see what is happening under the covers.  Consider the following sequence of commands:

    ```bash
    > ./sdbsc -a 1 john doe 345
    > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 128 Jan 17 10:01 ./student.db
    > du -h ./student.db
        4.0K    ./student.db
    > ./sdbsc -a 3 jane doe 390
    > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 256 Jan 17 10:02 ./student.db
    > du -h ./student.db
        4.0K    ./student.db
    > ./sdbsc -a 63 jim doe 285 
    > du -h ./student.db
        4.0K    ./student.db
    > ./sdbsc -a 64 janet doe 310
    > du -h ./student.db
        8.0K    ./student.db
    > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 4160 Jan 17 10:03 ./student.db
    ```

    For this question I am asking you to perform some online research to investigate why there is a difference between the size of the file reported by the `ls` command and the actual storage used on the disk reported by the `du` command.  Understanding why this happens by design is important since all good systems programmers need to understand things like how linux creates sparse files, and how linux physically stores data on disk using fixed block sizes.  Some good google searches to get you started: _"lseek syscall holes and sparse files"_, and _"linux file system blocks"_.  After you do some research please answer the following:

    - Please explain why the file size reported by the `ls` command was 128 bytes after adding student with ID=1, 256 after adding student with ID=3, and 4160 after adding the student with ID=64? 

        > **ANSWER:** Due to sparse files in Linux. Sparse files are files that contain "holes" (unallocated regions) where no data has been written. When adding students with non-sequential IDs, the database file likely uses lseek() to create gaps in the file without actually allocating disk space for them. When adding student ID=1, the file size is 128 bytes likely due to padding or metadata alignment beyond the 64-byte student_t structure. When adding student ID=3, the size increases to 256 bytes, accounting for another 64-byte record while skipping ID=2, possibly filling it with a hole. However, the real effect of sparse file behavior becomes evident when adding student ID=64. The file size jumps to 4160 bytes because lseek() moves the file pointer far beyond previously written data, creating a large hole. The ls command reports the logical file size (total offset of the last write), while du shows actual disk blocks allocated. Because Linux file systems allocate disk space in fixed block sizes commonly 4KB, du initially reports 4KB usage, but once sufficient data is written beyond that, it jumps to 8KB as new blocks are allocated.

    -   Why did the total storage used on the disk remain unchanged when we added the student with ID=1, ID=3, and ID=63, but increased from 4K to 8K when we added the student with ID=64? 

        > **ANSWER:** The total storage used on disk remained unchanged at 4KB when adding students with ID=1, ID=3, and ID=63 because the file system was utilizing a sparse file mechanism. When records were added at non-sequential offsets, the gaps between them (holes) were not physically allocated on disk, meaning the actual storage footprint did not increase. The ls command reported a growing logical file size, but du continued to show 4KB because the data written still fit within a single allocated disk block. When student ID=64 was added, the file size jumped to 4160 bytes, surpassing the 4KB block size threshold. At this point, the file system allocated another 4KB block, increasing actual disk usage from 4KB to 8KB

    - Now lets add one more student with a large student ID number  and see what happens:

        ```bash
        > ./sdbsc -a 99999 big dude 205 
        > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 6400000 Jan 17 10:28 ./student.db
        > du -h ./student.db
        12K     ./student.db
        ```
        We see from above adding a student with a very large student ID (ID=99999) increased the file size to 6400000 as shown by `ls` but the raw storage only increased to 12K as reported by `du`.  Can provide some insight into why this happened?

        > **ANSWER:**  Due to sparse file behavior combined with Linux's file system block allocation. The file system is using sparse file allocation. When adding a student with ID=99999, the program likely used lseek() to move the file pointer to the corresponding offset (99999 * 64 = 6,399,936 bytes) before writing data. Since lseek() does not physically allocate space, the logical file size reported by ls jumped to 6,400,000 bytes, reflecting the offset of the last write. Since most of this space consists of unallocated "holes," the actual disk usage, as reported by du, increased only slightly to 12KB.