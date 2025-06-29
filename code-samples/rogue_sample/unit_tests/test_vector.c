#include "test_vector.h"

START_TEST(create_and_destroy_vector)
{
    vector *data = create_vector();
    ck_assert_ptr_nonnull(data);

    destroy_vector(&data);
    ck_assert_ptr_null(data);
}
END_TEST

START_TEST(push_one_elem_vector)
{
    vector *data = create_vector();
    ck_assert_ptr_nonnull(data);

    push_back(data, STOP);
    ck_assert_int_eq(false, is_empty_vector(data));

    ck_assert_int_eq(1, data->size);
    ck_assert_int_eq(STOP, data->data[0]);

    destroy_vector(&data);
    ck_assert_ptr_null(data);
}
END_TEST

START_TEST(push_multiple_elements_vector)
{
    vector *data = create_vector();
    ck_assert_ptr_nonnull(data);

    directions_e expected[] = {
        FORWARD,
        BACK,
        LEFT,
        RIGHT,
        DIAGONNALY_FORWARD_LEFT,
        DIAGONALLY_FORWARD_RIGHT,
        DIAGONALLY_BACK_LEFT,
        DIAGONALLY_BACK_RIGHT,
        STOP,
    };
    size_t count_expected = sizeof(expected) / sizeof(*expected);

    for (size_t i = 0; i < count_expected; i++)
    {
        push_back(data, expected[i]);
        ck_assert_int_eq(false, is_empty_vector(data));
        ck_assert_int_eq(i + 1, data->size);
        ck_assert_int_eq(expected[i], data->data[i]);
    }

    for (size_t i = 0; i < count_expected; i++)
        ck_assert_int_eq(expected[i], data->data[i]);

    destroy_vector(&data);
    ck_assert_ptr_null(data);
}
END_TEST

START_TEST(reverse_empty_vector)
{
    vector *data = create_vector();
    ck_assert_ptr_nonnull(data);

    ck_assert_int_eq(0, data->size);
    reverse_vector(data);
    ck_assert_int_eq(0, data->size);

    destroy_vector(&data);
    ck_assert_ptr_null(data);
}
END_TEST

START_TEST(reverse_one_elem_vector)
{
    vector *data = create_vector();
    ck_assert_ptr_nonnull(data);

    push_back(data, STOP);
    ck_assert_int_eq(1, data->size);
    ck_assert_int_eq(STOP, data->data[0]);

    reverse_vector(data);
    ck_assert_int_eq(1, data->size);
    ck_assert_int_eq(STOP, data->data[0]);

    destroy_vector(&data);
    ck_assert_ptr_null(data);
}
END_TEST

START_TEST(reverse_with_even_count_elems_vector)
{
    vector *data = create_vector();
    ck_assert_ptr_nonnull(data);

    directions_e expected[] = {
        FORWARD,
        BACK,
        LEFT,
        RIGHT,
    };
    size_t count_expected = sizeof(expected) / sizeof(*expected);

    for (size_t i = 0; i < count_expected; i++)
    {
        push_back(data, expected[i]);
        ck_assert_int_eq(false, is_empty_vector(data));
        ck_assert_int_eq(i + 1, data->size);
        ck_assert_int_eq(expected[i], data->data[i]);
    }

    reverse_vector(data);
    ck_assert_int_eq(count_expected, data->size);
    for (size_t i = 0; i < count_expected; i++)
        ck_assert_int_eq(expected[count_expected - i - 1], data->data[i]);

    destroy_vector(&data);
    ck_assert_ptr_null(data);
}
END_TEST

START_TEST(reverse_with_odd_count_elems_vector)
{
    vector *data = create_vector();
    ck_assert_ptr_nonnull(data);

    directions_e expected[] = {
        FORWARD,
        BACK,
        LEFT,
        RIGHT,
        STOP,
    };
    size_t count_expected = sizeof(expected) / sizeof(*expected);

    for (size_t i = 0; i < count_expected; i++)
    {
        push_back(data, expected[i]);
        ck_assert_int_eq(false, is_empty_vector(data));
        ck_assert_int_eq(i + 1, data->size);
        ck_assert_int_eq(expected[i], data->data[i]);
    }

    reverse_vector(data);
    ck_assert_int_eq(count_expected, data->size);
    for (size_t i = 0; i < count_expected; i++)
        ck_assert_int_eq(expected[count_expected - i - 1], data->data[i]);

    destroy_vector(&data);
    ck_assert_ptr_null(data);
}
END_TEST

Suite *vector_suite(void)
{
    Suite *s = suite_create("vector");
    
    TCase *tc_pos = tcase_create("positives");
    tcase_add_test(tc_pos, create_and_destroy_vector);
    tcase_add_test(tc_pos, push_one_elem_vector);
    tcase_add_test(tc_pos, push_multiple_elements_vector);
    tcase_add_test(tc_pos, reverse_empty_vector);
    tcase_add_test(tc_pos, reverse_one_elem_vector);
    tcase_add_test(tc_pos, reverse_with_even_count_elems_vector);
    tcase_add_test(tc_pos, reverse_with_odd_count_elems_vector);
    suite_add_tcase(s, tc_pos);

    return s;
}
