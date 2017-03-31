#include <check.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "utils.h"

START_TEST(test_percentile_convertion)
{
    int percentile;
    fail_unless(to_percentile(-0.1, &percentile) != 0);
    fail_unless(to_percentile(1.1, &percentile) != 0);

    fail_unless(to_percentile(0, &percentile) == 0);
    fail_unless(percentile == 0);

    fail_unless(to_percentile(1.00, &percentile) == 0);
    fail_unless(percentile == 100);

    fail_unless(to_percentile(0.5, &percentile) == 0);
    fail_unless(percentile == 50);

    fail_unless(to_percentile(0.950000, &percentile) == 0);
    fail_unless(percentile == 95);

    fail_unless(to_percentile(0.999, &percentile) == 0);
    fail_unless(percentile == 999);

    fail_unless(to_percentile(0.99990, &percentile) == 0);
    fail_unless(percentile == 9999);

    // this should cause to_percentile to not converge
    // fail safe should kick in an abruptly exit.
    fail_unless(to_percentile(0.99999999999, &percentile) == -1);
}
END_TEST
