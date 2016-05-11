#include <check.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <math.h>
#include "gauge.h"

START_TEST(test_gauge_init)
{
    gauge_t g;
    int res = init_gauge(&g);
    fail_unless(res == 0);
}
END_TEST

START_TEST(test_gauge_init_add)
{
    gauge_t g;
    int res = init_gauge(&g);
    fail_unless(res == 0);

    fail_unless(gauge_add_sample(&g, 100, false) == 0);
    fail_unless(gauge_count(&g) == 1);
    fail_unless(gauge_sum(&g) == 100);
    fail_unless(gauge_value(&g) == 100);    
    fail_unless(gauge_mean(&g) == 100);
    fail_unless(gauge_min(&g) == 100);
    fail_unless(gauge_max(&g) == 100);
}
END_TEST

START_TEST(test_gauge_add_loop)
{
    gauge_t g;
    int res = init_gauge(&g);
    fail_unless(res == 0);

    fail_unless(gauge_add_sample(&g, 1, false) == 0);
    for (int i=2; i<=100; i++)
        fail_unless(gauge_add_sample(&g, i, true) == 0);

    fail_unless(gauge_count(&g) == 100);
    fail_unless(gauge_sum(&g) == 5050);
    fail_unless(gauge_mean(&g) == 50.5);
    fail_unless(gauge_value(&g) == 5050);
    fail_unless(gauge_min(&g) == 2);
    fail_unless(gauge_max(&g) == 100);
}
END_TEST

