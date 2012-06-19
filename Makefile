SUBDIR	= embedCalib		\
	  epicheck		\
	  pbmheader		\
	  pbmrectify		\
	  pbmselect		\
	  pbmrot		\
	  flow1394camera	\
	  fv			\
	  rec1394camera		\
	  pbmdecode		\
	  flowV4L2camera

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
