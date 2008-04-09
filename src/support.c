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

#include "support.h"
#include "cairo-support.h"

GtkTextDirection
murrine_get_direction (GtkWidget *widget)
{
	GtkTextDirection dir;

	if (widget)
		dir = gtk_widget_get_direction (widget);
	else
		dir = GTK_TEXT_DIR_LTR;

	return dir;
}

void murrine_gtk_treeview_get_header_index (GtkTreeView *tv, GtkWidget *header,
                                            gint *column_index, gint *columns,
                                            gboolean *resizable)
{
	GList *list, *list_start;
	*column_index = *columns = 0;
	list_start = list = gtk_tree_view_get_columns (tv);

	do
	{
		GtkTreeViewColumn *column = GTK_TREE_VIEW_COLUMN(list->data);
		if ( column->button == header )
		{
			*column_index = *columns;
			*resizable = column->resizable;
		}
		if ( column->visible )
			(*columns)++;
	} while ((list = g_list_next(list)));

	g_list_free (list_start);
}

void murrine_gtk_clist_get_header_index (GtkCList *clist, GtkWidget *button,
                                         gint *column_index, gint *columns)
{
	int i;
	*columns = clist->columns;

	for (i=0; i<*columns; i++)
	{
		if (clist->column[i].button == button)
		{
			*column_index = i;
			break;
		}
	}
}

static GtkRequisition default_option_indicator_size = { 7, 13 };
static GtkBorder default_option_indicator_spacing = { 7, 5, 2, 2 };

void
murrine_option_menu_get_props (GtkWidget      *widget,
                               GtkRequisition *indicator_size,
                               GtkBorder      *indicator_spacing)
{
	GtkRequisition *tmp_size = NULL;
	GtkBorder *tmp_spacing = NULL;

	if (widget)
		gtk_widget_style_get (widget, "indicator_size", &tmp_size,
		                      "indicator_spacing", &tmp_spacing, NULL);

	if (tmp_size)
	{
		*indicator_size = *tmp_size;
		gtk_requisition_free (tmp_size);
	}
	else
		*indicator_size = default_option_indicator_size;

	if (tmp_spacing)
	{
		*indicator_spacing = *tmp_spacing;
		gtk_border_free (tmp_spacing);
	}
	else
		*indicator_spacing = default_option_indicator_spacing;
}

GtkWidget *murrine_special_get_ancestor(GtkWidget * widget, GType widget_type)
{
	g_return_val_if_fail(GTK_IS_WIDGET(widget), NULL);

	while (widget && widget->parent
	       && !g_type_is_a(GTK_WIDGET_TYPE(widget->parent),
	       widget_type))
		widget = widget->parent;

	if (!
	    (widget && widget->parent
	     && g_type_is_a(GTK_WIDGET_TYPE(widget->parent), widget_type)))
		return NULL;

	return widget;
}

GtkWidget *murrine_get_parent_window (GtkWidget *widget)
{
	GtkWidget *parent = widget->parent;

	while (parent && GTK_WIDGET_NO_WINDOW (parent))
		parent = parent->parent;

	return parent;
}

GdkColor *murrine_get_parent_bgcolor (GtkWidget *widget)
{
	GtkWidget *parent = murrine_get_parent_window (widget);

	if (parent && parent->style)
		return &parent->style->bg[GTK_STATE_NORMAL];

	return NULL;
}

gboolean
murrine_object_is_a (const GObject * object, const gchar * type_name)
{
	gboolean result = FALSE;

	if ((object))
	{
		GType tmp = g_type_from_name (type_name);

		if (tmp)
			result = g_type_check_instance_is_a ((GTypeInstance *) object, tmp);
	}

	return result;
}

gboolean
murrine_widget_is_ltr (GtkWidget *widget)
{
	GtkTextDirection dir = GTK_TEXT_DIR_NONE;

	if (MRN_IS_WIDGET (widget))
		dir = gtk_widget_get_direction (widget);

	if (dir == GTK_TEXT_DIR_NONE)
		dir = gtk_widget_get_default_direction ();

	if (dir == GTK_TEXT_DIR_RTL)
		return FALSE;
	else
		return TRUE;
}

GtkWidget *
murrine_find_combo_box_widget (GtkWidget * widget)
{
	GtkWidget *result = NULL;

	if (widget && !GTK_IS_COMBO_BOX_ENTRY (widget))
	{
		if (GTK_IS_COMBO_BOX (widget))
			result = widget;
		else
			result = murrine_find_combo_box_widget(widget->parent);
	}

	return result;
}

gboolean
murrine_is_combo_box (GtkWidget * widget)
{
	return (murrine_find_combo_box_widget(widget) != NULL);
}

MurrineStepper
murrine_scrollbar_get_stepper (GtkWidget    *widget,
                               GdkRectangle *stepper)
{
	MurrineStepper value = MRN_STEPPER_UNKNOWN;
	GdkRectangle tmp;
	GdkRectangle check_rectangle;
	GtkOrientation orientation;

	g_return_val_if_fail (GTK_IS_RANGE (widget), MRN_STEPPER_UNKNOWN);

	check_rectangle.x      = widget->allocation.x;
	check_rectangle.y      = widget->allocation.y;
	check_rectangle.width  = stepper->width;
	check_rectangle.height = stepper->height;

	orientation = GTK_RANGE (widget)->orientation;

	if (widget->allocation.x == -1 && widget->allocation.y == -1)
		return MRN_STEPPER_UNKNOWN;

	if (gdk_rectangle_intersect (stepper, &check_rectangle, &tmp))
		value = MRN_STEPPER_A;

	if (value == MRN_STEPPER_UNKNOWN) /* Haven't found a match */
	{
		if (orientation == GTK_ORIENTATION_HORIZONTAL)
			check_rectangle.x = widget->allocation.x + stepper->width;
		else
			check_rectangle.y = widget->allocation.y + stepper->height;

		if (gdk_rectangle_intersect (stepper, &check_rectangle, &tmp))
			value = MRN_STEPPER_B;
	}

	if (value == MRN_STEPPER_UNKNOWN) /* Still haven't found a match */
	{
		if (orientation == GTK_ORIENTATION_HORIZONTAL)
			check_rectangle.x = widget->allocation.x + widget->allocation.width - (stepper->width * 2);
		else
			check_rectangle.y = widget->allocation.y + widget->allocation.height - (stepper->height * 2);

		if (gdk_rectangle_intersect (stepper, &check_rectangle, &tmp))
			value = MRN_STEPPER_C;
	}

	if (value == MRN_STEPPER_UNKNOWN) /* STILL haven't found a match */
	{
		if (orientation == GTK_ORIENTATION_HORIZONTAL)
			check_rectangle.x = widget->allocation.x + widget->allocation.width - stepper->width;
		else
			check_rectangle.y = widget->allocation.y + widget->allocation.height - stepper->height;

		if (gdk_rectangle_intersect (stepper, &check_rectangle, &tmp))
			value = MRN_STEPPER_D;
	}

	return value;
}

MurrineStepper
murrine_scrollbar_visible_steppers (GtkWidget *widget)
{
	MurrineStepper steppers = 0;

	g_return_val_if_fail (GTK_IS_RANGE (widget), MRN_STEPPER_UNKNOWN);

	if (GTK_RANGE (widget)->has_stepper_a)
		steppers |= MRN_STEPPER_A;

	if (GTK_RANGE (widget)->has_stepper_b)
		steppers |= MRN_STEPPER_B;

	if (GTK_RANGE (widget)->has_stepper_c)
		steppers |= MRN_STEPPER_C;

	if (GTK_RANGE (widget)->has_stepper_d)
		steppers |= MRN_STEPPER_D;
}

MurrineJunction
murrine_scrollbar_get_junction (GtkWidget    *widget)
{
	GtkAdjustment *adj;
	MurrineJunction junction = MRN_JUNCTION_NONE;

	g_return_val_if_fail (GTK_IS_RANGE (widget), MRN_JUNCTION_NONE);

	adj = GTK_RANGE (widget)->adjustment;

	if (adj->value <= adj->lower &&
	    (GTK_RANGE (widget)->has_stepper_a || GTK_RANGE (widget)->has_stepper_b))
	{
		if (!gtk_range_get_inverted (GTK_RANGE (widget)))
			junction |= MRN_JUNCTION_BEGIN;
		else
			junction |= MRN_JUNCTION_END;
	}

	if (adj->value >= adj->upper - adj->page_size &&
	    (GTK_RANGE (widget)->has_stepper_c || GTK_RANGE (widget)->has_stepper_d))
	{
		if (!gtk_range_get_inverted (GTK_RANGE (widget)))
			junction |= MRN_JUNCTION_END;
		else
			junction |= MRN_JUNCTION_BEGIN;
	}

	return junction;
}

gboolean murrine_is_panel_widget (GtkWidget *widget)
{
	return widget && (strcmp(G_OBJECT_TYPE_NAME (widget), "PanelApplet") == 0 ||
	                  strcmp(G_OBJECT_TYPE_NAME (widget), "PanelWidget") == 0);
}

void
murrine_set_toolbar_parameters (ToolbarParameters *toolbar,
                                GtkWidget *widget,
                                GdkWindow *window,
                                gint x, gint y)
{
	toolbar->topmost = FALSE;

	if (x == 0 && y == 0)
	{
		if (widget && widget->allocation.x == 0 && widget->allocation.y == 0)
		{
			if (widget->window == window && MRN_IS_TOOLBAR (widget))
			{
				toolbar->topmost = TRUE;
			}
		}
	}
}

void
murrine_get_notebook_tab_position (GtkWidget *widget,
                                   gboolean  *start,
                                   gboolean  *end)
{
	/* default value */
	*start = TRUE;
	*end = FALSE;

	if (MRN_IS_NOTEBOOK (widget))
	{
		gboolean found_tabs = FALSE;
		gint i, n_pages;
		GtkNotebook *notebook = GTK_NOTEBOOK (widget);

		/* got a notebook, so walk over all the tabs and decide based
		 * on that ...
		 * It works like this:
		 *   - If there is any visible tab that is expanded, set both.
		 *   - Set start/end if there is any visible tab that is at
		 *     the start/end.
		 *   - If one has the child_visibility set to false, arrows
		 *     are present; so none
		 * The heuristic falls over if there is a notebook that just
		 * happens to fill up all the available space. ie. All tabs
		 * are left aligned, but it does not require scrolling.
		 * (a more complex heuristic could calculate the tabs width
		 * and add them all up) */

		n_pages = gtk_notebook_get_n_pages (notebook);
		for (i = 0; i < n_pages; i++)
		{
			GtkWidget *tab_child;
			GtkWidget *tab_label;
			gboolean expand;
			GtkPackType pack_type;

			tab_child = gtk_notebook_get_nth_page (notebook, i);

			/* Skip invisible tabs */
			tab_label = gtk_notebook_get_tab_label (notebook, tab_child);
			if (!tab_label || !GTK_WIDGET_VISIBLE (tab_label))
				continue;
			/* This is the same what the notebook does internally. */
			if (tab_label && !gtk_widget_get_child_visible (tab_label))
			{
				/* One child is hidden because scroll arrows are present.
				 * So both corners are rounded. */
				*start = FALSE;
				*end = FALSE;
				return;
			}

			gtk_notebook_query_tab_label_packing (notebook, tab_child,
			                                      &expand,
			                                      NULL, /* don't need fill */
			                                      &pack_type);

			if (!found_tabs)
			{
				found_tabs = TRUE;
				*start = FALSE;
				*end = FALSE;
			}

			if (expand)
			{
				*start = TRUE;
				*end = TRUE;
			}
			else if (pack_type == GTK_PACK_START)
			{
				*start = TRUE;
			}
			else
			{
				*end = TRUE;
			}
		}
	}
}
