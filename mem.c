/*
 * Copyright (C) 2016 Masahiro Yamada <masahiroy@kernel.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "cmd.h"
#include "err.h"
#include "errno.h"
#include "io.h"
#include "utils.h"

#define MAPSIZE (4 * 1024)
#define MEMDEV "/dev/mem"

static void *xmmap(unsigned long offset)
{
	int fd;
	void *addr;

	fd = open(MEMDEV, O_RDWR);
	if (fd < 0)
		ERR(MEMDEV);

	addr = mmap(NULL, MAPSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
		    offset);
	if (addr == MAP_FAILED)
		ERR("mmap");

	return addr;
}

static int do_md(int argc, char *argv[])
{
	int width;
	unsigned long phys_addr;
	size_t size = 0x100;
	void *virt_addr;
	char *ep;
	size_t left, cnt_in_line;

	if (argc < 2)
		return -EINVAL;

	switch (*argv[0]) {
	case 'b':
		width = 1;
		break;
	case 'w':
		width = 2;
		break;
	case 'd':
	default:
		width = 4;
		break;
	};

	phys_addr = strtoul(argv[1], &ep, 16);
	if (*ep) {
		fprintf(stderr, "invalid address format \"%s\"\n", argv[1]);
		return -EINVAL;
	}

	if (argc >= 3) {
		size = strtoul(argv[2], &ep, 16);
		if (*ep) {
			fprintf(stderr, "invalid size format \"%s\"\n",
				argv[2]);
			return -EINVAL;
		}
	}

	virt_addr = xmmap(phys_addr);
	left = size / width;
	cnt_in_line = 16 / width;

	while (left) {
		int i, cnt;
		cnt = min(cnt_in_line, left);

		printf("%08lx:", phys_addr);

		for (i = 0; i < cnt; i++) {
			switch (width) {
			case 1:
				printf(" %02x", read8(virt_addr));
				break;
			case 2:
				printf(" %04x", read16(virt_addr));
				break;
			case 4:
				printf(" %08x", read32(virt_addr));
				break;
			default:
				BUG();
			}

			phys_addr += width;
			virt_addr += width;
		}

		left -= cnt;

		printf("\n");
	}

	return 0;
}

struct command cmd_md = {
	.name = "md",
	.help = "memory display",
	.long_help = "md[.b/w/l] address size",
	.func = do_md,
};

int do_mm(int argc, char *argv[])
{
	return 0;
}

struct command cmd_mm = {
	.name = "mm",
	.help = "memory modify",
	.long_help = "mm[.b/w/l] address size",
	.func = do_md,
};

int do_mw(int argc, char *argv[])
{
	int width;
	unsigned long phys_addr;
	uint32_t value;
	size_t size = 0x100;
	void *virt_addr;
	char *ep;
	size_t left;

	if (argc < 2)
		return -EINVAL;

	switch (*argv[0]) {
	case 'b':
		width = 1;
		break;
	case 'w':
		width = 2;
		break;
	case 'd':
	default:
		width = 4;
		break;
	};

	phys_addr = strtoul(argv[1], &ep, 16);
	if (*ep) {
		fprintf(stderr, "invalid address format \"%s\"\n", argv[1]);
		return -EINVAL;
	}

	value = strtoul(argv[2], &ep, 16);

	if (argc >= 4) {
		size = strtoul(argv[3], &ep, 16);
		if (*ep) {
			fprintf(stderr, "invalid size format \"%s\"\n",
				argv[2]);
			return -EINVAL;
		}
	}

	virt_addr = xmmap(phys_addr);
	left = size / width;

	while (left--) {
		switch (width) {
		case 1:
			write8(value, virt_addr);
			break;
		case 2:
			write16(value, virt_addr);
			break;
		case 4:
			write32(value, virt_addr);
			break;
		default:
			BUG();
		}

		virt_addr += width;
	}

	return 0;
}

struct command cmd_mw = {
	.name = "mw",
	.help = "memory write",
	.long_help = "mw[.b/w/l] address value count",
	.func = do_mw,
};
