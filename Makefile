SRC=systeml.c systeml.h

systeml: systeml.c systeml.h
		${CC} systeml.c -o systeml
