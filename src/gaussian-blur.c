////////////////////////////////////////////////////////////////////////////////
//3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
//      10        20        30        40        50        60        70        80
//
// raico-blur
//
// gaussian-blur.c - implements gaussian-blur function
//
// Copyright 2009 Canonical Ltd.
//
// Authors:
//    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
//
// Notes:
//    based on filters in libpixman
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License version 3, as published
// by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranties of
// MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
// PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <pixman.h>

#include "gaussian-blur.h"

pixman_fixed_t*
create_gaussian_blur_kernel (gint    radius,
                             gdouble sigma,
                             gint*   length)
{
	const gdouble   scale2 = 2.0f * sigma * sigma;
	const gdouble   scale1 = 1.0f / (G_PI * scale2);
	const gint      size = 2 * radius + 1;
	const gint      n_params = size * size;
	pixman_fixed_t* params;
	gdouble*        tmp;
	gdouble         sum;
	gint            x;
	gint            y;
	gint            i;

        tmp = g_newa (double, n_params);

        // caluclate gaussian kernel in floating point format
        for (i = 0, sum = 0, x = -radius; x <= radius; ++x) {
                for (y = -radius; y <= radius; ++y, ++i) {
                        const gdouble u = x * x;
                        const gdouble v = y * y;

                        tmp[i] = scale1 * exp (-(u+v)/scale2);

                        sum += tmp[i];
                }
        }

        // normalize gaussian kernel and convert to fixed point format
        params = g_new (pixman_fixed_t, n_params + 2);

        params[0] = pixman_int_to_fixed (size);
        params[1] = pixman_int_to_fixed (size);

        for (i = 0; i < n_params; ++i)
                params[2 + i] = pixman_double_to_fixed (tmp[i] / sum);

        if (length)
                *length = n_params + 2;

        return params;
}

void
_blur_image_surface (cairo_surface_t* surface,
		     gint             radius,
		     gdouble          sigma /* pass 0.0f for auto-calculation */)
{
        pixman_fixed_t* params = NULL;
        gint            n_params;
        pixman_image_t* src;
        gint            w;
        gint            h;
        gint            s;
        gpointer        p;
	gdouble         radiusf;

	radiusf = fabs (radius) + 1.0f;
	if (sigma == 0.0f)
		sigma = sqrt (-(radiusf * radiusf) / (2.0f * log (1.0f / 255.0f)));

        w = cairo_image_surface_get_width (surface);
        h = cairo_image_surface_get_height (surface);
        s = cairo_image_surface_get_stride (surface);

	// create pixman image for cairo image surface
	p = cairo_image_surface_get_data (surface);
	src = pixman_image_create_bits (PIXMAN_a8r8g8b8, w, h, p, s);

	// attach gaussian kernel to pixman image
	params = create_gaussian_blur_kernel (radius, sigma, &n_params);
	pixman_image_set_filter (src,
				 PIXMAN_FILTER_CONVOLUTION,
				 params,
				 n_params);
	g_free (params);

        // render blured image to new pixman image
        pixman_image_composite (PIXMAN_OP_SRC,
				src,
				NULL,
				src,
				0,
				0,
				0,
				0,
				0,
				0,
				w,
				h);
	pixman_image_unref (src);
}

void
surface_gaussian_blur (cairo_surface_t* surface,
		       guint            radius)
{
	guchar*        pixels;
	guint          width;
	guint          height;
	cairo_format_t format;

	// sanity checks are done in raico-blur.c

	// before we mess with the surface execute any pending drawing
	cairo_surface_flush (surface);

	pixels = cairo_image_surface_get_data (surface);
	width  = cairo_image_surface_get_width (surface);
	height = cairo_image_surface_get_height (surface);
	format = cairo_image_surface_get_format (surface);

	switch (format)
	{
		case CAIRO_FORMAT_ARGB32:
			_blur_image_surface (surface, radius, 0.0f);
		break;

		case CAIRO_FORMAT_RGB24:
			// do nothing, for now
		break;

		case CAIRO_FORMAT_A8:
			// do nothing, for now
		break;

		default :
			// do nothing
		break;
	}

	// inform cairo we altered the surfaces contents
	cairo_surface_mark_dirty (surface);	
}

