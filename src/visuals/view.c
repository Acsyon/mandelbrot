#include <visuals/view.h>

#include <cutil/io/log.h>
#include <cutil/std/stdlib.h>
#include <cutil/util/macro.h>

#include <util/util.h>

View *
View_create(void)
{
    View *const view = malloc(sizeof *view);

    view->prec = mpf_get_default_prec();
    mpf_init(view->cntr_re);
    mpf_init(view->cntr_im);
    mpf_init(view->upp);

    return view;
}

void
View_free(View *view)
{
    CUTIL_RETURN_IF_NULL(view);

    mpf_clear(view->cntr_re);
    mpf_clear(view->cntr_im);
    mpf_clear(view->upp);

    free(view);
}

void
View_set_precision(View *view, mp_bitcnt_t prec)
{
    view->prec = prec;
    mpf_set_prec(view->cntr_re, prec);
    mpf_set_prec(view->cntr_im, prec);
    mpf_set_prec(view->upp, prec);
}

void
View_fill_from_Settings(View *view, const Settings *settings)
{
    const double cntr_re = Settings_get_center_real(settings);
    const double cntr_im = Settings_get_center_imag(settings);
    const double upp = Settings_get_units_per_pixel(settings);

    mpf_set_d(view->cntr_re, cntr_re);
    mpf_set_d(view->cntr_im, cntr_im);
    mpf_set_d(view->upp, upp);
}

void
View_fill_from_Json(View *view, const Json *json)
{
    size_t prec = view->prec;
    Json_elem_to_size_t(json, "prec", &prec);
    view->prec = prec;
    View_set_precision(view, view->prec);

#define JSON_TO_MEMBER(MEMBER)                                                 \
    do {                                                                       \
        char *str = NULL;                                                      \
        Json_elem_to_str(json, #MEMBER, &str);                                 \
        mpf_set_str(view->MEMBER, str, 10);                                    \
        free(str);                                                             \
    } while (0)

    JSON_TO_MEMBER(cntr_re);
    JSON_TO_MEMBER(cntr_im);
    JSON_TO_MEMBER(upp);

#undef JSON_TO_MEMBER
}

void
View_fill_from_Json_void(void *vview, const Json *json)
{
    View_fill_from_Json(vview, json);
}

Json *
View_to_Json(const View *view)
{
    Json *const json = Json_create();

    const int status = Json_add_size_t(json, "prec", view->prec);
    if (status != EXIT_SUCCESS) {
        cutil_log_error(
          "Error while creating JSON from Settings member '%s'", "prec"
        );
        Json_free(json);
        return NULL;
    }

#define MEMBER_TO_JSON(MEMBER)                                                 \
    do {                                                                       \
        char *const str = Util_mpf_to_str_base10(view->MEMBER);                \
        const int status = Json_add_str(json, #MEMBER, str);                   \
        if (status != EXIT_SUCCESS) {                                          \
            cutil_log_error(                                                   \
              "Error while creating JSON from Settings member '%s'", #MEMBER   \
            );                                                                 \
            Json_free(json);                                                   \
            return NULL;                                                       \
        }                                                                      \
        free(str);                                                             \
    } while (0)

    MEMBER_TO_JSON(cntr_re);
    MEMBER_TO_JSON(cntr_im);
    MEMBER_TO_JSON(upp);

#undef MEMBER_TO_JSON

    return json;
}

Json *
View_to_Json_void(const void *vview)
{
    return View_to_Json(vview);
}
