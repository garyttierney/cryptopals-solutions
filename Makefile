CF_ALL          = -Wno-missing-braces -Wextra -Wno-missing-field-initializers -Wformat=2 \
		  -Wswitch-default -Wswitch-enum -Wcast-align -Wpointer-arith \
		  -Wbad-function-cast -Wstrict-overflow=5 -Wstrict-prototypes -Winline \
		  -Wundef -Wnested-externs -Wcast-qual -Wshadow -Wunreachable-code \
		  -Wlogical-op -Wfloat-equal -Wstrict-aliasing=2 -Wredundant-decls \
		  -Wold-style-definition -Werror -Wall \
		  -ggdb3 \
		  -O0 \
		  -fno-omit-frame-pointer -ffloat-store -fno-common -fstrict-aliasing

LF_ALL          =
LL_ALL          =

CC              = build/ccd-gcc
COMP            = $(CC) $(CF_ALL) $(CF_TGT) -o $@ -c $<
LINK            = $(CC) $(LF_ALL) $(LF_TGT) -o $@ $^ $(LL_TGT) $(LL_ALL)
COMPLINK        = $(CC) $(CF_ALL) $(CF_TGT) $(LF_ALL) $(LF_TGT) -o $@ $< $(LL_TGT) $(LL_ALL)

include Rules.mk
