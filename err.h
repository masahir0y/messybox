/*
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef BUG_H
#define BUG_H

#include <stdio.h>
#include <stdlib.h>

#define BUG() do { \
	fprintf(stderr, "BUG: failure at %s:%d/%s()!\n", \
		__FILE__, __LINE__, __func__); \
	exit(EXIT_FAILURE); \
} while (0)

#define ERR(msg) do { \
	perror(msg); \
	exit(EXIT_FAILURE); \
} while (0)

#endif /* BUG_H */
