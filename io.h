/*
 * Copyright (C) 2016 Masahiro Yamada <masahiroy@kernel.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef IO_H
#define IO_H

#include <stdint.h>

/* raw io: read[8/16/32], write[8/16/32] */
#define build_raw_io(size, type)				\
static inline type read##size(const void *addr)			\
{								\
	return *(volatile type *)addr;				\
}								\
 static inline void write##size(type val, const void *addr)	\
{								\
	*(volatile type *)addr = val;				\
}

build_raw_io(8, uint8_t)
build_raw_io(16, uint16_t)
build_raw_io(32, uint32_t)

#endif /* IO_H */
