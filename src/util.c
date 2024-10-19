#include "util.h"

#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "log.h"

char *
file_to_str(FILE *in)
{
    fseek(in, 0L, SEEK_END);
    const long fsize = ftell(in) + 1;
    rewind(in);

    char *const fstr = malloc(fsize * sizeof *fstr);
    if (fstr == NULL) {
        log_err("Cannot allocate memory for copy of file!\n");
        return NULL;
    }
    const size_t size = fread(fstr, 1UL, fsize, in);
    if (size != (size_t) fsize - 1) {
        log_err("Cannot copy contents of file!\n");
        free(fstr);
        return NULL;
    }
    return fstr;
}

char *
mpf_to_str_base10(mpf_srcptr mpf)
{
    static const int BASE = 10;
    const double log2 = log(2);
    const double logb = log(abs(BASE));

    const mp_bitcnt_t prec = mpf_get_prec(mpf);
    const size_t maxlen_mantissa = prec * ceil(log2 / logb);
    const size_t maxlen_exponent = ceil(log(maxlen_mantissa) / logb);

    const size_t maxlen = sizeof "+0.e" + maxlen_mantissa + maxlen_exponent;
    char *res = malloc(maxlen * sizeof *res);

    const bool is_negative = (mpf_sgn(mpf) < 0);
    sprintf(res, "%s", (is_negative) ? "-0." : "0.");

    const size_t buflen = maxlen_mantissa + 1;
    char *const buf = malloc(buflen * sizeof *res);

    mp_exp_t exp;
    mpf_get_str(buf, &exp, BASE, buflen, mpf);
    res = strcat(res, &buf[(is_negative) ? 1 : 0]);

    sprintf(buf, "e%li", exp);
    res = strcat(res, buf);

    free(buf);
    const size_t len = strlen(res) + 1;
    return realloc(res, len * sizeof *res);
}
