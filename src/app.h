/* app.h
 *
 * Header for (global app object)
 *
 */

#ifndef APP_H_INCLUDED
#define APP_H_INCLUDED

#include "data.h"
#include "settings.h"
#include "video.h"

/**
 * Runs application.
 *
 * @param[in] env_path path for working environment (can be NULL)
 * @param[in] settings pointer to Settings object (can be NULL)
 *
 * @return error code (EXIT_SUCCESS or EXIT_FAILURE)
 */
int
App_run(const char *env_path, const Settings *settings);

/**
 * Returns path for working environment stored in App.
 *
 * @return path for working environment stored in App
 */
const char *
App_get_env_path(void);

/**
 * Returns Settings object stored in App.
 *
 * @return Settings object stored in App
 */
const Settings *
App_get_settings(void);

/**
 * Returns ImageData object stored in App.
 *
 * @return ImageData object stored in App
 */
ImageData *
App_get_image_data(void);

/**
 * Returns Video object stored in App.
 *
 * @return Video object stored in App
 */
Video *
App_get_video(void);

#endif /* APP_H_INCLUDED */
