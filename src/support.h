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

#ifndef SUPPORT_H
#define SUPPORT_H

#include <gtk/gtk.h>
#include <math.h>
#include <string.h>

#include "murrine_types.h"
#include "cairo-support.h"

#define RADIO_SIZE 13
#define CHECK_SIZE 13

/* Opacity settings */
#define GRADIENT_OPACITY 0.90
#define WINDOW_OPACITY 0.88
#define ENTRY_OPACITY 0.90
#define NOTEBOOK_OPACITY 0.92
#define MENUBAR_OPACITY 0.88
#define MENUBAR_GLOSSY_OPACITY 0.88
#define MENUBAR_STRIPED_OPACITY 0.94
#define TOOLBAR_OPACITY 0.88
#define TOOLBAR_GLOSSY_OPACITY 0.88
#define MENU_OPACITY 0.90
#define TOOLTIP_OPACITY 0.90

/* From gtk-engines 20071109 */
#define MRN_IS_WIDGET(object) ((object)  && murrine_object_is_a ((GObject*)(object), "GtkWidget"))
#define MRN_IS_CONTAINER(object) ((object)  && murrine_object_is_a ((GObject*)(object), "GtkContainer"))
#define MRN_IS_BIN(object) ((object)  && murrine_object_is_a ((GObject*)(object), "GtkBin"))

#define MRN_IS_ARROW(object) ((object)  && murrine_object_is_a ((GObject*)(object), "GtkArrow"))

#define MRN_IS_SEPARATOR(object) ((object)  && murrine_object_is_a ((GObject*)(object), "GtkSeparator"))
#define MRN_IS_VSEPARATOR(object) ((object)  && murrine_object_is_a ((GObject*)(object), "GtkVSeparator"))
#define MRN_IS_HSEPARATOR(object) ((object)  && murrine_object_is_a ((GObject*)(object), "GtkHSeparator"))
 
#define MRN_IS_HANDLE_BOX(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkHandleBox"))
#define MRN_IS_HANDLE_BOX_ITEM(object) ((object) && MRN_IS_HANDLE_BOX(object->parent))
#define MRN_IS_BONOBO_DOCK_ITEM(object) ((object) && murrine_object_is_a ((GObject*)(object), "BonoboDockItem"))
#define MRN_IS_BONOBO_DOCK_ITEM_GRIP(object) ((object) && murrine_object_is_a ((GObject*)(object), "BonoboDockItemGrip"))
#define MRN_IS_BONOBO_TOOLBAR(object) ((object) && murrine_object_is_a ((GObject*)(object), "BonoboUIToolbar"))
#define MRN_IS_EGG_TOOLBAR(object) ((object) && murrine_object_is_a ((GObject*)(object), "Toolbar"))
#define MRN_IS_TOOLBAR(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkToolbar"))
#define MRN_IS_PANEL_WIDGET(object) ((object) && (murrine_object_is_a ((GObject*)(object), "PanelWidget") || murrine_object_is_a ((GObject*)(object), "PanelApplet")))

#define MRN_IS_COMBO_BOX_ENTRY(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkComboBoxEntry"))
#define MRN_IS_COMBO_BOX(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkComboBox"))
#define MRN_IS_COMBO(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkCombo"))
#define MRN_IS_OPTION_MENU(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkOptionMenu"))
 
#define MRN_IS_TOGGLE_BUTTON(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkToggleButton"))
#define MRN_IS_CHECK_BUTTON(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkCheckButton"))
#define MRN_IS_SPIN_BUTTON(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkSpinButton"))
 
#define MRN_IS_STATUSBAR(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkStatusbar"))
#define MRN_IS_PROGRESS_BAR(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkProgressBar"))
 
#define MRN_IS_MENU_SHELL(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkMenuShell"))
#define MRN_IS_MENU(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkMenu"))
#define MRN_IS_MENU_BAR(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkMenuBar"))
#define MRN_IS_MENU_ITEM(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkMenuItem"))

#define MRN_IS_CHECK_MENU_ITEM(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkCheckMenuItem"))

#define MRN_IS_RANGE(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkRange"))
 
#define MRN_IS_SCROLLBAR(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkScrollbar"))
#define MRN_IS_VSCROLLBAR(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkVScrollbar"))
#define MRN_IS_HSCROLLBAR(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkHScrollbar"))
 
#define MRN_IS_SCALE(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkScale"))
#define MRN_IS_VSCALE(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkVScale"))
#define MRN_IS_HSCALE(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkHScale"))
  
#define MRN_IS_PANED(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkPaned"))
#define MRN_IS_VPANED(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkVPaned"))
#define MRN_IS_HPANED(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkHPaned"))
 
#define MRN_IS_BOX(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkBox"))
#define MRN_IS_VBOX(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkVBox"))
#define MRN_IS_HBOX(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkHBox"))

#define MRN_IS_CLIST(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkCList"))
#define MRN_IS_TREE_VIEW(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkTreeView"))
#define MRN_IS_ENTRY(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkEntry"))
#define MRN_IS_BUTTON(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkButton"))
#define MRN_IS_FIXED(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkFixed"))
 
#define TOGGLE_BUTTON(object) (MRN_IS_TOGGLE_BUTTON(object)?(GtkToggleButton *)object:NULL)
 
#define MRN_IS_NOTEBOOK(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkNotebook"))
#define MRN_IS_CELL_RENDERER_TOGGLE(object) ((object) && murrine_object_is_a ((GObject*)(object), "GtkCellRendererToggle"))

#define MRN_WIDGET_HAS_DEFAULT(object) ((object) && MRN_IS_WIDGET(object) && GTK_WIDGET_HAS_DEFAULT(object))

G_GNUC_INTERNAL GtkTextDirection murrine_get_direction (GtkWidget *widget);
G_GNUC_INTERNAL GtkWidget *murrine_special_get_ancestor (GtkWidget *widget, GType widget_type);
G_GNUC_INTERNAL GdkColor* murrine_get_parent_bgcolor (GtkWidget *widget);
G_GNUC_INTERNAL GtkWidget* murrine_get_parent_window (GtkWidget *widget);
G_GNUC_INTERNAL gboolean murrine_is_combo_box (GtkWidget *widget);
G_GNUC_INTERNAL GtkWidget* murrine_find_combo_box_widget(GtkWidget *widget);
G_GNUC_INTERNAL void murrine_gtk_treeview_get_header_index (GtkTreeView *tv,
                                                            GtkWidget   *header,
                                                            gint        *column_index,
                                                            gint        *columns,
                                                            gboolean    *resizable);
G_GNUC_INTERNAL void murrine_gtk_clist_get_header_index (GtkCList  *clist,
                                                         GtkWidget *button,
                                                         gint      *column_index,
                                                         gint      *columns);
G_GNUC_INTERNAL void murrine_option_menu_get_props (GtkWidget      *widget,
                                                    GtkRequisition *indicator_size,
                                                    GtkBorder      *indicator_spacing);
G_GNUC_INTERNAL MurrineStepper murrine_scrollbar_get_stepper (GtkWidget *widget, GdkRectangle *stepper);
G_GNUC_INTERNAL MurrineStepper murrine_scrollbar_visible_steppers (GtkWidget *widget);
G_GNUC_INTERNAL MurrineJunction murrine_scrollbar_get_junction (GtkWidget *widget);
G_GNUC_INTERNAL gboolean murrine_is_panel_widget (GtkWidget *widget);
G_GNUC_INTERNAL void murrine_set_toolbar_parameters (ToolbarParameters *toolbar,
                                                     GtkWidget *widget,
                                                     GdkWindow *window,
                                                     gint x, gint y);
G_GNUC_INTERNAL void murrine_get_notebook_tab_position (GtkWidget *widget,
                                                        gboolean  *start,
                                                        gboolean  *end);

#endif /* SUPPORT_H */
