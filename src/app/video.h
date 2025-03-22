/* app/video.h
 *
 * Header for video output
 *
 */

#ifndef MANDELBROT_APP_VIDEO_H_INCLUDED
#define MANDELBROT_APP_VIDEO_H_INCLUDED

/**
 * Opaque Video type
 */
typedef struct Video Video;

/**
 * Initializes and returns Video object according to Settings in App.
 *
 * @return Video object according to Settings in App
 */
Video *
Video_app_init(void);

/**
 * Frees memory pointed to by `video`.
 *
 * @param[in] video pointer to Video object to be freed
 */
void
Video_free(Video *video);

/**
 * Performs video loop on `video`.
 *
 * @param[in] video pointer to Video object to run video loop on
 */
void
Video_loop(Video *video);

#endif /* MANDELBROT_APP_VIDEO_H_INCLUDED */
