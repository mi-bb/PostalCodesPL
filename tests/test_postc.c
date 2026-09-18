/**
 * @file  test_postc.c
 * @copyright Copyright (C) 2019-2026 Michal Babik. Licensed under GPL-3.0 License.
 *
 * @brief Unity tests for postc.c
 *
 * postc.c's data/logic helpers are declared static and the file defines its
 * own main(), so it is pulled in directly (rather than linked normally) with
 * main renamed out of the way. This keeps postc.c itself untouched.
 *
 * DB-backed tests query the real pc_base.db checked into the repo, so they
 * must be run from the repository root (see tests/run_tests.sh) and assume
 * its data is unchanged for the specific codes/cities referenced below.
 */
#include "unity/unity.h"

#define main postc_main_unused
#include "../examples/postc.c"
#undef main

void
setUp (void)
{
}

void
tearDown (void)
{
}

/*----------------------------------------------------------------------------*/
/* clear_pdata / clear_cdata                                                  */
/*----------------------------------------------------------------------------*/

static void
test_clear_pdata_zeroes_all_fields (void)
{
    Postal_data pd;
    memset (&pd, 0xAA, sizeof (pd));

    clear_pdata (&pd);

    TEST_ASSERT_EQUAL_STRING ("", pd.code1);
    TEST_ASSERT_EQUAL_STRING ("", pd.code2);
    TEST_ASSERT_EQUAL_STRING ("", pd.city_name);
    TEST_ASSERT_EQUAL_STRING ("", pd.voivodeship);
    TEST_ASSERT_EQUAL_STRING ("", pd.city_detcr);
    TEST_ASSERT_EQUAL_STRING ("", pd.street_name);
    TEST_ASSERT_EQUAL_STRING ("", pd.street_number);
    TEST_ASSERT_EQUAL_STRING ("", pd.post_un);
}

static void
test_clear_cdata_zeroes_all_fields (void)
{
    City_data cd;
    memset (&cd, 0xAA, sizeof (cd));

    clear_cdata (&cd);

    TEST_ASSERT_EQUAL_INT (0, cd.i_id);
    TEST_ASSERT_EQUAL_INT (0, cd.i_no);
    TEST_ASSERT_EQUAL_INT (0, cd.voivod_id);
    TEST_ASSERT_EQUAL_STRING ("", cd.city_name);
    TEST_ASSERT_EQUAL_STRING ("", cd.voivodeship);
}

/*----------------------------------------------------------------------------*/
/* set_codes                                                                  */
/*----------------------------------------------------------------------------*/

static void
test_set_codes_splits_dashed_six_char_string (void)
{
    char code1[3];
    char code2[4];

    set_codes (code1, code2, "01-001");

    TEST_ASSERT_EQUAL_STRING ("01", code1);
    TEST_ASSERT_EQUAL_STRING ("001", code2);
}

static void
test_set_codes_ignores_char_at_index_two (void)
{
    /* index 2 (the separator slot) is never read into either output */
    char code1[3];
    char code2[4];

    set_codes (code1, code2, "99X123");

    TEST_ASSERT_EQUAL_STRING ("99", code1);
    TEST_ASSERT_EQUAL_STRING ("123", code2);
}

/*----------------------------------------------------------------------------*/
/* get_pd_column_width                                                       */
/*----------------------------------------------------------------------------*/

static void
test_get_pd_column_width_counts_ascii_bytes (void)
{
    Postal_data pd[1];
    uint16_t i_w[6] = {0, 0, 0, 0, 0, 0};

    clear_pdata (&pd[0]);
    snprintf (pd[0].city_name, sizeof (pd[0].city_name), "Warszawa");

    get_pd_column_width (pd, i_w, 1);

    TEST_ASSERT_EQUAL_UINT16 (8, i_w[0]);
}

static void
test_get_pd_column_width_counts_utf8_codepoints_not_bytes (void)
{
    Postal_data pd[1];
    uint16_t i_w[6] = {0, 0, 0, 0, 0, 0};

    clear_pdata (&pd[0]);
    /* "Łódź" is 4 codepoints but 7 bytes in UTF-8. Split after \xb3 so the
       following 'd' (a valid hex digit) isn't swallowed into that escape. */
    snprintf (pd[0].city_name, sizeof (pd[0].city_name), "\xc5\x81\xc3\xb3" "d\xc5\xba");

    get_pd_column_width (pd, i_w, 1);

    TEST_ASSERT_EQUAL_UINT16 (4, i_w[0]);
}

static void
test_get_pd_column_width_takes_max_across_rows (void)
{
    Postal_data pd[2];
    uint16_t i_w[6] = {0, 0, 0, 0, 0, 0};

    clear_pdata (&pd[0]);
    clear_pdata (&pd[1]);
    snprintf (pd[0].voivodeship, sizeof (pd[0].voivodeship), "slaskie");
    snprintf (pd[1].voivodeship, sizeof (pd[1].voivodeship), "malopolskie");

    get_pd_column_width (pd, i_w, 2);

    TEST_ASSERT_EQUAL_UINT16 (11, i_w[1]);
}

/*----------------------------------------------------------------------------*/
/* get_post_code_info (reads the real pc_base.db)                            */
/*----------------------------------------------------------------------------*/

static void
test_get_post_code_info_returns_known_row (void)
{
    Postal_data *pd_data = NULL;

    uint16_t i_rcnt = get_post_code_info (&pd_data, "01", "001");

    TEST_ASSERT_EQUAL_UINT16 (1, i_rcnt);
    TEST_ASSERT_EQUAL_STRING ("Warszawa", pd_data[0].city_name);
    TEST_ASSERT_EQUAL_STRING ("Wola", pd_data[0].city_detcr);
    TEST_ASSERT_EQUAL_STRING ("mazowieckie", pd_data[0].voivodeship);
    free (pd_data);
}

static void
test_get_post_code_info_returns_zero_for_unknown_code (void)
{
    Postal_data *pd_data = NULL;

    uint16_t i_rcnt = get_post_code_info (&pd_data, "99", "999");

    TEST_ASSERT_EQUAL_UINT16 (0, i_rcnt);
    TEST_ASSERT_NULL (pd_data);
}

/*----------------------------------------------------------------------------*/
/* get_city_names_like (reads the real pc_base.db)                           */
/*----------------------------------------------------------------------------*/

static void
test_get_city_names_like_finds_krakow (void)
{
    City_data *c_data = NULL;
    int i_found = 0;

    uint16_t i_rcnt = get_city_names_like (&c_data, "%Krak%");

    /* 14, not 12: DISTINCT is over (city.id, city_name, voivod.id,
       voivodeship), and a few city names span more than one voivodeship. */
    TEST_ASSERT_EQUAL_UINT16 (14, i_rcnt);
    for (int i = 0; i < i_rcnt; ++i) {
        if (strcmp (c_data[i].city_name, "Krak\xc3\xb3w") == 0) {
            i_found = 1;
            TEST_ASSERT_EQUAL_INT (14381, c_data[i].i_id);
        }
    }
    TEST_ASSERT_TRUE (i_found);
    free (c_data);
}

static void
test_get_city_names_like_returns_zero_for_no_match (void)
{
    City_data *c_data = NULL;

    uint16_t i_rcnt = get_city_names_like (&c_data, "%Zzzzznotacity%");

    TEST_ASSERT_EQUAL_UINT16 (0, i_rcnt);
    TEST_ASSERT_NULL (c_data);
}

/*----------------------------------------------------------------------------*/
/* get_city_voivodeship_info (reads the real pc_base.db)                     */
/*----------------------------------------------------------------------------*/

static void
test_get_city_voivodeship_info_returns_known_count_and_first_row (void)
{
    /* Krakow / malopolskie ids, confirmed against pc_base.db directly */
    Postal_data *pd_data = NULL;

    uint16_t i_rcnt = get_city_voivodeship_info (&pd_data, 14381, 10);

    TEST_ASSERT_EQUAL_UINT16 (3622, i_rcnt);
    TEST_ASSERT_EQUAL_STRING ("30", pd_data[0].code1);
    TEST_ASSERT_EQUAL_STRING ("001", pd_data[0].code2);
    free (pd_data);
}

static void
test_get_city_voivodeship_info_returns_zero_for_unknown_ids (void)
{
    Postal_data *pd_data = NULL;

    uint16_t i_rcnt = get_city_voivodeship_info (&pd_data, -1, -1);

    TEST_ASSERT_EQUAL_UINT16 (0, i_rcnt);
    TEST_ASSERT_NULL (pd_data);
}

/*----------------------------------------------------------------------------*/

int
main (void)
{
    UNITY_BEGIN ();

    RUN_TEST (test_clear_pdata_zeroes_all_fields);
    RUN_TEST (test_clear_cdata_zeroes_all_fields);

    RUN_TEST (test_set_codes_splits_dashed_six_char_string);
    RUN_TEST (test_set_codes_ignores_char_at_index_two);

    RUN_TEST (test_get_pd_column_width_counts_ascii_bytes);
    RUN_TEST (test_get_pd_column_width_counts_utf8_codepoints_not_bytes);
    RUN_TEST (test_get_pd_column_width_takes_max_across_rows);

    RUN_TEST (test_get_post_code_info_returns_known_row);
    RUN_TEST (test_get_post_code_info_returns_zero_for_unknown_code);

    RUN_TEST (test_get_city_names_like_finds_krakow);
    RUN_TEST (test_get_city_names_like_returns_zero_for_no_match);

    RUN_TEST (test_get_city_voivodeship_info_returns_known_count_and_first_row);
    RUN_TEST (test_get_city_voivodeship_info_returns_zero_for_unknown_ids);

    return UNITY_END ();
}
