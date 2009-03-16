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

#include <gtk/gtk.h>
#include <math.h>

#include "murrine_types.h"

G_GNUC_INTERNAL void murrine_shade (const MurrineRGB *a, float k, MurrineRGB *b);

G_GNUC_INTERNAL void murrine_mix_color (const MurrineRGB *color1, const MurrineRGB *color2,
                                        gdouble mix_factor, MurrineRGB *composite);

G_GNUC_INTERNAL void murrine_gdk_color_to_rgb (GdkColor *c,
                                               double *r, double *g, double *b);

G_GNUC_INTERNAL void murrine_get_parent_bg (const GtkWidget *widget,
                                            MurrineRGB *color);

G_GNUC_INTERNAL void murrine_set_color_rgb (cairo_t *cr,
                                            const MurrineRGB *color);

G_GNUC_INTERNAL void murrine_set_color_rgba (cairo_t *cr,
                                             const MurrineRGB *color,
                                             double alpha);

G_GNUC_INTERNAL void murrine_pattern_add_color_stop_rgb (cairo_pattern_t *pat, double pos,
                                                         const MurrineRGB *color);

G_GNUC_INTERNAL void murrine_pattern_add_color_stop_rgba (cairo_pattern_t *pat, double pos,
                                                          const MurrineRGB *color, double alpha);

G_GNUC_INTERNAL void rotate_mirror_translate (cairo_t *cr,
                                              double radius, double x, double y,
                                              boolean mirror_horizontally, boolean mirror_vertically);

G_GNUC_INTERNAL double get_decreased_shade (double old, double factor);

G_GNUC_INTERNAL double get_increased_shade (double old, double factor);

G_GNUC_INTERNAL double get_contrast (double old, double factor);

G_GNUC_INTERNAL MurrineGradients get_decreased_gradient_shades (MurrineGradients mrn_gradient, double factor);

G_GNUC_INTERNAL void murrine_exchange_axis (cairo_t  *cr,
                                            gint     *x,
                                            gint     *y,
                                            gint     *width,
                                            gint     *height);

G_GNUC_INTERNAL void murrine_rounded_corner (cairo_t *cr,
                                             double x, double y,
                                             int radius, uint8 corner);

G_GNUC_INTERNAL void clearlooks_rounded_rectangle (cairo_t *cr,
                                                   double x, double y, double w, double h,
                                                   int radius, uint8 corners);

G_GNUC_INTERNAL void murrine_rounded_rectangle (cairo_t *cr,
                                                double x, double y, double w, double h,
                                                int radius, uint8 corners);

G_GNUC_INTERNAL void murrine_rounded_rectangle_closed (cairo_t *cr,
                                                       double x, double y, double w, double h,
                                                       int radius, uint8 corners);

G_GNUC_INTERNAL void murrine_rounded_rectangle_fast (cairo_t *cr,
                                                     double x, double y, double w, double h,
                                                     uint8 corners);

G_GNUC_INTERNAL void murrine_set_gradient (cairo_t *cr,
                                           const MurrineRGB *color,
                                           MurrineGradients mrn_gradient,
                                           int x, int y, int width, int height,
                                           boolean gradients, boolean alpha);

G_GNUC_INTERNAL void murrine_draw_glaze (cairo_t *cr,
                                         const MurrineRGB *fill,
                                         double glow_shade,
                                         double highlight_shade,
                                         double lightborder_shade,
                                         MurrineGradients mrn_gradient,
                                         const WidgetParameters *widget,
                                         int x, int y, int width, int height,
                                         int radius, uint8 corners, boolean horizontal);

