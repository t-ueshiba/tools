SUBDIR	= embedCalib		\
	  epicheck		\
	  pbmheader		\
	  pbmrectify		\
	  pbmselect		\
	  pbmrot		\
	  pbmdecode		\
	  fv			\
	  flow1394camera	\
	  rec1394camera		\
	  flowV4L2camera	\
	  recV4L2camera		\
	  hubctrl

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
