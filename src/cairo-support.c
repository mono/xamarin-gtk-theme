/* Murrine theme engine
 * Copyright (C) 2007 Andrea Cimitan
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include "cairo-support.h"
#include "support.h"
#include <math.h>

#include "murrine_types.h"

G_GNUC_INTERNAL void
murrine_rgb_to_hls (gdouble *r,
                    gdouble *g,
                    gdouble *b)
{
	gdouble min;
	gdouble max;
	gdouble red;
	gdouble green;
	gdouble blue;
	gdouble h, l, s;
	gdouble delta;

	red = *r;
	green = *g;
	blue = *b;

	if (red > green)
	{
		if (red > blue)
			max = red;
		else
			max = blue;

		if (green < blue)
			min = green;
		else
			min = blue;
	}
	else
	{
		if (green > blue)
			max = green;
		else
			max = blue;

		if (red < blue)
			min = red;
		else
			min = blue;
	}

	l = (max + min) / 2;
	if (fabs(max - min) < 0.0001)
	{
		h = 0;
		s = 0;
	}
	else
	{
		if (l <= 0.5)
			s = (max - min) / (max + min);
		else
			s = (max - min) / (2 - max - min);

		delta = max -min;
		if (red == max)
			h = (green - blue) / delta;
		else if (green == max)
			h = 2 + (blue - red) / delta;
		else if (blue == max)
			h = 4 + (red - green) / delta;

		h *= 60;
		if (h < 0.0)
			h += 360;
	}

	*r = h;
	*g = l;
	*b = s;
}

G_GNUC_INTERNAL void
murrine_hls_to_rgb (gdouble *h,
                    gdouble *l,
                    gdouble *s)
{
	gdouble hue;
	gdouble lightness;
	gdouble saturation;
	gdouble m1, m2;
	gdouble r, g, b;

	lightness = *l;
	saturation = *s;

	if (lightness <= 0.5)
		m2 = lightness * (1 + saturation);
	else
		m2 = lightness + saturation - lightness * saturation;

	m1 = 2 * lightness - m2;

	if (saturation == 0)
	{
		*h = lightness;
		*l = lightness;
		*s = lightness;
	}
	else
	{
		hue = *h + 120;
		while (hue > 360)
			hue -= 360;
		while (hue < 0)
			hue += 360;

		if (hue < 60)
			r = m1 + (m2 - m1) * hue / 60;
		else if (hue < 180)
			r = m2;
		else if (hue < 240)
			r = m1 + (m2 - m1) * (240 - hue) / 60;
		else
			r = m1;

		hue = *h;
		while (hue > 360)
			hue -= 360;
		while (hue < 0)
			hue += 360;

		if (hue < 60)
			g = m1 + (m2 - m1) * hue / 60;
		else if (hue < 180)
			g = m2;
		else if (hue < 240)
			g = m1 + (m2 - m1) * (240 - hue) / 60;
		else
			g = m1;

		hue = *h - 120;
		while (hue > 360)
			hue -= 360;
		while (hue < 0)
			hue += 360;

		if (hue < 60)
			b = m1 + (m2 - m1) * hue / 60;
		else if (hue < 180)
			b = m2;
		else if (hue < 240)
			b = m1 + (m2 - m1) * (240 - hue) / 60;
		else
			b = m1;

		*h = r;
		*l = g;
		*s = b;
	}
}

void
murrine_shade (const MurrineRGB * a, float k, MurrineRGB * b)
{
	double red;
	double green;
	double blue;

	red   = a->r;
	green = a->g;
	blue  = a->b;

	if (k == 1.0)
	{
		b->r = red;
		b->g = green;
		b->b = blue;
		return;
	}

	murrine_rgb_to_hls (&red, &green, &blue);

	green *= k;
	if (green > 1.0)
		green = 1.0;
	else if (green < 0.0)
		green = 0.0;

	blue *= k;
	if (blue > 1.0)
		blue = 1.0;
	else if (blue < 0.0)
		blue = 0.0;

	murrine_hls_to_rgb (&red, &green, &blue);

	b->r = red;
	b->g = green;
	b->b = blue;
}

void
murrine_mix_color (const MurrineRGB *color1, const MurrineRGB *color2,
                   gdouble mix_factor, MurrineRGB *composite)
{
	g_return_if_fail (color1 && color2 && composite);

	composite->r = color1->r * (1-mix_factor) + color2->r * mix_factor;
	composite->g = color1->g * (1-mix_factor) + color2->g * mix_factor;
	composite->b = color1->b * (1-mix_factor) + color2->b * mix_factor;
}

void
murrine_gdk_color_to_rgb (GdkColor *c, double *r, double *g, double *b)
{
	*r = (double)c->red   / (double)65535;
	*g = (double)c->green / (double)65535;
	*b = (double)c->blue  / (double)65535;
}

void
murrine_get_parent_bg (const GtkWidget *widget, MurrineRGB *color)
{
	GtkStateType state_type;
	const GtkWidget *parent;
	GdkColor *gcolor;
	gboolean stop;

	if (widget == NULL)
		return;

	parent = widget->parent;
	stop = FALSE;
	
	while (parent && !stop)
	{
		stop = FALSE;

		stop |= !GTK_WIDGET_NO_WINDOW (parent);
		stop |= GTK_IS_NOTEBOOK (parent) &&
		        !gtk_notebook_get_show_tabs (GTK_NOTEBOOK (parent)) &&
		        gtk_notebook_get_show_border (GTK_NOTEBOOK (parent));

		if (GTK_IS_TOOLBAR (parent))
		{
			GtkShadowType shadow = GTK_SHADOW_OUT;
			gtk_widget_style_get (GTK_WIDGET (parent), "shadow-type", &shadow, NULL);
			
			stop |= (shadow != GTK_SHADOW_NONE);
		}

		if (!stop)
			parent = parent->parent;
	}

	if (parent == NULL)
		return;

	state_type = GTK_WIDGET_STATE (parent);

	gcolor = &parent->style->bg[state_type];

	murrine_gdk_color_to_rgb (gcolor, &color->r, &color->g, &color->b);
}

void
murrine_set_color_rgb (cairo_t *cr, const MurrineRGB *color)
{
	g_return_if_fail (cr && color);

	cairo_set_source_rgb (cr, color->r, color->g, color->b);
}

void
murrine_set_color_rgba (cairo_t *cr, const MurrineRGB *color, double alpha)
{
	g_return_if_fail (cr && color);

	cairo_set_source_rgba (cr, color->r, color->g, color->b, alpha);
}

void
murrine_rounded_corner (cairo_t *cr,
                        double   x,
                        double   y,
                        int radius, uint8 corner)
{
	if (radius < 1)
	{
		cairo_line_to (cr, x, y);
	}
	else
	{
		switch (corner)
		{
			case MRN_CORNER_NONE:
				cairo_line_to (cr, x, y);
				break;
			case MRN_CORNER_TOPLEFT:
				cairo_arc (cr, x + radius, y + radius, radius, G_PI, G_PI * 3/2);
				break;
			case MRN_CORNER_TOPRIGHT:
				cairo_arc (cr, x - radius, y + radius, radius, G_PI * 3/2, G_PI * 2);
				break;
			case MRN_CORNER_BOTTOMRIGHT:
				cairo_arc (cr, x - radius, y - radius, radius, 0, G_PI * 1/2);
				break;
			case MRN_CORNER_BOTTOMLEFT:
				cairo_arc (cr, x + radius, y - radius, radius, G_PI * 1/2, G_PI);
				break;

			default:
				/* A bitfield and not a sane value ... */
				g_assert_not_reached ();
				cairo_line_to (cr, x, y);
				return;
		}
	}
}

void
murrine_rounded_rectangle_fast (cairo_t *cr,
                                double x, double y, double w, double h,
                                uint8 corners)
{
	const float RADIUS_CORNERS = 0.35;

	if (corners & MRN_CORNER_TOPLEFT)
		cairo_move_to (cr, x+RADIUS_CORNERS, y);
	else
		cairo_move_to (cr, x, y);

	if (corners & MRN_CORNER_TOPRIGHT)
	{
		cairo_line_to (cr, x+w-RADIUS_CORNERS, y);
		cairo_move_to (cr, x+w, y+RADIUS_CORNERS);
	}
	else
		cairo_line_to (cr, x+w, y);

	if (corners & MRN_CORNER_BOTTOMRIGHT)
	{
		cairo_line_to (cr, x+w, y+h-RADIUS_CORNERS);
		cairo_move_to (cr, x+w-RADIUS_CORNERS, y+h);
	}
	else
		cairo_line_to (cr, x+w, y+h);

	if (corners & MRN_CORNER_BOTTOMLEFT)
	{
		cairo_line_to (cr, x+RADIUS_CORNERS, y+h);
		cairo_move_to (cr, x, y+h-RADIUS_CORNERS);
	}
	else
		cairo_line_to (cr, x, y+h);

	if (corners & MRN_CORNER_TOPLEFT)
		cairo_line_to (cr, x, y+RADIUS_CORNERS);
	else
	{
		if (corners == MRN_CORNER_NONE)
			cairo_close_path (cr);
		else
			cairo_line_to (cr, x, y);
	}
}

void
clearlooks_rounded_rectangle (cairo_t *cr,
                              double x, double y, double w, double h,
                              int radius, uint8 corners)
{
	if (radius < 1)
	{
		cairo_rectangle (cr, x, y, w, h);
		return;
	}

	if (corners & MRN_CORNER_TOPLEFT)
		cairo_move_to (cr, x+radius, y);
	else
		cairo_move_to (cr, x, y);

	if (corners & MRN_CORNER_TOPRIGHT)
		cairo_arc (cr, x+w-radius, y+radius, radius, M_PI * 1.5, M_PI * 2);
	else
		cairo_line_to (cr, x+w, y);

	if (corners & MRN_CORNER_BOTTOMRIGHT)
		cairo_arc (cr, x+w-radius, y+h-radius, radius, 0, M_PI * 0.5);
	else
		cairo_line_to (cr, x+w, y+h);

	if (corners & MRN_CORNER_BOTTOMLEFT)
		cairo_arc (cr, x+radius,   y+h-radius, radius, M_PI * 0.5, M_PI);
	else
		cairo_line_to (cr, x, y+h);

	if (corners & MRN_CORNER_TOPLEFT)
		cairo_arc (cr, x+radius,   y+radius,   radius, M_PI, M_PI * 1.5);
	else
		cairo_line_to (cr, x, y);
}

void
murrine_rounded_rectangle (cairo_t *cr,
                           double x, double y, double w, double h,
                           int radius, uint8 corners)
{
	radius < 2 ? radius < 1 ? cairo_rectangle (cr, x, y, w, h) :
	                          murrine_rounded_rectangle_fast (cr, x, y, w, h, corners) :
	             clearlooks_rounded_rectangle (cr, x, y, w, h, radius, corners);
}

void
murrine_draw_flat_highlight (cairo_t *cr,
                             int x, int y, int width, int height)
{
	cairo_rectangle (cr, x, y, width, height/2);
}

void
murrine_draw_curved_highlight (cairo_t *cr,
                               double curve_pos, int width, int height)
{
	cairo_move_to (cr, curve_pos, height-curve_pos);
	cairo_curve_to (cr, curve_pos, height/2+height/5, height/5, height/2, height/2, height/2);
	cairo_line_to (cr, width-height/2, height/2);
	cairo_curve_to (cr, width-curve_pos-height/5, height/2, width-curve_pos-0.5, height/2-height/5, width-curve_pos, curve_pos);
	cairo_line_to (cr, curve_pos, curve_pos);
	cairo_line_to (cr, curve_pos, height-curve_pos);
	cairo_close_path (cr);
}

void
murrine_draw_curved_highlight_top (cairo_t *cr,
                                   double curve_pos, int width, int height)
{
	cairo_move_to (cr, curve_pos, curve_pos);
	cairo_curve_to (cr, curve_pos, height/2-height/5, height/5, height/2, height/2, height/2);
	cairo_line_to (cr, width-height/2, height/2);
	cairo_curve_to (cr, width-curve_pos-height/5, height/2, width-curve_pos-0.5, height/2-height/5, width-curve_pos, curve_pos);
	cairo_close_path (cr);
}

void
murrine_draw_curved_highlight_bottom (cairo_t *cr,
                                      double curve_pos, int width, int height)
{
	cairo_move_to (cr, curve_pos, height-curve_pos);
	cairo_curve_to (cr, curve_pos, height/2+height/5, height/5, height/2, height/2, height/2);
	cairo_line_to (cr, width-height/2, height/2);
	cairo_curve_to (cr, width-curve_pos-height/5, height/2, width-curve_pos-0.5, height/2+height/5, width-curve_pos, height-curve_pos);
	cairo_close_path (cr);
}

void
murrine_draw_lightborder (cairo_t *cr,
                          const MurrineRGB *highlight_color,
                          const MurrineRGB *fill,
                          MurrineGradients mrn_gradient,
                          double x, double y, int width, int height,
                          boolean gradients, boolean horizontal,
                          int glazestyle, int lightborderstyle,
                          int radius, uint8 corners)
{
	cairo_pattern_t *pattern;
	MurrineRGB shade1, shade2, shade3, shade4;
	double alpha_value = 1.0;
	if (mrn_gradient.use_rgba)
	{
		alpha_value = mrn_gradient.rgba_opacity;
	}

	if (gradients)
	{
		murrine_shade (highlight_color, mrn_gradient.gradient_stop_1, &shade1);
		murrine_shade (highlight_color, mrn_gradient.gradient_stop_2, &shade2);
		murrine_shade (highlight_color, mrn_gradient.gradient_stop_3, &shade3);
		murrine_shade (highlight_color, mrn_gradient.gradient_stop_4, &shade4);
	}
	else
	{
		murrine_shade (highlight_color, 1.0, &shade1);
		murrine_shade (highlight_color, 1.0, &shade2);
		murrine_shade (highlight_color, 1.0, &shade3);
		murrine_shade (highlight_color, 1.0, &shade4);
	}

	double fill_pos = 1.0-(1.0/(!horizontal ? (double)(width) : (double)(height)));
	if (corners == MRN_CORNER_ALL && radius > 2)
		fill_pos = 1.0-((((double)radius-1.0)/2.0)/(!horizontal ? (double)(width) : (double)(height)));

	radius < 2 ? cairo_rectangle (cr, x, y, width, height) :
	             clearlooks_rounded_rectangle (cr, x, y, width, height, radius-1, corners);

	pattern = cairo_pattern_create_linear (x, y, !horizontal ? width+x : x, !horizontal ? y : height+y);
	cairo_pattern_add_color_stop_rgba (pattern, 0.0,      shade1.r, shade1.g, shade1.b, 0.5*alpha_value);
	cairo_pattern_add_color_stop_rgba (pattern, 0.5,      shade2.r, shade2.g, shade2.b, 0.5*alpha_value);
	cairo_pattern_add_color_stop_rgba (pattern, 0.5,      shade3.r, shade3.g, shade3.b, 0.5*alpha_value);
	cairo_pattern_add_color_stop_rgba (pattern, fill_pos, shade4.r, shade4.g, shade4.b, 0.5*alpha_value);
	cairo_pattern_add_color_stop_rgba (pattern, fill_pos, shade4.r, shade4.g, shade4.b,
	                                   lightborderstyle > 0 ? 0.5*alpha_value : 0.0);
	cairo_pattern_add_color_stop_rgba (pattern, 1.0,      shade4.r, shade4.g, shade4.b,
	                                   lightborderstyle > 0 ? 0.5*alpha_value : 0.0);
	cairo_set_source (cr, pattern);
	cairo_pattern_destroy (pattern);

	cairo_stroke (cr);

	if (glazestyle == 2)
	{
		murrine_set_gradient (cr, fill, mrn_gradient, x, y, 0, height, mrn_gradient.gradients, FALSE);
		cairo_move_to (cr, width+x, y+0.5);
		cairo_line_to (cr, width+x, height+y);
		cairo_line_to (cr, x-0.5, height+y);
		cairo_stroke (cr);
	}
}

void
murrine_set_gradient (cairo_t *cr,
                      const MurrineRGB *color,
                      MurrineGradients mrn_gradient,
                      double x, double y, int width, int height,
                      boolean gradients, boolean alpha)
{
	double alpha_value = 1.0;
	if (mrn_gradient.use_rgba)
	{
		alpha_value = mrn_gradient.rgba_opacity;
	}
	else if (alpha)
	{
		alpha_value *= 0.8;
	}

	if (gradients)
	{
		cairo_pattern_t *pattern;
		MurrineRGB shade1, shade2, shade3, shade4;

		murrine_shade (color, mrn_gradient.gradient_stop_1, &shade1);
		murrine_shade (color, mrn_gradient.gradient_stop_2, &shade2);
		murrine_shade (color, mrn_gradient.gradient_stop_3, &shade3);
		murrine_shade (color, mrn_gradient.gradient_stop_4, &shade4);

		pattern = cairo_pattern_create_linear (x, y, width+x, height+y);
		if (mrn_gradient.use_rgba)
		{
			cairo_pattern_add_color_stop_rgba (pattern, 0.0, shade1.r, shade1.g, shade1.b, alpha_value);
			cairo_pattern_add_color_stop_rgba (pattern, 0.5, shade2.r, shade2.g, shade2.b, alpha_value);
			cairo_pattern_add_color_stop_rgba (pattern, 0.5, shade3.r, shade3.g, shade3.b, alpha_value);
			cairo_pattern_add_color_stop_rgba (pattern, 1.0, shade4.r, shade4.g, shade4.b, alpha_value);
		}
		else
		{
			if (!alpha)
			{
				cairo_pattern_add_color_stop_rgb (pattern, 0.0, shade1.r, shade1.g, shade1.b);
				cairo_pattern_add_color_stop_rgb (pattern, 0.5, shade2.r, shade2.g, shade2.b);
				cairo_pattern_add_color_stop_rgb (pattern, 0.5, shade3.r, shade3.g, shade3.b);
				cairo_pattern_add_color_stop_rgb (pattern, 1.0, shade4.r, shade4.g, shade4.b);
			}
			else
			{
				cairo_pattern_add_color_stop_rgba (pattern, 0.0, shade1.r, shade1.g, shade1.b, alpha_value);
				cairo_pattern_add_color_stop_rgba (pattern, 0.5, shade2.r, shade2.g, shade2.b, alpha_value);
				cairo_pattern_add_color_stop_rgba (pattern, 0.5, shade3.r, shade3.g, shade3.b, alpha_value);
				cairo_pattern_add_color_stop_rgba (pattern, 1.0, shade4.r, shade4.g, shade4.b, alpha_value);
			}
		}
		cairo_set_source (cr, pattern);
		cairo_pattern_destroy (pattern);
	}
	else
	{
		if (alpha_value < 1.0)
			murrine_set_color_rgba (cr, color, alpha_value);
		else
			murrine_set_color_rgb (cr, color);
	}
}

void
rotate_mirror_translate (cairo_t *cr,
                         double radius, double x, double y,
                         boolean mirror_horizontally, boolean mirror_vertically)
{
	cairo_matrix_t matrix_rotate;
	cairo_matrix_t matrix_mirror;
	cairo_matrix_t matrix_result;

	double r_cos = cos(radius);
	double r_sin = sin(radius);

	cairo_matrix_init (&matrix_rotate, r_cos, r_sin, r_sin, r_cos, x, y);

	cairo_matrix_init (&matrix_mirror,
	                   mirror_horizontally ? -1 : 1, 0, 0,
	                   mirror_vertically ? -1 : 1, 0, 0);

	cairo_matrix_multiply (&matrix_result, &matrix_mirror, &matrix_rotate);

	cairo_set_matrix (cr, &matrix_result);
}

void
murrine_exchange_axis (cairo_t  *cr,
                       gint     *x,
                       gint     *y,
                       gint     *width,
                       gint     *height)
{
	gint tmp;
	cairo_matrix_t matrix;

	cairo_translate (cr, *x, *y);
	cairo_matrix_init (&matrix, 0, 1, 1, 0, 0, 0);

	cairo_transform (cr, &matrix);

	/* swap width/height */
	tmp = *width;
	*x = 0;
	*y = 0;
	*width = *height;
	*height = tmp;
}

double
get_decreased_ratio (double old, double factor)
{
	if (old > 1.0)
		return ((old-1.0)/factor + 1.0);
	else if (old < 1.0)
		return (1.0 - (1.0-old)/factor);

	return old;
}

double
get_increased_ratio (double old, double factor)
{
	if (old > 1.0)
		return ((old-1.0)*factor + 1.0);
	else if (old < 1.0)
		return (1.0 - (1.0-old)*factor);

	return old;
}
