#
#  $Id: Makefile,v 1.7 2009-09-21 07:09:39 ueshiba Exp $
#
#################################
#  User customizable macros	#
#################################
DEST		= $(PREFIX)/bin
INCDIRS		= -I$(PREFIX)/include -I$(HOME)/stereo/corrStereo

PROGRAM		= $(shell basename $(PWD))
LIBS		= -lTUTools++
ifeq ($(OSTYPE), darwin)
  LIBS	       +=
#  LIBS	       += -framework IOKit -framework CoreFoundation
endif

CPPFLAGS	=
CFLAGS		= -g
NVCCFLAGS	= -g
ifeq ($(CCC), icpc)
  CFLAGS	= -O3
  NVCCFLAGS	= -O		# -O2以上にするとコンパイルエラーになる．
  ifeq ($(OSTYPE), darwin)
    CPPFLAGS   += -DSSE3
  else
    CPPFLAGS   += -DSSE3
    CFLAGS     += -xSSE3
  endif
endif
CCFLAGS		= $(CFLAGS)

LINKER		= $(CCC)

#########################
#  Macros set by mkmf	#
#########################
.SUFFIXES:	.cu
SUFFIX		= .cc:sC .cu:sC
EXTHDRS		= /Users/ueshiba/stereo/corrStereo/Rectify.h \
		/usr/local/include/TU/Array++.h \
		/usr/local/include/TU/Camera.h \
		/usr/local/include/TU/Geometry++.h \
		/usr/local/include/TU/Image++.h \
		/usr/local/include/TU/Normalize.h \
		/usr/local/include/TU/Thread++.h \
		/usr/local/include/TU/Vector++.h \
		/usr/local/include/TU/Warp.h \
		/usr/local/include/TU/mmInstructions.h \
		/usr/local/include/TU/types.h \
		/usr/local/include/TU/utility.h
HDRS		=
SRCS		= Rectify.cc \
		main.cc
OBJS		= Rectify.o \
		main.o

#########################
#  Macros used by RCS	#
#########################
REV		= $(shell echo $Revision: 1.7 $	|		\
		  sed 's/evision://'		|		\
		  awk -F"."					\
		  '{						\
		      for (count = 1; count < NF; count++)	\
			  printf("%d.", $$count);		\
		      printf("%d", $$count + 1);		\
		  }')

include $(PROJECT)/lib/p.mk

###
Rectify.o: /usr/local/include/TU/utility.h \
	/Users/ueshiba/stereo/corrStereo/Rectify.h \
	/usr/local/include/TU/Warp.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/Geometry++.h /usr/local/include/TU/Vector++.h \
	/usr/local/include/TU/Array++.h /usr/local/include/TU/types.h \
	/usr/local/include/TU/Normalize.h /usr/local/include/TU/Camera.h \
	/usr/local/include/TU/mmInstructions.h \
	/usr/local/include/TU/Thread++.h
main.o: /Users/ueshiba/stereo/corrStereo/Rectify.h \
	/usr/local/include/TU/Warp.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/Geometry++.h /usr/local/include/TU/Vector++.h \
	/usr/local/include/TU/Array++.h /usr/local/include/TU/types.h \
	/usr/local/include/TU/Normalize.h /usr/local/include/TU/Camera.h \
	/usr/local/include/TU/mmInstructions.h \
	/usr/local/include/TU/Thread++.h
