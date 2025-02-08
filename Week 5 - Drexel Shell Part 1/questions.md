1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**: fgets() is a good choice because it safely reads an entire line of input, preventing buffer overflow by allowing you to specify a maximum input size. It also handles cases like the user pressing Enter newline or EOF Ctrl+D well, which is important for a shell. 

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  malloc() is used to dynamically allocate memory for cmd_buff because the size of the input is not known in advance. Using malloc() allows you to allocate just enough memory to store the user's input making the program more flexible and memory-efficient. This avoids wasting memory with a fixed-size array, especially if the input size varies.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Trimming leading and trailing spaces is necessary to ensure that commands and arguments are correctly parsed. If we didn't trim spaces, commands like ls or echo hello would be treated as ls and echo hello with extra spaces, which could cause issues when trying to execute them.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  
    Output Redirection:
        This redirects the standard output of a command to a file, e.g., ls > output.txt. The challenge is parsing the command to separate the command and the file name, and then using system calls like open() or fopen() to write the output to the file.

    Input Redirection:
        This redirects the standard input of a command from a file, e.g., sort < input.txt. The challenge is to correctly handle file opening, ensuring that the command receives input from the specified file instead of from the terminal.

    Append Redirection:
        This appends output to a file rather than overwriting it, e.g., echo "Hello" >> output.txt. The challenge is similar to output redirection but requires ensuring that the file is opened in append mode, preventing the file's contents from being overwritten.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  
    
    Redirection involves changing the input or output of a command to or from a file. output.txt redirects the output of ls into a file. The shell handles this by writing to or reading from the file instead of the standard input or output.

    Piping connects the output of one command directly to the input of another using the | operator. ls | grep "file" sends the output of ls directly into grep. Unlike redirection, piping does not involve files; it allows multiple commands to communicate by passing data between them.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  It's important to keep STDERR and STDOUT separate because they serve different purposes. STDOUT is for regular output, while STDERR is for error messages. Keeping them separate allows users to handle errors and normal output differently so they dont get mixed up.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Our custom shell should handle errors from failed commands by providing clear feedback to the user. If a command outputs both STDOUT and STDERR, we should keep these streams separate by default, as they serve different purposes. we can provide an option to merge them, such as using the 2>&1 syntax in the command line, which would redirect STDERR to STDOUT. This allows both outputs to be handled together we