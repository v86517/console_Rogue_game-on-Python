#include "test_dsu.h"

START_TEST(create_sets)
{
    static const size_t size = 5;
    int parent[size], rank[size];
    make_sets(parent, rank, size);
    for (size_t i = 0; i < size; i++)
    {
        ck_assert_int_eq(rank[i], 0);
        ck_assert_int_eq(parent[i], i);
    }
}
END_TEST

START_TEST(union_all_sets_in_one_set)
{
    static const size_t size = 5;
    int parent[size], rank[size];
    make_sets(parent, rank, size);
    for (size_t i = 0; i < size; i++)
        union_sets(0, i, parent, rank);
    
    for (size_t i = 0; i < size; i++)
        ck_assert_int_eq(find_set(0, parent), find_set(i, parent));
}
END_TEST

START_TEST(union_all_sets_in_two_sets)
{
    static const size_t size = 8;
    int parent[size], rank[size];
    make_sets(parent, rank, size);
    for (size_t i = 0; i < size / 2; i++)
        union_sets(0, i, parent, rank);
    for (size_t i = size / 2; i < size; i++)
        union_sets(size / 2, i, parent, rank);
    
    for (size_t i = 0; i < size / 2; i++)
    {
        ck_assert_int_eq(find_set(0, parent), find_set(i, parent));
        ck_assert_int_ne(find_set(0, parent), find_set(size - i - 1, parent));
    }
    for (size_t i = size / 2; i < size; i++)
    {
        ck_assert_int_eq(find_set(size / 2, parent), find_set(i, parent));
        ck_assert_int_ne(find_set(size / 2, parent), find_set(size - i - 1, parent));
    }
}
END_TEST

START_TEST(dsu_with_one_elem)
{
    static const size_t size = 1;
    int parent[size], rank[size];
    make_sets(parent, rank, size);
    
    ck_assert_int_eq(0, find_set(0, parent));
    union_sets(0, 0, parent, rank);
    ck_assert_int_eq(0, find_set(0, parent));
}
END_TEST

Suite *dsu_suite(void)
{
    Suite *s = suite_create("disjoint set union");
    
    TCase *tc_pos = tcase_create("positives");
    tcase_add_test(tc_pos, create_sets);
    tcase_add_test(tc_pos, union_all_sets_in_one_set);
    tcase_add_test(tc_pos, union_all_sets_in_two_sets);
    tcase_add_test(tc_pos, dsu_with_one_elem);
    suite_add_tcase(s, tc_pos);

    return s;
}
