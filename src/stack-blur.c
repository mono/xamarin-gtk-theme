////////////////////////////////////////////////////////////////////////////////
//3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
//      10        20        30        40        50        60        70        80
//
// raico-blur
//
// stack-blur.c - implements stack-blur function
//
// Copyright 2009 Canonical Ltd.
//
// Authors:
//    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
//
// Notes:
//    based on stack-blur algorithm by Mario Klingemann <mario@quasimondo.com>
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

// FIXME: not working yet, unfinished

#include <math.h>
#include <stdlib.h>

#include "stack-blur.h"

void
surface_stack_blur (cairo_surface_t* surface,
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
		case CAIRO_FORMAT_ARGB32:{

	gint w   = width;
	gint h   = height;
	gint wm  = w - 1;
	gint hm  = h - 1;
	gint wh  = w * h;
	gint div = radius + radius + 1;

	if (radius < 1)
		break;

	gint* r = (gint*) g_malloc0 (wh);
	gint* g = (gint*) g_malloc0 (wh);
	gint* b = (gint*) g_malloc0 (wh);
	gint* a = (gint*) g_malloc0 (wh);

	gint rsum;
	gint gsum;
	gint bsum;
	gint asum;
	gint x;
	gint y;
	gint i;
	gint yp;
	gint yi;
	gint yw;
	gint* dv = NULL;

	gint* vmin = (gint*) g_malloc0 (MAX (w, h));

	gint divsum = (div + 1) >> 1;
	divsum *= divsum;
	dv = (gint*) g_malloc0 (256 * divsum);
	g_assert (dv != NULL);

	for (i = 0; i < 256 * divsum; ++i)
		dv[i] = (i / divsum);

	yw = yi = 0;

	gint** stack =  (gint**) g_malloc0 (div);

	for (i = 0; i < div; ++i)
		stack[i] = (gint*) g_malloc0 (4);

	gint  stackpointer;
	gint  stackstart;
	gint* sir;
	gint  rbs;
	gint  r1 = radius + 1;
	gint  routsum;
	gint  goutsum;
	gint  boutsum;
	gint  aoutsum;
	gint  rinsum;
	gint  ginsum;
	gint  binsum;
	gint  ainsum;

	for (y = 0; y < h; ++y)
	{
		rinsum = ginsum = binsum = ainsum = routsum = goutsum = boutsum = aoutsum = rsum = gsum = bsum = asum = 0;

		for(i =- radius; i <= radius; ++i)
		{
			sir = stack[i + radius];
			sir[0] = pixels[yi + MIN (wm, MAX (i, 0)) + 0];
			sir[1] = pixels[yi + MIN (wm, MAX (i, 0)) + 1];
			sir[2] = pixels[yi + MIN (wm, MAX (i, 0)) + 2];
			sir[3] = pixels[yi + MIN (wm, MAX (i, 0)) + 3];
            
			rbs = r1 - abs (i);
			rsum += sir[0] * rbs;
			gsum += sir[1] * rbs;
			bsum += sir[2] * rbs;
			asum += sir[3] * rbs;
            
			if (i > 0)
			{
				rinsum += sir[0];
				ginsum += sir[1];
				binsum += sir[2];
				ainsum += sir[3];
			}
			else
			{
				routsum += sir[0];
				goutsum += sir[1];
				boutsum += sir[2];
				aoutsum += sir[3];
			}
		}

		stackpointer = radius;

		for (x = 0; x < w; ++x)
		{
			r[yi] = dv[rsum];
			g[yi] = dv[gsum];
			b[yi] = dv[bsum];
			a[yi] = dv[asum];

			rsum -= routsum;
			gsum -= goutsum;
			bsum -= boutsum;
			asum -= aoutsum;

			stackstart = stackpointer - radius + div;
			sir = stack[stackstart % div];

			routsum -= sir[0];
			goutsum -= sir[1];
			boutsum -= sir[2];
			aoutsum -= sir[3];

			if (y == 0)
				vmin[x] = MIN (x + radius + 1, wm);

			sir[0] = pixels[yw + vmin[x] + 0];
			sir[1] = pixels[yw + vmin[x] + 1];
			sir[2] = pixels[yw + vmin[x] + 2];
			sir[3] = pixels[yw + vmin[x] + 3];

			rinsum += sir[0];
			ginsum += sir[1];
			binsum += sir[2];
			ainsum += sir[3];

			rsum += rinsum;
			gsum += ginsum;
			bsum += binsum;
			asum += ainsum;

			stackpointer = (stackpointer + 1) % div;
			sir = stack[(stackpointer) % div];

			routsum += sir[0];
			goutsum += sir[1];
			boutsum += sir[2];
			aoutsum += sir[3];

			rinsum -= sir[0];
			ginsum -= sir[1];
			binsum -= sir[2];
			ainsum -= sir[3];

			++yi;
		}

		yw += w;

	}

	for (x = 0; x < w; ++x)
	{
		rinsum = ginsum = binsum = ainsum = routsum = goutsum = boutsum = aoutsum = rsum = gsum = bsum = asum = 0;

		yp =- radius * w;
        
		for (i =- radius; i <= radius; ++i)
		{
			yi = MAX (0, yp) + x;

			sir = stack[i + radius];

			sir[0] = r[yi];
			sir[1] = g[yi];
			sir[2] = b[yi];
			sir[3] = a[yi];

			rbs = r1 - abs (i);

			rsum += r[yi] * rbs;
			gsum += g[yi] * rbs;
			bsum += b[yi] * rbs;
			asum += a[yi] * rbs;

			if (i > 0)
			{
				rinsum += sir[0];
				ginsum += sir[1];
				binsum += sir[2];
				ainsum += sir[3];
			}
			else
			{
				routsum += sir[0];
				goutsum += sir[1];
				boutsum += sir[2];
				aoutsum += sir[3];
			}

			if (i < hm)
				yp += w;
		}

		yi = x;
		stackpointer = radius;

		for (y = 0; y < h; ++y)
		{
			pixels[yi + 0] = dv[rsum];
			pixels[yi + 1] = dv[gsum];
			pixels[yi + 2] = dv[bsum];
			pixels[yi + 3] = dv[asum];

			rsum -= routsum;
			gsum -= goutsum;
			bsum -= boutsum;
			asum -= aoutsum;

			stackstart = stackpointer - radius + div;
			sir = stack[stackstart % div];

			routsum -= sir[0];
			goutsum -= sir[1];
			boutsum -= sir[2];
			aoutsum -= sir[3];

			if (x == 0)
				vmin[y] = MIN (y + r1, hm) * w;

			gint pixel = x + vmin[y];

			sir[0] = r[pixel];
			sir[1] = g[pixel];
			sir[2] = b[pixel];
			sir[3] = a[pixel];

			rinsum += sir[0];
			ginsum += sir[1];
			binsum += sir[2];
			ainsum += sir[3];

			rsum += rinsum;
			gsum += ginsum;
			bsum += binsum;
			asum += ainsum;

			stackpointer = (stackpointer + 1) % div;
			sir = stack[stackpointer];

			routsum += sir[0];
			goutsum += sir[1];
			boutsum += sir[2];
			aoutsum += sir[3];

			rinsum -= sir[0];
			ginsum -= sir[1];
			binsum -= sir[2];
			ainsum -= sir[3];

			yi += w;
		}
	}

	g_free ((gpointer) r);
	g_free ((gpointer) g);
	g_free ((gpointer) b);
	g_free ((gpointer) a);
	g_free ((gpointer) vmin);
	g_free ((gpointer) dv);

	for (i = 0; i < div; ++i)
		g_free ((gpointer) stack[i]);

	g_free ((gpointer) stack);
	}
		break;

		case CAIRO_FORMAT_RGB24:
			// do nothing, for the moment
		break;

		case CAIRO_FORMAT_A8:
			// do nothing, for the moment
		break;

		default :
			// really do nothing
		break;
	}

	// inform cairo we altered the surfaces contents
	cairo_surface_mark_dirty (surface);	
}

