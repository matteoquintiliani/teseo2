/* Teseo-2 Plug-in
 * Copyright (C) 2005  Stefano Pintore, Matteo Quintiliani (the "Authors").
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE Authors BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the Authors of the
 * Software shall not be used in advertising or otherwise to promote the
 * sale, use or other dealings in this Software without prior written
 * authorization from the Authors.
 */

#include "teseo_gimp_extends.h"


// Wrapper to find next guide with orientation in parameter
gint32 teseo_gimp_image_find_next_guide_orientation(gint32 g_image, gint32 g_guide, gint32 g_orientation) {
    gint32 curr_guide = g_guide;
    gint32 curr_orientation;
    gboolean continue_while = TRUE;

    while(continue_while) {
	curr_guide = gimp_image_find_next_guide(g_image, curr_guide);
	if(curr_guide !=0 ) {
	    curr_orientation = gimp_image_get_guide_orientation(g_image, curr_guide);
	    if(curr_orientation == g_orientation) {
		continue_while = FALSE;
	    }
	} else {
	    continue_while = FALSE;
	}
    }

    return curr_guide;
}

