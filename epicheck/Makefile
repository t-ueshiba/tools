#
#  $Id: Makefile,v 1.4 2009-07-08 04:21:57 ueshiba Exp $
#
#################################
#  User customizable macros	#
#################################
DEST		= $(PREFIX)/bin
INCDIRS		= -I$(PREFIX)/include

PROGRAM		= epicheck
LIBS		= -lTUv++ -lTUCalib++ -lTUTools++ \
		  -lXaw3d -lXt -lXext -lXmu -lX11

CPPFLAGS	=
CFLAGS		= -g
ifeq ($(CCC), icpc)
  ifeq ($(OSTYPE), darwin)
    CFLAGS	= -O3 -axP -ip
  else
    CFLAGS	= -O3 -xN -ip
  endif
endif
CCFLAGS		= $(CFLAGS)
LDFLAGS		= -L$(LIBDIR) -L$(X11HOME)/lib

LINKER		= $(CCC)

#########################
#  Macros set by mkmf	#
#########################
SUFFIX		= .cc:sC
EXTHDRS		= /usr/local/include/TU/Array++.h \
		/usr/local/include/TU/BlockMatrix++.h \
		/usr/local/include/TU/Calib++.h \
		/usr/local/include/TU/Camera.h \
		/usr/local/include/TU/Geometry++.h \
		/usr/local/include/TU/Image++.h \
		/usr/local/include/TU/List.h \
		/usr/local/include/TU/Manip.h \
		/usr/local/include/TU/Minimize.h \
		/usr/local/include/TU/Normalize.h \
		/usr/local/include/TU/Vector++.h \
		/usr/local/include/TU/types.h \
		/usr/local/include/TU/v/App.h \
		/usr/local/include/TU/v/CanvasPane.h \
		/usr/local/include/TU/v/CanvasPaneDC.h \
		/usr/local/include/TU/v/CmdPane.h \
		/usr/local/include/TU/v/CmdWindow.h \
		/usr/local/include/TU/v/Colormap.h \
		/usr/local/include/TU/v/DC.h \
		/usr/local/include/TU/v/Menu.h \
		/usr/local/include/TU/v/TUv++.h \
		/usr/local/include/TU/v/Widget-Xaw.h \
		/usr/local/include/TU/v/XDC.h
HDRS		= epicheck.h
SRCS		= MyCanvasPane.cc \
		MyCmdWindow.cc \
		main.cc \
		usage.cc
OBJS		= MyCanvasPane.o \
		MyCmdWindow.o \
		main.o \
		usage.o

#########################
#  Macros used by RCS	#
#########################
REV		= $(shell echo $Revision: 1.4 $	|		\
		  sed 's/evision://'		|		\
		  awk -F"."					\
		  '{						\
		      for (count = 1; count < NF; count++)	\
			  printf("%d.", $$count);		\
		      printf("%d", $$count + 1);		\
		  }')

include $(PROJECT)/lib/p.mk

###
MyCanvasPane.o: epicheck.h /usr/local/include/TU/v/CmdWindow.h \
	/usr/local/include/TU/v/TUv++.h /usr/local/include/TU/v/Colormap.h \
	/usr/local/include/TU/Image++.h /usr/local/include/TU/Geometry++.h \
	/usr/local/include/TU/Vector++.h /usr/local/include/TU/Array++.h \
	/usr/local/include/TU/types.h /usr/local/include/TU/Normalize.h \
	/usr/local/include/TU/List.h /usr/local/include/TU/v/Widget-Xaw.h \
	/usr/local/include/TU/v/CmdPane.h \
	/usr/local/include/TU/v/CanvasPane.h \
	/usr/local/include/TU/v/CanvasPaneDC.h /usr/local/include/TU/v/XDC.h \
	/usr/local/include/TU/v/DC.h /usr/local/include/TU/Manip.h \
	/usr/local/include/TU/v/Menu.h
MyCmdWindow.o: /usr/local/include/TU/v/App.h \
	/usr/local/include/TU/v/TUv++.h /usr/local/include/TU/v/Colormap.h \
	/usr/local/include/TU/Image++.h /usr/local/include/TU/Geometry++.h \
	/usr/local/include/TU/Vector++.h /usr/local/include/TU/Array++.h \
	/usr/local/include/TU/types.h /usr/local/include/TU/Normalize.h \
	/usr/local/include/TU/List.h /usr/local/include/TU/v/Widget-Xaw.h \
	epicheck.h /usr/local/include/TU/v/CmdWindow.h \
	/usr/local/include/TU/v/CmdPane.h \
	/usr/local/include/TU/v/CanvasPane.h \
	/usr/local/include/TU/v/CanvasPaneDC.h /usr/local/include/TU/v/XDC.h \
	/usr/local/include/TU/v/DC.h /usr/local/include/TU/Manip.h \
	/usr/local/include/TU/v/Menu.h
main.o: /usr/local/include/TU/v/App.h /usr/local/include/TU/v/TUv++.h \
	/usr/local/include/TU/v/Colormap.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/Geometry++.h /usr/local/include/TU/Vector++.h \
	/usr/local/include/TU/Array++.h /usr/local/include/TU/types.h \
	/usr/local/include/TU/Normalize.h /usr/local/include/TU/List.h \
	/usr/local/include/TU/v/Widget-Xaw.h /usr/local/include/TU/Calib++.h \
	/usr/local/include/TU/Camera.h /usr/local/include/TU/BlockMatrix++.h \
	/usr/local/include/TU/Minimize.h epicheck.h \
	/usr/local/include/TU/v/CmdWindow.h /usr/local/include/TU/v/CmdPane.h \
	/usr/local/include/TU/v/CanvasPane.h \
	/usr/local/include/TU/v/CanvasPaneDC.h /usr/local/include/TU/v/XDC.h \
	/usr/local/include/TU/v/DC.h /usr/local/include/TU/Manip.h \
	/usr/local/include/TU/v/Menu.h
