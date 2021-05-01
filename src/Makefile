# name of the program (Minix service)
PROG=proj

# source code files to be compiled
SRCS = proj.c mouse.c utils.c timer.c kbd.c vbe.c player.c fireball.c enemy.c objects.c rtc.c uart.c queue.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic


# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
