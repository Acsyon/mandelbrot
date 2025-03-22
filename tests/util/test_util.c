#include "unity.h"

#include <stdlib.h>
#include <string.h>

#include <gmp.h>

#include <util/util.h>

static void
_should_duplicateString_when_callUtilStrdup(void)
{
    /* Arrange */
    const char *const str = "test_string";

    /* Act */
    char *const result = Util_strdup(str);

    /* Assert */
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING(str, result);

    /* Cleanup */
    free(result);
}

static void
_should_returnNull_when_callUtilStrdupWithNullInput(void)
{
    /* Arrange */
    const char *const str = NULL;

    /* Act */
    char *const result = Util_strdup(str);

    /* Assert */
    TEST_ASSERT_NULL(result);
}

static void
_should_readFileToString_when_callUtilFileToStr(void)
{
    /* Arrange */
    FILE *const file = tmpfile();
    fprintf(file, "test_content");
    rewind(file);

    /* Act */
    char *const result = Util_file_to_str(file);

    /* Assert */
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("test_content", result);

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
    char *const result = Util_file_to_str(file);

    /* Assert */
    TEST_ASSERT_NULL(result);
}

static void
_should_convertMpfToString_when_callUtilMpfToStrBase10(void)
{
    /* Arrange */
    mpf_t mpf;
    mpf_init_set_d(mpf, 123.375);

    /* Act */
    char *const result = Util_mpf_to_str_base10(mpf);

    /* Assert */
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("0.123375e3", result);

    /* Cleanup */
    free(result);
    mpf_clear(mpf);
}

static void
_should_calculateNewPrecision_when_callUtilCalculateNewPrec(void)
{
    /* Arrange */
    mpf_t upp;
    mpf_init_set_str(upp, "1.0e-100", 10);

    /* Act */
    const mp_bitcnt_t result = Util_calculate_new_prec(upp);

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

    RUN_TEST(_should_duplicateString_when_callUtilStrdup);
    RUN_TEST(_should_returnNull_when_callUtilStrdupWithNullInput);
    RUN_TEST(_should_readFileToString_when_callUtilFileToStr);
    RUN_TEST(_should_returnNull_when_callUtilFileToStrWithInvalidFile);
    RUN_TEST(_should_convertMpfToString_when_callUtilMpfToStrBase10);
    RUN_TEST(_should_calculateNewPrecision_when_callUtilCalculateNewPrec);

    return UNITY_END();
}
