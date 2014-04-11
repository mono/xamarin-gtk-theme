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

#include "murrine_style.h"
#include "murrine_rc_style.h"

#include "animation.h"

#ifdef HAVE_ANIMATION
static void murrine_rc_style_finalize (GObject *object);
#endif
static GtkStyle *murrine_rc_style_create_style (GtkRcStyle *rc_style);
static guint murrine_rc_style_parse (GtkRcStyle  *rc_style,
                                     GtkSettings *settings,
                                     GScanner    *scanner);
static void murrine_rc_style_merge (GtkRcStyle *dest,
                                    GtkRcStyle *src);

enum
{
	TOKEN_ANIMATION = G_TOKEN_LAST + 1,
	TOKEN_ARROWSTYLE,
	TOKEN_BORDER_COLORS,
	TOKEN_BORDER_SHADES,
	TOKEN_COLORIZE_SCROLLBAR,
	TOKEN_CELLSTYLE,
	TOKEN_COMBOBOXSTYLE,
	TOKEN_CONTRAST,
	TOKEN_DEFAULT_BUTTON_COLOR,
	TOKEN_EXPANDERSTYLE,
	TOKEN_FOCUS_COLOR,
	TOKEN_FOCUSSTYLE,
	TOKEN_GLAZESTYLE,
	TOKEN_GLOW_SHADE,
	TOKEN_GLOWSTYLE,
	TOKEN_GRADIENT_COLORS,
	TOKEN_GRADIENT_SHADES,
	TOKEN_HANDLESTYLE,
	TOKEN_HIGHLIGHT_SHADE,
	TOKEN_LIGHTBORDER_SHADE,
	TOKEN_LIGHTBORDERSTYLE,
	TOKEN_LISTVIEWHEADERSTYLE,
	TOKEN_LISTVIEWSTYLE,
	TOKEN_MENUBARITEMSTYLE,
	TOKEN_MENUBARSTYLE,
	TOKEN_MENUITEMSTYLE,
	TOKEN_MENUSTYLE,
	TOKEN_PRELIGHT_SHADE,
	TOKEN_PROGRESSBARSTYLE,
	TOKEN_RELIEFSTYLE,
	TOKEN_RGBA,
	TOKEN_ROUNDNESS,
	TOKEN_SCROLLBARSTYLE,
	TOKEN_SEPARATORSTYLE,
	TOKEN_SHADOW_SHADES,
	TOKEN_SLIDERSTYLE,
	TOKEN_SPINBUTTONSTYLE,
	TOKEN_STEPPERSTYLE,
	TOKEN_TEXTSTYLE,
	TOKEN_TEXT_SHADE,
	TOKEN_TOOLBARSTYLE,
	TOKEN_TREEVIEW_EXPANDER_COLOR,
	TOKEN_TROUGH_BORDER_SHADES,
	TOKEN_TROUGH_SHADES,	

	TOKEN_TRUE,
	TOKEN_FALSE,

	/* stuff to ignore */
	TOKEN_GRADIENTS,
	TOKEN_HILIGHT_RATIO,
	TOKEN_HIGHLIGHT_RATIO,
	TOKEN_LIGHTBORDER_RATIO,
	TOKEN_PROFILE,
	TOKEN_SCROLLBAR_COLOR,
	TOKEN_SQUAREDSTYLE,
	TOKEN_STYLE
};

static struct
{
	const gchar *name;
	guint        token;
}
theme_symbols[] =
{
	{ "animation",           TOKEN_ANIMATION },
	{ "arrowstyle",          TOKEN_ARROWSTYLE },
	{ "border_colors",       TOKEN_BORDER_COLORS },
	{ "border_shades",       TOKEN_BORDER_SHADES },
	{ "colorize_scrollbar",  TOKEN_COLORIZE_SCROLLBAR },
	{ "cellstyle",           TOKEN_CELLSTYLE },	
	{ "comboboxstyle",       TOKEN_COMBOBOXSTYLE },
	{ "contrast",            TOKEN_CONTRAST },
	{ "default_button_color", TOKEN_DEFAULT_BUTTON_COLOR },
	{ "expanderstyle",       TOKEN_EXPANDERSTYLE },
	{ "focus_color",         TOKEN_FOCUS_COLOR },
	{ "focusstyle",          TOKEN_FOCUSSTYLE },
	{ "glazestyle",          TOKEN_GLAZESTYLE },
	{ "glow_shade",          TOKEN_GLOW_SHADE },
	{ "glowstyle",           TOKEN_GLOWSTYLE },
	{ "gradient_colors",     TOKEN_GRADIENT_COLORS },
	{ "gradient_shades",     TOKEN_GRADIENT_SHADES },
	{ "handlestyle",         TOKEN_HANDLESTYLE },
	{ "highlight_shade",     TOKEN_HIGHLIGHT_SHADE },
	{ "lightborder_shade",   TOKEN_LIGHTBORDER_SHADE },
	{ "lightborderstyle",    TOKEN_LIGHTBORDERSTYLE },
	{ "listviewheaderstyle", TOKEN_LISTVIEWHEADERSTYLE },
	{ "listviewstyle",       TOKEN_LISTVIEWSTYLE },
	{ "menubaritemstyle",    TOKEN_MENUBARITEMSTYLE },
	{ "menubarstyle",        TOKEN_MENUBARSTYLE },
	{ "menuitemstyle",       TOKEN_MENUITEMSTYLE },
	{ "menustyle",           TOKEN_MENUSTYLE },
	{ "prelight_shade",      TOKEN_PRELIGHT_SHADE },
	{ "progressbarstyle",    TOKEN_PROGRESSBARSTYLE },
	{ "reliefstyle",         TOKEN_RELIEFSTYLE },
	{ "rgba",                TOKEN_RGBA },
	{ "roundness",           TOKEN_ROUNDNESS },
	{ "scrollbarstyle",      TOKEN_SCROLLBARSTYLE },
	{ "separatorstyle",      TOKEN_SEPARATORSTYLE },
	{ "shadow_shades",       TOKEN_SHADOW_SHADES },
	{ "sliderstyle",         TOKEN_SLIDERSTYLE },
	{ "spinbuttonstyle",     TOKEN_SPINBUTTONSTYLE },
	{ "stepperstyle",        TOKEN_STEPPERSTYLE },
	{ "textstyle",           TOKEN_TEXTSTYLE },
	{ "text_shade",          TOKEN_TEXT_SHADE },
	{ "toolbarstyle",        TOKEN_TOOLBARSTYLE },
	{ "treeview_expander_color", TOKEN_TREEVIEW_EXPANDER_COLOR },
	{ "trough_border_shades", TOKEN_TROUGH_BORDER_SHADES },
	{ "trough_shades",       TOKEN_TROUGH_SHADES },

	{ "TRUE",                TOKEN_TRUE },
	{ "FALSE",               TOKEN_FALSE },

	/* stuff to ignore */
	{ "gradients",           TOKEN_GRADIENTS },
	{ "hilight_ratio",       TOKEN_HILIGHT_RATIO },
	{ "highlight_ratio",     TOKEN_HIGHLIGHT_RATIO },
	{ "lightborder_ratio",   TOKEN_LIGHTBORDER_RATIO },
	{ "profile",             TOKEN_PROFILE },
	{ "scrollbar_color",     TOKEN_SCROLLBAR_COLOR },
	{ "squaredstyle",        TOKEN_SQUAREDSTYLE },
	{ "style",               TOKEN_STYLE }
};

G_DEFINE_DYNAMIC_TYPE (XamarinRcStyle, murrine_rc_style, GTK_TYPE_RC_STYLE)

void
murrine_rc_style_register_types (GTypeModule *module)
{
	murrine_rc_style_register_type (module);
}

static void
murrine_rc_style_init (MurrineRcStyle *murrine_rc)
{
	murrine_rc->flags = 0;

	murrine_rc->animation = FALSE;
	murrine_rc->arrowstyle = 0;
	murrine_rc->border_shades[0] = 1.0;
	murrine_rc->border_shades[1] = 1.0;
	murrine_rc->cellstyle = 1;
	murrine_rc->colorize_scrollbar = TRUE;
	murrine_rc->comboboxstyle = 0;
	murrine_rc->contrast = 1.0;
	murrine_rc->expanderstyle = 0;
	murrine_rc->focusstyle = 2;
	murrine_rc->has_border_colors = FALSE;
	murrine_rc->has_default_button_color = FALSE;
	murrine_rc->has_gradient_colors = FALSE;
	murrine_rc->has_treeview_expander_color = FALSE;
	murrine_rc->handlestyle = 0;
	murrine_rc->glazestyle = 1;
	murrine_rc->glow_shade = 1.0;
	murrine_rc->glowstyle = 0;
	murrine_rc->gradient_shades[0] = 1.1;
	murrine_rc->gradient_shades[1] = 1.0;
	murrine_rc->gradient_shades[2] = 1.0;
	murrine_rc->gradient_shades[3] = 1.1;
	murrine_rc->highlight_shade = 1.1;
	murrine_rc->lightborder_shade = 1.1;
	murrine_rc->lightborderstyle = 0;
	murrine_rc->listviewheaderstyle = 1;
	murrine_rc->listviewstyle = 0;
	murrine_rc->menubaritemstyle = 0;
	murrine_rc->menubarstyle = 0;
	murrine_rc->menuitemstyle = 1;
	murrine_rc->menustyle = 1;
	murrine_rc->prelight_shade = 1.04;
	murrine_rc->progressbarstyle = 1;
	murrine_rc->reliefstyle = 2;
	murrine_rc->rgba = FALSE;
	murrine_rc->roundness = 1;
	murrine_rc->scrollbarstyle = 0;
	murrine_rc->separatorstyle = 0;
	murrine_rc->shadow_shades[0] = 1.0;
	murrine_rc->shadow_shades[1] = 1.0;
	murrine_rc->sliderstyle = 0;
	murrine_rc->spinbuttonstyle = 0;
	murrine_rc->stepperstyle = 0;
	murrine_rc->textstyle = 0;
	murrine_rc->text_shade = 1.12;
	murrine_rc->toolbarstyle = 0;
	murrine_rc->trough_border_shades[0] = 1.0;
	murrine_rc->trough_border_shades[1] = 1.0;
	murrine_rc->trough_shades[0] = 1.0;
	murrine_rc->trough_shades[1] = 1.0;
}

#ifdef HAVE_ANIMATION
static void
murrine_rc_style_finalize (GObject *object)
{
	/* cleanup all the animation stuff */
	murrine_animation_cleanup ();

	if (G_OBJECT_CLASS (murrine_rc_style_parent_class)->finalize != NULL)
		G_OBJECT_CLASS (murrine_rc_style_parent_class)->finalize(object);
}
#endif


static void
murrine_rc_style_class_init (MurrineRcStyleClass *klass)
{
	GtkRcStyleClass *rc_style_class = GTK_RC_STYLE_CLASS (klass);
#ifdef HAVE_ANIMATION
	GObjectClass    *g_object_class = G_OBJECT_CLASS (klass);
#endif

	rc_style_class->parse = murrine_rc_style_parse;
	rc_style_class->create_style = murrine_rc_style_create_style;
	rc_style_class->merge = murrine_rc_style_merge;

#ifdef HAVE_ANIMATION
	g_object_class->finalize = murrine_rc_style_finalize;
#endif
}

static void
murrine_rc_style_class_finalize (MurrineRcStyleClass *klass)
{
}

static guint
theme_parse_boolean (GtkSettings *settings,
                     GScanner     *scanner,
                     gboolean     *retval)
{
	guint token;

	/* Skip 'ANIMATION' */
	token = g_scanner_get_next_token(scanner);

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_EQUAL_SIGN)
		return G_TOKEN_EQUAL_SIGN;

	token = g_scanner_get_next_token(scanner);
	if (token == TOKEN_TRUE)
		*retval = TRUE;
	else if (token == TOKEN_FALSE)
		*retval = FALSE;
	else
		return TOKEN_TRUE;

	return G_TOKEN_NONE;
}

static guint
theme_parse_color (GtkSettings  *settings,
                   GScanner     *scanner,
                   GtkRcStyle   *style,
                   GdkColor     *color)
{
	guint token;

	/* Skip 'blah_color' */
	token = g_scanner_get_next_token(scanner);

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_EQUAL_SIGN)
		return G_TOKEN_EQUAL_SIGN;

	return gtk_rc_parse_color_full (scanner, style, color);
}

static guint
murrine_gtk2_rc_parse_dummy_color (GtkSettings      *settings,
                                   GScanner         *scanner,
                                   gchar            *name,
                                   GtkRcStyle       *style,
                                   GdkColor         *color)
{
	guint token;

	/* Skip option */
	token = g_scanner_get_next_token (scanner);

	/* print a warning. Isn't there a way to get the string from the scanner? */
	g_scanner_warn (scanner, "Murrine configuration option \"%s\" is no longer supported and will be ignored.", name);

	/* equal sign */
	/* Skip 'blah_color' */
	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_EQUAL_SIGN)
		return G_TOKEN_EQUAL_SIGN;

	return  gtk_rc_parse_color_full (scanner, style, color);
}

static guint
theme_parse_shade (GtkSettings  *settings,
                   GScanner     *scanner,
                   double       *ratio)
{
	guint token;

	/* Skip 'ratio' */
	token = g_scanner_get_next_token(scanner);

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_EQUAL_SIGN)
		return G_TOKEN_EQUAL_SIGN;

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_FLOAT)
		return G_TOKEN_FLOAT;

	*ratio = scanner->value.v_float;

	return G_TOKEN_NONE;
}

static guint
theme_parse_int (GtkSettings  *settings,
                 GScanner     *scanner,
                 guint8       *style)
{
	guint token;

	/* Skip '*style' */
	token = g_scanner_get_next_token(scanner);

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_EQUAL_SIGN)
		return G_TOKEN_EQUAL_SIGN;

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_INT)
		return G_TOKEN_INT;

	*style = scanner->value.v_int;

	return G_TOKEN_NONE;
}

static guint
theme_parse_gradient (GtkSettings  *settings,
                      GScanner     *scanner,
                      double       gradient_shades[4])
{
	guint               token;

	/* Skip 'blah_border' */
	token = g_scanner_get_next_token(scanner);

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_EQUAL_SIGN)
		return G_TOKEN_EQUAL_SIGN;

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_LEFT_CURLY)
		return G_TOKEN_LEFT_CURLY;

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_FLOAT)
		return G_TOKEN_FLOAT;
	gradient_shades[0] = scanner->value.v_float;
	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_COMMA)
		return G_TOKEN_COMMA;

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_FLOAT)
		return G_TOKEN_FLOAT;
	gradient_shades[1] = scanner->value.v_float;
	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_COMMA)
		return G_TOKEN_COMMA;

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_FLOAT)
		return G_TOKEN_FLOAT;
	gradient_shades[2] = scanner->value.v_float;
	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_COMMA)
		return G_TOKEN_COMMA;

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_FLOAT)
		return G_TOKEN_FLOAT;
	gradient_shades[3] = scanner->value.v_float;

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_RIGHT_CURLY)
		return G_TOKEN_RIGHT_CURLY;

	/* save those values */

	return G_TOKEN_NONE;
}

static guint
theme_parse_gradient_colors (GtkSettings  *settings,
                             GScanner     *scanner,
                             GtkRcStyle   *style,
                             gboolean     *retval,
                             GdkColor     gradient_colors[4])
{
	guint token;
	*retval = TRUE;

	/* Skip 'blah_border' */
	token = g_scanner_get_next_token(scanner);

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_EQUAL_SIGN)
		return G_TOKEN_EQUAL_SIGN;

	token = g_scanner_get_next_token(scanner);
	if (token == TOKEN_FALSE)
	{
		*retval = FALSE;
		return G_TOKEN_NONE;
	}
	else if (token != G_TOKEN_LEFT_CURLY)
		return G_TOKEN_LEFT_CURLY;

	gtk_rc_parse_color_full (scanner, style, &gradient_colors[0]);
	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_COMMA)
		return G_TOKEN_COMMA;

	gtk_rc_parse_color_full (scanner, style, &gradient_colors[1]);
	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_COMMA)
		return G_TOKEN_COMMA;

	gtk_rc_parse_color_full (scanner, style, &gradient_colors[2]);
	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_COMMA)
		return G_TOKEN_COMMA;

	gtk_rc_parse_color_full (scanner, style, &gradient_colors[3]);

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_RIGHT_CURLY)
		return G_TOKEN_RIGHT_CURLY;

	/* save those values */

	return G_TOKEN_NONE;
}

static guint
theme_parse_border_colors (GtkSettings  *settings,
                             GScanner     *scanner,
                             GtkRcStyle   *style,
                             gboolean     *retval,
                             GdkColor     border_colors[2])
{
	guint token;
	*retval = TRUE;

	/* Skip 'blah_border' */
	token = g_scanner_get_next_token(scanner);

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_EQUAL_SIGN)
		return G_TOKEN_EQUAL_SIGN;

	token = g_scanner_get_next_token(scanner);
	if (token == TOKEN_FALSE)
	{
		*retval = FALSE;
		return G_TOKEN_NONE;
	}
	else if (token != G_TOKEN_LEFT_CURLY)
		return G_TOKEN_LEFT_CURLY;

	gtk_rc_parse_color_full (scanner, style, &border_colors[0]);
	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_COMMA)
		return G_TOKEN_COMMA;

	gtk_rc_parse_color_full (scanner, style, &border_colors[1]);

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_RIGHT_CURLY)
		return G_TOKEN_RIGHT_CURLY;

	/* save those values */

	return G_TOKEN_NONE;
}

static guint
theme_parse_border (GtkSettings  *settings,
                    GScanner     *scanner,
                    double       border_shades[2])
{
	guint               token;

	/* Skip 'blah_border' */
	token = g_scanner_get_next_token(scanner);

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_EQUAL_SIGN)
		return G_TOKEN_EQUAL_SIGN;

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_LEFT_CURLY)
		return G_TOKEN_LEFT_CURLY;

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_FLOAT)
		return G_TOKEN_FLOAT;
	border_shades[0] = scanner->value.v_float;
	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_COMMA)
		return G_TOKEN_COMMA;

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_FLOAT)
		return G_TOKEN_FLOAT;
	border_shades[1] = scanner->value.v_float;

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_RIGHT_CURLY)
		return G_TOKEN_RIGHT_CURLY;

	/* save those values */

	return G_TOKEN_NONE;
}

static guint
murrine_gtk2_rc_parse_dummy (GtkSettings      *settings,
                             GScanner         *scanner,
                             gchar            *name)
{
	guint token;

	/* Skip option */
	token = g_scanner_get_next_token (scanner);

	/* print a warning. Isn't there a way to get the string from the scanner? */
	g_scanner_warn (scanner, "Murrine configuration option \"%s\" is no longer supported and will be ignored.", name);

	/* equal sign */
	token = g_scanner_get_next_token (scanner);
	if (token != G_TOKEN_EQUAL_SIGN)
		return G_TOKEN_EQUAL_SIGN;

	/* eat whatever comes next */
	token = g_scanner_get_next_token (scanner);

	return G_TOKEN_NONE;
}

static guint
murrine_rc_style_parse (GtkRcStyle *rc_style,
                        GtkSettings  *settings,
                        GScanner   *scanner)
{
	static GQuark scope_id = 0;
	MurrineRcStyle *murrine_style = MURRINE_RC_STYLE (rc_style);

	guint old_scope;
	guint token;
	guint i;

	/* Set up a new scope in this scanner. */

	if (!scope_id)
		scope_id = g_quark_from_string("murrine_theme_engine");

	/* If we bail out due to errors, we *don't* reset the scope, so the
	* error messaging code can make sense of our tokens.
	*/
	old_scope = g_scanner_set_scope(scanner, scope_id);

	/* Now check if we already added our symbols to this scope
	* (in some previous call to murrine_rc_style_parse for the
	* same scanner.
	*/

	if (!g_scanner_lookup_symbol(scanner, theme_symbols[0].name))
	{
		g_scanner_freeze_symbol_table(scanner);
		for (i = 0; i < G_N_ELEMENTS (theme_symbols); i++)
			g_scanner_scope_add_symbol(scanner, scope_id, theme_symbols[i].name, GINT_TO_POINTER(theme_symbols[i].token));
		g_scanner_thaw_symbol_table(scanner);
	}

	/* We're ready to go, now parse the top level */
	token = g_scanner_peek_next_token(scanner);
	while (token != G_TOKEN_RIGHT_CURLY)
	{
		switch (token)
		{
			case TOKEN_ANIMATION:
				token = theme_parse_boolean (settings, scanner, &murrine_style->animation);
				murrine_style->bflags |= MRN_FLAG_ANIMATION;
				break;
			case TOKEN_ARROWSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->arrowstyle);
				murrine_style->flags |= MRN_FLAG_ARROWSTYLE;
				break;
			case TOKEN_BORDER_COLORS:
				token = theme_parse_border_colors (settings, scanner, rc_style, &murrine_style->has_border_colors, murrine_style->border_colors);
				murrine_style->gflags |= MRN_FLAG_BORDER_COLORS;
				break;
			case TOKEN_BORDER_SHADES:
				token = theme_parse_border (settings, scanner, murrine_style->border_shades);
				murrine_style->gflags |= MRN_FLAG_BORDER_SHADES;
				break;
			case TOKEN_CELLSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->cellstyle);
				murrine_style->flags |= MRN_FLAG_CELLSTYLE;
				break;
			case TOKEN_COLORIZE_SCROLLBAR:
				token = theme_parse_boolean (settings, scanner, &murrine_style->colorize_scrollbar);
				murrine_style->bflags |= MRN_FLAG_COLORIZE_SCROLLBAR;
				break;
			case TOKEN_COMBOBOXSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->comboboxstyle);
				murrine_style->flags |= MRN_FLAG_COMBOBOXSTYLE;
				break;
			case TOKEN_CONTRAST:
				token = theme_parse_shade (settings, scanner, &murrine_style->contrast);
				murrine_style->bflags |= MRN_FLAG_CONTRAST;
				break;
			case TOKEN_DEFAULT_BUTTON_COLOR:
				token = theme_parse_color (settings, scanner, rc_style, &murrine_style->default_button_color);
				murrine_style->flags |= MRN_FLAG_DEFAULT_BUTTON_COLOR;
				break;
			case TOKEN_EXPANDERSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->expanderstyle);
				murrine_style->flags |= MRN_FLAG_EXPANDERSTYLE;
				break;
			case TOKEN_FOCUS_COLOR:
				token = theme_parse_color (settings, scanner, rc_style, &murrine_style->focus_color);
				murrine_style->flags |= MRN_FLAG_FOCUS_COLOR;
				break;
			case TOKEN_FOCUSSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->focusstyle);
				murrine_style->flags |= MRN_FLAG_FOCUSSTYLE;
				break;
			case TOKEN_GLAZESTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->glazestyle);
				murrine_style->flags |= MRN_FLAG_GLAZESTYLE;
				break;
			case TOKEN_GLOW_SHADE:
				token = theme_parse_shade (settings, scanner, &murrine_style->glow_shade);
				murrine_style->flags |= MRN_FLAG_GLOW_SHADE;
				break;
			case TOKEN_GLOWSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->glowstyle);
				murrine_style->flags |= MRN_FLAG_GLOWSTYLE;
				break;
			case TOKEN_GRADIENT_COLORS:
				token = theme_parse_gradient_colors (settings, scanner, rc_style, &murrine_style->has_gradient_colors, murrine_style->gradient_colors);
				murrine_style->gflags |= MRN_FLAG_GRADIENT_COLORS;
				break;
			case TOKEN_GRADIENT_SHADES:
				token = theme_parse_gradient (settings, scanner, murrine_style->gradient_shades);
				murrine_style->gflags |= MRN_FLAG_GRADIENT_SHADES;
				break;
			case TOKEN_HANDLESTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->handlestyle);
				murrine_style->flags |= MRN_FLAG_HANDLESTYLE;
				break;
			case TOKEN_HIGHLIGHT_SHADE:
				token = theme_parse_shade (settings, scanner, &murrine_style->highlight_shade);
				murrine_style->flags |= MRN_FLAG_HIGHLIGHT_SHADE;
				break;
			case TOKEN_LIGHTBORDER_SHADE:
				token = theme_parse_shade (settings, scanner, &murrine_style->lightborder_shade);
				murrine_style->flags |= MRN_FLAG_LIGHTBORDER_SHADE;
				break;
			case TOKEN_LIGHTBORDERSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->lightborderstyle);
				murrine_style->flags |= MRN_FLAG_LIGHTBORDERSTYLE;
				break;
			case TOKEN_LISTVIEWHEADERSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->listviewheaderstyle);
				murrine_style->flags |= MRN_FLAG_LISTVIEWHEADERSTYLE;
				break;
			case TOKEN_LISTVIEWSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->listviewstyle);
				murrine_style->flags |= MRN_FLAG_LISTVIEWSTYLE;
				break;
			case TOKEN_MENUBARITEMSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->menubaritemstyle);
				murrine_style->flags |= MRN_FLAG_MENUBARITEMSTYLE;
				break;
			case TOKEN_MENUBARSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->menubarstyle);
				murrine_style->flags |= MRN_FLAG_MENUBARSTYLE;
				break;
			case TOKEN_MENUITEMSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->menuitemstyle);
				murrine_style->flags |= MRN_FLAG_MENUITEMSTYLE;
				break;
			case TOKEN_MENUSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->menustyle);
				murrine_style->flags |= MRN_FLAG_MENUSTYLE;
				break;
			case TOKEN_PRELIGHT_SHADE:
				token = theme_parse_shade (settings, scanner, &murrine_style->prelight_shade);
				murrine_style->flags |= MRN_FLAG_PRELIGHT_SHADE;
				break;
			case TOKEN_PROGRESSBARSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->progressbarstyle);
				murrine_style->flags |= MRN_FLAG_PROGRESSBARSTYLE;
				break;
			case TOKEN_RELIEFSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->reliefstyle);
				murrine_style->flags |= MRN_FLAG_RELIEFSTYLE;
				break;
			case TOKEN_RGBA:
				token = theme_parse_boolean (settings, scanner, &murrine_style->rgba);
				murrine_style->bflags |= MRN_FLAG_RGBA;
				break;
			case TOKEN_ROUNDNESS:
				token = theme_parse_int (settings, scanner, &murrine_style->roundness);
				murrine_style->bflags |= MRN_FLAG_ROUNDNESS;
				break;
			case TOKEN_SCROLLBARSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->scrollbarstyle);
				murrine_style->flags |= MRN_FLAG_SCROLLBARSTYLE;
				break;
			case TOKEN_SEPARATORSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->separatorstyle);
				murrine_style->flags |= MRN_FLAG_SEPARATORSTYLE;
				break;
			case TOKEN_SHADOW_SHADES:
				token = theme_parse_border (settings, scanner, murrine_style->shadow_shades);
				murrine_style->gflags |= MRN_FLAG_SHADOW_SHADES;
				break;
			case TOKEN_SLIDERSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->sliderstyle);
				murrine_style->flags |= MRN_FLAG_SLIDERSTYLE;
				break;
			case TOKEN_SPINBUTTONSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->spinbuttonstyle);
				murrine_style->flags |= MRN_FLAG_SPINBUTTONSTYLE;
				break;
			case TOKEN_STEPPERSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->stepperstyle);
				murrine_style->flags |= MRN_FLAG_STEPPERSTYLE;
				break;
			case TOKEN_TEXTSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->textstyle);
				murrine_style->flags |= MRN_FLAG_TEXTSTYLE;
				break;
			case TOKEN_TEXT_SHADE:
				token = theme_parse_shade (settings, scanner, &murrine_style->text_shade);
				murrine_style->flags |= MRN_FLAG_TEXT_SHADE;
				break;
			case TOKEN_TREEVIEW_EXPANDER_COLOR:
				token = theme_parse_color (settings, scanner, rc_style, &murrine_style->treeview_expander_color);
				murrine_style->flags |= MRN_FLAG_TREEVIEW_EXPANDER_COLOR;
				break;
			case TOKEN_TOOLBARSTYLE:
				token = theme_parse_int (settings, scanner, &murrine_style->toolbarstyle);
				murrine_style->flags |= MRN_FLAG_TOOLBARSTYLE;
				break;
			case TOKEN_TROUGH_BORDER_SHADES:
				token = theme_parse_border (settings, scanner, murrine_style->trough_border_shades);
				murrine_style->gflags |= MRN_FLAG_TROUGH_BORDER_SHADES;
				break;
			case TOKEN_TROUGH_SHADES:
				token = theme_parse_border (settings, scanner, murrine_style->trough_shades);
				murrine_style->gflags |= MRN_FLAG_TROUGH_SHADES;
				break;

			/* stuff to ignore */
			case TOKEN_GRADIENTS:
				token = murrine_gtk2_rc_parse_dummy (settings, scanner, "gradients");
				break;
			case TOKEN_HILIGHT_RATIO:
				g_scanner_warn (scanner, "Murrine configuration option \"hilight_ratio\" will be deprecated in future releases. Please use \"highlight_shade\" instead.", "hilight_ratio");
				double hilight_ratio;
				token = theme_parse_shade (settings, scanner, &hilight_ratio);
				murrine_style->highlight_shade = hilight_ratio/0.909090;
				murrine_style->flags |= MRN_FLAG_HIGHLIGHT_SHADE;
				break;
			case TOKEN_HIGHLIGHT_RATIO:
				g_scanner_warn (scanner, "Murrine configuration option \"highlight_ratio\" will be deprecated in future releases. Please use \"highlight_shade\" instead.", "highlight_ratio");
				token = theme_parse_shade (settings, scanner, &murrine_style->highlight_shade);
				murrine_style->flags |= MRN_FLAG_HIGHLIGHT_SHADE;
				break;
			case TOKEN_LIGHTBORDER_RATIO:
				g_scanner_warn (scanner, "Murrine configuration option \"lightborder_ratio\" will be deprecated in future releases. Please use \"lightborder_shade\" instead.", "lightborder_ratio");
				token = theme_parse_shade (settings, scanner, &murrine_style->lightborder_shade);
				murrine_style->flags |= MRN_FLAG_LIGHTBORDER_SHADE;
				break;
			case TOKEN_PROFILE:
				token = murrine_gtk2_rc_parse_dummy (settings, scanner, "profile");
				break;
			case TOKEN_SCROLLBAR_COLOR:
			{
				GdkColor dummy_color;
				token = murrine_gtk2_rc_parse_dummy_color (settings, scanner, "scrollbar_color", rc_style, &dummy_color);
				break;
			}
			case TOKEN_SQUAREDSTYLE:
				token = murrine_gtk2_rc_parse_dummy (settings, scanner, "squaredstyle");
				break;
			case TOKEN_STYLE:
				token = murrine_gtk2_rc_parse_dummy (settings, scanner, "style");
				break;

			default:
				g_scanner_get_next_token(scanner);
				token = G_TOKEN_RIGHT_CURLY;
				break;
		}

		if (token != G_TOKEN_NONE)
			return token;

		token = g_scanner_peek_next_token(scanner);
	}

	g_scanner_get_next_token(scanner);

	g_scanner_set_scope(scanner, old_scope);

	return G_TOKEN_NONE;
}

static void
murrine_rc_style_merge (GtkRcStyle *dest,
                        GtkRcStyle *src)
{
	MurrineRcStyle *dest_w, *src_w;
	MurrineRcFlags flags;
	MurrineRcBasicFlags bflags;
	MurrineRcGradientFlags gflags;

	GTK_RC_STYLE_CLASS (murrine_rc_style_parent_class)->merge (dest, src);

	if (!MURRINE_IS_RC_STYLE (src))
		return;

	src_w = MURRINE_RC_STYLE (src);
	dest_w = MURRINE_RC_STYLE (dest);

	/* Flags */
	flags = (~dest_w->flags) & src_w->flags;

	if (flags & MRN_FLAG_ARROWSTYLE)
		dest_w->arrowstyle = src_w->arrowstyle;
	if (flags & MRN_FLAG_CELLSTYLE)
		dest_w->cellstyle = src_w->cellstyle;
	if (flags & MRN_FLAG_COMBOBOXSTYLE)
		dest_w->comboboxstyle = src_w->comboboxstyle;
	if (flags & MRN_FLAG_DEFAULT_BUTTON_COLOR)
	{
		dest_w->has_default_button_color = src_w->has_default_button_color;
		dest_w->default_button_color = src_w->default_button_color;
	}
	if (flags & MRN_FLAG_TREEVIEW_EXPANDER_COLOR)
	{
		dest_w->has_treeview_expander_color = src_w->has_treeview_expander_color;
		dest_w->treeview_expander_color = src_w->treeview_expander_color;
	}
	if (flags & MRN_FLAG_EXPANDERSTYLE)
		dest_w->expanderstyle = src_w->expanderstyle;
	if (flags & MRN_FLAG_FOCUS_COLOR)
	{
		dest_w->has_focus_color = src_w->has_focus_color;
		dest_w->focus_color = src_w->focus_color;
	}
	if (flags & MRN_FLAG_FOCUSSTYLE)
		dest_w->focusstyle = src_w->focusstyle;
	if (flags & MRN_FLAG_GLAZESTYLE)
		dest_w->glazestyle = src_w->glazestyle;
	if (flags & MRN_FLAG_GLOW_SHADE)
		dest_w->glow_shade = src_w->glow_shade;
	if (flags & MRN_FLAG_GLOWSTYLE)
		dest_w->glowstyle = src_w->glowstyle;
	if (flags & MRN_FLAG_HANDLESTYLE)
		dest_w->handlestyle = src_w->handlestyle;
	if (flags & MRN_FLAG_HIGHLIGHT_SHADE)
		dest_w->highlight_shade = src_w->highlight_shade;
	if (flags & MRN_FLAG_LIGHTBORDER_SHADE)
		dest_w->lightborder_shade = src_w->lightborder_shade;
	if (flags & MRN_FLAG_LIGHTBORDERSTYLE)
		dest_w->lightborderstyle = src_w->lightborderstyle;
	if (flags & MRN_FLAG_LISTVIEWHEADERSTYLE)
		dest_w->listviewheaderstyle = src_w->listviewheaderstyle;
	if (flags & MRN_FLAG_LISTVIEWSTYLE)
		dest_w->listviewstyle = src_w->listviewstyle;
	if (flags & MRN_FLAG_MENUBARITEMSTYLE)
		dest_w->menubaritemstyle = src_w->menubaritemstyle;
	if (flags & MRN_FLAG_MENUBARSTYLE)
		dest_w->menubarstyle = src_w->menubarstyle;
	if (flags & MRN_FLAG_MENUITEMSTYLE)
		dest_w->menuitemstyle = src_w->menuitemstyle;
	if (flags & MRN_FLAG_MENUSTYLE)
		dest_w->menustyle = src_w->menustyle;
	if (flags & MRN_FLAG_PRELIGHT_SHADE)
		dest_w->prelight_shade = src_w->prelight_shade;
	if (flags & MRN_FLAG_PROGRESSBARSTYLE)
		dest_w->progressbarstyle = src_w->progressbarstyle;
	if (flags & MRN_FLAG_RELIEFSTYLE)
		dest_w->reliefstyle = src_w->reliefstyle;
	if (flags & MRN_FLAG_SCROLLBARSTYLE)
		dest_w->scrollbarstyle = src_w->scrollbarstyle;
	if (flags & MRN_FLAG_SEPARATORSTYLE)
		dest_w->separatorstyle = src_w->separatorstyle;
	if (flags & MRN_FLAG_SLIDERSTYLE)
		dest_w->sliderstyle = src_w->sliderstyle;
	if (flags & MRN_FLAG_SPINBUTTONSTYLE)
		dest_w->spinbuttonstyle = src_w->spinbuttonstyle;
	if (flags & MRN_FLAG_STEPPERSTYLE)
		dest_w->stepperstyle = src_w->stepperstyle;
	if (flags & MRN_FLAG_TEXTSTYLE)
		dest_w->textstyle = src_w->textstyle;
	if (flags & MRN_FLAG_TEXT_SHADE)
		dest_w->text_shade = src_w->text_shade;
	if (flags & MRN_FLAG_TOOLBARSTYLE)
		dest_w->toolbarstyle = src_w->toolbarstyle;

	dest_w->flags |= src_w->flags;

	/* Basic Flags */
	bflags = (~dest_w->bflags) & src_w->bflags;

	if (bflags & MRN_FLAG_ANIMATION)
		dest_w->animation = src_w->animation;
	if (bflags & MRN_FLAG_COLORIZE_SCROLLBAR)
		dest_w->colorize_scrollbar = src_w->colorize_scrollbar;
	if (bflags & MRN_FLAG_CONTRAST)
		dest_w->contrast = src_w->contrast;
	if (bflags & MRN_FLAG_RGBA)
		dest_w->rgba = src_w->rgba;
	if (bflags & MRN_FLAG_ROUNDNESS)
		dest_w->roundness = src_w->roundness;

	dest_w->bflags |= src_w->bflags;

	/* Gradient Flags */
	gflags = (~dest_w->gflags) & src_w->gflags;

	if (gflags & MRN_FLAG_BORDER_COLORS)
	{
		dest_w->has_border_colors = src_w->has_border_colors;
		dest_w->border_colors[0]  = src_w->border_colors[0];
		dest_w->border_colors[1]  = src_w->border_colors[1];
	}
	if (gflags & MRN_FLAG_BORDER_SHADES)
	{
		dest_w->border_shades[0] = src_w->border_shades[0];
		dest_w->border_shades[1] = src_w->border_shades[1];
	}
	if (gflags & MRN_FLAG_GRADIENT_COLORS)
	{
		dest_w->has_gradient_colors = src_w->has_gradient_colors;
		dest_w->gradient_colors[0]  = src_w->gradient_colors[0];
		dest_w->gradient_colors[1]  = src_w->gradient_colors[1];
		dest_w->gradient_colors[2]  = src_w->gradient_colors[2];
		dest_w->gradient_colors[3]  = src_w->gradient_colors[3];
	}
	if (gflags & MRN_FLAG_GRADIENT_SHADES)
	{
		dest_w->gradient_shades[0] = src_w->gradient_shades[0];
		dest_w->gradient_shades[1] = src_w->gradient_shades[1];
		dest_w->gradient_shades[2] = src_w->gradient_shades[2];
		dest_w->gradient_shades[3] = src_w->gradient_shades[3];
	}
	if (gflags & MRN_FLAG_SHADOW_SHADES)
	{
		dest_w->shadow_shades[0] = src_w->shadow_shades[0];
		dest_w->shadow_shades[1] = src_w->shadow_shades[1];
	}
	if (gflags & MRN_FLAG_TROUGH_BORDER_SHADES)
	{
		dest_w->trough_border_shades[0] = src_w->trough_border_shades[0];
		dest_w->trough_border_shades[1] = src_w->trough_border_shades[1];
	}
	if (gflags & MRN_FLAG_TROUGH_SHADES)
	{
		dest_w->trough_shades[0] = src_w->trough_shades[0];
		dest_w->trough_shades[1] = src_w->trough_shades[1];
	}

	dest_w->gflags |= src_w->gflags;
}

/* Create an empty style suitable to this RC style
 */
static GtkStyle *
murrine_rc_style_create_style (GtkRcStyle *rc_style)
{
	return GTK_STYLE (g_object_new (MURRINE_TYPE_STYLE, NULL));
}
