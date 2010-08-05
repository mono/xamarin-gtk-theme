////////////////////////////////////////////////////////////////////////////////
//3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
//      10        20        30        40        50        60        70        80
//
// notify-osd
//
// raico-blur.c - implements public API for blurring cairo image-surfaces
//
// Copyright 2009 Canonical Ltd.
//
// Authors:
//    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
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

#include "raico-blur.h"
#include "exponential-blur.h"
#include "stack-blur.h"
#include "gaussian-blur.h"

struct _raico_blur_private_t
{
	raico_blur_quality_t quality; // low, medium, high
	guint                radius;  // blur-radius
};

raico_blur_t*
raico_blur_create (raico_blur_quality_t quality)
{
	raico_blur_t*         blur = NULL;
	raico_blur_private_t* priv = NULL;

	blur = g_new0 (raico_blur_t, 1);
	if (!blur)
	{
		g_debug ("raico_blur_create(): could not allocate blur struct");
		return NULL;
	}

	priv = g_new0 (raico_blur_private_t, 1);
	if (!priv)
	{
		g_debug ("raico_blur_create(): could not allocate priv struct");
		g_free ((gpointer) blur);
		return NULL;
	}

	priv->quality = quality;
	priv->radius  = 0;

	blur->priv = priv;

	return blur;
}

raico_blur_quality_t
raico_blur_get_quality (raico_blur_t* blur)
{
	g_assert (blur != NULL);

	return blur->priv->quality;
}

void
raico_blur_set_quality (raico_blur_t*        blur,
			raico_blur_quality_t quality)
{
	if (!blur)
	{
		g_debug ("raico_blur_set_quality(): NULL blur-pointer passed");
		return;
	}

	blur->priv->quality = quality;
}

guint
raico_blur_get_radius (raico_blur_t* blur)
{
	g_assert (blur != NULL);

	return blur->priv->radius;
}

void
raico_blur_set_radius (raico_blur_t* blur,
		       guint         radius)
{
	if (!blur)
	{
		g_debug ("raico_blur_set_radius(): NULL blur-pointer passed");
		return;
	}

	blur->priv->radius = radius;
}

void
raico_blur_apply (raico_blur_t*    blur,
		  cairo_surface_t* surface)
{
	cairo_format_t format;

	// sanity checks
	if (!blur)
	{
		g_debug ("raico_blur_apply(): NULL blur-pointer passed");
		return;
	}

	if (!surface)
	{
		g_debug ("raico_blur_apply(): NULL surface-pointer passed");
		return;
	}

	if (cairo_surface_status (surface) != CAIRO_STATUS_SUCCESS)
	{
		g_debug ("raico_blur_apply(): invalid surface status");
		return;
	}

	if (cairo_surface_get_type (surface) != CAIRO_SURFACE_TYPE_IMAGE)
	{
		g_debug ("raico_blur_apply(): non-image surface passed");
		return;
	}

	format = cairo_image_surface_get_format (surface);
	if (format != CAIRO_FORMAT_A8 &&
	    format != CAIRO_FORMAT_RGB24 &&
	    format != CAIRO_FORMAT_ARGB32)
	{
		g_debug ("raico_blur_apply(): unsupported image-format");
		return;
	}

	// stupid, but you never know
	if (blur->priv->radius == 0)
		return;

	// now do the real work
	switch (blur->priv->quality)
	{
		case RAICO_BLUR_QUALITY_LOW:
			surface_exponential_blur (surface, blur->priv->radius);
		break;

		case RAICO_BLUR_QUALITY_MEDIUM:
			//surface_stack_blur (surface, blur->priv->radius);
			surface_gaussian_blur (surface, blur->priv->radius);
		break;

		case RAICO_BLUR_QUALITY_HIGH:
			surface_gaussian_blur (surface, blur->priv->radius);
		break;
	}
}

void
raico_blur_destroy (raico_blur_t* blur)
{
	if (!blur)
	{
		g_debug ("raico_blur_destroy(): invalid blur-pointer passed");
		return;
	}

	g_free ((gpointer) blur->priv);
	g_free ((gpointer) blur);
}

