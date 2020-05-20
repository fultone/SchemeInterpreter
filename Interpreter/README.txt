README.txt

Interpreter Project: Final Portion

For the bonus, we did the load function. To use it you call (load "fileName.rkt")
and it tokenizes, parses, and interprets that file as if the text of that file was
replacing the load command.

To implement, we used the C function "freopen()" to put the file name given as a parameter
to "load" as an argument to stdin.
