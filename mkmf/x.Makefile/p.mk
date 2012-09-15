#
#  $Id: p.mk,v 1.1 2012-09-15 07:42:56 ueshiba Exp $
#
#########################
#  Intel Compiler	#
#########################
ifeq ($(CXX), icpc)
#  CCFLAGS      += -cxxlib-icc -no-gcc
#  LDFLAGS      += -cxxlib-icc
#  CCFLAGS      += -cxxlib-gcc
#  LDFLAGS      += -cxxlib-gcc
#  CCFLAGS	+= -gxx-name=/usr/bin/g++-3.3.6
#  LDFLAGS	+= -gxx-name=/usr/bin/g++-3.3.6
endif

#########################
#  Common macros	#
#########################
ifeq ($(OSTYPE), darwin)
  NVCCFLAGS    += -m64
  LDFLAGS      += $(NVCCFLAGS)
endif

INSTALL		= install
MAKEFILE	= Makefile

#########################
#  Making rules		#
#########################
all:		$(PROGRAM)

$(PROGRAM):     $(OBJS)
		$(LINKER) $(LDFLAGS) $(INCDIRS)	$(OBJS) $(LIBS) -o $@

.c.o:
		$(CC) $(CPPFLAGS) $(CFLAGS) $(INCDIRS) -c $<

.cc.o:
		$(CXX) $(CPPFLAGS) $(CCFLAGS) $(INCDIRS) -c $<

.cpp.o:
		$(CXX) $(CPPFLAGS) $(CCFLAGS) $(INCDIRS) -c $<

.cu.o:
		$(NVCC) $(CPPFLAGS) $(NVCCFLAGS) $(INCDIRS) -c $<

install:	$(PROGRAM)
		$(INSTALL) -m 0755 $(PROGRAM) $(DEST)/$(PROGRAM)

clean:
		$(RM) -r $(OBJS) $(PROGRAM)

depend:
		mkmf $(INCDIRS) -f $(MAKEFILE)

index:
		ctags -wx $(HDRS) $(SRCS)

tags:           $(HDRS) $(SRCS)
		ctags $(HDRS) $(SRCS)

link:		$(OBJS)
		$(RM) $(PROGRAM); make $(PROGRAM)

doc:		$(HDRS) $(SRCS) doxygen.cfg
		doxygen doxygen.cfg

doxygen.cfg:
		doxygen -g $@

#update:		$(DEST)/$(PROGRAM)
#
#$(DEST)/$(PROGRAM): $(SRCS) $(HDRS) $(EXTHDRS)
#		@make -f $(MAKEFILE) DEST=$(DEST) install
