sp 		:= $(sp).x
dirstack_$(sp)	:= $(d)
d		:= $(dir)

dir	:= $(d)/challenge1
include		$(dir)/Rules.mk

dir	:= $(d)/challenge2
include		$(dir)/Rules.mk

dir	:= $(d)/challenge3
include		$(dir)/Rules.mk

dir	:= $(d)/challenge4
include		$(dir)/Rules.mk

-include	$(DEPS_$(d))

d		:= $(dirstack_$(sp))
sp		:= $(basename $(sp))
