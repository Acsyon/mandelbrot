#include <util/mpf.h>

#include <cutil/debug/null.h>
#include <cutil/io/log.h>
#include <cutil/std/math.h>
#include <cutil/std/stdlib.h>
#include <cutil/string/builder.h>
#include <cutil/util/macro.h>

#define MINIMAL_VALID_BASE 2
#define MAXIMAL_VALID_BASE 64
#define AT_SIGN_AS_EXP_CHAR_THRESHOLD 10
#define UPPERCASE_DIGITS_THRESHOLD 36

#define DEFAULT_BUFLEN_EXPONENT 16
#define DEFAULT_BUFLEN_MANTISSA 64
#define RESIZE_FACTOR_BUFLEN 2

struct _mpfStringBuilder {
    cutil_StringBuilder *sb;
    mp_exp_t exp;
    size_t buflen_exp;
    char *buf_exp;
    size_t buflen_mantissa;
    char *buf_mantissa;
};

MpfStringBuilder *
MpfStringBuilder_create(void)
{
    MpfStringBuilder *const mpfsb = malloc(sizeof *mpfsb);

    mpfsb->sb = cutil_StringBuilder_alloc(0);
    mpfsb->buflen_exp = DEFAULT_BUFLEN_EXPONENT;
    mpfsb->buf_exp = malloc(mpfsb->buflen_exp * sizeof *mpfsb->buf_exp);
    mpfsb->buflen_mantissa = DEFAULT_BUFLEN_MANTISSA;
    mpfsb->buf_mantissa
      = malloc(mpfsb->buflen_mantissa * sizeof *mpfsb->buf_mantissa);

    return mpfsb;
}

void
MpfStringBuilder_free(MpfStringBuilder *mpfsb)
{
    CUTIL_RETURN_IF_NULL(mpfsb);

    cutil_StringBuilder_free(mpfsb->sb);
    free(mpfsb->buf_exp);
    free(mpfsb->buf_mantissa);

    free(mpfsb);
}

static size_t
_get_newlen(size_t maxlen, size_t buflen)
{
    size_t res = buflen;
    while (maxlen > res) {
        res *= RESIZE_FACTOR_BUFLEN;
    }
    return res;
}

static void
_mpfStringBuilder_convert_exp(MpfStringBuilder *mpfsb, int base)
{
    if (base == 10) {
        sprintf(mpfsb->buf_exp, "%li", mpfsb->exp);
        return;
    }

    /**
     * Digits as used by GMP
     */
    const char *const digits = (base <= UPPERCASE_DIGITS_THRESHOLD)
      ? "0123456789abcdefghijklmnopqrstuvwxyz"
      : "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    unsigned long nexp = labs(mpfsb->exp);
    const int is_negative = (mpfsb->exp < 0);
    const size_t maxlen_exp = ceil(log(nexp) / log(base)) + 1 + is_negative;

    const size_t newlen = _get_newlen(maxlen_exp, mpfsb->buflen_exp);
    if (newlen > mpfsb->buflen_exp) {
        mpfsb->buflen_exp = newlen;
        char **const p_buf = &mpfsb->buf_exp;
        *p_buf = realloc(*p_buf, newlen * sizeof **p_buf);
    }

    char *const buf = mpfsb->buf_exp;
    int i = 0;
    do {
        const size_t rem = nexp % base;
        buf[i++] = digits[rem];
        nexp /= base;
    } while (nexp > 0);

    if (is_negative) {
        buf[i++] = '-';
    }

    buf[i] = '\0';

    /* Reverse the string (since we generated digits in reverse order) */
    for (int j = 0, k = i - 1; j < k; ++j, --k) {
        const char temp = buf[j];
        buf[j] = buf[k];
        buf[k] = temp;
    }
}

static void
_mpfStringBuilder_fill_sb(MpfStringBuilder *mpfsb, mpf_srcptr mpf, int base)
{
    cutil_StringBuilder *const sb = mpfsb->sb;
    if (base < MINIMAL_VALID_BASE || base > MAXIMAL_VALID_BASE) {
        cutil_log_error(
          "Invalid base, has to be between %i and %i", MINIMAL_VALID_BASE,
          MAXIMAL_VALID_BASE
        );
        cutil_StringBuilder_clear(sb);
        return;
    }

    const double log2 = log(2);
    const double logb = log(base);

    const mp_bitcnt_t prec = mpf_get_prec(mpf);
    const size_t maxlen_mantissa = prec * ceil(log2 / logb) + 1;

    const size_t newlen = _get_newlen(maxlen_mantissa, mpfsb->buflen_mantissa);
    if (newlen > mpfsb->buflen_mantissa) {
        mpfsb->buflen_mantissa = newlen;
        char **const p_buf = &mpfsb->buf_mantissa;
        *p_buf = realloc(*p_buf, newlen * sizeof **p_buf);
    }

    const char *const sgn_str = (mpf_sgn(mpf) < 0) ? "-" : "";
    const char exp_char = (base <= AT_SIGN_AS_EXP_CHAR_THRESHOLD) ? 'e' : '@';
    mpf_get_str(mpfsb->buf_mantissa, &mpfsb->exp, base, maxlen_mantissa, mpf);
    _mpfStringBuilder_convert_exp(mpfsb, base);

    cutil_StringBuilder_clear(sb);
    cutil_StringBuilder_appendf(
      sb, "%s0.%s%c%s", sgn_str, mpfsb->buf_mantissa, exp_char, mpfsb->buf_exp
    );
}

char *
MpfStringBuilder_to_string(MpfStringBuilder *mpfsb, mpf_srcptr mpf, int base)
{
    CUTIL_NULL_CHECK(mpfsb);
    _mpfStringBuilder_fill_sb(mpfsb, mpf, base);
    return cutil_StringBuilder_duplicate_string(mpfsb->sb);
}

size_t
MpfStringBuilder_copy_string_to_buf(
  MpfStringBuilder *mpfsb, mpf_srcptr mpf, int base, size_t buflen, char *buf
)
{
    CUTIL_NULL_CHECK(mpfsb);
    _mpfStringBuilder_fill_sb(mpfsb, mpf, base);
    return cutil_StringBuilder_copy_string_to_buf(mpfsb->sb, buflen, buf);
}

size_t
MpfStringBuilder_copy_string(
  MpfStringBuilder *mpfsb,
  mpf_srcptr mpf,
  int base,
  size_t *p_buflen,
  char **p_buf
)
{
    CUTIL_NULL_CHECK(mpfsb);
    _mpfStringBuilder_fill_sb(mpfsb, mpf, base);
    return cutil_StringBuilder_copy_string(mpfsb->sb, p_buflen, p_buf);
}
