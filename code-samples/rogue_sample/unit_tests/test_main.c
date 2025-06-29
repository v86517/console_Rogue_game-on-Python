#include <check.h>
#include "test_dsu.h"
#include "test_queue.h"
#include "test_vector.h"

int main(void)
{
    SRunner *runner = srunner_create(vector_suite());
    srunner_add_suite(runner, queue_suite());
    srunner_add_suite(runner, dsu_suite());

    srunner_run_all(runner, CK_NORMAL);
    int failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return failed;
}
