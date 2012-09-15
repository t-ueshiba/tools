#
#  $Id: l.mk,v 1.1 2012-09-15 07:42:56 ueshiba Exp $
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
PUBHDRS		= $(filter-out %_.h, $(HDRS))
DESTPUBHDRS	= $(PUBHDRS:%=$(INCDIR)/%)

ALIB		= lib$(NAME).a

ifeq ($(OSTYPE), darwin)
  SOLIB		= lib$(NAME).dylib
  PIC		= -dynamic
  NVCCFLAGS    += -m64
  ifeq ($(LINKER), nvcc)
    LDFLAGS    += $(NVCCFLAGS) -shared -Xlinker "-dynamic -undefined dynamic_lookup"
  else
    LDFLAGS    += -dynamiclib -undefined dynamic_lookup 
  endif
else
  SOLIB		= lib$(NAME).so
  PIC		= -fpic
  LDFLAGS      += -shared
endif

LIBRARY		= $(SOLIB)
DESTLIBRARY	= $(LIBRARY:%=$(DEST)/%)

INSTALL		= install
MAKEFILE	= Makefile

#########################
#  Making rules		#
#########################
all:		archive $(LIBRARY)

$(ALIB):	$(OBJS)
		$(RM) $@
#		$(LINKER) -xar -o $@ archive/$(OBJS)
		$(AR) rv $@ archive/$(OBJS)
		ranlib $@

$(SOLIB):	$(OBJS)
		$(LINKER) $(LDFLAGS) $(OBJS) -o $@

archive:
		mkdir archive

install:	$(DESTLIBRARY) $(DESTPUBHDRS)

$(DEST)/$(ALIB):	$(ALIB)
		$(INSTALL) -m 0644 $(ALIB) $(DEST)
		ranlib $@

$(DEST)/$(SOLIB):	$(SOLIB)
		$(INSTALL) -m 0755 $(SOLIB) $(DEST)

clean:
		$(RM) -r $(LIBRARY) $(OBJS)
		(cd archive; $(RM) $(OBJS))

depend:
		mkmf $(INCDIRS) -f $(MAKEFILE)

index:
		ctags -wx $(HDRS) $(SRCS)

tags:           $(HDRS) $(SRCS)
		ctags $(HDRS) $(SRCS)

doc:		$(HDRS) $(SRCS) doxygen.cfg
		doxygen doxygen.cfg

doxygen.cfg:
		doxygen -g $@

#########################
#  Implicit rules	#
#########################
$(INCDIR)/%:	%
		$(INSTALL) -m 0644 $< $@

.c.o:
		$(CC) $(CPPFLAGS) $(CFLAGS) $(PIC) $(INCDIRS) -c $<
#		$(CC) $(CPPFLAGS) $(CFLAGS) $(INCDIRS) -c $< -o archive/$@

.cc.o:
		$(CXX) $(CPPFLAGS) $(CCFLAGS) $(PIC) $(INCDIRS) -c $<
#		$(CXX) $(CPPFLAGS) $(CCFLAGS) $(INCDIRS) -c $< -o archive/$@

.cpp.o:
		$(CXX) $(CPPFLAGS) $(CCFLAGS) $(PIC) $(INCDIRS) -c $<
#		$(CXX) $(CPPFLAGS) $(CCFLAGS) $(INCDIRS) -c $< -o archive/$@

.cu.o:
		$(NVCC) $(CPPFLAGS) $(NVCCFLAGS) -Xcompiler $(PIC) $(INCDIRS) -c $<
