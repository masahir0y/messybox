/*
 * Copyright (C) 2016 Masahiro Yamada <masahiroy@kernel.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef CMD_H
#define CMD_H

struct command {
	const char *name;
	const char *help;
	const char *long_help;
	int (*func)(int argc, char *argv[]);
};

extern struct command cmd_md;
extern struct command cmd_mm;
extern struct command cmd_mw;
extern struct command cmd_i2c;
extern struct command cmd_gettimeofday;

#endif /* CMD_H */
