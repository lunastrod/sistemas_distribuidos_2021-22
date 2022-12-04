#include <check.h>
#include <stdlib.h>

#include "client_list.h"

START_TEST(test_init_client_list) {
    struct client_list cl;
    init_client_list(&cl);
    ck_assert_int_eq(cl.pub_counter, 0);
    ck_assert_int_eq(cl.sub_counter, 0);
    ck_assert_int_eq(cl.id_counter, 0);
    for (int i = 0; i < TOPICS_MAX; i++) {
        ck_assert_int_eq(cl.topics[i].name[0], '\0');
        for (int j = 0; j < PUBLISHERS_MAX; j++) {
            ck_assert_int_eq(cl.topics[i].pubs[j].id, -1);
            ck_assert_int_eq(cl.topics[i].pubs[j].socket, -1);
        }
        for (int j = 0; j < SUBSCRIBERS_MAX; j++) {
            ck_assert_int_eq(cl.topics[i].subs[j].id, -1);
            ck_assert_int_eq(cl.topics[i].subs[j].socket, -1);
        }
    }
}
END_TEST

START_TEST(test_get_new_id) {
    struct client_list cl;
    init_client_list(&cl);
    for(int i = 0; i < 100; i++){
        ck_assert_int_eq(get_new_id(&cl, PUBLISHER), i);
    }
}
END_TEST

//test if the client is added to the list
START_TEST(test_add_client) {
    struct client_list cl;
    init_client_list(&cl);
    //int add_client(struct client_list *cl, enum client_type ct, char *topic, int socket);
    ck_assert_int_eq(add_client(&cl, PUBLISHER, "test", 0), 0);
    ck_assert_int_eq(cl.pub_counter, 1);
    ck_assert_int_eq(cl.topics[0].pubs[0].id, 0);
    ck_assert_int_eq(cl.topics[0].pubs[0].socket, 0);
}
END_TEST

int main() {
    Suite *s = suite_create("Suite");
    TCase *tc = tcase_create("Core");
    tcase_add_test(tc, test_init_client_list);
    tcase_add_test(tc, test_get_new_id);
    tcase_add_test(tc, test_add_client);
    suite_add_tcase(s, tc);
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}