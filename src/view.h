/* view.h
 *
 * Header for view objects
 *
 */

#ifndef VIEW_H_INCLUDED
#define VIEW_H_INCLUDED

#include <gmp.h>

#include "settings.h"

/**
 * Struct for view on section of Mandelbrot set
 */
typedef struct {
    mp_bitcnt_t prec;
    mpf_t cntr_re;
    mpf_t cntr_im;
    mpf_t upp;
} View;

/**
 * Creates newly malloc'd (and default-initialized) View object.
 *
 * @return pointer to newly malloc'd (and default-initialized) View object
 */
View *
View_create(void);

/**
 * Frees memory pointed to by `view`.
 *
 * @param[in] view pointer to View object to be freed
 */
void
View_free(View *view);

/**
 * Sets precision of members of `view` to `prec`.
 *
 * @param[in] view View object to change precision of
 * @param[in] prec new precision
 */
void
View_set_precision(View *view, mp_bitcnt_t prec);

/**
 * Fills members of `view` according to content of `settings`.
 *
 * @param[in, out] view View object to fill
 * @param[in] settings Settings object to get data from
 */
void
View_fill_from_Settings(View *view, const Settings *settings);

/**
 * Fills members of `view` according to content of `json`.
 *
 * @param[in, out] view View object to fill
 * @param[in] json Json object to get data from
 */
void
View_fill_from_Json(View *view, const Json *json);

/**
 * Fills members of `vview` according to content of `json`. Version with void
 * pointer to be used as a 'JsonUtilReadCallback'.
 *
 * @param[in, out] vview View object (as void pointer) to fill
 * @param[in] json Json object to get data from
 */
void
View_fill_from_Json_void(void *vview, const Json *json);

/**
 * Returns Json object with content of `view`.
 *
 * @param[in] view View object to read data from
 *
 * @return Json object with content of `view`
 */
Json *
View_to_Json(const View *view);

/**
 * Returns Json object with content of `view`. Version with void pointer to be
 * used as a 'JsonUtilWriteCallback'.
 *
 * @param[in] vview View object (as void pointer) to read data from
 *
 * @return Json object with content of `view`
 */
Json *
View_to_Json_void(const void *vview);

#endif /* VIEW_H_INCLUDED */
