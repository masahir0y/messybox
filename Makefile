include Kbuild.include

BIN := messybox
SCRIPTS := ldd2

CROSS_COMPILE	?= # override as needed
INSTALL		= /usr/bin/install
DESTDIR		=
PREFIX		= $(HOME)
BINDIR		= $(PREFIX)/bin

#INCLUDE		:= -include config.h
KBUILD_CFLAGS	:= -Wall -O2 -DBIN_NAME='"$(BIN)"'

obj-y	+= main.o
obj-y	+= mem.o
obj-y	+= i2c.o
obj-y	+= gettimeofday.o

all: $(BIN)

quiet_cmd_link = LINK    $@
      cmd_link = $(LD) -o $@ $(filter $(obj-y), $^)

$(BIN): $(obj-y) FORCE
	$(call if_changed,link)

clean-files := $(BIN)

quiet_cmd_install = INSTALL $@
      cmd_install = $(INSTALL) -D $(notdir $@) $@

SYMLINKS := $(addprefix $(DESTDIR)$(BINDIR)/, md mm mw i2c gettimeofday)

install: $(addprefix $(DESTDIR)$(BINDIR)/, $(BIN) $(SCRIPTS)) $(SYMLINKS)

$(addprefix $(DESTDIR)$(BINDIR)/, $(BIN) $(SCRIPTS)): FORCE
	$(call cmd,install)

quiet_cmd_symlink = SYMLINK $@
      cmd_symlink = ln -f -s $(BIN) $@

$(SYMLINKS): FORCE
	$(call cmd,symlink)

include Makefile.build
