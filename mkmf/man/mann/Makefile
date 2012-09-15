#DEST	      = $(DESTDIR)/usr/new/man/mann
#DEST	      = $(DESTDIR)/man/mann
DEST	      = /usr/local/man/mann

MAKEFILE      = Makefile

PRINT	      = vtroff -man

SRCS	      = mkmf.n

SUFFIX	      = .n:s

VERSION	      = V4BSD

all:;

clean:;

co:;

depend:;

diff:;

index:;

install:;	@echo Installing $(SRCS) in $(DEST)
		@install -o ueshiba -g ueshiba -m 644 -c $(SRCS) $(DEST)
		@ls -lg $(DEST)/$(SRCS)

lint:;

print:		$(SRCS)
		@$(PRINT) $?
		@touch print

tags:;

update:		$(SRCS)
		@echo Installing $? in $(DEST)
		@cp $? $(DEST)
###
