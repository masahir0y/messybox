/*
 * Copyright (C) 2016 Masahiro Yamada <masahiroy@kernel.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>

#include "cmd.h"
#include "err.h"
#include "errno.h"
#include "utils.h"

#define I2CDEV "/dev/i2c-%d"

enum i2c_subcmd {
	I2C_MD,
	I2C_MM,
	I2C_MW,
	I2C_LOOP,
};

static void do_i2c_md(int fd, char *buf, unsigned int offset,
		      unsigned int width, unsigned int size)
{
	int i;
	ssize_t ret;

	for (i = 0; i < width; i++)
		buf[i] = offset >> (8 * (width - 1 - i));

	ret = write(fd, buf, width);
	if (ret < 0)
		ERR("write");

	ret = read(fd, buf, size);
	if (ret < 0)
		ERR("read");

	while (size) {
		int cnt;

		cnt = min(16U, size);

		printf("%04x:", offset);

		for (i = 0; i < cnt; i++) {
			printf(" %02x", *buf++);
		}

		size -= cnt;

		printf("\n");
	}
}

static void do_i2c_mw(int fd, char *buf, unsigned int offset,
		      unsigned int width, unsigned int val, unsigned int cnt)
{
	int i;
	ssize_t ret;

	for (i = 0; i < width; i++)
		buf[i] = offset >> (8 * (width - 1 - i));

	for (i = 0; i < cnt; i++)
		buf[width + i] = val;

	ret = write(fd, buf, width + cnt);
	if (ret < 0)
		ERR("write");
}

static void do_i2c_loop(int fd, char *buf, unsigned int offset,
			unsigned int width, unsigned int iter)
{
	int i;
	ssize_t ret;
	bool infinite = (iter == 0);

	for (i = 0; i < width; i++) {
		buf[i] = offset >> (8 * (width - 1 - i));
	}

	for (i = 0; infinite || i < iter; i++) {
		ret = write(fd, buf, width);
		if (ret < 0)
			ERR("write");

		ret = read(fd, buf + width, 1);
		if (ret < 0)
			ERR("read");

		if (infinite && i % 0x2000 == 0)
			printf("ps=%4d: iteration = %d\n", getpid(), i);
	}

	printf("ps=%4d: iteration = %d done\n", getpid(), iter);
}

static int do_i2c(int argc, char *argv[])
{
	enum i2c_subcmd subcmd;
	unsigned int ch, slave_addr, offset, width = 1;
	unsigned int arg5 = 0x10, arg6 = 0x10;
	int fd;
	char buf[8192];
	char *ep;

	if (argc < 5)
		return -EINVAL;

	if (!strcmp(argv[1], "md"))
		subcmd = I2C_MD;
	else if (!strcmp(argv[1], "mm"))
		subcmd = I2C_MM;
	else if (!strcmp(argv[1], "mw"))
		subcmd = I2C_MW;
	else if (!strcmp(argv[1], "loop"))
		subcmd = I2C_LOOP;
	else
		return -EINVAL;

	ch = strtoul(argv[2], &ep, 16);
	if (*ep || ch > 9) {
		fprintf(stderr, "invalid channel \"%s\"\n", argv[2]);
		return -EINVAL;
	}

	slave_addr = strtoul(argv[3], &ep, 16);
	if (*ep) {
		fprintf(stderr, "invalid slave address \"%s\"\n", argv[3]);
		return -EINVAL;
	}

	offset = strtoul(argv[4], &ep, 16);
	if (*ep == '.' && *(ep + 1) != 0) {
		ep++;
		width = strtoul(ep, &ep, 16);
	}
	if (*ep || offset > sizeof(buf)) {
		fprintf(stderr, "invalid offset \"%s\"\n", argv[4]);
		return -EINVAL;
	}

	if (width != 1 && width != 2 && width != 4) {
		fprintf(stderr, "invalid width %d\n", width);
		return -EINVAL;
	}

	if (argc >= 6) {
		arg5 = strtoul(argv[5], &ep, 16);
		if (*ep) {
			fprintf(stderr, "invalid argument \"%s\"\n", argv[5]);
			return -EINVAL;
		}
	}

	if (argc >= 7) {
		arg6 = strtoul(argv[6], &ep, 16);
		if (*ep) {
			fprintf(stderr, "invalid argument \"%s\"\n", argv[6]);
			return -EINVAL;
		}
	}

	sprintf(buf, I2CDEV, ch);

	fd = open(buf, O_RDWR);
	if (fd < 0)
		ERR(buf);

	if (ioctl(fd, I2C_SLAVE, slave_addr) < 0)
		ERR(buf);

	printf("%s: set slave address to 0x%x\n", buf, slave_addr);

	switch (subcmd) {
	case I2C_MD:
		do_i2c_md(fd, buf, offset, width, arg5);
		break;
	case I2C_MM:
		printf("not implemented yet\n");
		break;
	case I2C_MW:
		do_i2c_mw(fd, buf, offset, width, arg5, arg6);
		break;
	case I2C_LOOP:
		do_i2c_loop(fd, buf, offset, width, arg5);
		break;
	default:
		BUG();
		break;
	}

	return 0;
}

struct command cmd_i2c = {
	.name = "i2c",
	.help = "I2C access",
	.long_help = "i2c md ch slave_addr addr size",
	.func = do_i2c,
};
