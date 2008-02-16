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

#include <gtk/gtkstyle.h>

#ifndef MURRINE_STYLE_H
#define MURRINE_STYLE_H

#include "animation.h"
#include "murrine_types.h"

typedef struct _MurrineStyle MurrineStyle;
typedef struct _MurrineStyleClass MurrineStyleClass;

G_GNUC_INTERNAL extern GType murrine_type_style;

#define MURRINE_TYPE_STYLE              murrine_type_style
#define MURRINE_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), MURRINE_TYPE_STYLE, MurrineStyle))
#define MURRINE_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MURRINE_TYPE_STYLE, MurrineStyleClass))
#define MURRINE_IS_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), MURRINE_TYPE_STYLE))
#define MURRINE_IS_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), MURRINE_TYPE_STYLE))
#define MURRINE_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), MURRINE_TYPE_STYLE, MurrineStyleClass))

struct _MurrineStyle
{
	GtkStyle parent_instance;

	MurrineColors colors;

	MurrineStyles style;

	double   contrast;
	double   gradient_stop_1;
	double   gradient_stop_2;
	double   gradient_stop_3;
	double   gradient_stop_4;
	double   highlight_ratio;
	double   innerborder_ratio;
	double   opacity_ratio;

	guint8   glazestyle;
	guint8   listviewheaderstyle;
	guint8   listviewstyle;
	guint8   menubaritemstyle;
	guint8   menubarstyle;
	guint8   menuitemstyle;
	guint8   menustyle;
	guint8   roundness;
	guint8   scrollbarstyle;
	guint8   sliderstyle;
	guint8   stepperstyle;
	guint8   toolbarstyle;

	gboolean animation;
	gboolean gradients;
	gboolean colorize_scrollbar;
	gboolean has_gradient_stop;
	gboolean has_scrollbar_color;
	gboolean rgba;

	GdkColor scrollbar_color;
};

struct _MurrineStyleClass
{
	GtkStyleClass parent_class;

	MurrineStyleFunctions style_functions[MRN_NUM_DRAW_STYLES];
};

G_GNUC_INTERNAL void murrine_style_register_type (GTypeModule *module);

#endif /* MURRINE_STYLE_H */
