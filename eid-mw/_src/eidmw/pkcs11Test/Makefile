# Makefile for the PKCS#11 integration tests
# Platform: Linux

.PHONY : all
all : open-close-session

open-close-session : open-close-session.c
	gcc -o open-close-session open-close-session.c -ldl

.PHONY : clean
clean :
	rm -f open-close-session *~
