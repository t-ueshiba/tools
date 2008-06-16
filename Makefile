SUBDIR	= embedCalib		\
	  epicheck		\
	  pbmheader		\
	  pbmrectify		\
	  pbmselect

TARGETS	= all install clean depend

all:

$(TARGETS):
	@for d in $(SUBDIR); do				\
	  echo "";					\
	  echo "*** Current directory: $$d ***";	\
	  cd $$d;					\
	  $(MAKE) NAME=$$d $@;				\
	  cd ..;					\
	done
