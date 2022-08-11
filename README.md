## This is systeml - a system(3) replacement
designed to be non-blocking.
It returns the exit status of the shell.
As with all of the 'l' family types of functions a length in nanoseconds has to be
specified to ensure the integrity of the calling process (as opposed to the buffer
length in related functions).