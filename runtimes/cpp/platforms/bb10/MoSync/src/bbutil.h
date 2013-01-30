/*
 * Copyright (c) 2011-2012 Research In Motion Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _UTILITY_H_INCLUDED
#define _UTILITY_H_INCLUDED

#include <EGL/egl.h>
#include <screen/screen.h>
#include <sys/platform.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes EGL
 *
 * @param gl2 If non-zero, OpenGL v2.0 will be used, otherwise v1.1.
 * @return EXIT_SUCCESS if initialization succeeded otherwise EXIT_FAILURE.
 */
int bbutil_init_egl(screen_window_t, int gl2);

/**
 * Terminates EGL.
 */
void bbutil_terminate(void);

/**
 * Swaps default bbutil window surface to the screen.
 */
void bbutil_swap(void);

#define EXIT_FAILURE 0
#define EXIT_SUCCESS 1

#ifdef __cplusplus
}
#endif

#endif
