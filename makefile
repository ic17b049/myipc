##
## @file Makefile
## Betriebssysteme myipc Makefile
## Beispiel 3
##
## @author Dominic Schebeck <ic17b049@technikum-wien.at>
## @author Thomas Neugschwandtner <ic17b082@technikum-wien.at>
## @author Dominik Rychly <ic17b052@technikum-wien.at>
## @date 2018/06/11
##
## @version 1.0
##
## 
##
## ------------------------------------------------------------- variables --
##

CC=gcc52
CFLAGS= -Wall -Werror -Wextra -Wstrict-prototypes -Wformat=2 -pedantic -fno-common -ftrapv -O3 -g -std=gnu11
CP=cp
CD=cd
MV=mv
GREP=grep
DOXYGEN=doxygen

OBJECTS=sender.o empfaenger.o

EXCLUDE_PATTERN=footrulewidth

##
## ----------------------------------------------------------------- rules --
##

%.o: %.c
	$(CC) $(CFLAGS) -c $<

##
## --------------------------------------------------------------- targets --
##

.PHONY: all
all: sender empfaenger 

sender: $(OBJECTS) 
	$(CC) $(CFLAGS) sender.o ­lpthread –lrt -o sender

empfaenger: $(OBJECTS) 
	$(CC) $(CFLAGS) empfaenger.o ­lpthread –lrt -o empfaenger

.PHONY: clean
clean:
	$(RM) -f *.o *~ sender
	$(RM) -f *.o *~ empfaenger	

.PHONY: distclean
distclean: clean
	$(RM) -r doc

 doc: html pdf

.PHONY: html
html:
	$(DOXYGEN) Doxyfile.dcf

pdf: html
	$(CD) doc/pdf && \
	$(MV) refman.tex refman_save.tex && \
	$(GREP) -v $(EXCLUDE_PATTERN) refman_save.tex > refman.tex && \
	$(RM) refman_save.tex && \
	make && \
	$(MV) refman.pdf refman.save && \
	$(RM) *.pdf *.html *.tex *.aux *.sty *.log *.eps *.out *.ind *.idx \
	      *.ilg *.toc *.tps Makefile && \
	$(MV) refman.save refman.pdf

##
## ---------------------------------------------------------- dependencies --
##

##
## =================================================================== eof ==
##
