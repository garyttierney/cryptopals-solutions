sp 		:= $(sp).x
dirstack_$(sp)	:= $(d)
d		:= $(dir)

TGTS_$(d)	:= $(d)/challenge2
DEPS_$(d)	:= $(TGTS_$(d):%=%.d)

TGT_BIN		:= $(TGT_BIN) $(TGTS_$(d))
CLEAN		:= $(CLEAN) $(TGTS_$(d)) $(DEPS_$(d))

$(TGTS_$(d)):	$(d)/Rules.mk

$(TGTS_$(d)):	CF_TGT := -Icommon/include
$(TGTS_$(d)):	LF_TGT := -lcryptopal-common -Lcommon/
$(TGTS_$(d)):	$(d)/src/main.c common/libcryptopal-common.so
		$(COMPLINK)

-include	$(DEPS_$(d))

d		:= $(dirstack_$(sp))
sp		:= $(basename $(sp))

