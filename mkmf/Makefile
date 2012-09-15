SUBDIR	      = src x.Makefile man/mann

#DESTDIR	      =${HOME}
DESTDIR	      =
#MANDIR	      =${HOME}/man
MANDIR	      =

all:		${SUBDIR}

${SUBDIR}:	FRC
		cd $@; make ${MFLAGS}

install:
	for i in ${SUBDIR}; do \
		(cd $$i; make ${MFLAGS} DESTDIR=${DESTDIR} MANDIR=$(MANDIR) install); done

clean:
	for i in ${SUBDIR}; do (echo $$i; cd $$i; make ${MFLAGS} clean); done

depend:
	for i in ${SUBDIR}; do (echo $$i; cd $$i; make ${MFLAGS} depend); done

FRC:
