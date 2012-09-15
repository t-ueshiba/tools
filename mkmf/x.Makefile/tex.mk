#########################
#  Common macros	#
#########################
#ifeq ($(CXX), g++)
#    CCFLAGS    += -fno-for-scope
#endif

INSTALL		= install
PRINT		= pr
MAKEFILE	= Makefile

#########################
#  Making rules		#
#########################
all:
		$(LATEX) $(NAME).tex

preview:
		$(XDVI) $(NAME)

clean:
		$(RM) $(NAME).toc $(NAME).aux $(NAME).bst $(NAME).dvi
