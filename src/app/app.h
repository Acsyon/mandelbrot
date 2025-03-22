/* app/app.h
 *
 * Header for (global app object)
 *
 */

#ifndef MANDELBROT_APP_APP_H_INCLUDED
#define MANDELBROT_APP_APP_H_INCLUDED

#include <app/data.h>
#include <app/settings.h>
#include <app/video.h>

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
 * Returns GraphicsData object stored in App.
 *
 * @return GraphicsData object stored in App
 */
GraphicsData *
App_get_graphics_data(void);

/**
 * Returns Video object stored in App.
 *
 * @return Video object stored in App
 */
Video *
App_get_video(void);

#endif /* MANDELBROT_APP_APP_H_INCLUDED */
