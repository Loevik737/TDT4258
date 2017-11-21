#
# Simple development makefile
#

#
# add more flags here,
# but don't remove the "+="
#
OBJS += game.o snake.o graphics.o utils.o gamepad.o
CFLAGS += -O2 -g -Wall #-Wsign-compare -Wfloat-equal -Wformat-security #-Werror
CPPFLAGS +=
LDFLAGS +=


all: game

game: $(OBJS)

clean:
	-rm -f game

install:


.PHONY: all install clean

# end of development makefile
