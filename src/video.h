/* video.h
 *
 * Header for video output
 *
 */

#ifndef VIDEO_H_INCLUDED
#define VIDEO_H_INCLUDED

/**
 * Initializes video stuff.
 */
void
Video_init(void);

/**
 * Quits video stuff.
 */
void
Video_quit(void);

/**
 * Performs video loop on global ImageData object.
 */
void
Video_loop(void);

#endif /* VIDEO_H_INCLUDED */
