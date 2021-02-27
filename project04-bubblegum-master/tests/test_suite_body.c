#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "body.h"
#include "list.h"
#include "test_util.h"

void test_velocity_init() {
    vector_t* v = velocity_init(100);
    assert(v->x == 100 / 3 && v->y == -20);
    free(v);
}

void test_body_init() {
    body_t* shape = body_init(.2, .4, .6, 100);
    assert(get_velocity(shape)->x == 100 / 3 && get_velocity(shape)->y == -20);
    assert(get_r(shape) == (float).2);
    assert(get_g(shape) == (float).4);
    assert(get_b(shape) == (float).6);
    body_free(shape);
}

int main(int argc, char* argv[]) {
    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_velocity_init)
    DO_TEST(test_body_init)

    puts("body_test PASS");
}
