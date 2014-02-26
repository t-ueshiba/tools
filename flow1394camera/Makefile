#
#  $Id: Makefile,v 1.16 2012-08-30 01:14:07 ueshiba Exp $
#
#################################
#  User customizable macros	#
#################################
DEST		= $(PREFIX)/bin
INCDIRS		= -I$(PREFIX)/include

PROGRAM		= $(shell basename $(PWD))
LIBS		= -lTUIeee1394++ -lTUv++ -lTUTools++ \
		  -lXaw3d -lXt -lXext -lXmu -lX11 -lraw1394
ifeq ($(OSTYPE), darwin)
  LIBS	       += -framework IOKit -framework CoreFoundation -framework CoreServices
endif

CPPFLAGS	= -DHAVE_LIBTUTOOLS__
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
		/usr/local/include/TU/Ieee1394++.h \
		/usr/local/include/TU/Ieee1394CameraArray.h \
		/usr/local/include/TU/Image++.h \
		/usr/local/include/TU/List.h \
		/usr/local/include/TU/Minimize.h \
		/usr/local/include/TU/Vector++.h \
		/usr/local/include/TU/functional.h \
		/usr/local/include/TU/io.h \
		/usr/local/include/TU/iterator.h \
		/usr/local/include/TU/mmInstructions.h \
		/usr/local/include/TU/types.h \
		/usr/local/include/TU/v/App.h \
		/usr/local/include/TU/v/CmdPane.h \
		/usr/local/include/TU/v/CmdWindow.h \
		/usr/local/include/TU/v/Colormap.h \
		/usr/local/include/TU/v/Dialog.h \
		/usr/local/include/TU/v/ModalDialog.h \
		/usr/local/include/TU/v/TUv++.h \
		/usr/local/include/TU/v/Timer.h \
		/usr/local/include/TU/v/Widget-Xaw.h
HDRS		= CaptureAndSave.h \
		MyCmdWindow.h \
		MyModalDialog.h \
		flow1394camera.h
SRCS		= CaptureAndSave.cc \
		MyCmdWindow.cc \
		MyModalDialog.cc \
		createFeatureCmds.cc \
		createMenuCmds.cc \
		main.cc
OBJS		= CaptureAndSave.o \
		MyCmdWindow.o \
		MyModalDialog.o \
		createFeatureCmds.o \
		createMenuCmds.o \
		main.o

include $(PROJECT)/lib/p.mk

###
CaptureAndSave.o: CaptureAndSave.h \
	/usr/local/include/TU/Ieee1394CameraArray.h \
	/usr/local/include/TU/Array++.h /usr/local/include/TU/types.h \
	/usr/local/include/TU/iterator.h /usr/local/include/TU/functional.h \
	/usr/local/include/TU/mmInstructions.h \
	/usr/local/include/TU/Ieee1394++.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/Geometry++.h /usr/local/include/TU/Vector++.h \
	/usr/local/include/TU/Minimize.h /usr/local/include/TU/io.h
MyCmdWindow.o: flow1394camera.h /usr/local/include/TU/v/TUv++.h \
	/usr/local/include/TU/v/Colormap.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/Geometry++.h /usr/local/include/TU/iterator.h \
	/usr/local/include/TU/functional.h /usr/local/include/TU/Vector++.h \
	/usr/local/include/TU/Array++.h /usr/local/include/TU/types.h \
	/usr/local/include/TU/mmInstructions.h \
	/usr/local/include/TU/Minimize.h /usr/local/include/TU/List.h \
	/usr/local/include/TU/v/Widget-Xaw.h \
	/usr/local/include/TU/Ieee1394++.h MyCmdWindow.h \
	/usr/local/include/TU/v/App.h /usr/local/include/TU/v/CmdWindow.h \
	/usr/local/include/TU/v/CmdPane.h /usr/local/include/TU/v/Timer.h \
	CaptureAndSave.h /usr/local/include/TU/Ieee1394CameraArray.h \
	/usr/local/include/TU/io.h MyModalDialog.h \
	/usr/local/include/TU/v/ModalDialog.h \
	/usr/local/include/TU/v/Dialog.h
MyModalDialog.o: MyModalDialog.h /usr/local/include/TU/Ieee1394++.h \
	/usr/local/include/TU/Image++.h /usr/local/include/TU/Geometry++.h \
	/usr/local/include/TU/iterator.h /usr/local/include/TU/functional.h \
	/usr/local/include/TU/Vector++.h /usr/local/include/TU/Array++.h \
	/usr/local/include/TU/types.h /usr/local/include/TU/mmInstructions.h \
	/usr/local/include/TU/Minimize.h \
	/usr/local/include/TU/v/ModalDialog.h \
	/usr/local/include/TU/v/Dialog.h /usr/local/include/TU/v/CmdPane.h \
	/usr/local/include/TU/v/CmdWindow.h /usr/local/include/TU/v/TUv++.h \
	/usr/local/include/TU/v/Colormap.h /usr/local/include/TU/List.h \
	/usr/local/include/TU/v/Widget-Xaw.h
createFeatureCmds.o: flow1394camera.h /usr/local/include/TU/v/TUv++.h \
	/usr/local/include/TU/v/Colormap.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/Geometry++.h /usr/local/include/TU/iterator.h \
	/usr/local/include/TU/functional.h /usr/local/include/TU/Vector++.h \
	/usr/local/include/TU/Array++.h /usr/local/include/TU/types.h \
	/usr/local/include/TU/mmInstructions.h \
	/usr/local/include/TU/Minimize.h /usr/local/include/TU/List.h \
	/usr/local/include/TU/v/Widget-Xaw.h \
	/usr/local/include/TU/Ieee1394++.h
createMenuCmds.o: flow1394camera.h /usr/local/include/TU/v/TUv++.h \
	/usr/local/include/TU/v/Colormap.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/Geometry++.h /usr/local/include/TU/iterator.h \
	/usr/local/include/TU/functional.h /usr/local/include/TU/Vector++.h \
	/usr/local/include/TU/Array++.h /usr/local/include/TU/types.h \
	/usr/local/include/TU/mmInstructions.h \
	/usr/local/include/TU/Minimize.h /usr/local/include/TU/List.h \
	/usr/local/include/TU/v/Widget-Xaw.h \
	/usr/local/include/TU/Ieee1394++.h
main.o: MyCmdWindow.h /usr/local/include/TU/v/App.h \
	/usr/local/include/TU/v/TUv++.h /usr/local/include/TU/v/Colormap.h \
	/usr/local/include/TU/Image++.h /usr/local/include/TU/Geometry++.h \
	/usr/local/include/TU/iterator.h /usr/local/include/TU/functional.h \
	/usr/local/include/TU/Vector++.h /usr/local/include/TU/Array++.h \
	/usr/local/include/TU/types.h /usr/local/include/TU/mmInstructions.h \
	/usr/local/include/TU/Minimize.h /usr/local/include/TU/List.h \
	/usr/local/include/TU/v/Widget-Xaw.h \
	/usr/local/include/TU/v/CmdWindow.h /usr/local/include/TU/v/CmdPane.h \
	/usr/local/include/TU/v/Timer.h CaptureAndSave.h \
	/usr/local/include/TU/Ieee1394CameraArray.h \
	/usr/local/include/TU/Ieee1394++.h /usr/local/include/TU/io.h
