#
#  $Id$
#
#################################
#  User customizable macros	#
#################################
PROGRAM		= $(shell basename $(PWD))
#LIBRARY		= lib$(shell basename $(PWD))

VPATH		=

IDLS		=
MOCHDRS		=

INCDIRS		= -I. -I$(PREFIX)/include
CPPFLAGS	= -DNDEBUG
CFLAGS		= -g
NVCCFLAGS	= -g
ifneq ($(findstring icpc,$(CXX)),)
  CFLAGS	= -O3
  NVCCFLAGS	= -O			# must < -O2
  CPPFLAGS     += -DSSE3
endif
CCFLAGS		= $(CFLAGS)

LIBS		= -lTUTools++
ifneq ($(findstring darwin,$(OSTYPE)),)
  LIBS	       += -framework IOKit -framework CoreFoundation \
		  -framework CoreServices
endif

LINKER		= $(CXX)

BINDIR		= $(PREFIX)/bin
LIBDIR		= $(PREFIX)/lib
INCDIR		= $(PREFIX)/include

#########################
#  Macros set by mkmf	#
#########################
SUFFIX		= .cc:sC .cpp:sC .cu:sC
EXTHDRS		= /usr/local/include/TU/Array++.h \
		/usr/local/include/TU/Geometry++.h \
		/usr/local/include/TU/Image++.h \
		/usr/local/include/TU/Minimize.h \
		/usr/local/include/TU/Vector++.h \
		/usr/local/include/TU/functional.h \
		/usr/local/include/TU/iterator.h \
		/usr/local/include/TU/simd/allocator.h \
		/usr/local/include/TU/simd/arithmetic.h \
		/usr/local/include/TU/simd/arm/allocator.h \
		/usr/local/include/TU/simd/arm/arch.h \
		/usr/local/include/TU/simd/arm/arithmetic.h \
		/usr/local/include/TU/simd/arm/bit_shift.h \
		/usr/local/include/TU/simd/arm/cast.h \
		/usr/local/include/TU/simd/arm/compare.h \
		/usr/local/include/TU/simd/arm/cvt.h \
		/usr/local/include/TU/simd/arm/cvt_mask.h \
		/usr/local/include/TU/simd/arm/insert_extract.h \
		/usr/local/include/TU/simd/arm/load_store.h \
		/usr/local/include/TU/simd/arm/logical.h \
		/usr/local/include/TU/simd/arm/select.h \
		/usr/local/include/TU/simd/arm/shift.h \
		/usr/local/include/TU/simd/arm/type_traits.h \
		/usr/local/include/TU/simd/arm/vec.h \
		/usr/local/include/TU/simd/arm/zero.h \
		/usr/local/include/TU/simd/bit_shift.h \
		/usr/local/include/TU/simd/cast.h \
		/usr/local/include/TU/simd/compare.h \
		/usr/local/include/TU/simd/config.h \
		/usr/local/include/TU/simd/cvt.h \
		/usr/local/include/TU/simd/cvt_mask.h \
		/usr/local/include/TU/simd/cvtdown_iterator.h \
		/usr/local/include/TU/simd/cvtdown_mask_iterator.h \
		/usr/local/include/TU/simd/cvtup_iterator.h \
		/usr/local/include/TU/simd/cvtup_mask_iterator.h \
		/usr/local/include/TU/simd/insert_extract.h \
		/usr/local/include/TU/simd/intel/allocator.h \
		/usr/local/include/TU/simd/intel/arch.h \
		/usr/local/include/TU/simd/intel/arithmetic.h \
		/usr/local/include/TU/simd/intel/bit_shift.h \
		/usr/local/include/TU/simd/intel/cast.h \
		/usr/local/include/TU/simd/intel/compare.h \
		/usr/local/include/TU/simd/intel/cvt.h \
		/usr/local/include/TU/simd/intel/cvt_mask.h \
		/usr/local/include/TU/simd/intel/dup.h \
		/usr/local/include/TU/simd/intel/insert_extract.h \
		/usr/local/include/TU/simd/intel/load_store.h \
		/usr/local/include/TU/simd/intel/logical.h \
		/usr/local/include/TU/simd/intel/logical_base.h \
		/usr/local/include/TU/simd/intel/select.h \
		/usr/local/include/TU/simd/intel/shift.h \
		/usr/local/include/TU/simd/intel/shuffle.h \
		/usr/local/include/TU/simd/intel/svml.h \
		/usr/local/include/TU/simd/intel/type_traits.h \
		/usr/local/include/TU/simd/intel/unpack.h \
		/usr/local/include/TU/simd/intel/vec.h \
		/usr/local/include/TU/simd/intel/zero.h \
		/usr/local/include/TU/simd/load_iterator.h \
		/usr/local/include/TU/simd/load_store.h \
		/usr/local/include/TU/simd/logical.h \
		/usr/local/include/TU/simd/misc.h \
		/usr/local/include/TU/simd/row_vec_iterator.h \
		/usr/local/include/TU/simd/select.h \
		/usr/local/include/TU/simd/shift.h \
		/usr/local/include/TU/simd/shift_iterator.h \
		/usr/local/include/TU/simd/simd.h \
		/usr/local/include/TU/simd/store_iterator.h \
		/usr/local/include/TU/simd/type_traits.h \
		/usr/local/include/TU/simd/vec.h \
		/usr/local/include/TU/simd/zero.h \
		/usr/local/include/TU/tuple.h \
		/usr/local/include/TU/types.h
HDRS		=
SRCS		= main.cc
OBJS		= main.o

#include $(PROJECT)/lib/rtc.mk		# IDLHDRS, IDLSRCS, CPPFLAGS, OBJS, LIBS
#include $(PROJECT)/lib/qt.mk		# MOCSRCS, OBJS
#include $(PROJECT)/lib/cnoid.mk	# CPPFLAGS, LIBS, LIBDIR
#include $(PROJECT)/lib/lib.mk		# PUBHDRS TARGHDRS
include $(PROJECT)/lib/common.mk
###
main.o: /usr/local/include/TU/Image++.h /usr/local/include/TU/types.h \
	/usr/local/include/TU/Geometry++.h /usr/local/include/TU/Vector++.h \
	/usr/local/include/TU/Array++.h /usr/local/include/TU/iterator.h \
	/usr/local/include/TU/tuple.h /usr/local/include/TU/functional.h \
	/usr/local/include/TU/simd/simd.h /usr/local/include/TU/simd/config.h \
	/usr/local/include/TU/simd/vec.h \
	/usr/local/include/TU/simd/type_traits.h \
	/usr/local/include/TU/simd/intel/type_traits.h \
	/usr/local/include/TU/simd/arm/type_traits.h \
	/usr/local/include/TU/simd/intel/vec.h \
	/usr/local/include/TU/simd/intel/arch.h \
	/usr/local/include/TU/simd/arm/vec.h \
	/usr/local/include/TU/simd/arm/arch.h \
	/usr/local/include/TU/simd/allocator.h \
	/usr/local/include/TU/simd/intel/allocator.h \
	/usr/local/include/TU/simd/arm/allocator.h \
	/usr/local/include/TU/simd/load_store.h \
	/usr/local/include/TU/simd/intel/load_store.h \
	/usr/local/include/TU/simd/arm/load_store.h \
	/usr/local/include/TU/simd/zero.h \
	/usr/local/include/TU/simd/intel/zero.h \
	/usr/local/include/TU/simd/arm/zero.h \
	/usr/local/include/TU/simd/cast.h \
	/usr/local/include/TU/simd/intel/cast.h \
	/usr/local/include/TU/simd/arm/cast.h \
	/usr/local/include/TU/simd/insert_extract.h \
	/usr/local/include/TU/simd/intel/insert_extract.h \
	/usr/local/include/TU/simd/arm/insert_extract.h \
	/usr/local/include/TU/simd/shift.h \
	/usr/local/include/TU/simd/intel/shift.h \
	/usr/local/include/TU/simd/arm/shift.h \
	/usr/local/include/TU/simd/bit_shift.h \
	/usr/local/include/TU/simd/intel/bit_shift.h \
	/usr/local/include/TU/simd/arm/bit_shift.h \
	/usr/local/include/TU/simd/cvt.h \
	/usr/local/include/TU/simd/intel/cvt.h \
	/usr/local/include/TU/simd/intel/dup.h \
	/usr/local/include/TU/simd/intel/unpack.h \
	/usr/local/include/TU/simd/arm/cvt.h \
	/usr/local/include/TU/simd/cvt_mask.h \
	/usr/local/include/TU/simd/intel/cvt_mask.h \
	/usr/local/include/TU/simd/arm/cvt_mask.h \
	/usr/local/include/TU/simd/logical.h \
	/usr/local/include/TU/simd/intel/logical.h \
	/usr/local/include/TU/simd/intel/logical_base.h \
	/usr/local/include/TU/simd/arm/logical.h \
	/usr/local/include/TU/simd/compare.h \
	/usr/local/include/TU/simd/intel/compare.h \
	/usr/local/include/TU/simd/arm/compare.h \
	/usr/local/include/TU/simd/select.h \
	/usr/local/include/TU/simd/intel/select.h \
	/usr/local/include/TU/simd/arm/select.h \
	/usr/local/include/TU/simd/arithmetic.h \
	/usr/local/include/TU/simd/intel/arithmetic.h \
	/usr/local/include/TU/simd/arm/arithmetic.h \
	/usr/local/include/TU/simd/misc.h \
	/usr/local/include/TU/simd/intel/shuffle.h \
	/usr/local/include/TU/simd/intel/svml.h \
	/usr/local/include/TU/simd/load_iterator.h \
	/usr/local/include/TU/simd/store_iterator.h \
	/usr/local/include/TU/simd/cvtdown_iterator.h \
	/usr/local/include/TU/simd/cvtup_iterator.h \
	/usr/local/include/TU/simd/cvtdown_mask_iterator.h \
	/usr/local/include/TU/simd/cvtup_mask_iterator.h \
	/usr/local/include/TU/simd/shift_iterator.h \
	/usr/local/include/TU/simd/row_vec_iterator.h \
	/usr/local/include/TU/Minimize.h
