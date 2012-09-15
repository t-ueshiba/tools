#
#  $Id: Makefile,v 1.1 2012-09-15 07:42:52 ueshiba Exp $
#
#################################
#  User customizable macros	#
#################################
DEST		= $(PREFIX)/bin
INCDIRS		= -I$(PREFIX)/include

PROGRAM		= mkmf
LIBS		=
ifeq ($(OSTYPE), darwin)
  LIBS	       +=
#  LIBS	       += -framework IOKit -framework CoreFoundation -framework CoreServices
endif

CPPFLAGS	=
CFLAGS		= -g
NVCCFLAGS	= -g
ifeq ($(CXX), icpc)
  CFLAGS	= -O3
  NVCCFLAGS	= -O		# -O2以上にするとコンパイルエラーになる．
  CPPFLAGS     += -DSSE3
endif
CCFLAGS		= $(CFLAGS)

LINKER		= $(CC)

#########################
#  Macros set by mkmf	#
#########################
.SUFFIXES:	.cu
SUFFIX		= .cc:sC .cu:sC .cpp:sC
EXTHDRS		=
HDRS		= Mkmf.h \
		defaultrul.h \
		defaultsfx.h \
		dlist.h \
		getarg.h \
		hash.h \
		macro.h \
		null.h \
		path.h \
		rule.h \
		sccsid.h \
		slist.h \
		spms.h \
		suffix.h \
		system.h \
		target.h \
		yesno.h
SRCS		= Mkmf.c \
		badopt.c \
		buildlist.c \
		depend.c \
		dlist.c \
		editmf.c \
		getcwp.c \
		gethdrdirs.c \
		getproject.c \
		hash.c \
		iolin.c \
		macro.c \
		misc.c \
		mustfopen.c \
		optpath.c \
		pathcat.c \
		pathhead.c \
		pperror.c \
		rule.c \
		slappend.c \
		slinit.c \
		slsort.c \
		strpcpy.c \
		strsav.c \
		suffix.c \
		usage.c \
		warn.c \
		warn2.c \
		warns.c
OBJS		= Mkmf.o \
		badopt.o \
		buildlist.o \
		depend.o \
		dlist.o \
		editmf.o \
		getcwp.o \
		gethdrdirs.o \
		getproject.o \
		hash.o \
		iolin.o \
		macro.o \
		misc.o \
		mustfopen.o \
		optpath.o \
		pathcat.o \
		pathhead.o \
		pperror.o \
		rule.o \
		slappend.o \
		slinit.o \
		slsort.o \
		strpcpy.o \
		strsav.o \
		suffix.o \
		usage.o \
		warn.o \
		warn2.o \
		warns.o

include $(PROJECT)/lib/p.mk

###
Mkmf.o: sccsid.h Mkmf.h getarg.h hash.h null.h path.h target.h slist.h \
	suffix.h system.h yesno.h defaultrul.h defaultsfx.h
badopt.o: null.h
buildlist.o: Mkmf.h hash.h null.h path.h slist.h suffix.h system.h yesno.h \
	macro.h
depend.o: Mkmf.h dlist.h hash.h macro.h null.h path.h slist.h system.h \
	yesno.h
dlist.o: Mkmf.h dlist.h hash.h null.h slist.h yesno.h
editmf.o: Mkmf.h dlist.h hash.h macro.h null.h slist.h system.h yesno.h
getcwp.o: null.h
gethdrdirs.o: null.h
getproject.o: null.h
hash.o: null.h hash.h macro.h
iolin.o: yesno.h
macro.o: Mkmf.h hash.h macro.h null.h slist.h yesno.h
misc.o: Mkmf.h hash.h macro.h null.h path.h target.h spms.h suffix.h \
	system.h yesno.h
optpath.o: path.h
pathcat.o: path.h
pathhead.o: null.h path.h
rule.o: Mkmf.h null.h rule.h slist.h suffix.h system.h yesno.h
slappend.o: macro.h null.h slist.h
slinit.o: null.h slist.h
slsort.o: null.h slist.h yesno.h
strsav.o: null.h
suffix.o: Mkmf.h hash.h macro.h null.h suffix.h yesno.h
usage.o: null.h
warn.o: null.h
warn2.o: null.h
warns.o: null.h
