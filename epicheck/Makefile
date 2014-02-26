#
#  $Id: Makefile,v 1.14 2012-08-30 01:14:06 ueshiba Exp $
#
#################################
#  User customizable macros	#
#################################
DEST		= $(PREFIX)/bin
INCDIRS		= -I$(PREFIX)/include

PROGRAM		= $(shell basename $(PWD))
LIBS		= -lTUv++ -lTUTools++ -lXaw3d -lXt -lXext -lXmu -lX11

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

LINKER		= $(CXX)

#########################
#  Macros set by mkmf	#
#########################
.SUFFIXES:	.cu
SUFFIX		= .cc:sC .cu:sC .cpp:sC
EXTHDRS		= /usr/local/include/TU/Array++.h \
		/usr/local/include/TU/Geometry++.h \
		/usr/local/include/TU/Image++.h \
		/usr/local/include/TU/List.h \
		/usr/local/include/TU/Manip.h \
		/usr/local/include/TU/Minimize.h \
		/usr/local/include/TU/Vector++.h \
		/usr/local/include/TU/functional.h \
		/usr/local/include/TU/iterator.h \
		/usr/local/include/TU/mmInstructions.h \
		/usr/local/include/TU/types.h \
		/usr/local/include/TU/v/App.h \
		/usr/local/include/TU/v/CanvasPane.h \
		/usr/local/include/TU/v/CanvasPaneDC.h \
		/usr/local/include/TU/v/CmdPane.h \
		/usr/local/include/TU/v/CmdWindow.h \
		/usr/local/include/TU/v/Colormap.h \
		/usr/local/include/TU/v/DC.h \
		/usr/local/include/TU/v/Menu.h \
		/usr/local/include/TU/v/ShmDC.h \
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

include $(PROJECT)/lib/p.mk

###
MyCanvasPane.o: epicheck.h /usr/local/include/TU/v/CmdWindow.h \
	/usr/local/include/TU/v/TUv++.h /usr/local/include/TU/v/Colormap.h \
	/usr/local/include/TU/Image++.h /usr/local/include/TU/Geometry++.h \
	/usr/local/include/TU/iterator.h /usr/local/include/TU/functional.h \
	/usr/local/include/TU/Vector++.h /usr/local/include/TU/Array++.h \
	/usr/local/include/TU/types.h /usr/local/include/TU/mmInstructions.h \
	/usr/local/include/TU/Minimize.h /usr/local/include/TU/List.h \
	/usr/local/include/TU/v/Widget-Xaw.h \
	/usr/local/include/TU/v/CmdPane.h \
	/usr/local/include/TU/v/CanvasPane.h /usr/local/include/TU/v/ShmDC.h \
	/usr/local/include/TU/v/CanvasPaneDC.h /usr/local/include/TU/v/XDC.h \
	/usr/local/include/TU/v/DC.h /usr/local/include/TU/Manip.h \
	/usr/local/include/TU/v/Menu.h
MyCmdWindow.o: /usr/local/include/TU/v/App.h \
	/usr/local/include/TU/v/TUv++.h /usr/local/include/TU/v/Colormap.h \
	/usr/local/include/TU/Image++.h /usr/local/include/TU/Geometry++.h \
	/usr/local/include/TU/iterator.h /usr/local/include/TU/functional.h \
	/usr/local/include/TU/Vector++.h /usr/local/include/TU/Array++.h \
	/usr/local/include/TU/types.h /usr/local/include/TU/mmInstructions.h \
	/usr/local/include/TU/Minimize.h /usr/local/include/TU/List.h \
	/usr/local/include/TU/v/Widget-Xaw.h epicheck.h \
	/usr/local/include/TU/v/CmdWindow.h /usr/local/include/TU/v/CmdPane.h \
	/usr/local/include/TU/v/CanvasPane.h /usr/local/include/TU/v/ShmDC.h \
	/usr/local/include/TU/v/CanvasPaneDC.h /usr/local/include/TU/v/XDC.h \
	/usr/local/include/TU/v/DC.h /usr/local/include/TU/Manip.h \
	/usr/local/include/TU/v/Menu.h
main.o: /usr/local/include/TU/v/App.h /usr/local/include/TU/v/TUv++.h \
	/usr/local/include/TU/v/Colormap.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/Geometry++.h /usr/local/include/TU/iterator.h \
	/usr/local/include/TU/functional.h /usr/local/include/TU/Vector++.h \
	/usr/local/include/TU/Array++.h /usr/local/include/TU/types.h \
	/usr/local/include/TU/mmInstructions.h \
	/usr/local/include/TU/Minimize.h /usr/local/include/TU/List.h \
	/usr/local/include/TU/v/Widget-Xaw.h epicheck.h \
	/usr/local/include/TU/v/CmdWindow.h /usr/local/include/TU/v/CmdPane.h \
	/usr/local/include/TU/v/CanvasPane.h /usr/local/include/TU/v/ShmDC.h \
	/usr/local/include/TU/v/CanvasPaneDC.h /usr/local/include/TU/v/XDC.h \
	/usr/local/include/TU/v/DC.h /usr/local/include/TU/Manip.h \
	/usr/local/include/TU/v/Menu.h
