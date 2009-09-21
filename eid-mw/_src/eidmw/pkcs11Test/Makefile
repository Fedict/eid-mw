# Makefile for the PKCS#11 integration tests
# Platform: Linux

.PHONY : all
all : open-close-session close-twice

open-close-session : open-close-session.c

close-twice : close-twice.c

% : %.c
	gcc -o $@ $< -ldl


.PHONY : clean
clean :
	rm -f open-close-session close-twice *~
