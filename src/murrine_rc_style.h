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

#include <gtk/gtkrc.h>

typedef struct _MurrineRcStyle MurrineRcStyle;
typedef struct _MurrineRcStyleClass MurrineRcStyleClass;

G_GNUC_INTERNAL extern GType murrine_type_rc_style;

#define MURRINE_TYPE_RC_STYLE              murrine_type_rc_style
#define MURRINE_RC_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), MURRINE_TYPE_RC_STYLE, MurrineRcStyle))
#define MURRINE_RC_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MURRINE_TYPE_RC_STYLE, MurrineRcStyleClass))
#define MURRINE_IS_RC_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), MURRINE_TYPE_RC_STYLE))
#define MURRINE_IS_RC_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), MURRINE_TYPE_RC_STYLE))
#define MURRINE_RC_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), MURRINE_TYPE_RC_STYLE, MurrineRcStyleClass))

typedef enum
{
	MRN_FLAG_ANIMATION = 1 << 0,
	MRN_FLAG_COLORIZE_SCROLLBAR = 1 << 1,
	MRN_FLAG_CONTRAST = 1 << 2,
	MRN_FLAG_GLAZESTYLE = 1 << 3,
	MRN_FLAG_GRADIENT_STOP_1 = 1 << 4,
	MRN_FLAG_GRADIENT_STOP_2 = 1 << 5,
	MRN_FLAG_GRADIENT_STOP_3 = 1 << 6,
	MRN_FLAG_GRADIENT_STOP_4 = 1 << 7,
	MRN_FLAG_GRADIENTS = 1 << 8,
	MRN_FLAG_HIGHLIGHT_RATIO = 1 << 9,
	MRN_FLAG_INNERBORDER_RATIO = 1 << 10,
	MRN_FLAG_LISTVIEWHEADERSTYLE = 1 << 11,
	MRN_FLAG_LISTVIEWSTYLE = 1 << 12,
	MRN_FLAG_MENUBARITEMSTYLE = 1 << 13,
	MRN_FLAG_MENUBARSTYLE = 1 << 14,
	MRN_FLAG_MENUITEMSTYLE = 1 << 15,
	MRN_FLAG_MENUSTYLE = 1 << 16,
	MRN_FLAG_RGBA = 1 << 17,
	MRN_FLAG_ROUNDNESS = 1 << 18,
	MRN_FLAG_SCROLLBAR_COLOR = 1 << 19,
	MRN_FLAG_SCROLLBARSTYLE = 1 << 20,
	MRN_FLAG_SLIDERSTYLE = 1 << 21,
	MRN_FLAG_STEPPERSTYLE = 1 << 22,
	MRN_FLAG_STYLE = 1 << 23,
	MRN_FLAG_TOOLBARSTYLE = 1 << 24
} MurrineRcFlags;

struct _MurrineRcStyle
{
	GtkRcStyle parent_instance;

	MurrineRcFlags flags;

	double   contrast;
	double   gradient_stop_1;
	double   gradient_stop_2;
	double   gradient_stop_3;
	double   gradient_stop_4;
	double   highlight_ratio;
	double   innerborder_ratio;

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

	MurrineStyles style;
};

struct _MurrineRcStyleClass
{
	GtkRcStyleClass parent_class;
};

G_GNUC_INTERNAL void murrine_rc_style_register_type (GTypeModule *module);
