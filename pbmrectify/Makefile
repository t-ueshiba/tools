#
#  $Id: Makefile,v 1.2 2008-09-08 08:09:38 ueshiba Exp $
#
#################################
#  User customizable macros	#
#################################
DEST		= $(BINDIR)
INCDIRS		= -I$(HOME)/include -I$(HOME)/stereo/corrStereo

PROGRAM		= pbmrectify
LIBS		= -lTUTools++

CPPFLAGS	=
CFLAGS		= -g
ifeq ($(CCC), icpc)
  ifeq ($(OSTYPE), darwin)
    CFLAGS	= -O3 -axP -parallel -ip
  else
    CFLAGS	= -O3 -tpp7 -xW -parallel -ip
  endif
endif
CCFLAGS		= $(CFLAGS)
LDFLAGS		= -L$(LIBDIR)

LINKER		= $(CCC)

#########################
#  Macros set by mkmf	#
#########################
SUFFIX		= .cc:sC
EXTHDRS		= /home/ueshiba/include/TU/Array++.h \
		/home/ueshiba/include/TU/Camera.h \
		/home/ueshiba/include/TU/Geometry++.h \
		/home/ueshiba/include/TU/Image++.h \
		/home/ueshiba/include/TU/Thread++.h \
		/home/ueshiba/include/TU/Vector++.h \
		/home/ueshiba/include/TU/Warp.h \
		/home/ueshiba/include/TU/types.h \
		/home/ueshiba/include/TU/utility.h \
		/home/ueshiba/stereo/corrStereo/Rectify.h
HDRS		=
SRCS		= Rectify.cc \
		main.cc
OBJS		= Rectify.o \
		main.o

#########################
#  Macros used by RCS	#
#########################
REV		= $(shell echo $Revision: 1.2 $	|		\
		  sed 's/evision://'		|		\
		  awk -F"."					\
		  '{						\
		      for (count = 1; count < NF; count++)	\
			  printf("%d.", $$count);		\
		      printf("%d", $$count + 1);		\
		  }')

include $(PROJECT)/lib/p.mk

###
Rectify.o: /home/ueshiba/include/TU/utility.h \
	/home/ueshiba/stereo/corrStereo/Rectify.h \
	/home/ueshiba/include/TU/Warp.h /home/ueshiba/include/TU/Image++.h \
	/home/ueshiba/include/TU/Geometry++.h \
	/home/ueshiba/include/TU/Vector++.h \
	/home/ueshiba/include/TU/Array++.h /home/ueshiba/include/TU/types.h \
	/home/ueshiba/include/TU/Camera.h /home/ueshiba/include/TU/Thread++.h
main.o: /home/ueshiba/stereo/corrStereo/Rectify.h \
	/home/ueshiba/include/TU/Warp.h /home/ueshiba/include/TU/Image++.h \
	/home/ueshiba/include/TU/Geometry++.h \
	/home/ueshiba/include/TU/Vector++.h \
	/home/ueshiba/include/TU/Array++.h /home/ueshiba/include/TU/types.h \
	/home/ueshiba/include/TU/Camera.h /home/ueshiba/include/TU/Thread++.h
