#include <util/util.h>

#include <cutil/io/log.h>
#include <cutil/std/math.h>
#include <cutil/std/stdlib.h>
#include <cutil/string/builder.h>
#include <cutil/util/macro.h>

char *
util_concat_paths(const char *path1, const char *path2)
{
    cutil_StringBuilder *const sb = cutil_StringBuilder_create();
    cutil_StringBuilder_appendf(sb, "%s/%s", path1, path2);
    char *const res = cutil_StringBuilder_duplicate_string(sb);
    cutil_StringBuilder_free(sb);
    return res;
}

char *
util_file_to_str(FILE *in)
{
    CUTIL_RETURN_NULL_IF_NULL(in);

    fseek(in, 0L, SEEK_END);
    const long fsize = ftell(in) + 1;
    rewind(in);

    char *const fstr = malloc(fsize * sizeof *fstr);
    if (fstr == NULL) {
        cutil_log_error("Cannot allocate memory for copy of file");
        return NULL;
    }
    const size_t size = fread(fstr, 1UL, fsize, in);
    if (size != (size_t) fsize - 1) {
        cutil_log_error("Cannot copy contents of file");
        free(fstr);
        return NULL;
    }
    return fstr;
}

static char *
_get_mpf_str_base10(mpf_srcptr mpf, mp_exp_t *p_exp)
{
    static const int BASE = 10;
    const double log2 = log(2);
    const double logb = log(abs(BASE));

    const mp_bitcnt_t prec = mpf_get_prec(mpf);
    const size_t maxlen_mantissa = prec * ceil(log2 / logb) + 1;
    char *const mantissa_str = malloc(maxlen_mantissa * sizeof *mantissa_str);

    mpf_get_str(mantissa_str, p_exp, BASE, maxlen_mantissa, mpf);

    return mantissa_str;
}

char *
util_mpf_to_str_base10(mpf_srcptr mpf)
{
    cutil_StringBuilder *const sb = cutil_StringBuilder_create();

    const char *const sgn_str = (mpf_sgn(mpf) < 0) ? "-0." : "0.";
    mp_exp_t exp;
    char *const mantissa_str = _get_mpf_str_base10(mpf, &exp);

    cutil_StringBuilder_appendf(sb, "%s", sgn_str);
    cutil_StringBuilder_appendf(sb, "%s", mantissa_str);
    cutil_StringBuilder_appendf(sb, "e%li", exp);

    free(mantissa_str);

    char *const res = cutil_StringBuilder_duplicate_string(sb);
    cutil_StringBuilder_free(sb);
    return res;
}

mp_bitcnt_t
util_calculate_new_prec(mpf_srcptr upp)
{
    static const mp_bitcnt_t MIN_PREC = GMP_LIMB_BITS;
    static const mp_bitcnt_t MAX_PREC = GMP_LIMB_BITS * 1024;
    static const mp_bitcnt_t MAX_DIFF = GMP_LIMB_BITS / 4;

    long int exp = 0;
    CUTIL_UNUSED(mpf_get_d_2exp(&exp, upp));
    exp = labs(exp);

    const mp_bitcnt_t new_prec
      = ((exp + MAX_DIFF + GMP_LIMB_BITS) / GMP_LIMB_BITS) * GMP_LIMB_BITS;
    if (new_prec < MIN_PREC) {
        return MIN_PREC;
    }
    if (new_prec > MAX_PREC) {
        return MAX_PREC;
    }
    return new_prec;
}
