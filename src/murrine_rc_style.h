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

#include <gtk/gtkrc.h>

#ifndef MURRINE_RC_STYLE_H
#define MURRINE_RC_STYLE_H

typedef struct _MurrineRcStyle MurrineRcStyle;
typedef struct _MurrineRcStyleClass MurrineRcStyleClass;

#define MURRINE_TYPE_RC_STYLE              (murrine_rc_style_get_type ())
#define MURRINE_RC_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), MURRINE_TYPE_RC_STYLE, MurrineRcStyle))
#define MURRINE_RC_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MURRINE_TYPE_RC_STYLE, MurrineRcStyleClass))
#define MURRINE_IS_RC_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), MURRINE_TYPE_RC_STYLE))
#define MURRINE_IS_RC_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), MURRINE_TYPE_RC_STYLE))
#define MURRINE_RC_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), MURRINE_TYPE_RC_STYLE, MurrineRcStyleClass))

typedef enum
{
	MRN_FLAG_ARROWSTYLE = 1 << 1,
	MRN_FLAG_CELLSTYLE = 1 << 2,
	MRN_FLAG_COMBOBOXSTYLE = 1 << 3,
	MRN_FLAG_FOCUS_COLOR = 1 << 4,
	MRN_FLAG_EXPANDERSTYLE = 1 << 5,
	MRN_FLAG_GLAZESTYLE = 1 << 6,
	MRN_FLAG_GLOW_SHADE = 1 << 7,
	MRN_FLAG_GLOWSTYLE = 1 << 8,
	MRN_FLAG_HIGHLIGHT_SHADE = 1 << 9,
	MRN_FLAG_LIGHTBORDER_SHADE = 1 << 10,
	MRN_FLAG_LIGHTBORDERSTYLE= 1 << 11,
	MRN_FLAG_LISTVIEWHEADERSTYLE = 1 << 12,
	MRN_FLAG_LISTVIEWSTYLE = 1 << 13,
	MRN_FLAG_MENUBARITEMSTYLE = 1 << 14,
	MRN_FLAG_MENUBARSTYLE = 1 << 15,
	MRN_FLAG_MENUITEMSTYLE = 1 << 16,
	MRN_FLAG_MENUSTYLE = 1 << 17,
	MRN_FLAG_PRELIGHT_SHADE = 1 << 18,
	MRN_FLAG_PROGRESSBARSTYLE = 1 << 19,
	MRN_FLAG_RELIEFSTYLE = 1 << 20,
	MRN_FLAG_SCROLLBARSTYLE = 1 << 21,
	MRN_FLAG_SEPARATORSTYLE = 1 << 22,
	MRN_FLAG_SLIDERSTYLE = 1 << 23,
	MRN_FLAG_SPINBUTTONSTYLE = 1 << 24,
	MRN_FLAG_STEPPERSTYLE = 1 << 25,
	MRN_FLAG_TEXTSTYLE = 1 << 26,
	MRN_FLAG_TEXT_SHADE = 1 << 27,
	MRN_FLAG_TOOLBARSTYLE = 1 << 28
} MurrineRcFlags;

typedef enum
{
	MRN_FLAG_ANIMATION = 1 << 0,
	MRN_FLAG_COLORIZE_SCROLLBAR = 1 << 1,
	MRN_FLAG_CONTRAST = 1 << 2,
	MRN_FLAG_DISABLE_FOCUS = 1 << 3,
	MRN_FLAG_RGBA = 1 << 4,
	MRN_FLAG_ROUNDNESS = 1 << 5
} MurrineRcBasicFlags;

typedef enum
{
	MRN_FLAG_BORDER_COLORS = 1 << 1,
	MRN_FLAG_BORDER_SHADES = 1 << 2,
	MRN_FLAG_GRADIENT_COLORS = 1 << 3,
	MRN_FLAG_GRADIENT_SHADES = 1 << 4,
	MRN_FLAG_SHADOW_SHADES = 1 << 5,
	MRN_FLAG_TROUGH_SHADES = 1 << 6
} MurrineRcGradientFlags;

struct _MurrineRcStyle
{
	GtkRcStyle parent_instance;

	MurrineRcFlags flags;
	MurrineRcBasicFlags bflags;
	MurrineRcGradientFlags gflags;

	double   border_shades[2];
	double   contrast;
	double   glow_shade;
	double   gradient_shades[4];
	double   highlight_shade;
	double   lightborder_shade;
	double   prelight_shade;
	double   shadow_shades[2];
	double   text_shade;
	double   trough_shades[2];

	guint8   arrowstyle;
	guint8   cellstyle;
	guint8   comboboxstyle;
	guint8   expanderstyle;
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

struct _MurrineRcStyleClass
{
	GtkRcStyleClass parent_class;
};

GType murrine_rc_style_get_type	(void);

#endif /* MURRINE_RC_STYLE_H */
