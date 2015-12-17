/* Murrine theme engine
 * Copyright (C) 2006-2007-2008-2009 Andrea Cimitan
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

#include <cairo.h>

#include "murrine_draw.h"
#include "murrine_style.h"
#include "murrine_types.h"
#include "support.h"
#include "cairo-support.h"
#include "raico-blur.h"

static void
murrine_draw_inset (cairo_t *cr,
                    const MurrineRGB *bg_color,
                    double x, double y, double w, double h,
                    double radius, uint8 corners)
{
	MurrineRGB shadow, highlight;
	radius = MIN (radius, MIN (w/2.0, h/2.0));

	murrine_shade (bg_color, 0.74, &shadow);
	murrine_shade (bg_color, 1.3, &highlight);

	/* highlight */
	cairo_move_to (cr, x+w+(radius*-0.2928932188), y-(radius*-0.2928932188));

	if (corners & MRN_CORNER_TOPRIGHT)
		cairo_arc (cr, x+w-radius, y+radius, radius, M_PI*1.75, M_PI*2);
	else
		cairo_line_to (cr, x+w, y);

	if (corners & MRN_CORNER_BOTTOMRIGHT)
		cairo_arc (cr, x+w-radius, y+h-radius, radius, 0, M_PI*0.5);
	else
		cairo_line_to (cr, x+w, y+h);

	if (corners & MRN_CORNER_BOTTOMLEFT)
		cairo_arc (cr, x+radius, y+h-radius, radius, M_PI*0.5, M_PI*0.75);
	else
		cairo_line_to (cr, x, y+h);

	murrine_set_color_rgba (cr, &highlight, 0.42);
	cairo_stroke (cr);

	/* shadow */
	cairo_move_to (cr, x+(radius*0.2928932188), y+h+(radius*-0.2928932188));

	if (corners & MRN_CORNER_BOTTOMLEFT)
		cairo_arc (cr, x+radius, y+h-radius, radius, M_PI*0.75, M_PI);
	else
		cairo_line_to (cr, x, y+h);

	if (corners & MRN_CORNER_TOPLEFT)
		cairo_arc (cr, x+radius, y+radius, radius, M_PI, M_PI*1.5);
	else
		cairo_line_to (cr, x, y);

	if (corners & MRN_CORNER_TOPRIGHT)
		cairo_arc (cr, x+w-radius, y+radius, radius, M_PI*1.5, M_PI*1.75);
	else
		cairo_line_to (cr, x+w, y);

	murrine_set_color_rgba (cr, &shadow, 0.16);
	cairo_stroke (cr);
}

static void
murrine_draw_highlight_and_shade (cairo_t *cr,
                                  const MurrineColors    *colors,
                                  const ShadowParameters *widget,
                                  int width, int height, int radius)
{
	MurrineRGB highlight;
	MurrineRGB shadow;
	uint8 corners = widget->corners;
	double x = 1.0;
	double y = 1.0;
	width  -= 3;
	height -= 3;
	radius = MIN (radius, MIN ((double)width/2.0, (double)height/2.0));

	if (radius < 0)
		radius = 0;

	murrine_shade (&colors->bg[0], 1.04, &highlight);
	murrine_shade (&colors->bg[0], 0.96, &shadow);

	cairo_save (cr);

	/* Top/Left highlight */
	if (corners & MRN_CORNER_BOTTOMLEFT)
		cairo_move_to (cr, x, y+height-radius);
	else
		cairo_move_to (cr, x, y+height);

	murrine_rounded_corner (cr, x, y, radius, corners & MRN_CORNER_TOPLEFT);

	if (corners & MRN_CORNER_TOPRIGHT)
		cairo_line_to (cr, x+width-radius, y);
	else
		cairo_line_to (cr, x+width, y);

	if (widget->shadow & MRN_SHADOW_OUT)
		murrine_set_color_rgb (cr, &highlight);
	else
		murrine_set_color_rgb (cr, &shadow);

	cairo_stroke (cr);

	/* Bottom/Right highlight -- this includes the corners */
	cairo_move_to (cr, x+width-radius, y); /* topright and by radius to the left */
	murrine_rounded_corner (cr, x+width, y, radius, corners & MRN_CORNER_TOPRIGHT);
	murrine_rounded_corner (cr, x+width, y+height, radius, corners & MRN_CORNER_BOTTOMRIGHT);
	murrine_rounded_corner (cr, x, y+height, radius, corners & MRN_CORNER_BOTTOMLEFT);

	if (widget->shadow & MRN_SHADOW_OUT)
		murrine_set_color_rgb (cr, &shadow);
	else
		murrine_set_color_rgb (cr, &highlight);

	cairo_stroke (cr);

	cairo_restore (cr);
}

static void
murrine_draw_button (cairo_t *cr,
                     const MurrineColors    *colors,
                     const WidgetParameters *widget,
                     const ButtonParameters *button,
                     int x, int y, int width, int height,
                     boolean horizontal)
{
	int os = (widget->xthickness > 2 && widget->ythickness > 2) ? 1 : 0;
	double glow_shade_new = widget->glow_shade;
	double highlight_shade_new = widget->highlight_shade;
	double lightborder_shade_new = widget->lightborder_shade;
	MurrineGradients mrn_gradient_new = widget->mrn_gradient;
	MurrineRGB border, lower_fill;
	MurrineRGB fill = colors->bg[widget->state_type];
	cairo_pattern_t *pat;
/*
	if (widget->active)
	{
		mrn_gradient_new.has_border_colors = FALSE;
		mrn_gradient_new.has_gradient_colors = FALSE;
	}
*/
	murrine_get_fill_color (&fill, &mrn_gradient_new);

	if (widget->disabled)
	{
		mrn_gradient_new = murrine_get_decreased_gradient_shades (widget->mrn_gradient, 3.0);
		mrn_gradient_new.border_shades[0] = murrine_get_decreased_shade (widget->mrn_gradient.border_shades[0], 2.0);
		mrn_gradient_new.border_shades[1] = murrine_get_decreased_shade (widget->mrn_gradient.border_shades[1], 2.0);
		glow_shade_new = murrine_get_decreased_shade (widget->glow_shade, 2.0);
		highlight_shade_new = murrine_get_decreased_shade (widget->highlight_shade, 2.0);
		lightborder_shade_new = murrine_get_decreased_shade (widget->lightborder_shade, 2.0);
		murrine_shade (&fill, murrine_get_contrast(0.75, widget->contrast), &border);
	}
	else
		murrine_shade (&fill, button->border_shade, &border);

	/* Default button */
	if (widget->is_default && !widget->disabled)
	{
		murrine_shade (&border, murrine_get_contrast(0.8, widget->contrast), &border);

		if (button->has_default_button_color)
		{
			mrn_gradient_new.has_border_colors = FALSE;
			mrn_gradient_new.has_gradient_colors = FALSE;
			murrine_mix_color (&fill, &button->default_button_color, 0.8, &fill);
		}
		else
			murrine_mix_color (&fill, &colors->spot[1], 0.2, &fill);

		if (mrn_gradient_new.has_border_colors)
		{
			murrine_shade (&mrn_gradient_new.border_colors[0], 0.8, &mrn_gradient_new.border_colors[0]);
			murrine_shade (&mrn_gradient_new.border_colors[1], 0.8, &mrn_gradient_new.border_colors[1]);
		}
	}

	if (!horizontal)
		murrine_exchange_axis (cr, &x, &y, &width, &height);

	cairo_translate (cr, x, y);

	if (!widget->active && !widget->disabled && widget->reliefstyle > 1 && os > 0)
	{
		if (widget->reliefstyle == 5)
			murrine_draw_shadow (cr, &widget->parentbg,
			                     0.5, 0.5, width-1, height-1,
			                     widget->roundness+1, widget->corners,
			                     widget->reliefstyle,
			                     mrn_gradient_new, 0.5);
		else
		{
			murrine_draw_shadow (cr, &border,
			                     os-0.5, os-0.5, width-(os*2)+1, height-(os*2)+1,
			                     widget->roundness+1, widget->corners,
			                     widget->reliefstyle,
			                     mrn_gradient_new, 0.08);
		}
	}
	else if (widget->reliefstyle != 0 && os > 0)
	{
		mrn_gradient_new = murrine_get_inverted_border_shades (mrn_gradient_new);
		murrine_draw_inset (cr, &widget->parentbg, os-0.5, os-0.5,
		                    width-(os*2)+1, height-(os*2)+1,
		                    widget->roundness+1, widget->corners);
	}

	cairo_save (cr);

	murrine_rounded_rectangle_closed (cr, os+1, os+1, width-(os*2)-2, height-(os*2)-2, widget->roundness-1, widget->corners);
	cairo_clip_preserve (cr);

	if (widget->active && button->draw_glaze)
	{
                mrn_gradient_new = murrine_get_inverted_gradient_shades (mrn_gradient_new);
		murrine_draw_glaze (cr, &fill,
				    glow_shade_new, highlight_shade_new, !widget->active ? lightborder_shade_new : 1.0,
				    mrn_gradient_new, widget,
				    os+1, os+1, width-(os*2)-2, height-(os*2)-2,
				    widget->roundness-1, widget->corners, horizontal);
	}
	else
	{
		murrine_shade (&fill, button->fill_shade, &lower_fill);
		pat = cairo_pattern_create_linear (os, os+2, os, height-(os*2)-2);
		murrine_pattern_add_color_stop_rgb (pat, 0.0, &fill);
		murrine_pattern_add_color_stop_rgb (pat, 1.0, &lower_fill);
		cairo_set_source (cr, pat);
		cairo_fill (cr);
		cairo_pattern_destroy (pat);
	}

	cairo_restore (cr);

	/* Draw pressed button shadow */
	double active_shadow_shade = murrine_get_inverted_shade(highlight_shade_new);
	if (widget->active && active_shadow_shade != 1.0)
	{
		cairo_pattern_t *pat;
		MurrineRGB shadow;

		murrine_shade (&fill, active_shadow_shade, &shadow);

		cairo_save (cr);

		murrine_rounded_rectangle_closed (cr, os+1, os+1, width-(os*2)-2, height-(os*2)-2, widget->roundness-1,
		                                  widget->corners & (MRN_CORNER_TOPLEFT | MRN_CORNER_TOPRIGHT | MRN_CORNER_BOTTOMLEFT));
		cairo_clip (cr);

		cairo_rectangle (cr, os+1, os+1, width-(os*2)-2, 5);
		pat = cairo_pattern_create_linear (os+1, os+1, os+1, os+6);
		murrine_pattern_add_color_stop_rgba (pat, 0.0, &shadow, 0.58);
		murrine_pattern_add_color_stop_rgba (pat, 1.0, &shadow, 0.1);
		cairo_set_source (cr, pat);
		cairo_fill (cr);
		cairo_pattern_destroy (pat);

		cairo_rectangle (cr, os+1, height-(os*2)-4, width-(os*2)-2, 5);
		pat = cairo_pattern_create_linear (os+1, height-(os*2), os+1, height-(os*2)-6);
		murrine_pattern_add_color_stop_rgba (pat, 0.0, &shadow, 0.58);
		murrine_pattern_add_color_stop_rgba (pat, 1.0, &shadow, 0.1);
		cairo_set_source (cr, pat);
		cairo_fill (cr);
		cairo_pattern_destroy (pat);

		cairo_rectangle (cr, os+1, os+1, 5, height-(os*2)-2);
		pat = cairo_pattern_create_linear (os+1, os+1, os+6, os+1);
		murrine_pattern_add_color_stop_rgba (pat, 0.0, &shadow, 0.58);
		murrine_pattern_add_color_stop_rgba (pat, 1.0, &shadow, 0.1);
		cairo_set_source (cr, pat);
		cairo_fill (cr);
		cairo_pattern_destroy (pat);

		cairo_rectangle (cr, width-(os*2)-4, os+1, 5, height-(os*2)-2);
		pat = cairo_pattern_create_linear (width-(os*2), os+1, width-(os*2)-6, os+1);
		murrine_pattern_add_color_stop_rgba (pat, 0.0, &shadow, 0.58);
		murrine_pattern_add_color_stop_rgba (pat, 1.0, &shadow, 0.1);
		cairo_set_source (cr, pat);
		cairo_fill (cr);
		cairo_pattern_destroy (pat);

		cairo_restore (cr);
	}

	murrine_draw_border (cr, widget->draw_border ? &border : &colors->bg[widget->state_type],
	                     os+0.5, os+0.5, width-(os*2)-1, height-(os*2)-1,
	                     widget->roundness, widget->corners,
	                     mrn_gradient_new, 1.0);
}

static void
murrine_draw_entry_box (cairo_t *cr,
			const MurrineColors    *colors,
			const WidgetParameters *widget,
			const FocusParameters  *focus,
			int x, int y, int width, int height,
			int radius)
{
	MurrineGradients mrn_gradient_new = widget->mrn_gradient;
	const MurrineRGB *base = &colors->base[widget->state_type];
	MurrineRGB border = colors->shade[widget->disabled ? 4 : 6];

	if (widget->focus)
		border = focus->color;

	cairo_translate (cr, x+0.5, y+0.5);

	/* Fill the entry's base color */
	murrine_rounded_rectangle_closed (cr, 2, 2, width-5, height-5, radius,
					  (MRN_CORNER_TOPLEFT | MRN_CORNER_TOPRIGHT |
					   MRN_CORNER_BOTTOMLEFT | MRN_CORNER_BOTTOMRIGHT));
	murrine_set_color_rgb (cr, base);
	cairo_fill (cr);

	if (!widget->disabled)
		murrine_shade (&colors->base[widget->state_type], 0.6, &border);
	else
		border = colors->shade[4];

	/* Draw the focused border */
	if (widget->focus)
	{
		murrine_draw_border (cr,
				     &colors->base[GTK_STATE_SELECTED],
				     0, 0, width-1, height-1,
				     radius + 2, widget->corners,
				     mrn_gradient_new, 0.1);

		murrine_draw_border (cr,
				     &colors->base[GTK_STATE_SELECTED],
				     1, 1, width-3, height-3,
				     radius + 1, widget->corners,
				     mrn_gradient_new, 0.28);
	}

	if (widget->mrn_gradient.gradients)
	{
		MurrineRGB shadow;
		murrine_shade (&border, 0.925, &shadow);

		cairo_move_to (cr, 3, height-5);
		cairo_line_to (cr, 3, 3);
		cairo_line_to (cr, width-5, 3);

		murrine_set_color_rgba (cr, &shadow, widget->disabled ? 0.05 : 0.15);
		cairo_stroke (cr);
	}

	mrn_gradient_new = murrine_get_inverted_border_shades (mrn_gradient_new);

	if (widget->focus)
		border = colors->base[GTK_STATE_SELECTED];

	/* Draw border */
	murrine_draw_border (cr, &border,
	                     2, 2, width-5, height-5,
	                     radius, widget->corners,
	                     mrn_gradient_new, 1.0);
}

static void
murrine_draw_entry (cairo_t *cr,
                    const MurrineColors    *colors,
                    const WidgetParameters *widget,
                    const FocusParameters  *focus,
                    int x, int y, int width, int height)
{
	MurrineGradients mrn_gradient_new = widget->mrn_gradient;
	const MurrineRGB *base = &colors->base[widget->state_type];
	MurrineRGB border = colors->shade[widget->disabled ? 4 : 6];
	int radius = CLAMP (widget->roundness, 0, 3);

	murrine_draw_entry_box (cr, colors, widget, focus,
				x, y, width, height, radius);
}

static void
murrine_draw_spinbutton_entry (cairo_t *cr,
			       const MurrineColors    *colors,
			       const WidgetParameters *widget,
			       const FocusParameters  *focus,
			       int x, int y, int width, int height)
{
	MurrineGradients mrn_gradient_new = widget->mrn_gradient;
	const MurrineRGB *base = &colors->base[widget->state_type];
	MurrineRGB border = colors->shade[widget->disabled ? 4 : 6];
	int radius = CLAMP (widget->roundness, 0, 3);

	if (widget->focus)
		border = focus->color;

	cairo_translate (cr, x+0.5, y+0.5);

	/* Fill the entry's base color */
	cairo_rectangle (cr, 1.5, 1.5, width-4, height-4);
	murrine_set_color_rgb (cr, base);
	cairo_fill (cr);

	if (!widget->disabled)
		murrine_shade (&colors->base[widget->state_type], 0.6, &border);
	else
		border = colors->shade[4];

	if (widget->reliefstyle != 0)
		murrine_draw_inset (cr, &widget->parentbg, 0, 0, width-1, height-1, radius+1, widget->corners);

	/* Draw the focused border */
	if (widget->focus)
	{
		MurrineRGB focus_shadow;
		murrine_shade (&border, 1.54, &focus_shadow);

		cairo_rectangle (cr, 2, 2, width-5, height-5);
		murrine_set_color_rgba (cr, &focus_shadow, 0.5);
		cairo_stroke(cr);
	}
	else if (widget->mrn_gradient.gradients)
	{
		MurrineRGB shadow;
		murrine_shade (&border, 0.925, &shadow);

		cairo_move_to (cr, 2, height-3);
		cairo_line_to (cr, 2, 2);
		cairo_line_to (cr, width-3, 2);

		murrine_set_color_rgba (cr, &shadow, widget->disabled ? 0.05 : 0.15);
		cairo_stroke (cr);
	}

	mrn_gradient_new = murrine_get_inverted_border_shades (mrn_gradient_new);

	/* Draw border */
	murrine_draw_border (cr, &border,
	                     1, 1, width-3, height-3,
	                     radius, widget->corners,
	                     mrn_gradient_new, 1.0);
}

static void
murrine_draw_entry_progress (cairo_t *cr,
                             const MurrineColors    *colors,
                             const WidgetParameters *widget,
                             const EntryProgressParameters *progress,
                             int x, int y, int width, int height)
{
	MurrineRGB border;
	MurrineRGB fill;
	gint entry_width, entry_height;
	double entry_radius;
	double radius;

	cairo_save (cr);

	fill = colors->bg[widget->state_type];
	murrine_shade (&fill, 0.9, &border);

	if (progress->max_size_known)
	{
		entry_width = progress->max_size.width+progress->border.left+progress->border.right;
		entry_height = progress->max_size.height+progress->border.top+progress->border.bottom;
		entry_radius = MIN (widget->roundness, MIN ((entry_width-4.0)/2.0, (entry_height-4.0)/2.0));
	}
	else
	{
		entry_radius = widget->roundness;
	}

	radius = MAX (0, entry_radius+1.0-MAX (MAX (progress->border.left, progress->border.right),
	                                            MAX (progress->border.top, progress->border.bottom)));

	if (progress->max_size_known)
	{
		/* Clip to the max size, and then draw a (larger) rectangle ... */
		clearlooks_rounded_rectangle (cr, progress->max_size.x,
		                              progress->max_size.y,
		                              progress->max_size.width,
		                              progress->max_size.height,
		                              radius,
		                              MRN_CORNER_ALL);
		cairo_clip (cr);

		/* We just draw wider by one pixel ... */
		murrine_set_color_rgb (cr, &fill);
		cairo_rectangle (cr, x, y+1, width, height-2);
		cairo_fill (cr);

		cairo_set_line_width (cr, 1.0);
		murrine_set_color_rgb (cr, &border);
		cairo_rectangle (cr, x-0.5, y+0.5, width+1, height-1);
		cairo_stroke (cr);
	}
	else
	{
		clearlooks_rounded_rectangle (cr, x, y, width+10, height+10, radius, MRN_CORNER_ALL);
		cairo_clip (cr);
		clearlooks_rounded_rectangle (cr, x-10, y-10, width+10, height+10, radius, MRN_CORNER_ALL);
		cairo_clip (cr);

		murrine_set_color_rgb (cr, &fill);
		clearlooks_rounded_rectangle (cr, x+1, y+1, width-2, height-2, radius, MRN_CORNER_ALL);
		cairo_fill (cr);

		cairo_set_line_width (cr, 1.0);
		murrine_set_color_rgb (cr, &border);
		clearlooks_rounded_rectangle (cr, x+0.5, y+0.5, width-1.0, height-1.0, radius, MRN_CORNER_ALL);
		cairo_stroke (cr);
	}

	cairo_restore (cr);
}

static void
murrine_draw_search_entry (cairo_t *cr,
			   const MurrineColors    *colors,
			   const WidgetParameters *widget,
			   const FocusParameters  *focus,
			   int x, int y, int width, int height)
{
	murrine_draw_entry_box (cr, colors, widget, focus,
				x, y, width, height,
				MIN (width, height));
}

static void
murrine_draw_spinbutton (cairo_t *cr,
	                     const MurrineColors    *colors,
	                     const WidgetParameters *widget,
	                     const SpinbuttonParameters *spinbutton,
	                     int x, int y, int width, int height,
	                     boolean horizontal)
{
	ButtonParameters button;
	button.has_default_button_color = FALSE;
	if (widget->has_fill_shade)
		button.fill_shade = widget->fill_shade;
	else
		button.fill_shade = 0.855;
	button.border_shade = 0.6;
	button.draw_glaze = FALSE;

	cairo_save (cr);

	widget->style_functions->draw_button (cr, colors, widget, &button, x, y, width, height, horizontal);

	cairo_restore (cr);

	switch (spinbutton->style)
	{
		default:
		case 0:
			break;
		case 1:
		{
			MurrineRGB line = colors->shade[!widget->disabled ? 6 : 5];
			MurrineRGB highlight = colors->bg[widget->state_type];
			double lightborder_shade_new = widget->lightborder_shade;
			MurrineGradients mrn_gradient_new = widget->mrn_gradient;

			if (widget->disabled)
			{
				mrn_gradient_new = murrine_get_decreased_gradient_shades (widget->mrn_gradient, 3.0);
				lightborder_shade_new = murrine_get_decreased_shade (widget->lightborder_shade, 2.0);
				mrn_gradient_new.border_shades[0] = murrine_get_decreased_shade (widget->mrn_gradient.border_shades[0], 2.0);
				mrn_gradient_new.border_shades[1] = murrine_get_decreased_shade (widget->mrn_gradient.border_shades[1], 2.0);
			}
			else
				murrine_shade (&colors->shade[6], 0.95, &line);

			/* adjust line accordingly to buttons */
			if (widget->mrn_gradient.has_border_colors)
				murrine_mix_color (&mrn_gradient_new.border_colors[0], &mrn_gradient_new.border_colors[1], 0.5, &line);
			else if (widget->mrn_gradient.has_gradient_colors)
				murrine_mix_color (&line, &mrn_gradient_new.gradient_colors[2], 0.4, &line);
			else
				murrine_mix_color (&line, &colors->bg[widget->state_type], 0.4, &line);
			murrine_shade (&line, (mrn_gradient_new.border_shades[0]+mrn_gradient_new.border_shades[1])/2.0, &line);

			/* adjust highlight accordingly to buttons */
			if (widget->mrn_gradient.has_gradient_colors)
				murrine_shade (&mrn_gradient_new.gradient_colors[2], mrn_gradient_new.gradient_shades[2], &highlight);
			murrine_shade (&highlight, lightborder_shade_new*mrn_gradient_new.gradient_shades[2], &highlight);

			/* this will align the path to the cairo grid */
			if (height % 2 != 0)
				height++;

			cairo_move_to (cr, x+2, y+height/2.0-0.5);
			cairo_line_to (cr, width-3,  y+height/2.0-0.5);
			murrine_set_color_rgb (cr, &line);
			cairo_stroke (cr);

			cairo_move_to (cr, x+3, y+height/2.0+0.5);
			cairo_line_to (cr, width-4,  y+height/2.0+0.5);
			murrine_set_color_rgba (cr, &highlight, 0.5);
			cairo_stroke (cr);
			break;
		}
	}
}

static void
murrine_draw_spinbutton_down (cairo_t *cr,
                              const MurrineColors    *colors,
                              const WidgetParameters *widget,
                              int x, int y, int width, int height)
{
	cairo_pattern_t *pat;
	MurrineRGB shadow;
	murrine_shade (&colors->bg[0], 0.8, &shadow);

	cairo_translate (cr, x+1, y+1);

	cairo_rectangle (cr, 1, 1, width-4, height-4);
	pat = cairo_pattern_create_linear (0, 0, 0, height);
	murrine_pattern_add_color_stop_rgb (pat, 0.0, &shadow);
	murrine_pattern_add_color_stop_rgba (pat, 1.0, &shadow, 0.0);
	cairo_set_source (cr, pat);
	cairo_fill (cr);
	cairo_pattern_destroy (pat);
}

static void
murrine_scale_draw_gradient (cairo_t *cr,
                             const MurrineRGB *c1,
                             const MurrineRGB *c2,
                             double lightborder_shade,
                             int lightborderstyle,
                             int roundness, uint8 corners,
                             int x, int y, int width, int height,
                             boolean horizontal)
{
	murrine_set_color_rgb (cr, c1);
	murrine_rounded_rectangle_closed (cr, x, y, width, height, roundness, corners);
	cairo_fill (cr);

	if (lightborder_shade != 1.0)
	{
		cairo_pattern_t *pat;
		double fill_pos = horizontal ? 1.0-1.0/(double)(height-2) :
		                               1.0-1.0/(double)(width-2);
		MurrineRGB lightborder;
		murrine_shade (c1, lightborder_shade, &lightborder);

		roundness < 2 ? cairo_rectangle (cr, x, y, width, height) :
		                clearlooks_rounded_rectangle (cr, x+1, y+1, width-2, height-2, roundness-1, corners);
		pat = cairo_pattern_create_linear (x+1, y+1, horizontal ? x+1 : width+x+1, horizontal ? height+y+1 : y+1);

		murrine_pattern_add_color_stop_rgba (pat, 0.00,     &lightborder, 0.75);
		murrine_pattern_add_color_stop_rgba (pat, fill_pos, &lightborder, 0.75);
		murrine_pattern_add_color_stop_rgba (pat, fill_pos, &lightborder, 0.0);
		murrine_pattern_add_color_stop_rgba (pat, 1.00,     &lightborder, 0.0);

		cairo_set_source (cr, pat);
		cairo_pattern_destroy (pat);

		cairo_stroke (cr);
	}

	murrine_set_color_rgb (cr, c2);
	murrine_rounded_rectangle (cr, x, y, width, height, roundness, corners);
	cairo_stroke (cr);
}

static void
murrine_scale_draw_trough (cairo_t *cr,
                           const MurrineRGB *c1,
                           const MurrineRGB *c2,
                           MurrineGradients mrn_gradient,
                           int roundness, uint8 corners,
                           int x, int y, int width, int height,
                           boolean horizontal)
{
	murrine_draw_trough (cr, c1, x, y, width, height, roundness, corners, mrn_gradient, 1.0, horizontal);
	murrine_draw_trough_border (cr, c2, x, y, width, height, roundness, corners, mrn_gradient, 1.0, horizontal);
}

#define TROUGH_SIZE 6
static void
murrine_draw_scale_trough (cairo_t *cr,
                           const MurrineColors    *colors,
                           const WidgetParameters *widget,
                           const SliderParameters *slider,
                           int x, int y, int width, int height)
{
	int     trough_width, trough_height;
	double  translate_x, translate_y;

	cairo_save (cr);

	if (slider->horizontal)
	{
		trough_width  = width;
		trough_height = TROUGH_SIZE;

		translate_x   = x;
		translate_y   = y+(height/2)-(TROUGH_SIZE/2);
	}
	else
	{
		trough_width  = TROUGH_SIZE;
		trough_height = height;

		translate_x   = x+(width/2)-(TROUGH_SIZE/2);
		translate_y   = y;
	}

	cairo_translate (cr, translate_x+0.5, translate_y+0.5);

	if (!slider->fill_level && widget->reliefstyle != 0)
		murrine_draw_inset (cr, &widget->parentbg, 0, 0, trough_width, trough_height, widget->roundness, widget->corners);

	if (!slider->lower && !slider->fill_level)
	{
		MurrineRGB fill, border;
		murrine_shade (&colors->bg[GTK_STATE_ACTIVE], 1.0, &fill);
		murrine_shade (&colors->bg[GTK_STATE_ACTIVE], murrine_get_contrast(0.82, widget->contrast), &border);

		murrine_scale_draw_trough (cr, &fill, &border, widget->mrn_gradient,
		                           widget->roundness, widget->corners,
		                           1.0, 1.0, trough_width-2, trough_height-2,
		                           slider->horizontal);
	}
	else
	{
		MurrineRGB fill, border;
		murrine_mix_color (&colors->bg[GTK_STATE_SELECTED], &widget->parentbg, widget->disabled ? 0.25 : 0.0, &fill);
		murrine_shade (&fill, murrine_get_contrast(0.65, widget->contrast), &border);

		murrine_scale_draw_gradient (cr, &fill, &border,
		                             widget->disabled ? 1.0 : widget->lightborder_shade,
		                             widget->lightborderstyle,
		                             widget->roundness, widget->corners,
		                             1.0, 1.0, trough_width-2, trough_height-2,
		                             slider->horizontal);
	}

	cairo_restore (cr);
}

static void
murrine_draw_slider_path (cairo_t *cr,
                          int x, int y, int width, int height,
                          int roundness)
{
	int radius = MIN (roundness, MIN (width/2.0, height/2.0));

	cairo_move_to (cr, x+radius, y);
	cairo_arc (cr, x+width-radius, y+radius, radius, M_PI*1.5, M_PI*2);
	cairo_line_to (cr, x+width, y+height-width/2.0);
	cairo_line_to (cr, x+width/2.0, y+height);
	cairo_line_to (cr, x, y+height-width/2.0);
	cairo_arc (cr, x+radius, y+radius, radius, M_PI, M_PI*1.5);
}

static void
murrine_draw_slider (cairo_t *cr,
                     const MurrineColors    *colors,
                     const WidgetParameters *widget,
                     const SliderParameters *slider,
                     int x, int y, int width, int height)
{
	int os = (widget->xthickness > 2 && widget->ythickness > 2) ? 1 : 0;
	double glow_shade_new = widget->glow_shade;
	double highlight_shade_new = widget->highlight_shade;
	double lightborder_shade_new = widget->lightborder_shade;
	MurrineGradients mrn_gradient_new = widget->mrn_gradient;
	MurrineRGB border;
	MurrineRGB fill = colors->bg[widget->state_type];

	murrine_get_fill_color (&fill, &mrn_gradient_new);

	if (widget->disabled)
	{
		mrn_gradient_new = murrine_get_decreased_gradient_shades (widget->mrn_gradient, 3.0);
		mrn_gradient_new.border_shades[0] = murrine_get_decreased_shade (widget->mrn_gradient.border_shades[0], 2.0);
		mrn_gradient_new.border_shades[1] = murrine_get_decreased_shade (widget->mrn_gradient.border_shades[1], 2.0);
		glow_shade_new = murrine_get_decreased_shade (widget->glow_shade, 2.0);
		highlight_shade_new = murrine_get_decreased_shade (widget->highlight_shade, 2.0);
		lightborder_shade_new = murrine_get_decreased_shade (widget->lightborder_shade, 2.0);
		murrine_shade (&fill, murrine_get_contrast(0.75, widget->contrast), &border);
	}
	else
		murrine_shade (&fill, murrine_get_contrast(0.475, widget->contrast), &border);

	if (!slider->horizontal)
		murrine_exchange_axis (cr, &x, &y, &width, &height);

	cairo_save (cr);
	
	cairo_translate (cr, x+0.5, y+0.5);

	if (!widget->active && !widget->disabled && widget->reliefstyle > 1 && os > 0)
	{
		murrine_draw_slider_path (cr, os-1, os, width-(os*2)+2, height-(os*2)+1, widget->roundness+1);
		if (widget->reliefstyle == 5)
			murrine_draw_shadow_from_path (cr, &widget->parentbg,
			                               os-1, os, width-(os*2)+2, height-(os*2)+1,
			                               widget->reliefstyle,
			                               mrn_gradient_new, 0.5);
		else
			murrine_draw_shadow_from_path (cr, &border,
			                              os-1, os, width-(os*2)+2, height-(os*2)+1,
			                              widget->reliefstyle,
			                              mrn_gradient_new, 0.08);
	}

	murrine_mix_color (&border, &fill, 0.2, &border);

	cairo_save (cr);

	murrine_draw_slider_path (cr, os, os+1, width-(os*2), height-(os*2)-1, widget->roundness);
	cairo_clip_preserve (cr);

	murrine_draw_glaze (cr, &fill,
	                    glow_shade_new, highlight_shade_new, !widget->active ? lightborder_shade_new : 1.0,
	                    mrn_gradient_new, widget,
	                    os, os+1, width-(os*2), height-(os*2)-1,
	                    widget->roundness, widget->corners, TRUE);

	cairo_restore (cr);
	
	murrine_draw_slider_path (cr, os, os+1, width-(os*2), height-(os*2)-1, widget->roundness);

	murrine_draw_border_from_path (cr, &border,
	                     os, os+1, width-(os*2), height-(os*2)-1,
	                     mrn_gradient_new, 1.0);

	cairo_restore (cr);
	 
	if (!slider->horizontal)
		murrine_exchange_axis (cr, &x, &y, &width, &height);
}


static void
murrine_draw_slider_handle (cairo_t *cr,
                            const MurrineColors    *colors,
                            const WidgetParameters *widget,
                            const HandleParameters *handle,
                            int x, int y, int width, int height,
                            boolean horizontal)
{
	int num_handles = 2, bar_x, i;
	MurrineRGB color, inset;
	murrine_shade (&colors->shade[6], 0.95, &color);

	murrine_mix_color (&color, &colors->bg[widget->state_type], 0.4, &color);

	if (!horizontal)
	{
		rotate_mirror_translate (cr, M_PI/2, x, y, FALSE, FALSE);
		int tmp = height; height = width; width = tmp;
	}

	if (width % 2 != 0)
		num_handles = 3;
	bar_x = width/2 - num_handles;

	cairo_translate (cr, 0.5, 0.5);

	switch (handle->style)
	{
		default:
		case 0:
		{
			for (i=0; i<num_handles; i++)
			{
				cairo_move_to (cr, bar_x, 4.5);
				cairo_line_to (cr, bar_x, height-5.5);
				murrine_set_color_rgb (cr, &color);
				cairo_stroke (cr);

				bar_x += 3;
			}			
			break;
		}
		case 1:
		{	
			murrine_shade (&colors->bg[widget->state_type], 1.08, &inset);

			for (i=0; i<num_handles; i++)
			{
				cairo_move_to (cr, bar_x, 4.5);
				cairo_line_to (cr, bar_x, height-5.5);
				murrine_set_color_rgb (cr, &color);
				cairo_stroke (cr);

				cairo_move_to (cr, bar_x+1, 4.5);
				cairo_line_to (cr, bar_x+1, height-5.5);
				murrine_set_color_rgb (cr, &inset);
				cairo_stroke (cr);

				bar_x += 3;
			}			
			break;
		}
		case 2:
		{
			murrine_shade (&colors->bg[widget->state_type], 1.04, &inset);

			bar_x++;

			for (i=0; i<num_handles; i++)
			{
				cairo_move_to (cr, bar_x, 4.5);
				cairo_line_to (cr, bar_x, height-5.5);
				murrine_set_color_rgb (cr, &color);
				cairo_stroke (cr);

				cairo_move_to (cr, bar_x+1, 4.5);
				cairo_line_to (cr, bar_x+1, height-5.5);
				murrine_set_color_rgb (cr, &inset);
				cairo_stroke (cr);

				bar_x += 2;
			}			
			break;
		}			
	}
}

static void
murrine_draw_progressbar_trough (cairo_t *cr,
                                 const MurrineColors    *colors,
                                 const WidgetParameters *widget,
                                 const ProgressBarParameters *progressbar,
                                 int x, int y, int width, int height)
{
	MurrineRGB border, fill;
	int roundness = 1;
	boolean horizontal = progressbar->orientation < 2;
	MurrineRGB shadow;

	murrine_shade (&colors->bg[GTK_STATE_ACTIVE], 1.0, &fill);
	murrine_shade (&colors->bg[GTK_STATE_ACTIVE], murrine_get_contrast(0.82, widget->contrast), &border);
	murrine_shade (&colors->bg[GTK_STATE_ACTIVE], 0.765, &shadow);

	/* Create trough box */
	murrine_draw_trough (cr, &fill, x+1, y+1, width-2, height-2, roundness-1, widget->corners, widget->mrn_gradient, 1.0, horizontal);

	/* Draw border */
	murrine_draw_trough_border (cr, &border, x+0.5, y+0.5, width-1, height-1, roundness, widget->corners, widget->mrn_gradient, 1.0, horizontal);

	/* Draw shadow on top */
	cairo_move_to (cr, x+1, y+1.5);
	cairo_line_to (cr, x+width-2, y+1.5);
	murrine_set_color_rgb (cr, &shadow);
	cairo_stroke (cr);

	murrine_shade (&colors->bg[GTK_STATE_ACTIVE], 0.543, &shadow);
	cairo_move_to (cr, x+roundness, y+0.5);
	cairo_line_to (cr, x+width-(2*roundness), y+0.5);
	murrine_set_color_rgb (cr, &shadow);
	cairo_stroke (cr);
}

static void
murrine_draw_progressbar_fill (cairo_t *cr,
                               const MurrineColors         *colors,
                               const WidgetParameters      *widget,
                               const ProgressBarParameters *progressbar,
                               int x, int y, int width, int height,
                               gint offset)
{
	double     tile_pos = 0;
	double     stroke_width;
	int        x_step;
	int        roundness;
	MurrineRGB border = colors->spot[2];
	MurrineRGB effect;
	MurrineRGB fill = colors->spot[1];
	cairo_pattern_t *pat;
	MurrineRGB shade1, shade2, shade3, shade4;

	murrine_get_fill_color (&fill, &widget->mrn_gradient);
	murrine_shade (&fill, murrine_get_contrast(0.65, widget->contrast), &effect);

	/* progressbar->orientation < 2 == boolean is_horizontal */
	if (progressbar->orientation < 2)
	{
		if (progressbar->orientation == MRN_ORIENTATION_LEFT_TO_RIGHT)
			rotate_mirror_translate (cr, 0, x, y, FALSE, FALSE);
		else
			rotate_mirror_translate (cr, 0, x+width, y, TRUE, FALSE);
	}
	else
	{
		int tmp = height; height = width; width = tmp;

		x = x+1; y = y-1; width = width+2; height = height-2;

		if (progressbar->orientation == MRN_ORIENTATION_TOP_TO_BOTTOM)
			rotate_mirror_translate (cr, M_PI/2, x, y, FALSE, FALSE);
		else
			rotate_mirror_translate (cr, M_PI/2, x, y+width, TRUE, FALSE);
	}

	roundness = MIN (widget->roundness-widget->xthickness, height/2.0);
	int yos = 0;
	if ((2*roundness > width) && roundness > 0)
	{
		int h = height*sin((M_PI*(width))/(4*roundness));
		roundness = round(width/2.0);
		yos = 0.5+(height-h)/2.0;
		height = h;
	}
	stroke_width = height*2;
	x_step = (((float)stroke_width/10)*offset);

	cairo_save (cr);

	murrine_rounded_rectangle_closed (cr, 2, 1+yos, width-4, height-2, roundness-1, widget->corners);
	cairo_clip (cr);

	cairo_rectangle (cr, 2, 1+yos, width-4, height-2);

	murrine_shade (&colors->bg[GTK_STATE_SELECTED], widget->mrn_gradient.gradient_shades[0], &shade1);
	murrine_shade (&colors->bg[GTK_STATE_SELECTED], widget->mrn_gradient.gradient_shades[1], &shade2);
	murrine_shade (&colors->bg[GTK_STATE_SELECTED], widget->mrn_gradient.gradient_shades[2], &shade3);
	murrine_shade (&colors->bg[GTK_STATE_SELECTED], widget->mrn_gradient.gradient_shades[3], &shade4);

	pat = cairo_pattern_create_linear (2, 1+yos, 2, height-2);
	murrine_pattern_add_color_stop_rgb (pat, 0, &shade1);
	murrine_pattern_add_color_stop_rgb (pat, 0.65, &shade2);
	murrine_pattern_add_color_stop_rgb (pat, 0.85, &shade3);
	murrine_pattern_add_color_stop_rgb (pat, 1, &shade4);

	cairo_set_source (cr, pat);
	cairo_pattern_destroy (pat);

	murrine_rounded_rectangle_closed (cr, 2, 1+yos, width-4, height-2,
					  roundness, widget->corners);
	cairo_fill (cr);

	murrine_shade (&colors->bg[GTK_STATE_SELECTED], 1.2, &shade1);
	cairo_move_to (cr, 0, 1+yos);
	cairo_line_to (cr, width, 1+yos);
	murrine_set_color_rgb (cr, &shade1);
	cairo_stroke (cr);

	switch (progressbar->style)
	{
		case 0:
			break;
		default:
		case 1:
		{
			/* Draw strokes */
			while (stroke_width > 0 && tile_pos <= width+x_step)
			{
				cairo_move_to (cr, stroke_width/2-x_step, 0);
				cairo_line_to (cr, stroke_width-x_step, 0);
				cairo_line_to (cr, stroke_width/2-x_step, height);
				cairo_line_to (cr, -x_step, height);

				cairo_translate (cr, stroke_width, 0);
				tile_pos += stroke_width;
			}

			murrine_set_color_rgba (cr, &effect, 0.15);
			cairo_fill (cr);
			break;
		}
		case 2:
		{
			MurrineRGB highlight;
			int step = 18;
			int i;

			murrine_shade (&fill, widget->lightborder_shade*widget->highlight_shade, &highlight);

			for (i=step; i<width-3; i+=step)
			{
				cairo_move_to (cr, i-0.5, 1);
				cairo_line_to (cr, i-0.5, height-1);
				murrine_set_color_rgba (cr, &highlight, 0.5);
				cairo_stroke (cr);

				cairo_move_to (cr, i+0.5, 1);
				cairo_line_to (cr, i+0.5, height-1);
				murrine_set_color_rgba (cr, &effect, 0.25);
				cairo_stroke (cr);
			}
			break;
		}
	}

	cairo_restore (cr);

	cairo_save (cr);

	murrine_rounded_rectangle_closed (cr, 0.5, -0.5+yos, width-1, height+1, roundness-1, widget->corners);
	cairo_clip (cr);

	/* Draw border */
	murrine_draw_border (cr, widget->draw_border ? &colors->fg[GTK_STATE_SELECTED] : &colors->bg[GTK_STATE_SELECTED],
	                     1.5, 0.5+yos, width-3, height-1,
	                     roundness, widget->corners,
	                     widget->mrn_gradient, 1.0);

	cairo_restore (cr);
}

static void
murrine_draw_combobox (cairo_t *cr,
                       MurrineColors  colors,
                       WidgetParameters  widget,
                       const ComboBoxParameters *combobox,
                       int x, int y, int w, int h, boolean horizontal)
{
	switch (combobox->style)
	{
		default:
		case 0:
		{
			ButtonParameters button;
			button.has_default_button_color = FALSE;
			if (widget.has_fill_shade)
				button.fill_shade = widget.fill_shade;
			else
				button.fill_shade = 0.899;
			button.border_shade = 0.529;
			button.draw_glaze = FALSE;

			widget.style_functions->draw_button (cr, &colors, &widget, &button, x, y, w, h, horizontal);
			break;
		}
		case 1:
		case 2:
		{
			WidgetParameters params = widget;
			MurrineColors colors_new = colors;
			ButtonParameters button;
			int box_w = combobox->box_w;
			int os = (widget.xthickness > 2 && widget.ythickness > 2) ? 1 : 0;

			if (box_w % 2 == 0)
				box_w--;

			button.has_default_button_color = FALSE;
			if (widget.has_fill_shade)
				button.fill_shade = widget.fill_shade;
			else
				button.fill_shade = 0.899;
			button.border_shade = 0.6;
			button.draw_glaze = FALSE;

			if (combobox->style == 1) {
				murrine_shade (&colors.bg[GTK_STATE_NORMAL], 0.933,
				               &colors_new.bg[GTK_STATE_NORMAL]);
				murrine_shade (&colors_new.bg[GTK_STATE_NORMAL], combobox->prelight_shade, 
				               &colors_new.bg[GTK_STATE_PRELIGHT]);
			} else {
				colors_new.bg[GTK_STATE_NORMAL] = colors.base[GTK_STATE_NORMAL];
				colors_new.bg[GTK_STATE_ACTIVE] = colors.base[GTK_STATE_ACTIVE];
				colors_new.bg[GTK_STATE_PRELIGHT] = colors.base[GTK_STATE_PRELIGHT];
			}

			if (combobox->as_list)
			{
				params.style_functions->draw_button (cr, &colors_new, &params, &button, x, y, w, h, horizontal);
				break;
			}

			if (combobox->style == 2)
				button.fill_shade = 1.0;

			cairo_save (cr);
			if (params.ltr)
			{
				params.corners = MRN_CORNER_TOPLEFT | MRN_CORNER_BOTTOMLEFT;
				cairo_rectangle (cr, x, y, w-box_w, h);
				cairo_clip (cr);
				params.style_functions->draw_button (cr, &colors, &params, &button, x, y, w-box_w+1+os, h, horizontal);
			}
			else
			{
				params.corners = MRN_CORNER_TOPRIGHT | MRN_CORNER_BOTTOMRIGHT;
				cairo_rectangle (cr, x+box_w, y, w-box_w, h);
				cairo_clip (cr);
				params.style_functions->draw_button (cr, &colors, &params, &button, x+box_w-1-os, y, w-box_w+1+os, h, horizontal);
			}
			cairo_restore (cr);

			params.mrn_gradient.has_border_colors = FALSE;
			params.mrn_gradient.has_gradient_colors = FALSE;
			if (widget.has_fill_shade)
				button.fill_shade = widget.fill_shade;
			else
				button.fill_shade = 0.866;

			cairo_save (cr);
			if (params.ltr)
			{
				params.corners = MRN_CORNER_TOPRIGHT | MRN_CORNER_BOTTOMRIGHT;
				cairo_rectangle (cr, x+w-box_w, y, box_w, h);
				cairo_clip (cr);
				params.style_functions->draw_button (cr, &colors_new, &params, &button, x+w-(box_w+os), y, box_w+os, h, horizontal);
			}
			else
			{
				params.corners = MRN_CORNER_TOPLEFT | MRN_CORNER_BOTTOMLEFT;
				cairo_rectangle (cr, x, y, box_w, h);
				cairo_clip (cr);
				params.style_functions->draw_button (cr, &colors_new, &params, &button, x, y, box_w+os, h, horizontal);
			}
			cairo_restore (cr);
			break;
		}
	}
}

static void
murrine_draw_optionmenu (cairo_t *cr,
                         const MurrineColors        *colors,
                         const WidgetParameters     *widget,
                         const OptionMenuParameters *optionmenu,
                         int x, int y, int width, int height)
{
	ButtonParameters button;
	int offset = widget->ythickness + 1;

	boolean horizontal = TRUE;

	button.has_default_button_color = FALSE;
	if (widget->has_fill_shade)
		button.fill_shade = widget->fill_shade;
	else
		button.fill_shade = 0.855;
	button.border_shade = 0.6;
	button.draw_glaze = FALSE;

	if (((float)width/height<0.5) || (widget->glazestyle > 0 && width<height))
		horizontal = FALSE;

	widget->style_functions->draw_button (cr, colors, widget, &button, x, y, width, height, horizontal);

	/* Draw the separator */
	MurrineRGB *dark = (MurrineRGB*)&colors->shade[6];

	cairo_translate        (cr, optionmenu->linepos+0.5, 1);

	murrine_set_color_rgba (cr, dark, 0.4);
	cairo_move_to          (cr, 0.0, offset);
	cairo_line_to          (cr, 0.0, height - offset - widget->ythickness + 1);
	cairo_stroke           (cr);
}

static void
murrine_draw_menubar (cairo_t *cr,
                      const MurrineColors *colors,
                      const WidgetParameters *widget,
                      int x, int y, int width, int height,
                      int menubarstyle)
{
	const MurrineRGB *fill = &colors->bg[0];
	MurrineRGB dark = colors->shade[3];

	if(widget->mrn_gradient.has_border_colors)
		dark = widget->mrn_gradient.border_colors[1];

	cairo_translate (cr, x, y);
	cairo_rectangle (cr, 0, 0, width, height);

	switch (menubarstyle)
	{
		default:
		case 0:
			murrine_set_color_rgb (cr, fill);
			cairo_fill (cr);
			break;
		case 1:
		{
			int os = (widget->glazestyle == 2) ? 1 : 0;
			murrine_draw_glaze (cr, fill,
			                    widget->glow_shade, widget->highlight_shade, widget->lightborder_shade,
			                    widget->mrn_gradient, widget, os, os, width-os*2, height-os*2,
			                    widget->roundness, widget->corners, TRUE);
			break;
		}
		case 2:
		{
			cairo_pattern_t *pat;
			double alpha = !widget->mrn_gradient.use_rgba ? 1.0 : 0.7;
			MurrineRGB lower;
			murrine_shade (fill, 0.95, &lower);

			pat = cairo_pattern_create_linear (0, 0, 0, height);
			murrine_pattern_add_color_stop_rgba (pat, 0.0, fill, alpha);
			murrine_pattern_add_color_stop_rgba (pat, 1.0, &lower, alpha);
			cairo_set_source (cr, pat);
			cairo_fill (cr);
			cairo_pattern_destroy (pat);
			break;
		}
		case 3:
		{
			cairo_pattern_t *pat;
			int counter = -height;
			MurrineRGB low, top;
			murrine_shade (fill, 0.9, &top);
			murrine_shade (fill, 1.1, &low);

			pat = cairo_pattern_create_linear (0, 0, 0, height);
			murrine_pattern_add_color_stop_rgb (pat, 0.0, &top);
			murrine_pattern_add_color_stop_rgb (pat, 1.0, &low);
			cairo_set_source (cr, pat);
			cairo_fill (cr);
			cairo_pattern_destroy (pat);

			murrine_shade (&low, 0.9, &low);
			murrine_set_color_rgb (cr, &low);
			while (counter < width)
			{
				cairo_move_to (cr, counter, height);
				cairo_line_to (cr, counter+height, 0);
				cairo_stroke  (cr);
				counter += 5;
			}
			break;
		}
	}

	/* Draw bottom line */
	if (menubarstyle == 1 && widget->glazestyle == 2)
		cairo_rectangle (cr, 0.5, 0.5, width-1, height-1);
	else
	{
		cairo_move_to        (cr, 0, height-0.5);
		cairo_line_to        (cr, width, height-0.5);
	}
	murrine_set_color_rgb (cr, &dark);
	cairo_stroke          (cr);
}

/* We can't draw transparent things here, since it will be called on the same
 * surface multiple times, when placed on a handlebox_bin or dockitem_bin */
static void
murrine_draw_toolbar (cairo_t *cr,
                      const MurrineColors    *colors,
                      const WidgetParameters *widget,
                      const ToolbarParameters *toolbar,
                      int x, int y, int width, int height)
{
	const MurrineRGB *fill = &colors->bg[0];
	const MurrineRGB *top  = &colors->shade[0];
	MurrineRGB dark = colors->shade[3];

	if(widget->mrn_gradient.has_border_colors)
		dark = widget->mrn_gradient.border_colors[1];

	cairo_translate      (cr, x, y);
	cairo_rectangle (cr, 0, 0, width, height);

	/* Glass toolbar */
	switch (toolbar->style)
	{
		default:
		case 0:
			murrine_set_color_rgb (cr, fill);
			cairo_fill (cr);

			/* Draw highlight */
			if (!toolbar->topmost)
			{
				cairo_move_to         (cr, 0, 0.5);
				cairo_line_to         (cr, width, 0.5);
				murrine_set_color_rgb (cr, top);
				cairo_stroke          (cr);
			}
			break;
		case 1:
		{
			int os = (widget->glazestyle == 2) ? 1 : 0;
			murrine_draw_glaze (cr, fill,
			                    widget->glow_shade, widget->highlight_shade, widget->lightborder_shade,
			                    widget->mrn_gradient, widget, os, os, width-os*2, height-os*2,
			                    widget->roundness, widget->corners, TRUE);
			break;
		}
		case 2:
		{
			cairo_pattern_t *pat;
			MurrineRGB lower;
			murrine_shade (fill, 0.95, &lower);
			pat = cairo_pattern_create_linear (0, 0, 0, height);
			murrine_pattern_add_color_stop_rgb (pat, 0.0, fill);
			murrine_pattern_add_color_stop_rgb (pat, 1.0, &lower);
			cairo_set_source (cr, pat);
			cairo_pattern_destroy (pat);
			cairo_fill (cr);

			/* Draw highlight */
			if (!toolbar->topmost)
			{
				cairo_move_to         (cr, 0, 0.5);
				cairo_line_to         (cr, width, 0.5);
				murrine_set_color_rgb (cr, top);
				cairo_stroke          (cr);
			}
			break;
		}
	}

	/* Draw shadow */
	murrine_set_color_rgb (cr, &dark);
	if (toolbar->style == 1 && widget->glazestyle == 2)
		cairo_rectangle (cr, 0.5, 0.5, width-1, height-1);
	else
	{
		cairo_move_to         (cr, 0, height-0.5);
		cairo_line_to         (cr, width, height-0.5);
	}
	cairo_stroke (cr);
}

static void
murrine_get_frame_gap_clip (int x, int y, int width, int height,
                            const FrameParameters *frame,
                            MurrineRectangle      *bevel,
                            MurrineRectangle      *border)
{
	switch (frame->gap_side)
	{
		case MRN_GAP_TOP:
			MURRINE_RECTANGLE_SET ((*bevel),  1.5+frame->gap_x, -0.5,
			                       frame->gap_width-3, 2.0);
			MURRINE_RECTANGLE_SET ((*border), 0.5+frame->gap_x, -0.5,
			                       frame->gap_width-2, 2.0);
			break;
		case MRN_GAP_BOTTOM:
			MURRINE_RECTANGLE_SET ((*bevel),  1.5+frame->gap_x, height-2.5,
			                       frame->gap_width-3, 2.0);
			MURRINE_RECTANGLE_SET ((*border), 0.5+frame->gap_x, height-1.5,
			                       frame->gap_width-2, 2.0);
			break;
		case MRN_GAP_LEFT:
			MURRINE_RECTANGLE_SET ((*bevel),  -0.5, 1.5+frame->gap_x,
			                       2.0, frame->gap_width-3);
			MURRINE_RECTANGLE_SET ((*border), -0.5, 0.5+frame->gap_x,
			                       1.0, frame->gap_width-2);
			break;
		case MRN_GAP_RIGHT:
			MURRINE_RECTANGLE_SET ((*bevel),  width-2.5, 1.5+frame->gap_x,
			                       2.0, frame->gap_width-3);
			MURRINE_RECTANGLE_SET ((*border), width-1.5, 0.5+frame->gap_x,
			                       1.0, frame->gap_width-2);
			break;
	}
}

static void
murrine_draw_frame (cairo_t *cr,
                    const MurrineColors    *colors,
                    const WidgetParameters *widget,
                    const FrameParameters  *frame,
                    int x, int y, int width, int height)
{
	MurrineRGB *border = frame->border;
	MurrineRectangle bevel_clip;
	MurrineRectangle frame_clip;
	const MurrineRGB *dark = &colors->shade[3];
	MurrineRGB highlight, shadow_color;

	murrine_shade (&colors->bg[0], 1.04, &highlight);
	murrine_shade (&colors->bg[0], 0.96, &shadow_color);

	if (frame->shadow == MRN_SHADOW_NONE)
		return;

	if (frame->gap_x != -1)
		murrine_get_frame_gap_clip (x, y, width, height,
		                            frame, &bevel_clip, &frame_clip);

	cairo_translate      (cr, x+0.5, y+0.5);

	/* save everything */
	cairo_save (cr);

	/* Set clip for the bevel */
	if (frame->gap_x != -1)
	{
		/* Set clip for gap */
		cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);
		cairo_rectangle     (cr, -0.5, -0.5, width, height);
		cairo_rectangle     (cr, bevel_clip.x, bevel_clip.y, bevel_clip.width, bevel_clip.height);
		cairo_clip          (cr);
	}

	/* Draw the bevel */
	if (frame->shadow == MRN_SHADOW_ETCHED_IN || frame->shadow == MRN_SHADOW_ETCHED_OUT)
	{
		murrine_set_color_rgb (cr, &highlight);
		if (frame->shadow == MRN_SHADOW_ETCHED_IN)
			murrine_rounded_rectangle (cr, 1, 1, width-2, height-2, widget->roundness, widget->corners);
		else
			murrine_rounded_rectangle (cr, 0, 0, width-2, height-2, widget->roundness, widget->corners);
		cairo_stroke (cr);
	}
	else if (frame->shadow != MRN_SHADOW_FLAT)
	{
		ShadowParameters shadow;
		shadow.corners = widget->corners;
		shadow.shadow  = frame->shadow;
		murrine_draw_highlight_and_shade (cr, colors, &shadow, width, height, widget->roundness-1);
	}

	/* restore the previous clip region */
	cairo_restore (cr);
	cairo_save    (cr);
	if (frame->gap_x != -1)
	{
		/* Set clip for gap */
		cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);
		cairo_rectangle     (cr, -0.5, -0.5, width, height);
		cairo_rectangle     (cr, frame_clip.x, frame_clip.y, frame_clip.width, frame_clip.height);
		cairo_clip          (cr);
	}

	/* Draw frame */
	if (frame->shadow == MRN_SHADOW_ETCHED_IN || frame->shadow == MRN_SHADOW_ETCHED_OUT)
	{
		murrine_set_color_rgb (cr, dark);
		if (frame->shadow == MRN_SHADOW_ETCHED_IN)
			murrine_rounded_rectangle (cr, 0, 0, width-2, height-2, widget->roundness, widget->corners);
		else
			murrine_rounded_rectangle (cr, 1, 1, width-2, height-2, widget->roundness, widget->corners);
	}
	else
	{
		murrine_set_color_rgb (cr, border);
		murrine_rounded_rectangle (cr, 0, 0, width-1, height-1, widget->roundness, widget->corners);
	}
	cairo_stroke  (cr);
	cairo_restore (cr);
}

static void
murrine_draw_tab (cairo_t *cr,
                  const MurrineColors    *colors,
                  const WidgetParameters *widget,
                  const TabParameters    *tab,
                  int x, int y, int width, int height)
{
	const MurrineRGB *stripe_fill = &colors->spot[1];
	const MurrineRGB *stripe_border = &colors->spot[2];
	const MurrineRGB *fill = &colors->bg[widget->state_type];
	const MurrineRGB *border = &colors->shade[!widget->active ? 5 : 4];
	cairo_pattern_t  *pat;

	/* Set clip */
	cairo_rectangle (cr, x, y, width, height);
	cairo_clip      (cr);
	cairo_new_path  (cr);

	cairo_translate      (cr, x+0.5, y+0.5);

	/* Make the tabs slightly bigger than they should be, to create a gap */
	/* And calculate the strip size too, while you're at it */
	if (tab->gap_side == MRN_GAP_TOP || tab->gap_side == MRN_GAP_BOTTOM)
	{
		height += 3.0;

		if (tab->gap_side == MRN_GAP_TOP)
			cairo_translate (cr, 0.0, -3.0); /* gap at the other side */
	}
	else
	{
		width += 3.0;

		if (tab->gap_side == MRN_GAP_LEFT)
			cairo_translate (cr, -3.0, 0.0); /* gap at the other side */
	}

	/* Set tab shape */
	murrine_rounded_rectangle_closed (cr, 0, 0, width-1, height-1, widget->roundness, widget->corners);

	/* Draw fill */
	murrine_set_color_rgb (cr, fill);
	cairo_fill (cr);

	if (widget->active)
	{
		MurrineRGB shade1, shade2, shade3, shade4, highlight;
		MurrineGradients mrn_gradient_new = murrine_get_decreased_gradient_shades (widget->mrn_gradient, 3.0);
		double highlight_shade_new = murrine_get_decreased_shade (widget->highlight_shade, 2.0);
		double lightborder_shade_new = murrine_get_decreased_shade (widget->lightborder_shade, 2.0);

		murrine_shade (fill, mrn_gradient_new.gradient_shades[0], &shade1);
		murrine_shade (fill, mrn_gradient_new.gradient_shades[1], &shade2);
		murrine_shade (fill, mrn_gradient_new.gradient_shades[2], &shade3);
		murrine_shade (fill, mrn_gradient_new.gradient_shades[3], &shade4);

		switch (tab->gap_side)
		{
			case MRN_GAP_TOP:
				pat = cairo_pattern_create_linear (0, height-2, 0, 0);
				break;
			case MRN_GAP_BOTTOM:
				pat = cairo_pattern_create_linear (0, 1, 0, height);
				break;
			case MRN_GAP_LEFT:
				pat = cairo_pattern_create_linear (width-2, 0, 1, 0);
				break;
			case MRN_GAP_RIGHT:
				pat = cairo_pattern_create_linear (1, 0, width-2, 0);
				break;
		}

		murrine_rounded_rectangle_closed (cr, 0, 0, width-1, height-1, widget->roundness, widget->corners);

		murrine_pattern_add_color_stop_rgb (pat, 0.00, &shade1);
		murrine_pattern_add_color_stop_rgb (pat, 0.45, &shade2);
		murrine_pattern_add_color_stop_rgb (pat, 0.45, &shade3);
		murrine_pattern_add_color_stop_rgb (pat, 1.00, &shade4);
		cairo_set_source (cr, pat);
		cairo_fill (cr);
		cairo_pattern_destroy (pat);

		/* Draw lightborder */
		murrine_shade (fill, lightborder_shade_new*highlight_shade_new, &highlight);
		murrine_shade (&highlight, mrn_gradient_new.gradient_shades[0]*highlight_shade_new, &shade1);
		murrine_shade (&highlight, mrn_gradient_new.gradient_shades[1]*highlight_shade_new, &shade2);
		murrine_shade (&highlight, mrn_gradient_new.gradient_shades[2], &shade3);
		murrine_shade (&highlight, mrn_gradient_new.gradient_shades[3], &shade4);

		switch (tab->gap_side)
		{
			case MRN_GAP_TOP:
				pat = cairo_pattern_create_linear (0, height-2, 0, 0);
				break;
			case MRN_GAP_BOTTOM:
				pat = cairo_pattern_create_linear (0, 1, 0, height);
				break;
			case MRN_GAP_LEFT:
				pat = cairo_pattern_create_linear (width-2, 0, 1, 0);
				break;
			case MRN_GAP_RIGHT:
				pat = cairo_pattern_create_linear (1, 0, width-2, 0);
				break;
		}

		murrine_rounded_rectangle_closed (cr, 1, 1, width-3, height-3, widget->roundness, widget->corners);

		murrine_pattern_add_color_stop_rgba (pat, 0.00, &shade1, 0.5);
		murrine_pattern_add_color_stop_rgba (pat, 0.45, &shade2, 0.5);
		murrine_pattern_add_color_stop_rgba (pat, 0.45, &shade3, 0.5);
		murrine_pattern_add_color_stop_rgba (pat, 1.00, &shade4, 0.5);
		cairo_set_source (cr, pat);
		cairo_stroke (cr);
		cairo_pattern_destroy (pat);
	}
	else
	{
		MurrineRGB shade1, shade2, shade3, shade4, highlight;
		MurrineGradients mrn_gradient_new = murrine_get_decreased_gradient_shades (widget->mrn_gradient, 3.0);
		double highlight_shade_new = murrine_get_decreased_shade (widget->highlight_shade, 2.0);

		murrine_shade (fill, mrn_gradient_new.gradient_shades[0]*highlight_shade_new, &shade1);
		murrine_shade (fill, mrn_gradient_new.gradient_shades[1]*highlight_shade_new, &shade2);
		murrine_shade (fill, mrn_gradient_new.gradient_shades[2], &shade3);
		murrine_shade (fill, 1.0, &shade4);

		/* Draw shade */
		switch (tab->gap_side)
		{
			case MRN_GAP_TOP:
				pat = cairo_pattern_create_linear (0, height-2, 0, 0);
				break;
			case MRN_GAP_BOTTOM:
				pat = cairo_pattern_create_linear (0, 0, 0, height);
				break;
			case MRN_GAP_LEFT:
				pat = cairo_pattern_create_linear (width-2, 0, 0, 0);
				break;
			case MRN_GAP_RIGHT:
				pat = cairo_pattern_create_linear (0, 0, width, 0);
				break;
		}

		murrine_rounded_rectangle_closed (cr, 0, 0, width-1, height-1, widget->roundness, widget->corners);

		murrine_pattern_add_color_stop_rgb (pat, 0.00, &shade1);
		murrine_pattern_add_color_stop_rgb (pat, 0.45, &shade2);
		murrine_pattern_add_color_stop_rgb (pat, 0.45, &shade3);
		murrine_pattern_add_color_stop_rgb (pat, 1.00, &shade4);
		cairo_set_source (cr, pat);
		cairo_fill (cr);
		cairo_pattern_destroy (pat);

		/* Draw lightborder */
		murrine_shade (fill, widget->lightborder_shade*highlight_shade_new, &highlight);
		murrine_shade (&highlight, mrn_gradient_new.gradient_shades[0]*highlight_shade_new, &shade1);
		murrine_shade (&highlight, mrn_gradient_new.gradient_shades[1]*highlight_shade_new, &shade2);
		murrine_shade (&highlight, mrn_gradient_new.gradient_shades[2], &shade3);
		murrine_shade (fill, 1.04, &shade4); /* this value should change as draw_frame */

		switch (tab->gap_side)
		{
			case MRN_GAP_TOP:
				pat = cairo_pattern_create_linear (0, height-2, 0, 0);
				break;
			case MRN_GAP_BOTTOM:
				pat = cairo_pattern_create_linear (0, 0, 0, height);
				break;
			case MRN_GAP_LEFT:
				pat = cairo_pattern_create_linear (width-2, 0, 0, 0);
				break;
			case MRN_GAP_RIGHT:
				pat = cairo_pattern_create_linear (0, 0, width, 0);
				break;
		}

		murrine_rounded_rectangle_closed (cr, 1, 1, width-3, height-3, widget->roundness, widget->corners);

		murrine_pattern_add_color_stop_rgba (pat, 0.00, &shade1, 0.5);
		murrine_pattern_add_color_stop_rgba (pat, 0.45, &shade2, 0.5);
		murrine_pattern_add_color_stop_rgba (pat, 0.45, &shade3, 0.5);
		murrine_pattern_add_color_stop_rgb  (pat, 1.00, &shade4);
		cairo_set_source (cr, pat);
		cairo_stroke (cr);
		cairo_pattern_destroy (pat);
	}

	murrine_set_color_rgb (cr, border);
	murrine_rounded_rectangle (cr, 0, 0, width-1, height-1, widget->roundness, widget->corners);
	cairo_stroke (cr);
}

static void
murrine_draw_separator (cairo_t *cr,
                        const MurrineColors       *colors,
                        const WidgetParameters    *widget,
                        const SeparatorParameters *separator,
                        int x, int y, int width, int height)
{
	MurrineRGB dark, highlight;
	murrine_shade (&colors->bg[0], murrine_get_contrast(0.7, widget->contrast), &dark);
	murrine_shade (&colors->bg[0], murrine_get_contrast(1.3, widget->contrast), &highlight);

	if (separator->horizontal)
	{
		cairo_translate (cr, x, y+0.5);

		switch (separator->style)
		{
			default:
			case 0:
				murrine_set_color_rgba (cr, &dark, 0.5);
				break;
			case 1:
			{
				cairo_pattern_t *pat;
				pat = cairo_pattern_create_linear (0, 0, width, 0);
				murrine_pattern_add_color_stop_rgba (pat, 0.00, &dark, 0.0);
				murrine_pattern_add_color_stop_rgba (pat, 0.25, &dark, 0.5);
				murrine_pattern_add_color_stop_rgba (pat, 0.75, &dark, 0.5);
				murrine_pattern_add_color_stop_rgba (pat, 1.00, &dark, 0.0);
				cairo_set_source (cr, pat);
				cairo_pattern_destroy (pat);
				break;
			}
		}
		
		cairo_move_to (cr, 0.0,     0.0);
		cairo_line_to (cr, width+1, 0.0);
		cairo_stroke  (cr);

		switch (separator->style)
		{
			default:
			case 0:
				murrine_set_color_rgba (cr, &highlight, 0.5);
				break;
			case 1:
			{
				cairo_pattern_t *pat;
				pat = cairo_pattern_create_linear (0, 0, width, 0);
				murrine_pattern_add_color_stop_rgba (pat, 0.00, &highlight, 0.0);
				murrine_pattern_add_color_stop_rgba (pat, 0.25, &highlight, 0.5);
				murrine_pattern_add_color_stop_rgba (pat, 0.75, &highlight, 0.5);
				murrine_pattern_add_color_stop_rgba (pat, 1.00, &highlight, 0.0);
				cairo_set_source (cr, pat);
				cairo_pattern_destroy (pat);
				break;
			}
			case 3:
				return;
				break;
		}

		cairo_move_to (cr, 0.0,   1.0);
		cairo_line_to (cr, width, 1.0);
		cairo_stroke  (cr);
	}
	else
	{
		cairo_translate (cr, x+0.5, y);

		switch (separator->style)
		{
			default:
			case 0:
				murrine_set_color_rgba (cr, &dark, 0.5);
				break;
			case 1:
			{
				cairo_pattern_t *pat;
				pat = cairo_pattern_create_linear (0, 0, 0, height);
				murrine_pattern_add_color_stop_rgba (pat, 0.00, &dark, 0.0);
				murrine_pattern_add_color_stop_rgba (pat, 0.25, &dark, 0.5);
				murrine_pattern_add_color_stop_rgba (pat, 0.75, &dark, 0.5);
				murrine_pattern_add_color_stop_rgba (pat, 1.00, &dark, 0.0);
				cairo_set_source (cr, pat);
				cairo_pattern_destroy (pat);
				break;
			}
			case 2:
			{
				cairo_pattern_t *pat;
				murrine_shade (&colors->bg[0], 0.758, &dark);
				pat = cairo_pattern_create_linear (0, 0, 0, height);
				murrine_pattern_add_color_stop_rgba (pat, 0.00, &dark, 0.0);
				murrine_pattern_add_color_stop_rgba (pat, 1.00, &dark, 1.0);
				cairo_set_source (cr, pat);
				cairo_pattern_destroy (pat);
				break;
			}
		}
		cairo_move_to (cr, 0.0, 0.0);
		cairo_line_to (cr, 0.0, height);
		cairo_stroke  (cr);

		switch (separator->style)
		{
			default:
			case 0:
				murrine_set_color_rgba (cr, &highlight, 0.5);
				break;
			case 1:
			{
				cairo_pattern_t *pat;
				pat = cairo_pattern_create_linear (0, 0, 0, height);
				murrine_pattern_add_color_stop_rgba (pat, 0.00, &highlight, 0.0);
				murrine_pattern_add_color_stop_rgba (pat, 0.25, &highlight, 0.5);
				murrine_pattern_add_color_stop_rgba (pat, 0.75, &highlight, 0.5);
				murrine_pattern_add_color_stop_rgba (pat, 1.00, &highlight, 0.0);
				cairo_set_source (cr, pat);
				cairo_pattern_destroy (pat);
				break;
			}
			case 2:
			{
				cairo_pattern_t *pat;
				pat = cairo_pattern_create_linear (0, 0, 0, height);
				murrine_pattern_add_color_stop_rgba (pat, 0.00, &highlight, 0.0);
				murrine_pattern_add_color_stop_rgba (pat, 1.00, &highlight, 1.0);
				cairo_set_source (cr, pat);
				cairo_pattern_destroy (pat);
				break;
			}
			case 3:
				return;
				break;
		}

		cairo_move_to (cr, 1.0, 0.0);
		cairo_line_to (cr, 1.0, height);
		cairo_stroke  (cr);
	}
}

static void
murrine_draw_combo_separator (cairo_t *cr,
                              const MurrineColors    *colors,
                              const WidgetParameters *widget,
                              int x, int y, int width, int height)
{
	const MurrineRGB *dark = &colors->shade[6];

	cairo_translate        (cr, x+0.5, y);

	murrine_set_color_rgba (cr, dark, 0.4);
	cairo_move_to          (cr, 0.0, 0.0);
	cairo_line_to          (cr, 0.0, height+1);
	cairo_stroke           (cr);
}

static void
murrine_draw_list_view_header (cairo_t *cr,
                               const MurrineColors            *colors,
                               const WidgetParameters         *widget,
                               const ListViewHeaderParameters *header,
                               int x, int y, int width, int height)
{
	const MurrineRGB *fill   = &colors->bg[widget->state_type];
	MurrineRGB border = colors->shade[4];
	MurrineRGB grip = colors->shade[3];
	MurrineRGB highlight;

	murrine_shade (&border, 1.3, &highlight);

	cairo_translate (cr, x, y);

	if (header->order & MRN_ORDER_FIRST)
	{
		cairo_move_to (cr, 0.5, height-1);
		cairo_line_to (cr, 0.5, 0.5);
	}
	else
		cairo_move_to (cr, 0.0, 0.5);

	/* Effects */
	switch (header->style)
	{
		case 0:
			murrine_set_color_rgb (cr, &highlight);
			cairo_line_to (cr, width, 0.5);
			cairo_stroke (cr);
			break;
		case 1:
			cairo_rectangle (cr, 0, 0, width, height);

			murrine_draw_glaze (cr, fill,
			                    widget->glow_shade, widget->highlight_shade, widget->glazestyle != 0 ? widget->lightborder_shade : 1.0,
			                    widget->mrn_gradient, widget, 0, 0, width, height-1,
			                    widget->roundness, widget->corners, TRUE);

			if (widget->mrn_gradient.has_border_colors)
			{
				border = grip = widget->mrn_gradient.border_colors[1];
				grip =  widget->mrn_gradient.border_colors[0];
			}
			break;
		case 2:
			border = colors->shade[4];
			MurrineRGB shadow_header;
			murrine_shade (fill, 0.925, &shadow_header);

			if (!widget->mrn_gradient.gradients)
			{
				murrine_set_color_rgb (cr, &shadow_header);
				cairo_rectangle       (cr, 0.0, height-3.0, width, 2.0);
			}
			else
			{
				cairo_pattern_t *pat;
				pat = cairo_pattern_create_linear (0.0, height-4.0, 0.0, height-1.0);
				murrine_pattern_add_color_stop_rgba (pat, 0.0, &shadow_header, 0.0);
				murrine_pattern_add_color_stop_rgb (pat, 1.0, &shadow_header);
				cairo_set_source      (cr, pat);
				cairo_pattern_destroy (pat);
				cairo_rectangle       (cr, 0.0, height-4.0, width, 3.0);
			}
			cairo_fill (cr);
			break;
		case 3:
		{
			cairo_pattern_t *pat;
			MurrineRGB shade;

			murrine_shade (fill, 0.948, &shade);
			pat = cairo_pattern_create_linear (0, 0, 0, height);
			murrine_pattern_add_color_stop_rgb (pat, 0, fill);
			murrine_pattern_add_color_stop_rgb (pat, 1, &shade);
			cairo_set_source (cr, pat);
			cairo_pattern_destroy (pat);

			cairo_rectangle (cr, 0, 0, width, height);
			cairo_fill (cr);

			/* Top line */
			murrine_shade (fill, 1.023, &shade);
			murrine_set_color_rgb (cr, &shade);
			cairo_move_to (cr, 0, 0.5);
			cairo_line_to (cr, width, 0.5);
			cairo_stroke (cr);

			murrine_shade (&colors->bg[0], 0.788, &border);
			break;
		}
	}
	murrine_shade (&border, widget->mrn_gradient.border_shades[1], &border);

	/* Draw bottom border */
	murrine_set_color_rgb (cr, &border);
	cairo_move_to (cr, 0.0, height-0.5);
	cairo_line_to (cr, width, height-0.5);
	cairo_stroke (cr);

	/* Draw resize grip */
	if ((widget->ltr && !(header->order & MRN_ORDER_LAST)) ||
	    (!widget->ltr && !(header->order & MRN_ORDER_FIRST)) || header->resizable)
	{
		murrine_shade (&grip, widget->mrn_gradient.border_shades[0], &grip);

		if (header->style == 1 && widget->glazestyle > 0)
		{
			cairo_translate       (cr, width-0.5, 0);

			murrine_set_color_rgb (cr, &grip);
			cairo_move_to         (cr, 0, 0);
			cairo_line_to         (cr, 0, height-1);
			cairo_stroke          (cr);
		}
		else
		{
			SeparatorParameters separator;
			separator.horizontal = FALSE;
			separator.style = 2;

			murrine_draw_separator (cr, colors, widget, &separator, width-2.5, 4.0, 2, height-5.0);
		}
	}
}

static void
murrine_draw_menuitem (cairo_t *cr,
                       const MurrineColors    *colors,
                       const WidgetParameters *widget,
                       int x, int y, int width, int height,
                       int menuitemstyle)
{
	MurrineGradients mrn_gradient_new = widget->mrn_gradient;
	MurrineRGB border = colors->spot[2];
	MurrineRGB fill = colors->spot[1];

	murrine_get_fill_color (&fill, &mrn_gradient_new);

	cairo_translate (cr, x, y);
	murrine_rounded_rectangle_closed (cr, 0, 0, width, height, widget->roundness, widget->corners);

	switch (menuitemstyle)
	{
		case 0:
			mrn_gradient_new.has_border_colors = FALSE;
			mrn_gradient_new.has_gradient_colors = FALSE;

			murrine_set_gradient (cr, &fill, mrn_gradient_new, 0, 0, 0, height, mrn_gradient_new.gradients, FALSE);
			cairo_fill (cr);

			murrine_set_color_rgba (cr, &border, 0.15);
			murrine_rounded_rectangle_closed (cr, 0.5, 0.5, width-1, height-1, widget->roundness, widget->corners);
			cairo_fill_preserve (cr);
			break;
		default:
		case 1:
			cairo_clip_preserve (cr);

			murrine_draw_glaze (cr, &fill,
			                    widget->glow_shade, widget->highlight_shade, widget->lightborder_shade,
			                    mrn_gradient_new, widget, 1, 1, width-2, height-2,
			                    widget->roundness, widget->corners, TRUE);
			break;
		case 2:
		{
			murrine_set_gradient (cr, &fill, mrn_gradient_new, 0, 0, 0, height, mrn_gradient_new.gradients, FALSE);
			cairo_fill (cr);

			MurrineRGB effect;
			double tile_pos = 0;
			double stroke_width;
			int    x_step;
			stroke_width = height*2;
			x_step = (((float)stroke_width/10));

			murrine_shade (&fill, murrine_get_contrast(0.65, widget->contrast), &effect);

			cairo_save (cr);
			/* Draw strokes */
			while (stroke_width > 0 && tile_pos <= width+x_step)
			{
				cairo_move_to (cr, stroke_width/2-x_step, 0);
				cairo_line_to (cr, stroke_width-x_step, 0);
				cairo_line_to (cr, stroke_width/2-x_step, height);
				cairo_line_to (cr, -x_step, height);

				cairo_translate (cr, stroke_width, 0);
				tile_pos += stroke_width;
			}
			murrine_set_color_rgba (cr, &effect, 0.15);
			cairo_fill (cr);
			cairo_restore (cr);
			break;
		}
	}

	murrine_draw_border (cr, &border,
	                     0.5, 0.5, width-1, height-1,
	                     widget->roundness, widget->corners,
	                     mrn_gradient_new, 0.8);
}

static void
murrine_draw_scrollbar_trough (cairo_t *cr,
                               const MurrineColors       *colors,
                               const WidgetParameters    *widget,
                               const ScrollBarParameters *scrollbar,
                               int x, int y, int width, int height)
{
	MurrineRGB border;
	MurrineRGB fill;

	murrine_shade (&widget->parentbg,
	               murrine_get_contrast (scrollbar->stepperstyle != 1 ? 0.86 : 0.8, widget->contrast),
	               &border);
	murrine_shade (&widget->parentbg, scrollbar->stepperstyle != 1 ? 0.97 : 1.026, &fill);
/*
	murrine_shade (&colors->bg[widget->state_type],
	               murrine_get_contrast (scrollbar->stepperstyle < 1 ? 0.86 : 0.8, widget->contrast),
	               &border_bg);
	murrine_shade (&colors->bg[widget->state_type], scrollbar->stepperstyle < 1 ? 0.97 : 1.026, &fill_bg);
	murrine_mix_color (&border_bg, &border, 0.5, &border);
	murrine_mix_color (&fill_bg, &fill, 0.5, &fill);
*/
	if (!scrollbar->horizontal)
	{
		cairo_translate (cr, x, y);
	}
	else
	{
		int tmp = height;
		rotate_mirror_translate (cr, M_PI/2, x, y, FALSE, FALSE);
		height = width;
		width = tmp;
	}

	/* Draw fill */
	murrine_draw_trough (cr, &fill, 0, 0, width, height, widget->roundness, widget->corners, widget->mrn_gradient, 1.0, FALSE);

	if (scrollbar->stepperstyle == 3)
	{
		uint8 corners;
		MurrineRGB fill_stepper;
		MurrineRGB border_stepper;

		murrine_shade (&widget->parentbg, 1.02, &fill_stepper);
		murrine_shade (&border, (widget->mrn_gradient.trough_shades[0]+widget->mrn_gradient.trough_shades[1])/2.0, &border_stepper);

		cairo_save (cr);

		murrine_rounded_rectangle_closed (cr, 0.5, 0.5, width-1, height-1, widget->roundness, widget->corners);
		cairo_clip (cr);

		corners = MRN_CORNER_BOTTOMLEFT | MRN_CORNER_BOTTOMRIGHT;
		murrine_rounded_rectangle_inverted (cr, 0.5, 0.5, width-1, scrollbar->steppersize, widget->roundness, corners);
		murrine_set_color_rgb (cr, &fill_stepper);
		cairo_fill_preserve (cr);
		murrine_draw_trough_border_from_path (cr, &border,0.5, 0.5, width-1, scrollbar->steppersize, widget->mrn_gradient, 1.0, FALSE);

		corners = MRN_CORNER_TOPLEFT | MRN_CORNER_TOPRIGHT;
		murrine_rounded_rectangle_inverted (cr, 0.5, height-scrollbar->steppersize-0.5, width-1, scrollbar->steppersize, widget->roundness, corners);
		murrine_set_color_rgb (cr, &fill_stepper);
		cairo_fill_preserve (cr);
		murrine_draw_trough_border_from_path (cr, &border, 0.5, height-scrollbar->steppersize-0.5, width-1, scrollbar->steppersize, widget->mrn_gradient, 1.0, FALSE);

		cairo_restore (cr);
	}

	/* Draw border */
	if (!scrollbar->within_bevel)
		murrine_draw_trough_border (cr, &border, 0.5, 0.5, width-1, height-1, widget->roundness, widget->corners, widget->mrn_gradient, 1.0, FALSE);
	else
	{
		murrine_shade (&border, widget->mrn_gradient.trough_shades[0], &border);
		murrine_set_color_rgb (cr, &border);
		cairo_move_to (cr, 0.5, 0);
		cairo_line_to (cr, 0.5, height);
		cairo_stroke (cr);
	}
}

static void
murrine_draw_scrollbar_stepper (cairo_t *cr,
                                const MurrineColors       *colors,
                                const WidgetParameters    *widget,
                                const ScrollBarParameters *scrollbar,
                                int x, int y, int width, int height)
{
	MurrineGradients mrn_gradient_new = widget->mrn_gradient;
	double border_stop_mid = ((mrn_gradient_new.border_shades[0])+
	                          (mrn_gradient_new.border_shades[1]))/2.0;
	MurrineRGB border;
	MurrineRGB fill  = colors->bg[widget->state_type];

	murrine_get_fill_color (&fill, &mrn_gradient_new);
	murrine_shade (&colors->shade[6], 0.95, &border);

	mrn_gradient_new.border_shades[0] = border_stop_mid;
	mrn_gradient_new.border_shades[1] = border_stop_mid;

	if (!scrollbar->horizontal)
		murrine_exchange_axis (cr, &x, &y, &width, &height);

	/* Border color */
	murrine_mix_color (&border, &fill, 0.4, &border);

	cairo_translate (cr, x, y);

	cairo_save (cr);

	murrine_rounded_rectangle_closed (cr, 1, 1, width-2, height-2, widget->roundness-1, widget->corners);
	cairo_clip_preserve(cr);

	murrine_draw_glaze (cr, &fill,
	                    widget->glow_shade, widget->highlight_shade, widget->lightborder_shade,
	                    mrn_gradient_new, widget, 1, 1, width-2, height-2,
	                    widget->roundness, widget->corners, TRUE);

	cairo_restore (cr);

	murrine_draw_border (cr, widget->draw_border ? &border : &colors->bg[widget->state_type],
	                     0.5, 0.5, width-1, height-1,
	                     widget->roundness, widget->corners,
	                     mrn_gradient_new, 1.0);
}

static void
murrine_draw_scrollbar_slider (cairo_t *cr,
                               const MurrineColors       *colors,
                               const WidgetParameters    *widget,
                               const ScrollBarParameters *scrollbar,
                               int x, int y, int width, int height)
{
	MurrineGradients mrn_gradient_new = widget->mrn_gradient;
	double border_stop_mid = ((mrn_gradient_new.border_shades[0])+
	                          (mrn_gradient_new.border_shades[1]))/2.0;
	MurrineRGB fill = scrollbar->has_color ? scrollbar->color : colors->bg[widget->state_type];
	MurrineRGB border, shade;
	uint8 corners = widget->corners;
	cairo_pattern_t *pat;

	murrine_get_fill_color (&fill, &mrn_gradient_new);

	if (scrollbar->stepperstyle != 1 && scrollbar->stepperstyle != 3)
	{
		if (scrollbar->junction & MRN_JUNCTION_BEGIN)
		{
			if (scrollbar->horizontal)
			{
				x -= 1;
				width += 1;
			}
			else
			{
				y -= 1;
				height += 1;
			}
		}
		if (scrollbar->junction & MRN_JUNCTION_END)
		{
			if (scrollbar->horizontal)
				width += 1;
			else
				height += 1;
		}
	}

	if (scrollbar->stepperstyle == 2)
	{
		if (scrollbar->junction & MRN_JUNCTION_BEGIN)
		{
			if (scrollbar->horizontal)
				corners ^= MRN_CORNER_TOPLEFT | MRN_CORNER_BOTTOMLEFT;
			else
				corners ^= MRN_CORNER_TOPLEFT | MRN_CORNER_BOTTOMLEFT;
		}
		if (scrollbar->junction & MRN_JUNCTION_END)
		{
			if (scrollbar->horizontal)
				corners ^= MRN_CORNER_TOPRIGHT | MRN_CORNER_BOTTOMRIGHT;
			else
				corners ^= MRN_CORNER_TOPRIGHT | MRN_CORNER_BOTTOMRIGHT;
		}
	}

	murrine_shade (&colors->shade[6], 0.95, &border);

	mrn_gradient_new.border_shades[0] = border_stop_mid;
	mrn_gradient_new.border_shades[1] = border_stop_mid;

	if (widget->prelight && scrollbar->has_color)
		murrine_shade (&fill, scrollbar->prelight_shade, &fill);

	murrine_mix_color (&border, &fill, 0.4, &border);

	if (scrollbar->horizontal)
		cairo_translate (cr, x, y);
	else
	{
		int tmp = height;
		rotate_mirror_translate (cr, M_PI/2, x, y, FALSE, FALSE);
		height = width;
		width = tmp;
	}

	cairo_save (cr);

	murrine_rounded_rectangle_closed (cr, 1, 1, width-2, height-2, widget->roundness-1, corners);
	cairo_clip_preserve (cr);

	double fill_shade = 0.9;
	if (widget->has_fill_shade)
		fill_shade = widget->fill_shade;
	murrine_shade (&colors->bg[widget->state_type], fill_shade, &shade);

	pat = cairo_pattern_create_linear (1, 1, 1, height-2);
	murrine_pattern_add_color_stop_rgb (pat, 0, &shade);
	murrine_pattern_add_color_stop_rgb (pat, 1, &colors->bg[widget->state_type]);

	murrine_rounded_rectangle_closed (cr, 1, 1, width-2, height-2,
					  MIN (width, height), corners);
	cairo_set_source (cr, pat);
	cairo_pattern_destroy (pat);

	cairo_fill (cr);

	/* Draw the options */
	MurrineRGB style;
	if (scrollbar->style > 0)
		murrine_shade (&fill, 0.55, &style);

	switch (scrollbar->style)
	{
		case 1:
		{
			int circ_radius = 2;
			int circ_space = 5;
			int i;
			int x1 = circ_space+circ_radius;
			int y1 = height/2;
			for (i = circ_space; i < width-circ_space; i += 2*circ_radius+circ_space)
			{
				cairo_move_to (cr, i, 1);
				cairo_arc (cr, x1, y1, circ_radius, 0, M_PI*2);

				x1 += 2*circ_radius+circ_space;

				cairo_close_path (cr);
				murrine_set_color_rgba (cr, &style, 0.15);
				cairo_fill (cr);
			}
			break;
		}
		case 3:
		case 4:
		{
			int counter = -width;
			cairo_save (cr);
			cairo_rectangle (cr, 1, 1, width-2, height-2);
			cairo_clip (cr);
			cairo_new_path (cr);
			cairo_set_line_width (cr, 5.0); /* stroke width */
			murrine_set_color_rgba (cr, &style, 0.08);
			while (counter < height)
			{
				cairo_move_to (cr, width, counter);
				cairo_line_to (cr, 0, counter+width);
				cairo_stroke  (cr);
				counter += 12;
			}
			cairo_restore (cr);
			break;
		}
		case 5:
		case 6:
		{
			int stroke_width = 7;
			int stroke_space = 5;
			int i;
			murrine_set_color_rgba (cr, &style, 0.08);
			for (i = stroke_space; i < width-stroke_space; i += stroke_width+stroke_space)
			{
				cairo_move_to (cr, i, 1);
				cairo_rel_line_to (cr, 0, height-2);
				cairo_rel_line_to (cr, stroke_width, 0);
				cairo_rel_line_to (cr, 0, -(height-2));
				cairo_fill (cr);
			}
			break;
		}
	}
	/* Draw the handle */
	if (scrollbar->style > 0 && scrollbar->style % 2 == 0)
	{
		double bar_x = width/2-3.5;
		int i;

		switch (scrollbar->handlestyle)
		{
			default:
			case 0:
			{
				for (i=0; i<3; i++)
				{
					cairo_move_to (cr, bar_x, 5);
					cairo_line_to (cr, bar_x, height-5);
					murrine_set_color_rgb (cr, &border);
					cairo_stroke (cr);

					bar_x += 3;
				}
				break;
			}
			case 1:
			{
				MurrineRGB inset;				
				murrine_shade (&fill, 1.08, &inset);

				for (i=0; i<3; i++)
				{
					cairo_move_to (cr, bar_x, 5);
					cairo_line_to (cr, bar_x, height-5);
					murrine_set_color_rgb (cr, &border);
					cairo_stroke (cr);

					cairo_move_to (cr, bar_x+1, 5);
					cairo_line_to (cr, bar_x+1, height-5);
					murrine_set_color_rgb (cr, &inset);
					cairo_stroke (cr);

					bar_x += 3;
				}
				break;
			}
			case 2:
			{	
				MurrineRGB inset;				
				murrine_shade (&fill, 1.08, &inset);
			
				bar_x++;

				for (i=0; i<3; i++)
				{
					cairo_move_to (cr, bar_x, 5);
					cairo_line_to (cr, bar_x, height-5);
					murrine_set_color_rgb (cr, &border);
					cairo_stroke (cr);

					cairo_move_to (cr, bar_x+1, 4);
					cairo_line_to (cr, bar_x+1, height-5);
					murrine_set_color_rgb (cr, &inset);
					cairo_stroke (cr);

					bar_x += 2;
				}			
				break;
			}
		}
	}

	cairo_restore (cr);

	murrine_draw_border (cr, widget->draw_border ? &border : &colors->bg[widget->state_type],
	                     0.5, 0.5, width-1, height-1,
			     MIN (width, height), corners,
	                     mrn_gradient_new, 1.0);
}

static void
murrine_draw_selected_cell (cairo_t *cr,
                            const MurrineColors    *colors,
                            const WidgetParameters *widget,
                            const CellParameters   *cell,
                            int x, int y, int width, int height)
{
	cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
	cairo_set_line_join (cr, CAIRO_LINE_JOIN_MITER);
	MurrineRGB fill = widget->focus ? colors->base[widget->state_type] :
	                                  colors->base[GTK_STATE_ACTIVE];

	cairo_save (cr);
	cairo_translate (cr, x, y);

	murrine_set_gradient (cr, &fill, widget->mrn_gradient, 0, 0, 0, height, widget->mrn_gradient.gradients, FALSE);
	cairo_rectangle (cr, 0, 0, width, height);
	cairo_fill (cr);

	cairo_restore (cr);
}

static void
murrine_draw_statusbar (cairo_t *cr,
                        const MurrineColors    *colors,
                        const WidgetParameters *widget,
                        int x, int y, int width, int height)
{
	const MurrineRGB *dark = &colors->shade[3];
	const MurrineRGB *highlight = &colors->shade[0];

	cairo_translate       (cr, x, y+0.5);

	murrine_set_color_rgb (cr, dark);
	cairo_move_to         (cr, 0, 0);
	cairo_line_to         (cr, width, 0);
	cairo_stroke          (cr);

	murrine_set_color_rgb (cr, highlight);
	cairo_move_to         (cr, 0, 1);
	cairo_line_to         (cr, width, 1);
	cairo_stroke          (cr);
}

static void
murrine_draw_menu_frame (cairo_t *cr,
                         const MurrineColors    *colors,
                         const WidgetParameters *widget,
                         int x, int y, int width, int height,
                         int menustyle)
{
	cairo_translate       (cr, x, y);

	switch (menustyle)
	{
		case 1:
		{
			MurrineRGB *fill = (MurrineRGB*)&colors->spot[1];
			MurrineRGB border2;
			murrine_shade (fill, 0.5, &border2);

			murrine_set_color_rgb (cr, &border2);
			cairo_rectangle (cr, 0.5, 0.5, 3, height-1);
			cairo_stroke_preserve (cr);

			murrine_set_color_rgb (cr, fill);
			cairo_fill (cr);
		}
		default:
		case 0:
		{
			const MurrineRGB *border = &colors->shade[5];

			murrine_set_color_rgb (cr, border);
			cairo_rectangle       (cr, 0.5, 0.5, width-1, height-1);
			cairo_stroke          (cr);
			break;
		}
		case 2:
		{
			const MurrineRGB *border = &colors->shade[2];
			MurrineRGB fill;
			raico_blur_t* blur = NULL;
			cairo_t *cr_surface; 
			cairo_surface_t *surface;
			cairo_pattern_t *pat;
			int bradius = 30;
			int bheight = MIN (height, 300);

			murrine_shade (&colors->bg[0], 1.14, &fill);

			murrine_set_color_rgb (cr, border);
			cairo_rectangle       (cr, 0.5, 0.5, width-1, height-1);
			cairo_stroke          (cr);

			/* draw glow */
			surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, bheight);
			cr_surface = cairo_create (surface); 
			blur = raico_blur_create (RAICO_BLUR_QUALITY_LOW);
			raico_blur_set_radius (blur, bradius);
			cairo_set_line_width (cr_surface, 1.0);
			cairo_rectangle (cr_surface, bradius, bradius-15, width-bradius*2, bheight-bradius*2+15);
			murrine_set_color_rgb (cr_surface, &fill);
			cairo_fill (cr_surface);
			raico_blur_apply (blur, surface);
			cairo_rectangle (cr_surface, 0, -15, width, bheight+15);
			pat = cairo_pattern_create_linear (0, -15, 0.0, bheight+15);
			murrine_pattern_add_color_stop_rgba (pat, 0.25, &colors->bg[0], 0.0);
			murrine_pattern_add_color_stop_rgba (pat, 1.0, &colors->bg[0], 1.0);
			cairo_set_source (cr_surface, pat);
			cairo_pattern_destroy (pat);
			cairo_fill (cr_surface);
			cairo_set_source_surface (cr, surface, 0, 0); 
			cairo_paint (cr);
			cairo_surface_destroy (surface); 
			cairo_destroy (cr_surface);
			break;
		}
		case 3:
		{
			MurrineRGB border;
			MurrineRGB fill;
			raico_blur_t* blur = NULL;
			cairo_t *cr_surface; 
			cairo_surface_t *surface;
			cairo_pattern_t *pat;
			int bradius = 30;
			int bheight = MIN (height, 300);

			murrine_shade (&colors->bg[0], murrine_get_contrast(1.1, widget->contrast), &border);
			murrine_shade (&colors->bg[0], 0.96, &fill);

			murrine_set_color_rgb (cr, &border);
			cairo_rectangle       (cr, 0.5, 0.5, width-1, height-1);
			cairo_stroke          (cr);

			/* draw glow */
			surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, bheight);
			cr_surface = cairo_create (surface); 
			blur = raico_blur_create (RAICO_BLUR_QUALITY_LOW);
			raico_blur_set_radius (blur, bradius);
			cairo_set_line_width (cr_surface, 1.0);
			cairo_rectangle (cr_surface, bradius, bradius-15, width-bradius*2, bheight-bradius*2+15);
			murrine_set_color_rgb (cr_surface, &fill);
			cairo_fill (cr_surface);
			raico_blur_apply (blur, surface);
			cairo_rectangle (cr_surface, 0, -15, width, bheight+15);
			pat = cairo_pattern_create_linear (0, -15, 0.0, bheight+15);
			murrine_pattern_add_color_stop_rgba (pat, 0.25, &colors->bg[0], 0.0);
			murrine_pattern_add_color_stop_rgba (pat, 1.0, &colors->bg[0], 1.0);
			cairo_set_source (cr_surface, pat);
			cairo_pattern_destroy (pat);
			cairo_fill (cr_surface);
			cairo_set_source_surface (cr, surface, 0, 0); 
			cairo_paint (cr);
			cairo_surface_destroy (surface); 
			cairo_destroy (cr_surface);
			break;
		}
	}
}

static void
murrine_draw_tooltip (cairo_t *cr,
                      const MurrineColors    *colors,
                      const WidgetParameters *widget,
                      int x, int y, int width, int height)
{
	MurrineRGB border;
	MurrineRGB fill = colors->bg[widget->state_type];
	MurrineGradients mrn_gradient_new = murrine_get_decreased_gradient_shades (widget->mrn_gradient, 2.0);
	double glow_shade_new = murrine_get_decreased_shade (widget->glow_shade, 2.0);
	double highlight_shade_new = murrine_get_decreased_shade (widget->highlight_shade, 2.0);

	murrine_shade (&fill, murrine_get_contrast(0.6, widget->contrast), &border);
	murrine_get_fill_color (&fill, &mrn_gradient_new);

	cairo_save (cr);

	cairo_translate (cr, x, y);

	cairo_rectangle (cr, 1, 1, width-2, height-2);

#if 0
	murrine_draw_glaze (cr, &colors->bg[widget->state_type],
	                    glow_shade_new, highlight_shade_new, widget->lightborder_shade,
	                    mrn_gradient_new, widget, 1, 1, width-2, height-2,
	                    widget->roundness, widget->corners, TRUE);
#else
	murrine_set_color_rgb (cr, &fill);
	cairo_fill (cr);
#endif

	murrine_draw_border (cr, &border,
	                     0.5, 0.5, width-1, height-1,
	                     widget->roundness, widget->corners,
	                     mrn_gradient_new, 1.0);

	cairo_restore (cr);
}

static void
murrine_draw_iconview (cairo_t *cr,
                       const MurrineColors    *colors,
                       const WidgetParameters *widget,
                       int x, int y, int width, int height)
{
	MurrineRGB border;
	MurrineRGB fill = widget->focus ? colors->base[widget->state_type] :
	                                  colors->base[GTK_STATE_ACTIVE];

	murrine_shade (&fill, murrine_get_contrast(0.9, widget->contrast), &border);
	cairo_save (cr);

	cairo_translate (cr, x, y);
	cairo_save (cr);

	murrine_rounded_rectangle_closed (cr, 1, 1, width-2, height-2, widget->roundness-1, widget->corners);
	cairo_clip_preserve (cr);

#if 0
	murrine_draw_glaze (cr, &fill,
	                    widget->glow_shade, widget->highlight_shade, widget->lightborder_shade,
	                    widget->mrn_gradient, widget, 1, 1, width-2, height-2,
	                    widget->roundness-1, widget->corners, TRUE);
#else
	murrine_set_color_rgb (cr, &fill);
	cairo_fill (cr);
#endif

	cairo_restore (cr);

	murrine_draw_border (cr, &border,
	                     0.5, 0.5, width-1, height-1,
	                     widget->roundness, widget->corners,
	                     widget->mrn_gradient, 1.0);

	cairo_restore (cr);
}

static void
murrine_draw_handle (cairo_t *cr,
                     const MurrineColors    *colors,
                     const WidgetParameters *widget,
                     const HandleParameters *handle,
                     int x, int y, int width, int height)
{
	int bar_height;
	int bar_width = 4;
	int i, bar_y = 1;
	int num_bars, bar_spacing;
	num_bars = 3;
	bar_spacing = 3;
	bar_height = num_bars*bar_spacing;

	if (handle->horizontal)
	{
		int tmp = height;
		rotate_mirror_translate (cr, M_PI/2, x+0.5+width/2-bar_height/2, y+height/2-bar_width/2, FALSE, FALSE);
		height = width;
		width = tmp;
	}
	else
	{
		cairo_translate (cr, x+width/2-bar_width/2, y+height/2-bar_height/2+0.5);
	}

	switch (handle->style)
	{
		default:
		case 0:
		{
			for (i=0; i<num_bars; i++)
			{
				cairo_move_to (cr, 0, bar_y);
				cairo_line_to (cr, bar_width, bar_y);
				murrine_set_color_rgb (cr, &colors->shade[4]);
				cairo_stroke (cr);

				bar_y += bar_spacing;
			}
			break;
		}
		case 1:
		{	
			for (i=0; i<num_bars; i++)
			{
				cairo_move_to (cr, 0, bar_y);
				cairo_line_to (cr, bar_width, bar_y);
				murrine_set_color_rgb (cr, &colors->shade[4]);
				cairo_stroke (cr);

				cairo_move_to (cr, 0, bar_y+1);
				cairo_line_to (cr, bar_width, bar_y+1);
				murrine_set_color_rgb (cr, &colors->shade[0]);
				cairo_stroke (cr);

				bar_y += bar_spacing;
			}
			break;
		}
		case 2:
		{	
			bar_y++;

			for (i=0; i<num_bars; i++)
			{
				cairo_move_to (cr, 0, bar_y);
				cairo_line_to (cr, bar_width, bar_y);
				murrine_set_color_rgb (cr, &colors->shade[4]);
				cairo_stroke (cr);

				cairo_move_to (cr, 0, bar_y+1);
				cairo_line_to (cr, bar_width, bar_y+1);
				murrine_set_color_rgb (cr, &colors->shade[0]);
				cairo_stroke (cr);

				bar_y += 2;
			}			
			break;
		}
	}
}

static void
murrine_draw_normal_arrow (cairo_t *cr,
                           const MurrineRGB *color,
                           double x, double y, double width, double height)
{
	double arrow_width;
	double arrow_height;
	double line_width_2;

	cairo_save (cr);

	arrow_width = MIN (height*2.0 + MAX (1.0, ceil (height*2.0/6.0*2.0)/2.0)/2.0, width);
	line_width_2 = MAX (1.0, ceil (arrow_width/6.0*2.0)/2.0)/2.0;
	arrow_height = arrow_width/2.0+line_width_2;

	cairo_translate (cr, x, y-arrow_height/2.0);

	cairo_move_to (cr, -arrow_width/2.0, line_width_2);
	cairo_line_to (cr, -arrow_width/2.0 + line_width_2, 0);
	cairo_arc_negative (cr, 0, arrow_height-2*line_width_2-2*line_width_2*sqrt(2), 2*line_width_2, M_PI_2+M_PI_4, M_PI_4);
	cairo_line_to (cr, arrow_width/2.0-line_width_2, 0);
	cairo_line_to (cr, arrow_width/2.0, line_width_2);
	cairo_line_to (cr, 0, arrow_height);
	cairo_close_path (cr);

	murrine_set_color_rgb (cr, color);
	cairo_fill (cr);

	cairo_restore (cr);
}

static void
murrine_draw_normal_arrow_filled (cairo_t *cr,
                                  const MurrineRGB *color,
                                  double x, double y, double width, double height)
{
	int arrow_width = width;
	int arrow_height = height;
	cairo_pattern_t *pat;

	cairo_save (cr);

	cairo_translate (cr, 0, -0.5);
	cairo_move_to (cr, -arrow_width/2, -arrow_height/2);
	cairo_line_to (cr, 0, arrow_height/2);
	cairo_line_to (cr, arrow_width/2, -arrow_height/2);
	cairo_close_path (cr);

	pat = cairo_pattern_create_linear (0, -arrow_height/2, 0, arrow_height/2);
	murrine_pattern_add_color_stop_rgba (pat, 0.0, color, 0.6);
	murrine_pattern_add_color_stop_rgba (pat, 1.0, color, 0.8);
	cairo_set_source (cr, pat);
	cairo_fill_preserve (cr);
	cairo_pattern_destroy (pat);

	murrine_set_color_rgb (cr, color);
	cairo_stroke (cr);

	cairo_restore (cr);
}

static void
murrine_draw_normal_arrow_filled_equilateral (cairo_t *cr,
                                              const MurrineRGB *color,
                                              double x, double y, double width, double height)
{
	gdouble arrow_width = width+2;
	gdouble arrow_height = height;
	cairo_pattern_t *pat;

	cairo_save (cr);

	cairo_translate (cr, 0, -0.5);
	cairo_move_to (cr, -arrow_width/2, -arrow_height/2);
	cairo_line_to (cr, 0, arrow_height/2);
	cairo_line_to (cr, arrow_width/2, -arrow_height/2);
	cairo_close_path (cr);

	pat = cairo_pattern_create_linear (0, -arrow_height/2, 0, arrow_height/2);
	murrine_pattern_add_color_stop_rgba (pat, 0.0, color, 0.6);
	murrine_pattern_add_color_stop_rgba (pat, 1.0, color, 0.8);
	cairo_set_source (cr, pat);
	cairo_fill_preserve (cr);
	cairo_pattern_destroy (pat);

	murrine_set_color_rgb (cr, color);
	cairo_stroke (cr);

	cairo_restore (cr);
}

static void
murrine_draw_combo_arrow (cairo_t *cr,
                          const MurrineRGB *color,
                          double x, double y, double width, double height)
{
	double arrow_width = MIN (height*2/3.0, width);
	double arrow_height = arrow_width/2.0;
	double gap_size = arrow_height;

	cairo_save (cr);
	cairo_translate (cr, x, y-(arrow_height+gap_size)/2.0);
	cairo_rotate (cr, M_PI);
	murrine_draw_normal_arrow (cr, color, 0, 0, arrow_width, arrow_height);
	cairo_restore (cr);

	murrine_draw_normal_arrow (cr, color, x, y+(arrow_height+gap_size)/2.0, arrow_width, arrow_height);
}

static void
murrine_draw_combo_arrow_filled (cairo_t *cr,
                                 const MurrineRGB *color,
                                 double x, double y, double width, double height)
{
	double arrow_width = 4;
	double arrow_height = 5;

	cairo_save (cr);
	cairo_translate (cr, x, y-5.5);
	cairo_rotate (cr, M_PI);
	murrine_draw_normal_arrow_filled (cr, color, 0, 0, arrow_width, arrow_height);
	cairo_restore (cr);

	cairo_translate (cr, x, y+5.5);

	murrine_draw_normal_arrow_filled (cr, color, 0, 0, arrow_width, arrow_height);
}

static void
murrine_draw_combo_arrow_filled_equilateral (cairo_t *cr,
                                             const MurrineRGB *color,
                                             double x, double y, double width, double height)
{
	double arrow_width = 3;
	double arrow_height = 3;

	cairo_save (cr);
	cairo_translate (cr, x-1, y-5);
	cairo_rotate (cr, M_PI);
	murrine_draw_normal_arrow_filled_equilateral (cr, color, 0, 0, arrow_width, arrow_height);
	cairo_restore (cr);

	cairo_translate (cr, x-1, y+2);

	murrine_draw_normal_arrow_filled_equilateral (cr, color, 0, 0, arrow_width, arrow_height);
}

static void
_murrine_draw_arrow (cairo_t *cr,
                     const MurrineRGB *color,
                     const ArrowParameters *arrow,
                     double x, double y, double width, double height)
{
	double rotate;

	switch (arrow->direction)
	{
		default:
		case MRN_DIRECTION_DOWN:
			rotate = 0;
			break;
		case MRN_DIRECTION_UP:
			rotate = M_PI;
			break;
		case MRN_DIRECTION_LEFT:
			rotate = M_PI*1.5;
			break;
		case MRN_DIRECTION_RIGHT:
			rotate = M_PI*0.5;
			break;
	}

	if (arrow->type == MRN_ARROW_NORMAL)
	{
		cairo_translate (cr, x, y);
		cairo_rotate (cr, -rotate);
		switch (arrow->style)
		{
			default:
			case 0:
				murrine_draw_normal_arrow (cr, color, 0, 0, width, height);
				break;
			case 1:
				murrine_draw_normal_arrow_filled (cr, color, 0, 0, width, height);
				break;
			case 2:
				cairo_translate (cr, 0, 1.0);
				murrine_draw_normal_arrow_filled_equilateral (cr, color, 2, 1, width-4, height - 2);
				break;
			case 3:
				murrine_draw_normal_arrow_filled_equilateral (cr, color, 0, 0, width-2, height-2);
				break;
		}
	}
	else if (arrow->type == MRN_ARROW_COMBO)
	{
		cairo_translate (cr, x, y);
		switch (arrow->style)
		{
			default:
			case 0:
				murrine_draw_combo_arrow (cr, color, 0, 0, width, height);
				break;
			case 1:
				murrine_draw_combo_arrow_filled (cr, color, 0, 0, width, height);
				break;
			case 2:
				murrine_draw_combo_arrow_filled_equilateral (cr, color, 1, 1, width-2, height-2);
				break;
			case 3:
				cairo_translate (cr, -1, 0);
				murrine_draw_normal_arrow_filled_equilateral (cr, color, 1, 1, width-2, height-2);
				break;
			case 4:
				cairo_translate (cr, 0, 1);
				murrine_draw_normal_arrow_filled_equilateral (cr, color, 1, 1, width-2, height-2);
				break;
		}
	}
}

static void
murrine_draw_arrow (cairo_t *cr,
                    const MurrineColors    *colors,
                    const WidgetParameters *widget,
                    const ArrowParameters  *arrow,
                    int x, int y, int width, int height)
{
	MurrineRGB color = colors->fg[widget->state_type];
	murrine_mix_color (&color, &colors->bg[widget->state_type], 0.2, &color);
	gdouble tx, ty;

	tx = x+width/2.0;
	ty = y+height/2.0;

	if (widget->disabled)
	{
		_murrine_draw_arrow (cr, &colors->shade[0], arrow,
		                     tx+0.5, ty+0.5, width, height);
	}

	cairo_identity_matrix (cr);

	_murrine_draw_arrow (cr, &color, arrow,
	                     tx, ty, width, height);
}

static void
murrine_draw_radiobutton (cairo_t *cr,
                          const MurrineColors      *colors,
                          const WidgetParameters   *widget,
                          const CheckboxParameters *checkbox,
                          int x, int y, int width, int height,
                          double trans)
{
	MurrineRGB dot, upper_fill, border;
	MurrineRGB bg = colors->base[widget->state_type];
	gboolean inconsistent = FALSE;
	gboolean draw_box = !checkbox->in_menu;
	gboolean draw_bullet = (checkbox->shadow_type == GTK_SHADOW_IN);
	int roundness = width+height;
	double highlight_shade_new = widget->highlight_shade;
	double lightborder_shade_new = widget->lightborder_shade;
	MurrineGradients mrn_gradient_new = widget->mrn_gradient;
	cairo_pattern_t *pat;

	inconsistent = (checkbox->shadow_type == GTK_SHADOW_ETCHED_IN);
	draw_bullet |= inconsistent;
	
	// a optionbox has no active state, so we use this color for the checked state
	if (draw_bullet && widget->state_type == GTK_STATE_NORMAL)
		bg = colors->base[GTK_STATE_SELECTED];

	if (widget->state_type == GTK_STATE_INSENSITIVE)
	{
		border = colors->shade[3];
		dot    = colors->shade[3];
		bg     = colors->bg[0];

		mrn_gradient_new = murrine_get_decreased_gradient_shades (widget->mrn_gradient, 3.0);
		mrn_gradient_new.border_shades[0] = murrine_get_decreased_shade (widget->mrn_gradient.border_shades[0], 3.0);
		mrn_gradient_new.border_shades[1] = murrine_get_decreased_shade (widget->mrn_gradient.border_shades[1], 3.0);
		highlight_shade_new = murrine_get_decreased_shade (widget->highlight_shade, 2.0);
		lightborder_shade_new = murrine_get_decreased_shade (widget->lightborder_shade, 2.0);
	}
	else
	{
		murrine_shade (&bg, 0.6, &border);
		dot    = colors->text[GTK_STATE_SELECTED];
	}

	cairo_translate (cr, x, y);

	if (draw_box)
	{	
		if (widget->xthickness > 2 && widget->ythickness > 2)
		{
			if (widget->reliefstyle > 1 && draw_bullet && widget->state_type != GTK_STATE_INSENSITIVE)
			{
				if (widget->reliefstyle == 5)
					murrine_draw_shadow (cr, &widget->parentbg,
					                     0.5, 0.5, width-1, height-1,
					                     roundness+1, widget->corners,
					                     widget->reliefstyle,
					                     mrn_gradient_new, 0.5);
				else
				{
					MurrineRGB shadow;
					murrine_shade (&border, 0.9, &shadow);

					murrine_draw_shadow (cr, &shadow,
					                     0.5, 0.5, width-1, height-1,
					                     roundness+1, widget->corners,
					                     widget->reliefstyle,
					                     mrn_gradient_new, 0.08);
				}
			}
			else if (widget->reliefstyle != 0)
				murrine_draw_inset (cr, &widget->parentbg, 0.5, 0.5, width-1, height-1, roundness+1, widget->corners);
		}

		cairo_save (cr);

		murrine_rounded_rectangle_closed (cr, 1.5, 1.5, width-3, height-3, roundness, widget->corners);
		cairo_clip_preserve (cr);

		double fill_shade = 0.85;
		if (widget->has_fill_shade)
			fill_shade = widget->fill_shade;
		murrine_shade (&bg, fill_shade, &upper_fill);
		pat = cairo_pattern_create_linear (2, 2, 2, height-2);
		murrine_pattern_add_color_stop_rgb (pat, 0.0, &upper_fill);
		murrine_pattern_add_color_stop_rgb (pat, 1.0, &bg);
		cairo_set_source (cr, pat);
		cairo_pattern_destroy (pat);
		cairo_fill (cr);

		cairo_restore (cr);

		if (checkbox->in_cell)
		{
			mrn_gradient_new.border_shades[0] = 1.0;
			mrn_gradient_new.border_shades[1] = 1.0;
			if (!draw_bullet)
				mrn_gradient_new.has_border_colors = FALSE;
		}
		else if (!draw_bullet)
		{
			mrn_gradient_new = murrine_get_inverted_border_shades (mrn_gradient_new);
			mrn_gradient_new.has_border_colors = FALSE;
		}

		murrine_draw_border (cr, &border,
			             1.5, 1.5, width-3, height-3,
			             roundness, widget->corners,
			             mrn_gradient_new, 1.0);
	}

	if (draw_bullet)
	{
		if (inconsistent)
		{
			cairo_save (cr);
			cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
			cairo_set_line_width (cr, 2.0);

			cairo_move_to(cr, 5, (double)height/2);
			cairo_line_to(cr, width-5, (double)height/2);

			murrine_set_color_rgba (cr, &dot, trans);
			cairo_stroke (cr);
			cairo_restore (cr);
		}
		else
		{
			if (!draw_box)
			{
				cairo_arc (cr, (double)width/2, (double)height/2, (double)(width+height)/4-4, 0, G_PI*2);
			}
			else
			{
				MurrineRGB outline;
				murrine_invert_text (&dot, &outline);

				cairo_arc (cr, (double)width/2, (double)height/2, (double)(width+height)/4 - 3, 0, G_PI*2);
				murrine_set_color_rgba (cr, &outline, 0.3*trans*(widget->state_type == GTK_STATE_INSENSITIVE? 0.2 : 1.0));
				cairo_fill (cr);

				cairo_arc (cr, (double)width/2, (double)height/2, (double)(width+height)/4 - 4, 0, G_PI*2);
			}

			murrine_set_color_rgb (cr, &dot);
			cairo_fill (cr);
		}
	}
}

static void
murrine_draw_checkbox (cairo_t *cr,
                       const MurrineColors      *colors,
                       const WidgetParameters   *widget,
                       const CheckboxParameters *checkbox,
                       int x, int y, int width, int height,
                       double trans)
{
	MurrineRGB border;
	const MurrineRGB *dot;
	MurrineRGB bg = colors->base[widget->state_type];
	gboolean inconsistent = FALSE;
	gboolean draw_box = !checkbox->in_menu;
	gboolean draw_bullet = (checkbox->shadow_type == GTK_SHADOW_IN);
	int roundness = CLAMP (widget->roundness, 0, 2);
	double highlight_shade_new = widget->highlight_shade;
	double lightborder_shade_new = widget->lightborder_shade;
	MurrineGradients mrn_gradient_new = widget->mrn_gradient;

	inconsistent = (checkbox->shadow_type == GTK_SHADOW_ETCHED_IN);
	draw_bullet |= inconsistent;
	
	// a checkbox has no active state, so we use this color for the checked state
	if (draw_bullet && widget->state_type == GTK_STATE_NORMAL)
		bg = colors->base[GTK_STATE_SELECTED];

	if (widget->state_type == GTK_STATE_INSENSITIVE)
	{
		border = colors->shade[5];
		dot    = &colors->shade[3];

		mrn_gradient_new = murrine_get_decreased_gradient_shades (widget->mrn_gradient, 3.0);
		mrn_gradient_new.border_shades[0] = murrine_get_decreased_shade (widget->mrn_gradient.border_shades[0], 3.0);
		mrn_gradient_new.border_shades[1] = murrine_get_decreased_shade (widget->mrn_gradient.border_shades[1], 3.0);
		highlight_shade_new = murrine_get_decreased_shade (widget->highlight_shade, 2.0);
		lightborder_shade_new = murrine_get_decreased_shade (widget->lightborder_shade, 2.0);
	}
	else
	{
		murrine_shade (&bg, 0.6, &border);
		dot    = &colors->text[GTK_STATE_SELECTED];
	}

	cairo_translate (cr, x, y);

	if (draw_box)
	{
		MurrineRGB upper_fill;
		cairo_pattern_t *pat;

		if (widget->xthickness > 2 && widget->ythickness > 2)
		{
			if (widget->reliefstyle > 1 && draw_bullet && widget->state_type != GTK_STATE_INSENSITIVE)
			{
				if (widget->reliefstyle == 5)
					murrine_draw_shadow (cr, &widget->parentbg,
					                     0.5, 0.5, width-1, height-1,
					                     roundness+1, widget->corners,
					                     widget->reliefstyle,
					                     mrn_gradient_new, 0.5);
				else
				{
					MurrineRGB shadow;
					murrine_shade (&border, 0.9, &shadow);

					murrine_draw_shadow (cr, &shadow,
					                     0.5, 0.5, width-1, height-1,
					                     roundness+1, widget->corners,
					                     widget->reliefstyle,
					                     mrn_gradient_new, 0.08);
				}
			}
			else if (widget->reliefstyle != 0)
				murrine_draw_inset (cr, &widget->parentbg, 0.5, 0.5, width-1, height-1, roundness+1, widget->corners);
		}

		cairo_save (cr);

		murrine_rounded_rectangle_closed (cr, 1.5, 1.5, width-3, height-3, roundness, widget->corners);
		cairo_clip_preserve (cr);

		double fill_shade = 0.85;
		if (widget->has_fill_shade)
			fill_shade = widget->fill_shade;
		murrine_shade (&bg, fill_shade, &upper_fill);
		pat = cairo_pattern_create_linear (2, 2, 2, height-2);
		murrine_pattern_add_color_stop_rgb (pat, 0.0, &upper_fill);
		murrine_pattern_add_color_stop_rgb (pat, 0.2, &bg);
		cairo_set_source (cr, pat);
		cairo_pattern_destroy (pat);
		cairo_fill (cr);

		cairo_restore (cr);

		if (checkbox->in_cell)
		{
			mrn_gradient_new.border_shades[0] = 1.0;
			mrn_gradient_new.border_shades[1] = 1.0;
			if (!draw_bullet)
				mrn_gradient_new.has_border_colors = FALSE;
		}
		else if (!draw_bullet)
		{
			mrn_gradient_new = murrine_get_inverted_border_shades (mrn_gradient_new);
			mrn_gradient_new.has_border_colors = FALSE;
		}

		murrine_draw_border (cr, &border,
		                     1.5, 1.5, width-3, height-3,
		                     roundness, widget->corners,
		                     mrn_gradient_new, 1.0);
	}

	if (draw_bullet)
	{
		if (inconsistent)
		{
			cairo_save (cr);
			cairo_set_line_width (cr, 2.0);
			cairo_move_to (cr, 3, (double)height/2);
			cairo_line_to (cr, width-3, (double)height/2);
			cairo_restore (cr);
		}
		else
		{
			cairo_scale (cr, (double)width/18.0, (double)height/18.0);
			cairo_translate (cr, 3.0, 1.0);

			cairo_move_to (cr, 0, 8);
			cairo_line_to (cr, 5, 13);
			cairo_line_to (cr, 8, 9);
			cairo_line_to (cr, 14, 1);
			cairo_line_to (cr, 6, 7);
			cairo_line_to (cr, 5, 9);
			cairo_line_to (cr, 2, 6);
			cairo_close_path (cr);
		}

		murrine_set_color_rgba (cr, dot, trans);
		cairo_fill (cr);
	}
}

static void
murrine_draw_resize_grip (cairo_t *cr,
                          const MurrineColors        *colors,
                          const WidgetParameters     *widget,
                          const ResizeGripParameters *grip,
                          int x, int y, int width, int height)
{
	const MurrineRGB *dark = &colors->shade[3];
	const MurrineRGB *highlight = &colors->shade[0];
	int lx, ly;

	for (ly=0; ly<4; ly++) /* vertically, four rows of dots */
	{
		for (lx=0; lx<=ly; lx++) /* horizontally */
		{
			int ny = (3.5-ly)*3;
			int nx = lx*3;

			murrine_set_color_rgb (cr, highlight);
			cairo_rectangle (cr, x+width-nx-1, y+height-ny-1, 2, 2);
			cairo_fill (cr);

			murrine_set_color_rgb (cr, dark);
			cairo_rectangle (cr, x+width-nx-1, y+height-ny-1, 1, 1);
			cairo_fill (cr);
		}
	}
}

static void
murrine_draw_expander_arrow (cairo_t *cr,
	                     const MurrineColors    *colors,
	                     const MurrineRGB        custom_treeview_expander_color,
	                     const WidgetParameters *widget,
	                     const ExpanderParameters *expander,
	                     int x, int y)
{
	MurrineRGB color;
	cairo_pattern_t *pat;

	int diameter;
	int offset = (expander->arrowstyle == 2) ? 1 : 0;
	double vertical_overshoot;
	double radius;
	double interp; /* interpolation factor for center position */
	double x_double_horz, y_double_horz;
	double x_double_vert, y_double_vert;
	double x_double, y_double;
	gint degrees = 0;
	gint line_width = 1;

	switch (expander->expander_style)
	{
		case GTK_EXPANDER_COLLAPSED:
			degrees = (expander->text_direction == GTK_TEXT_DIR_RTL) ? 180 : 0;
			interp = 0.0;
			break;
		case GTK_EXPANDER_SEMI_COLLAPSED:
			degrees = (expander->text_direction == GTK_TEXT_DIR_RTL) ? 150 : 30;
			interp = 0.25;
			break;
		case GTK_EXPANDER_SEMI_EXPANDED:
			degrees = (expander->text_direction == GTK_TEXT_DIR_RTL) ? 120 : 60;
			interp = 0.75;
			break;
		case GTK_EXPANDER_EXPANDED:
			degrees = 90;
			interp = 1.0;
			break;
		default:
			g_assert_not_reached ();
	}

	/* Compute distance that the stroke extends beyonds the end
	* of the triangle we draw.
	*/
	vertical_overshoot = line_width / 2.0 * (1. / tan (G_PI / 8));

	/* For odd line widths, we end the vertical line of the triangle
	* at a half pixel, so we round differently.
	*/
	if (line_width % 2 == 1)
		vertical_overshoot = ceil (0.5 + vertical_overshoot) - 0.5;
	else
		vertical_overshoot = ceil (vertical_overshoot);

	/* Adjust the size of the triangle we draw so that the entire stroke fits
	*/
	diameter = MAX (3, expander->size - 2 * vertical_overshoot);

	/* If the line width is odd, we want the diameter to be even,
	* and vice versa, so force the sum to be odd. This relationship
	* makes the point of the triangle look right.
	*/
	diameter -= (1 - (diameter + line_width) % 2);

	radius = diameter / 2. + 4;

	/* Adjust the center so that the stroke is properly aligned with
	* the pixel grid. The center adjustment is different for the
	* horizontal and vertical orientations. For intermediate positions
	* we interpolate between the two.
	*/
	x_double_vert = floor (x - (radius + line_width) / 2.) + (radius + line_width) / 2. + ceil (radius / 8.0);
	y_double_vert = y - 0.5;

	x_double_horz = x - 0.5 + ceil (radius / 8.0);
	y_double_horz = floor (y - (radius + line_width) / 2.) + (radius + line_width) / 2.;

	x_double = x_double_vert * (1 - interp) + x_double_horz * interp;
	y_double = y_double_vert * (1 - interp) + y_double_horz * interp;

	cairo_translate (cr, x_double, y_double);
	cairo_rotate (cr, degrees * G_PI / 180);

	cairo_move_to (cr, -radius / 2.0 + offset, -radius / 2.0);
	cairo_line_to (cr,  radius / 2.0 - offset,  0);
	cairo_line_to (cr, -radius / 2.0 + offset,  radius / 2.0);
	cairo_close_path (cr);

	if (expander->in_treeview)
	{
                color = custom_treeview_expander_color;
	}
	else
	{
		color = colors->fg[widget->state_type];
	}

	pat = cairo_pattern_create_linear (-radius/2.0, 0, radius/2.0, 0);
	murrine_pattern_add_color_stop_rgba (pat, 0.0, &color, 0.6);
	murrine_pattern_add_color_stop_rgba (pat, 1.0, &color, 0.8);
	cairo_set_source (cr, pat);
	cairo_fill_preserve (cr);
	cairo_pattern_destroy (pat);

	murrine_set_color_rgb (cr, &color);
	cairo_stroke (cr);
}

static void
murrine_draw_expander_circle (cairo_t *cr,
	                      const MurrineColors    *colors,
	                      const WidgetParameters *widget,
	                      const ExpanderParameters *expander,
	                      int x, int y)
{
	int expander_size = expander->size;

	if (expander_size % 2 != 0)
		expander_size--;

	cairo_translate (cr, x-expander_size/2, y-expander_size/2);

	cairo_arc (cr, expander_size/2.0, expander_size/2.0, expander_size/2.0, 0, G_PI*2);

	if (expander->in_treeview)
		murrine_set_color_rgb (cr, &colors->text[widget->state_type]);
	else
		murrine_set_color_rgb (cr, &colors->fg[widget->state_type]);

	cairo_fill (cr);

	cairo_set_line_width (cr, 2.0);

	switch (expander->expander_style)
	{
		case GTK_EXPANDER_SEMI_COLLAPSED:
		case GTK_EXPANDER_COLLAPSED:
			cairo_move_to (cr, (double)expander_size/2-expander_size/4-0.5, (double)expander_size/2);
			cairo_line_to (cr, (double)expander_size/2+expander_size/4+0.5, (double)expander_size/2);
			cairo_move_to (cr, (double)expander_size/2, (double)expander_size/2-expander_size/4-0.5);
			cairo_line_to (cr, (double)expander_size/2, (double)expander_size/2+expander_size/4+0.5);
		break;
		case GTK_EXPANDER_SEMI_EXPANDED:
		case GTK_EXPANDER_EXPANDED:
			cairo_move_to (cr, (double)expander_size/2-expander_size/4-0.5, (double)expander_size/2);
			cairo_line_to (cr, (double)expander_size/2+expander_size/4+0.5, (double)expander_size/2);
		break;
		default:
			g_assert_not_reached ();
	}

	if (expander->in_treeview)
		murrine_set_color_rgb (cr, &colors->base[widget->state_type]);
	else
		murrine_set_color_rgb (cr, &colors->bg[widget->state_type]);
	cairo_stroke (cr);
}

static void
murrine_draw_expander_button (cairo_t *cr,
	                      const MurrineColors    *colors,
	                      const WidgetParameters *widget,
	                      const ExpanderParameters *expander,
	                      int x, int y)
{
	int expander_size = expander->size;

	if (expander_size % 2 == 0)
		expander_size--;

	cairo_translate (cr, x-expander_size/2, y-expander_size/2);

	cairo_save (cr);

	murrine_rounded_rectangle_closed (cr, 1, 1, expander_size-2, expander_size-2, widget->roundness-1, widget->corners);
	cairo_clip_preserve (cr);

	murrine_draw_glaze (cr, &colors->bg[widget->state_type],
	                    widget->glow_shade, widget->highlight_shade, widget->lightborder_shade,
	                    widget->mrn_gradient, widget, 1, 1, expander_size-2, expander_size-2,
	                    widget->roundness, widget->corners, TRUE);

	cairo_restore (cr);

	switch (expander->expander_style)
	{
		case GTK_EXPANDER_SEMI_COLLAPSED:
		case GTK_EXPANDER_COLLAPSED:
			cairo_move_to (cr, (double)expander_size/2-expander_size/4-0.5, (double)expander_size/2);
			cairo_line_to (cr, (double)expander_size/2+expander_size/4+0.5, (double)expander_size/2);
			cairo_move_to (cr, (double)expander_size/2, (double)expander_size/2-expander_size/4-0.5);
			cairo_line_to (cr, (double)expander_size/2, (double)expander_size/2+expander_size/4+0.5);
		break;
		case GTK_EXPANDER_SEMI_EXPANDED:
		case GTK_EXPANDER_EXPANDED:
			cairo_move_to (cr, (double)expander_size/2-expander_size/4-0.5, (double)expander_size/2);
			cairo_line_to (cr, (double)expander_size/2+expander_size/4+0.5, (double)expander_size/2);
		break;
		default:
			g_assert_not_reached ();
	}
	murrine_set_color_rgb  (cr, &colors->fg[widget->state_type]);
	cairo_stroke (cr);

	murrine_rounded_rectangle (cr, 0.5, 0.5, expander_size-1, expander_size-1, widget->roundness, widget->corners);
	murrine_set_color_rgb (cr, &colors->shade[4]);
	cairo_stroke (cr);
}

void 
murrine_draw_expander (cairo_t *cr,
                       const MurrineColors    *colors,
                       const MurrineRGB        treeview_expander_color,
                       const WidgetParameters *widget,
                       const ExpanderParameters *expander,
                       int x, int y)
{
	switch (expander->style)
	{
		default:
		case 0:
			murrine_draw_expander_arrow (cr, colors, treeview_expander_color, widget, expander, x, y);
			break;
		case 1:
			murrine_draw_expander_circle (cr, colors, widget, expander, x, y);
			break;
		case 2:
			murrine_draw_expander_button (cr, colors, widget, expander, x, y);
			break;
	}
}

static void
murrine_draw_focus_classic (cairo_t *cr,
                            const MurrineColors    *colors,
                            const WidgetParameters *widget,
                            const FocusParameters  *focus,
                            int x, int y, int width, int height)
{
	cairo_set_line_width (cr, focus->line_width);

	if (focus->has_color)
		murrine_set_color_rgb (cr, &focus->color);
	else if (focus->type == MRN_FOCUS_COLOR_WHEEL_LIGHT)
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	else if (focus->type == MRN_FOCUS_COLOR_WHEEL_DARK)
		cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
	else
		murrine_set_color_rgba (cr, &colors->fg[widget->state_type], 0.7);

	if (focus->dash_list[0])
	{
		gint n_dashes = strlen ((gchar *)focus->dash_list);
		gdouble *dashes = g_new (gdouble, n_dashes);
		gdouble total_length = 0;
		gdouble dash_offset;
		gint i;

		for (i = 0; i < n_dashes; i++)
		{
			dashes[i] = focus->dash_list[i];
			total_length += focus->dash_list[i];
		}

		dash_offset = -focus->line_width / 2.0;
		while (dash_offset < 0)
			dash_offset += total_length;

		cairo_set_dash (cr, dashes, n_dashes, dash_offset);
		g_free (dashes);
	}

	cairo_rectangle (cr, x+focus->line_width/2.0, y+focus->line_width/2.0,
	                     width-focus->line_width, height-focus->line_width);
	cairo_stroke (cr);
}

static void
murrine_draw_tab_focus (cairo_t *cr,
                        const MurrineColors    *colors,
                        const WidgetParameters *widget,
                        const FocusParameters  *focus,
                        int x, int y, int width, int height)
{

}

static void
murrine_draw_focus_border (cairo_t *cr,
                           const MurrineColors    *colors,
                           const WidgetParameters *widget,
                           const FocusParameters  *focus,
                           int x, int y, int width, int height)
{
	MurrineRGB fill = focus->color;

	/* Default values */
	int radius = 0;
	double xoffset = 1.0;
	double yoffset = 1.0;
	double border_alpha = 0.72;
	double fill_alpha = 0.18;
	double shadow_alpha = 0.36;
	boolean focus_fill = TRUE;
	boolean focus_border = TRUE;
	boolean focus_shadow = FALSE;

	/* Do some useful things to adjust focus */
	switch (focus->type)
	{
		case MRN_FOCUS_BUTTON_DEFAULT:
			xoffset = -(focus->padding)-2.0;
			yoffset = -(focus->padding)-2.0;
			radius = widget->roundness;
			focus_fill = FALSE;
			focus_shadow = TRUE;
			border_alpha = 0.2;
			shadow_alpha = 0.4;
			break;
		case MRN_FOCUS_BUTTON:
			xoffset = -(focus->padding)-2.0;
			yoffset = -(focus->padding)-2.0;
			radius = widget->roundness;
			focus_fill = FALSE;
			focus_shadow = TRUE;
			border_alpha = 0.8;
			shadow_alpha = 0.4;
			break;
		case MRN_FOCUS_BUTTON_FLAT:
			xoffset = -(focus->padding)-2.0;
			yoffset = -(focus->padding)-2.0;
			radius = widget->roundness;
			break;
		case MRN_FOCUS_LABEL:
			xoffset = 0.0;
			yoffset = 0.0;
			radius = widget->roundness;
			break;
		case MRN_FOCUS_TREEVIEW:
			xoffset = -1.0;
			yoffset = -1.0;
			focus_border = FALSE;
			break;
		case MRN_FOCUS_TREEVIEW_DND:
			radius = widget->roundness;
			break;
		case MRN_FOCUS_TREEVIEW_HEADER:
			cairo_translate (cr, -1, 0);
			radius = widget->roundness-1;
			break;
		case MRN_FOCUS_TREEVIEW_ROW:
			if (widget->state_type == GTK_STATE_SELECTED)
			{
				/* Fallback to classic function, dots */
				murrine_draw_focus_classic (cr, colors, widget, focus, x, y, width, height);
				return;
			}
			xoffset = 1.0;
			yoffset = 1.0;
			radius = widget->roundness;
			break;
		case MRN_FOCUS_TAB:
			xoffset = CLAMP (-(widget->xthickness)-1.0, -3.0, 0);
			yoffset = CLAMP (-(widget->ythickness)-1.0, -3.0, 0);
			radius = widget->roundness-1;			
			focus_border = FALSE;
			break;
		case MRN_FOCUS_SCALE:
			radius = widget->roundness;
			break;
		case MRN_FOCUS_ICONVIEW:
			return;
			break;
		case MRN_FOCUS_UNKNOWN:
			/* Fallback to classic function, dots */
			murrine_draw_focus_classic (cr, colors, widget, focus, x, y, width, height);
			return;
			break;
		default:
			break;
	};

	cairo_translate (cr, x, y);
	cairo_set_line_width (cr, focus->line_width);

	if (focus_fill)
	{
		clearlooks_rounded_rectangle (cr, xoffset, yoffset, width-(xoffset*2), height-(yoffset*2), radius, widget->corners);
		murrine_set_color_rgba (cr, &fill, fill_alpha);
		cairo_fill (cr);
	}

	if (focus_border)
	{
		clearlooks_rounded_rectangle (cr, xoffset+0.5, yoffset+0.5, width-(xoffset*2)-1, height-(yoffset*2)-1, radius, widget->corners);
		murrine_set_color_rgba (cr, &fill, border_alpha);
		cairo_stroke (cr);
	}
	
	if (focus_shadow)
	{
		clearlooks_rounded_rectangle (cr, xoffset-0.5, yoffset-0.5, width-(xoffset*2)+1, height-(yoffset*2)+1, radius+1, widget->corners);
		murrine_set_color_rgba (cr, &fill, shadow_alpha);
		cairo_stroke (cr);
	}

}

static void
murrine_draw_focus_inner (cairo_t *cr,
                          const MurrineColors    *colors,
                          const WidgetParameters *widget,
                          const FocusParameters  *focus,
                          int x, int y, int width, int height)
{
	MurrineRGB fill = focus->color;

	/* Default values */
	int radius = 0;
	double xoffset = 1.0;
	double yoffset = 1.0;
	double border_alpha = 0.72;
	double fill_alpha = 0.18;
	boolean focus_fill = TRUE;
	boolean focus_border = TRUE;

	/* Do some useful things to adjust focus */
	switch (focus->type)
	{
		case MRN_FOCUS_BUTTON_DEFAULT:
		case MRN_FOCUS_BUTTON:
			xoffset = -(focus->padding)+1.0;
			yoffset = -(focus->padding)+1.0;
			radius = widget->roundness-1;
			break;
		case MRN_FOCUS_BUTTON_FLAT:
			xoffset = -(focus->padding)+1.0;
			yoffset = -(focus->padding)+1.0;
			radius = widget->roundness-1;
			break;
		case MRN_FOCUS_LABEL:
			xoffset = 0.0;
			yoffset = 0.0;
			radius = widget->roundness;
			break;
		case MRN_FOCUS_TREEVIEW:
			xoffset = -1.0;
			yoffset = -1.0;
			focus_border = FALSE;
			break;
		case MRN_FOCUS_TREEVIEW_DND:
			radius = widget->roundness;
			break;
		case MRN_FOCUS_TREEVIEW_HEADER:
			cairo_translate (cr, -1, 0);
			radius = widget->roundness-1;
			break;
		case MRN_FOCUS_TREEVIEW_ROW:
			if (widget->state_type == GTK_STATE_SELECTED)
			{
				/* Fallback to classic function, dots */
				murrine_draw_focus_classic (cr, colors, widget, focus, x, y, width, height);
				return;
			}
			xoffset = 1.0;
			yoffset = 1.0;
			radius = widget->roundness;
			break;
		case MRN_FOCUS_TAB:
			xoffset = 0.0;
			yoffset = 0.0;
			radius = widget->roundness-1;
			break;
		case MRN_FOCUS_SCALE:
			radius = widget->roundness;
			break;
		case MRN_FOCUS_ICONVIEW:
			return;
			break;
		case MRN_FOCUS_UNKNOWN:
			/* Fallback to classic function, dots */
			murrine_draw_focus_classic (cr, colors, widget, focus, x, y, width, height);
			return;
			break;
		default:
			break;
	};

	cairo_translate (cr, x, y);
	cairo_set_line_width (cr, focus->line_width);

	if (focus_fill)
	{
		clearlooks_rounded_rectangle (cr, xoffset, yoffset, width-(xoffset*2), height-(yoffset*2), radius, widget->corners);
		murrine_set_color_rgba (cr, &fill, fill_alpha);
		cairo_fill (cr);
	}

	if (focus_border)
	{
		clearlooks_rounded_rectangle (cr, xoffset+0.5, yoffset+0.5, width-(xoffset*2)-1, height-(yoffset*2)-1, radius, widget->corners);
		murrine_set_color_rgba (cr, &fill, border_alpha);
		cairo_stroke (cr);
	}
}

void
murrine_draw_focus (cairo_t *cr,
                    const MurrineColors    *colors,
                    const WidgetParameters *widget,
                    const FocusParameters  *focus,
                    int x, int y, int width, int height)
{
	switch (focus->style)
	{
		default:
		case 1:
			murrine_draw_focus_classic (cr, colors, widget, focus, x, y, width, height);
			break;
		case 2:
			murrine_draw_focus_inner (cr, colors, widget, focus, x, y, width, height);
			break;
		case 3:
			murrine_draw_focus_border (cr, colors, widget, focus, x, y, width, height);
			break;
	}
}

void
murrine_register_style_murrine (MurrineStyleFunctions *functions)
{
	g_assert (functions);

	functions->draw_button             = murrine_draw_button;
	functions->draw_combobox           = murrine_draw_combobox;
	functions->draw_scale_trough       = murrine_draw_scale_trough;
	functions->draw_progressbar_trough = murrine_draw_progressbar_trough;
	functions->draw_progressbar_fill   = murrine_draw_progressbar_fill;
	functions->draw_entry              = murrine_draw_entry;
	functions->draw_entry_progress     = murrine_draw_entry_progress;
	functions->draw_search_entry       = murrine_draw_search_entry;
	functions->draw_spinbutton_entry   = murrine_draw_spinbutton_entry;
	functions->draw_expander           = murrine_draw_expander;
	functions->draw_slider             = murrine_draw_slider;
	functions->draw_slider_handle      = murrine_draw_slider_handle;
	functions->draw_spinbutton         = murrine_draw_spinbutton;
	functions->draw_spinbutton_down    = murrine_draw_spinbutton_down;
	functions->draw_optionmenu         = murrine_draw_optionmenu;
	functions->draw_combo_separator    = murrine_draw_combo_separator;
	functions->draw_menubar	           = murrine_draw_menubar;
	functions->draw_tab                = murrine_draw_tab;
	functions->draw_frame              = murrine_draw_frame;
	functions->draw_separator          = murrine_draw_separator;
	functions->draw_list_view_header   = murrine_draw_list_view_header;
	functions->draw_toolbar            = murrine_draw_toolbar;
	functions->draw_tooltip            = murrine_draw_tooltip;
	functions->draw_menuitem           = murrine_draw_menuitem;
	functions->draw_selected_cell      = murrine_draw_selected_cell;
	functions->draw_scrollbar_stepper  = murrine_draw_scrollbar_stepper;
	functions->draw_scrollbar_slider   = murrine_draw_scrollbar_slider;
	functions->draw_scrollbar_trough   = murrine_draw_scrollbar_trough;
	functions->draw_statusbar          = murrine_draw_statusbar;
	functions->draw_menu_frame         = murrine_draw_menu_frame;
	functions->draw_tooltip            = murrine_draw_tooltip;
	functions->draw_iconview           = murrine_draw_iconview;
	functions->draw_handle             = murrine_draw_handle;
	functions->draw_resize_grip        = murrine_draw_resize_grip;
	functions->draw_arrow              = murrine_draw_arrow;
	functions->draw_checkbox           = murrine_draw_checkbox;
	functions->draw_radiobutton        = murrine_draw_radiobutton;
	functions->draw_focus              = murrine_draw_focus;
}
