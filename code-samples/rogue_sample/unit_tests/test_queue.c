#include "test_queue.h"

START_TEST(create_and_destroy_queue)
{
    queue *q = create_queue();
    ck_assert_ptr_nonnull(q);
    ck_assert_int_eq(true, is_empty(q));
    clear_queue(q);
}
END_TEST

START_TEST(enqueue_one_elem)
{
    queue *q = create_queue();
    ck_assert_ptr_nonnull(q);

    object_t objects[] = {
        (object_t) { .coordinates = { 0, 0 }, .size = { 0, 0 } },
    };
    size_t count_objects = sizeof(objects) / sizeof(*objects);

    for (size_t i = 0; i < count_objects; i++)
        enqueue(q, objects + i);

    size_t cnt = 0;
    while (!is_empty(q))
    {
        object_t obj = dequeue(q);
        ck_assert_int_eq(obj.coordinates[X], objects[cnt].coordinates[X]);
        ck_assert_int_eq(obj.coordinates[Y], objects[cnt].coordinates[Y]);
        ck_assert_int_eq(obj.size[X], objects[cnt].size[X]);
        ck_assert_int_eq(obj.size[Y], objects[cnt].size[Y]);
        cnt++;
    }
    ck_assert_int_eq(cnt, count_objects);
    clear_queue(q);
}
END_TEST

START_TEST(enqueue_multiple_elems)
{
    queue *q = create_queue();
    ck_assert_ptr_nonnull(q);

    object_t objects[] = {
        (object_t) { .coordinates = { 0, 0 }, .size = { 0, 0 } },
        (object_t) { .coordinates = { 1, 1 }, .size = { 0, 0 } },
        (object_t) { .coordinates = { 2, 2 }, .size = { 0, 0 } },
        (object_t) { .coordinates = { 3, 3 }, .size = { 0, 0 } },
        (object_t) { .coordinates = { 4, 4 }, .size = { 0, 0 } },
    };
    size_t count_objects = sizeof(objects) / sizeof(*objects);

    for (size_t i = 0; i < count_objects; i++)
        enqueue(q, objects + i);

    size_t cnt = 0;
    while (!is_empty(q))
    {
        object_t obj = dequeue(q);
        ck_assert_int_eq(obj.coordinates[X], objects[cnt].coordinates[X]);
        ck_assert_int_eq(obj.coordinates[Y], objects[cnt].coordinates[Y]);
        ck_assert_int_eq(obj.size[X], objects[cnt].size[X]);
        ck_assert_int_eq(obj.size[Y], objects[cnt].size[Y]);
        cnt++;
    }
    ck_assert_int_eq(cnt, count_objects);
    clear_queue(q);
}
END_TEST

START_TEST(enqueue_with_dequeue)
{
    queue *q = create_queue();
    ck_assert_ptr_nonnull(q);

    object_t objects[] = {
        (object_t) { .coordinates = { 0, 0 }, .size = { 0, 0 } },
        (object_t) { .coordinates = { 1, 1 }, .size = { 0, 0 } },
        (object_t) { .coordinates = { 2, 2 }, .size = { 0, 0 } },
        (object_t) { .coordinates = { 3, 3 }, .size = { 0, 0 } },
        (object_t) { .coordinates = { 4, 4 }, .size = { 0, 0 } },
    };
    size_t count_objects = sizeof(objects) / sizeof(*objects);

    for (size_t i = 0; i < count_objects; i++)
    {
        enqueue(q, objects + i);
        ck_assert_int_eq(false, is_empty(q));
        object_t obj = dequeue(q);
        ck_assert_int_eq(obj.coordinates[X], objects[i].coordinates[X]);
        ck_assert_int_eq(obj.coordinates[Y], objects[i].coordinates[Y]);
        ck_assert_int_eq(obj.size[X], objects[i].size[X]);
        ck_assert_int_eq(obj.size[Y], objects[i].size[Y]);
        ck_assert_int_eq(true, is_empty(q));
    }
    clear_queue(q);
}
END_TEST

Suite *queue_suite(void)
{
    Suite *s = suite_create("queue");
    
    TCase *tc_pos = tcase_create("positives");
    tcase_add_test(tc_pos, create_and_destroy_queue);
    tcase_add_test(tc_pos, enqueue_one_elem);
    tcase_add_test(tc_pos, enqueue_multiple_elems);
    tcase_add_test(tc_pos, enqueue_with_dequeue);
    suite_add_tcase(s, tc_pos);

    return s;
}
