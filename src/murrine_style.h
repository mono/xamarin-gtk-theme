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

#include <gtk/gtkstyle.h>

#ifndef MURRINE_STYLE_H
#define MURRINE_STYLE_H

#include "animation.h"
#include "murrine_types.h"

typedef struct _MurrineStyle MurrineStyle;
typedef struct _MurrineStyleClass MurrineStyleClass;

#define MURRINE_TYPE_STYLE              (murrine_style_get_type())
#define MURRINE_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), MURRINE_TYPE_STYLE, MurrineStyle))
#define MURRINE_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MURRINE_TYPE_STYLE, MurrineStyleClass))
#define MURRINE_IS_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), MURRINE_TYPE_STYLE))
#define MURRINE_IS_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), MURRINE_TYPE_STYLE))
#define MURRINE_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), MURRINE_TYPE_STYLE, MurrineStyleClass))

struct _MurrineStyle
{
	GtkStyle parent_instance;

	MurrineColors colors;

	double   border_shades[2];
	double   contrast;
	double   glow_shade;
	double   gradient_shades[4];
	double   highlight_shade;
	double   lightborder_shade;
	double   prelight_shade;
	double   shadow_shades[2];
	double   trough_shades[2];

	guint8   arrowstyle;
	guint8   cellstyle;
	guint8   comboboxstyle;
	guint8   glazestyle;
	guint8   glowstyle;
	guint8   lightborderstyle;
	guint8   listviewheaderstyle;
	guint8   listviewstyle;
	guint8   menubaritemstyle;
	guint8   menubarstyle;
	guint8   menuitemstyle;
	guint8   menustyle;
	guint8   progressbarstyle;
	guint8   reliefstyle;
	guint8   roundness;
	guint8   scrollbarstyle;
	guint8   separatorstyle;
	guint8   sliderstyle;
	guint8   spinbuttonstyle;
	guint8   stepperstyle;
	guint8   textstyle;
	guint8   toolbarstyle;

	gboolean animation;
	gboolean colorize_scrollbar;
	gboolean disable_focus;
	gboolean has_border_colors;
	gboolean has_focus_color;
	gboolean has_gradient_colors;
	gboolean rgba;

	GdkColor border_colors[2];
	GdkColor focus_color;
	GdkColor gradient_colors[4];
};

struct _MurrineStyleClass
{
	GtkStyleClass parent_class;

	MurrineStyleFunctions style_functions[MRN_NUM_DRAW_STYLES];
};

GType murrine_style_get_type (void);

#endif /* MURRINE_STYLE_H */
