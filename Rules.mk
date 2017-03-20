.SUFFIXES:
.SUFFIXES:	.c .o

all:		targets

dir	:= common
include		$(dir)/Rules.mk
dir	:= set1
include		$(dir)/Rules.mk

%.o:		%.c
		$(COMP)

%:		%.o
		$(LINK)

%:		%.c
		$(COMPLINK)

.PHONY:		targets
targets:	$(TGT_BIN) $(TGT_SBIN) $(TGT_ETC) $(TGT_LIB)

.PHONY:		clean
clean:
		rm -f $(CLEAN)

.PHONY:		run-all
run-all: ./run-all.sh
	$(SH) ./run-all.sh

.SECONDARY:	$(CLEAN)

