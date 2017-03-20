sp 		:= $(sp).x
dirstack_$(sp)	:= $(d)
d		:= $(dir)

OBJS_$(d)	:= $(d)/src/rfc4648_encoding.o $(d)/src/cipher_xor.o
DEPS_$(d)	:= $(OBJS_$(d):%=%.d)

CLEAN		:= $(CLEAN) $(OBJS_$(d)) $(DEPS_$(d)) \
		   $(d)/libcryptopal-common.so

$(OBJS_$(d)):	CF_TGT := -I$(d)/include -fPIC
$(d)/libcryptopal-common.so: $(OBJS_$(d))
	$(CC) ${LDFLAGS} -o $@ $^ -shared

-include	$(DEPS_$(d))

d		:= $(dirstack_$(sp))
sp		:= $(basename $(sp))


