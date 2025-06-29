#include "unity.h"

#include <gmp.h>

#include <cutil/std/stdlib.h>
#include <cutil/util/macro.h>

#include <util/mpf.h>

#define DEFAULT_PRECISION 512

static void
_should_convertMpfToStringAndBack_when_useBase10(void)
{
    /* Arrange */
    static const int BASE = 10;
    static const char *const ASSERTS[] = {
      "0.123375e3", "0.31415e-100", "0.271828e1024",
      "0."
      "316227766016837933199889354443271853371955513932521682685750485279259443"
      "863923822134424810837930029518734728415284005514854885603045388001469052"
      "e1"
    };
    static const size_t NUM_ASSERTS = (sizeof ASSERTS) / (sizeof *ASSERTS);

    mpf_t old_mpf, new_mpf;
    mpf_inits(old_mpf, new_mpf, NULL);

    MpfStringBuilder *const mpfsb = MpfStringBuilder_create();
    size_t buflen = 0;
    char *str = NULL;

    for (size_t i = 0; i < NUM_ASSERTS; ++i) {
        const char *const assert_str = ASSERTS[i];
        mpf_set_str(old_mpf, assert_str, BASE);

        /* Act */
        MpfStringBuilder_copy_string(mpfsb, old_mpf, BASE, &buflen, &str);
        mpf_set_str(new_mpf, str, BASE);

        /* Assert */
        TEST_ASSERT_NOT_NULL(str);
        TEST_ASSERT_EQUAL_STRING(assert_str, str);
        TEST_ASSERT_EQUAL_INT(0, mpf_cmp(old_mpf, new_mpf));
    }

    /* Cleanup */
    free(str);
    MpfStringBuilder_free(mpfsb);
    mpf_clears(old_mpf, new_mpf, NULL);
}

static void
_should_convertMpfToStringAndBack_when_useOtherBases(void)
{
    /* Arrange */
    static const char *const INPUT_STR = "0.123375e3";
    static const int INPUT_STR_BASE = 10;
    mpf_t old_mpf;
    mpf_init_set_str(old_mpf, INPUT_STR, INPUT_STR_BASE);

    /* Instead of checking for equality, we calculate relative differences in
     * this test because the input might be repeating for certain bases. */
    mpf_t max_reldiff;
    mpf_init_set_str(max_reldiff, "1.0e-100", INPUT_STR_BASE);

    mpf_t new_mpf, reldiff;
    mpf_inits(new_mpf, reldiff, NULL);

    MpfStringBuilder *const mpfsb = MpfStringBuilder_create();
    size_t buflen = 0;
    char *str = NULL;

    for (int base = 2; base <= 62; ++base) {
        /* Act */
        MpfStringBuilder_copy_string(mpfsb, old_mpf, base, &buflen, &str);
        mpf_set_str(new_mpf, str, base);

        /* Assert */
        TEST_ASSERT_NOT_NULL(str);
        mpf_reldiff(reldiff, old_mpf, new_mpf);
        TEST_ASSERT_EQUAL_INT(-1, mpf_cmp(reldiff, max_reldiff));
    }

    /* Cleanup */
    free(str);
    MpfStringBuilder_free(mpfsb);
    mpf_clears(old_mpf, new_mpf, reldiff, max_reldiff, NULL);
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

    mpf_set_default_prec(DEFAULT_PRECISION);
    RUN_TEST(_should_convertMpfToStringAndBack_when_useBase10);
    RUN_TEST(_should_convertMpfToStringAndBack_when_useOtherBases);

    return UNITY_END();
}
