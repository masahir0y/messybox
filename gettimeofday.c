#include <stdio.h>
#include <sys/time.h>

#include "cmd.h"
#include "err.h"

static int do_gettimeofday(int argc, char *argv[])
{
	struct timeval tv;
	int ret;

	ret = gettimeofday(&tv, NULL);
	if (ret)
		ERR("gettimeofday");

	printf("%lu.%06lu\n", tv.tv_sec, tv.tv_usec);

	return 0;
}

struct command cmd_gettimeofday = {
	.name = "gettimeofday",
	.help = "Call gettimeofday",
	.long_help = "gettimeofday",
	.func = do_gettimeofday,
};
