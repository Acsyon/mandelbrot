#include "unity.h"

#include <gmp.h>

#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>

#include <util/util.h>

static void
_should_readFileToString_when_callUtilFileToStr(void)
{
    /* Arrange */
    FILE *const file = tmpfile();
    const char *const str = "test_content\n";
    fprintf(file, str);
    rewind(file);

    /* Act */
    char *const result = util_file_to_str(file);

    /* Assert */
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING(str, result);

    /* Cleanup */
    free(result);
    fclose(file);
}

static void
_should_returnNull_when_callUtilFileToStrWithInvalidFile(void)
{
    /* Arrange */
    FILE *const file = NULL;

    /* Act */
    char *const result = util_file_to_str(file);

    /* Assert */
    TEST_ASSERT_NULL(result);
}

static void
_should_convertMpfToString_when_callUtilMpfToStrBase10(void)
{
    /* Arrange */
    const double assert_dbl = 123.375;
    mpf_t mpf;
    mpf_init_set_d(mpf, assert_dbl);

    /* Act */
    char *const str = util_mpf_to_str_base10(mpf);
    mpf_set_str(mpf, str, 10);
    const double dbl = mpf_get_d(mpf);

    /* Assert */
    TEST_ASSERT_NOT_NULL(str);
    TEST_ASSERT_EQUAL_STRING("0.123375e3", str);
    TEST_ASSERT_EQUAL_FLOAT(assert_dbl, dbl);

    /* Cleanup */
    free(str);
    mpf_clear(mpf);
}

static void
_should_calculateNewPrecision_when_callUtilCalculateNewPrec(void)
{
    /* Arrange */
    mpf_t upp;
    mpf_init_set_str(upp, "1.0e-100", 10);

    /* Act */
    const mp_bitcnt_t result = util_calculate_new_prec(upp);

    /* Assert */
    TEST_ASSERT_EQUAL(384, result);
    mpf_clear(upp);
}

void
setUp(void)
{}

void
tearDown(void)
{}

int
main(void)
{
    UNITY_BEGIN();

    RUN_TEST(_should_readFileToString_when_callUtilFileToStr);
    RUN_TEST(_should_returnNull_when_callUtilFileToStrWithInvalidFile);
    RUN_TEST(_should_convertMpfToString_when_callUtilMpfToStrBase10);
    RUN_TEST(_should_calculateNewPrecision_when_callUtilCalculateNewPrec);

    return UNITY_END();
}
