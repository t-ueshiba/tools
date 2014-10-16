#
#  $Id: Makefile,v 1.3 2012-08-30 01:14:09 ueshiba Exp $
#
#################################
#  User customizable macros	#
#################################
DEST		= $(PREFIX)/bin
INCDIRS		= -I../flow1394camera -I$(PREFIX)/include

PROGRAM		= $(shell basename $(PWD))
LIBS		= -lTUvV4L2++ -lTUV4L2++ -lTUv++ -lTUTools++ \
		  -lXaw3d -lXv -lXt -lXext -lXmu -lX11
ifeq ($(OSTYPE), darwin)
  LIBS	       +=
#  LIBS	       += -framework IOKit -framework CoreFoundation -framework CoreServices
endif

CPPFLAGS	= -DHAVE_LIBTUTOOLS__ -DNDEBUG
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
EXTHDRS		= ../flow1394camera/CaptureAndSave.h \
		../flow1394camera/MyCmdWindow.h \
		../flow1394camera/flowcamera.h \
		/usr/local/include/TU/Array++.h \
		/usr/local/include/TU/Geometry++.h \
		/usr/local/include/TU/Image++.h \
		/usr/local/include/TU/List.h \
		/usr/local/include/TU/Minimize.h \
		/usr/local/include/TU/V4L2++.h \
		/usr/local/include/TU/V4L2CameraArray.h \
		/usr/local/include/TU/Vector++.h \
		/usr/local/include/TU/functional.h \
		/usr/local/include/TU/io.h \
		/usr/local/include/TU/iterator.h \
		/usr/local/include/TU/mmInstructions.h \
		/usr/local/include/TU/tuple.h \
		/usr/local/include/TU/types.h \
		/usr/local/include/TU/v/App.h \
		/usr/local/include/TU/v/CmdPane.h \
		/usr/local/include/TU/v/CmdWindow.h \
		/usr/local/include/TU/v/Colormap.h \
		/usr/local/include/TU/v/TUv++.h \
		/usr/local/include/TU/v/Timer.h \
		/usr/local/include/TU/v/Widget-Xaw.h \
		/usr/local/include/TU/v/vV4L2++.h
HDRS		=
SRCS		= CaptureAndSave.cc \
		MyCmdWindow.cc \
		main.cc
OBJS		= CaptureAndSave.o \
		MyCmdWindow.o \
		main.o

include $(PROJECT)/lib/p.mk

###
CaptureAndSave.o: /usr/local/include/TU/V4L2++.h \
	/usr/local/include/TU/Image++.h /usr/local/include/TU/types.h \
	/usr/local/include/TU/Geometry++.h /usr/local/include/TU/Vector++.h \
	/usr/local/include/TU/Array++.h /usr/local/include/TU/iterator.h \
	/usr/local/include/TU/functional.h \
	/usr/local/include/TU/mmInstructions.h /usr/local/include/TU/tuple.h \
	/usr/local/include/TU/Minimize.h ../flow1394camera/CaptureAndSave.h
MyCmdWindow.o: /usr/local/include/TU/V4L2CameraArray.h \
	/usr/local/include/TU/V4L2++.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/types.h /usr/local/include/TU/Geometry++.h \
	/usr/local/include/TU/Vector++.h /usr/local/include/TU/Array++.h \
	/usr/local/include/TU/iterator.h /usr/local/include/TU/functional.h \
	/usr/local/include/TU/mmInstructions.h /usr/local/include/TU/tuple.h \
	/usr/local/include/TU/Minimize.h /usr/local/include/TU/io.h \
	/usr/local/include/TU/v/vV4L2++.h /usr/local/include/TU/v/CmdPane.h \
	/usr/local/include/TU/v/CmdWindow.h /usr/local/include/TU/v/TUv++.h \
	/usr/local/include/TU/v/Colormap.h /usr/local/include/TU/List.h \
	/usr/local/include/TU/v/Widget-Xaw.h ../flow1394camera/MyCmdWindow.h \
	/usr/local/include/TU/v/App.h /usr/local/include/TU/v/Timer.h \
	../flow1394camera/CaptureAndSave.h ../flow1394camera/flowcamera.h
main.o: /usr/local/include/TU/v/vV4L2++.h \
	/usr/local/include/TU/v/CmdPane.h /usr/local/include/TU/v/CmdWindow.h \
	/usr/local/include/TU/v/TUv++.h /usr/local/include/TU/v/Colormap.h \
	/usr/local/include/TU/Image++.h /usr/local/include/TU/types.h \
	/usr/local/include/TU/Geometry++.h /usr/local/include/TU/Vector++.h \
	/usr/local/include/TU/Array++.h /usr/local/include/TU/iterator.h \
	/usr/local/include/TU/functional.h \
	/usr/local/include/TU/mmInstructions.h /usr/local/include/TU/tuple.h \
	/usr/local/include/TU/Minimize.h /usr/local/include/TU/List.h \
	/usr/local/include/TU/v/Widget-Xaw.h /usr/local/include/TU/V4L2++.h \
	/usr/local/include/TU/V4L2CameraArray.h /usr/local/include/TU/io.h \
	../flow1394camera/MyCmdWindow.h /usr/local/include/TU/v/App.h \
	/usr/local/include/TU/v/Timer.h ../flow1394camera/CaptureAndSave.h \
	../flow1394camera/flowcamera.h
