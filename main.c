/*
 * Copyright (C) 2016 Masahiro Yamada <masahiroy@kernel.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmd.h"
#include "errno.h"

static void show_help(void);

static int do_help(int argc, char *argv[])
{
	show_help();
	return 0;
}

struct command cmd_help = {
	.name = "help",
	.help = "show help",
	.long_help = "help [<command>]",
	.func = do_help,
};

static struct command *cmd_table[] = {
	&cmd_help,
	&cmd_i2c,
	&cmd_mm,
	&cmd_mw,
	&cmd_md,
	&cmd_gettimeofday,
	NULL,
};

static void show_help(void)
{
	struct command **cmd;

	printf("Usage: %s function [arguments...]\n"
	       "   or: function [arguments...]\n"
	       "\n"
       	       "Currently defined functions:\n",
	       BIN_NAME);

	for (cmd = cmd_table; *cmd; cmd++)
		printf("  %s\t\t%s\n", (*cmd)->name, (*cmd)->help);
}

int main(int argc, char *argv[])
{
	struct command **cmd;
	char *cmdname, *s;
	size_t cmdlen;
	int ret;

	s = strrchr(argv[0], '/');
	if (s)
		argv[0] = s + 1;

	if (!strcmp(argv[0], BIN_NAME)) {
		argc--;
		argv++;
	}

	if (argc < 1)
		goto help;

	cmdname = argv[0];

	s = strchr(cmdname, '.');
	if (s) {
		cmdlen = s - cmdname;
		argv[0] = s + 1;	/* sub-command */
	} else {
		cmdlen = strlen(cmdname);
		argv[0] = cmdname + cmdlen;
	}

	for (cmd = cmd_table; *cmd; cmd++) {
		if (!strncmp((*cmd)->name, cmdname, cmdlen)) {
			ret = (*cmd)->func(argc, argv);
			if (ret == -EINVAL) {
				printf("Usage:\n %s\n", (*cmd)->long_help);
			}
			exit(ret ? EXIT_FAILURE : EXIT_SUCCESS);
		}
	}

	printf("%s: applet not found\n", cmdname);

help:
	show_help();
	exit(EXIT_FAILURE);
}
