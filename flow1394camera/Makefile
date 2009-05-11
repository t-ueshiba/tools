#
#  $Id: Makefile,v 1.3 2009-05-11 04:00:58 ueshiba Exp $
#
#################################
#  User customizable macros	#
#################################
DEST		= $(BINDIR)
INCDIRS		= -I$(HOME)/include -I/usr/local/include

PROGRAM		= flow1394camera
LIBS		= -lTUIeee1394++ -lTUTools++ -lraw1394
ifeq ($(OSTYPE), darwin)
  LIBS	       += -framework IOKit -framework CoreFoundation
endif
CPPFLAGS	= -DHAVE_LIBTUTOOLS__ -DMONO_IMAGE
CFLAGS		= -O3
CCFLAGS		= -O3
LDFLAGS		= -L$(LIBDIR)

LINKER		= $(CCC)

#########################
#  Macros set by mkmf	#
#########################
SUFFIX		= .cc:sC
EXTHDRS		= /Users/ueshiba/include/TU/Array++.h \
		/Users/ueshiba/include/TU/Geometry++.h \
		/Users/ueshiba/include/TU/Ieee1394++.h \
		/Users/ueshiba/include/TU/Ieee1394CameraArray.h \
		/Users/ueshiba/include/TU/Image++.h \
		/Users/ueshiba/include/TU/Normalize.h \
		/Users/ueshiba/include/TU/Vector++.h \
		/Users/ueshiba/include/TU/types.h
HDRS		=
SRCS		= main.cc
OBJS		= main.o

#########################
#  Macros used by RCS	#
#########################
REV		= $(shell echo $Revision: 1.3 $	|		\
		  sed 's/evision://'		|		\
		  awk -F"."					\
		  '{						\
		      for (count = 1; count < NF; count++)	\
			  printf("%d.", $$count);		\
		      printf("%d", $$count + 1);		\
		  }')

include $(PROJECT)/lib/p.mk
include $(PROJECT)/lib/RCS.mk
###
main.o: /Users/ueshiba/include/TU/Ieee1394CameraArray.h \
	/Users/ueshiba/include/TU/Array++.h /Users/ueshiba/include/TU/types.h \
	/Users/ueshiba/include/TU/Ieee1394++.h \
	/Users/ueshiba/include/TU/Image++.h \
	/Users/ueshiba/include/TU/Geometry++.h \
	/Users/ueshiba/include/TU/Vector++.h \
	/Users/ueshiba/include/TU/Normalize.h
