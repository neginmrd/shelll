This program illustrates a termianl shell with the following capabilities:
1. The built-in commands of the linux shell (commands such as ls, pwd, echo, etc)
2. Some custom commands related to file editing, which are as follows:
    a) "head10 <filename>" will print the first 10 lines of given file.
    b) "howmanylines <filename>" will print number of lines in given file.
    c) "headlines <filename>" prints the word each line starts with.
    d) "noSpace <filename>" prints content of the file, except all white space is removed from it.
    e) "noComment <filename>" prints content of the file, except lines that start with # character (which are comments).
    f) "freq <filename>" will print out the one word that is most repeated in the file.
3. Supports pipelining (both for built-in commands and custom commands)
4. With the use of "readline" library, this program supports tab completion, and the user can move between previously run commands using up and down arrow keys.
(All entered commands from all sessions are stored in a history file within the same directory as the program.)
5. The use of Ctrl+C in this shell will not close the terminal, only cancels the current command and prompts the user for next command. (cancellation for current command)


