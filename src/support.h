#ifndef SUPPORT_H
#define SUPPORT_H

#include <gtk/gtk.h>
#include <math.h>
#include <string.h>

#include "equinox_types.h"

#define RADIO_SIZE 15
#define CHECK_SIZE 15


#define EQUINOX_IS_WIDGET(object) ((object)  && equinox_object_is_a ((GObject*)(object), "GtkWidget"))
#define EQUINOX_IS_CONTAINER(object) ((object)  && equinox_object_is_a ((GObject*)(object), "GtkContainer"))
#define EQUINOX_IS_BIN(object) ((object)  && equinox_object_is_a ((GObject*)(object), "GtkBin"))

#define EQUINOX_IS_ARROW(object) ((object)  && equinox_object_is_a ((GObject*)(object), "GtkArrow"))

#define EQUINOX_IS_SEPARATOR(object) ((object)  && equinox_object_is_a ((GObject*)(object), "GtkSeparator"))
#define EQUINOX_IS_VSEPARATOR(object) ((object)  && equinox_object_is_a ((GObject*)(object), "GtkVSeparator"))
#define EQUINOX_IS_HSEPARATOR(object) ((object)  && equinox_object_is_a ((GObject*)(object), "GtkHSeparator"))

#define EQUINOX_IS_HANDLE_BOX(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkHandleBox"))
#define EQUINOX_IS_HANDLE_BOX_ITEM(object) ((object) && EQUINOX_IS_HANDLE_BOX(object->parent))
#define EQUINOX_IS_BONOBO_DOCK_ITEM(object) ((object) && equinox_object_is_a ((GObject*)(object), "BonoboDockItem"))
#define EQUINOX_IS_BONOBO_DOCK_ITEM_GRIP(object) ((object) && equinox_object_is_a ((GObject*)(object), "BonoboDockItemGrip"))
#define EQUINOX_IS_BONOBO_TOOLBAR(object) ((object) && equinox_object_is_a ((GObject*)(object), "BonoboUIToolbar"))
#define EQUINOX_IS_EGG_TOOLBAR(object) ((object) && equinox_object_is_a ((GObject*)(object), "Toolbar"))
#define EQUINOX_IS_TOOLBAR(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkToolbar"))
#define EQUINOX_IS_PANEL_WIDGET(object) ((object) && (equinox_object_is_a ((GObject*)(object), "PanelWidget") || equinox_object_is_a ((GObject*)(object), "PanelApplet")))
#define EQUINOX_IS_WINDOW(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkWindow"))


#define EQUINOX_IS_COMBO_BOX_ENTRY(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkComboBoxEntry"))
#define EQUINOX_IS_COMBO_BOX(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkComboBox"))
#define EQUINOX_IS_COMBO(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkCombo"))
#define EQUINOX_IS_OPTION_MENU(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkOptionMenu"))

#define EQUINOX_IS_TOGGLE_BUTTON(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkToggleButton"))
#define EQUINOX_IS_CHECK_BUTTON(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkCheckButton"))
#define EQUINOX_IS_RADIO_BUTTON(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkRadioButton"))
#define EQUINOX_IS_SPIN_BUTTON(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkSpinButton"))

#define EQUINOX_IS_STATUSBAR(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkStatusbar"))
#define EQUINOX_IS_PROGRESS_BAR(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkProgressBar"))

#define EQUINOX_IS_MENU_SHELL(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkMenuShell"))
#define EQUINOX_IS_MENU(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkMenu"))
#define EQUINOX_IS_MENU_BAR(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkMenuBar"))
#define EQUINOX_IS_MENU_ITEM(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkMenuItem"))

#define EQUINOX_IS_CHECK_MENU_ITEM(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkCheckMenuItem"))

#define EQUINOX_IS_RANGE(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkRange"))

#define EQUINOX_IS_SCROLLBAR(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkScrollbar"))
#define EQUINOX_IS_VSCROLLBAR(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkVScrollbar"))
#define EQUINOX_IS_HSCROLLBAR(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkHScrollbar"))

#define EQUINOX_IS_SCALE(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkScale"))
#define EQUINOX_IS_VSCALE(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkVScale"))
#define EQUINOX_IS_HSCALE(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkHScale"))

#define EQUINOX_IS_PANED(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkPaned"))
#define EQUINOX_IS_VPANED(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkVPaned"))
#define EQUINOX_IS_HPANED(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkHPaned"))

#define EQUINOX_IS_BOX(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkBox"))
#define EQUINOX_IS_VBOX(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkVBox"))
#define EQUINOX_IS_HBOX(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkHBox"))

#define EQUINOX_IS_CLIST(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkCList"))
#define EQUINOX_IS_TREE_VIEW(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkTreeView"))
#define EQUINOX_IS_ENTRY(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkEntry"))
#define EQUINOX_IS_BUTTON(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkButton"))
#define EQUINOX_IS_FIXED(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkFixed"))
#define EQUINOX_IS_VIEWPORT(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkViewport"))
#define EQUINOX_IS_SCROLLED_WINDOW(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkScrolledWindow"))

#define TOGGLE_BUTTON(object) (EQUINOX_IS_TOGGLE_BUTTON(object)?(GtkToggleButton *)object:NULL)

#define EQUINOX_IS_NOTEBOOK(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkNotebook"))
#define EQUINOX_IS_CELL_RENDERER_TOGGLE(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkCellRendererToggle"))

#define EQUINOX_WIDGET_HAS_DEFAULT(object) ((object) && EQUINOX_IS_WIDGET(object) && GTK_WIDGET_HAS_DEFAULT(object))

#define EQUINOX_IS_COLOR_BUTTON(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkColorButton"))
#define EQUINOX_IS_EVENT_BOX(object) ((object) && equinox_object_is_a ((GObject*)(object), "GtkEventBox"))







G_GNUC_INTERNAL gboolean equinox_sanitize_size (GdkWindow * window, gint * width, gint * height);

G_GNUC_INTERNAL GtkTextDirection equinox_get_direction     (GtkWidget *widget);

G_GNUC_INTERNAL GtkWidget *equinox_special_get_ancestor    (GtkWidget    *widget,
                                    GType         widget_type);

G_GNUC_INTERNAL void murrine_shade (const EquinoxRGB *a, float k, EquinoxRGB *b);

G_GNUC_INTERNAL void equinox_shade (const EquinoxRGB *a, EquinoxRGB *b, double k);
G_GNUC_INTERNAL void equinox_shade_shift (const EquinoxRGB *a, EquinoxRGB *b, double k);
G_GNUC_INTERNAL void equinox_hue_shift (const EquinoxRGB *base, EquinoxRGB *composite, double shift_amount);

G_GNUC_INTERNAL void equinox_match_lightness (const EquinoxRGB * a, EquinoxRGB * b);
G_GNUC_INTERNAL gdouble equinox_get_lightness (const EquinoxRGB * a);
G_GNUC_INTERNAL void equinox_set_lightness (EquinoxRGB * a, double new_lightness);
G_GNUC_INTERNAL void equinox_tweak_saturation (const EquinoxRGB * a, EquinoxRGB * b);
G_GNUC_INTERNAL void equinox_set_lightness (EquinoxRGB * a, double new_lightness);


G_GNUC_INTERNAL GtkWidget*        equinox_get_parent_window       (GtkWidget    *widget);
G_GNUC_INTERNAL GdkColor*         equinox_get_parent_bgcolor      (GtkWidget    *widget);
G_GNUC_INTERNAL gboolean          mrn_is_combo_box         (GtkWidget    *widget);
G_GNUC_INTERNAL GtkWidget*        mrn_find_combo_box_widget(GtkWidget    *widget);

G_GNUC_INTERNAL void              equinox_gdk_color_to_rgb   (GdkColor     *c,
                                                 double       *r,
                                                 double       *g,
                                                 double       *b);

G_GNUC_INTERNAL void  equinox_gtk_treeview_get_header_index (GtkTreeView  *tv,
                                                 GtkWidget    *header,
                                                 gint         *column_index,
                                                 gint         *columns,
                                                 gboolean     *resizable,
                                                 gboolean     *sorted);

G_GNUC_INTERNAL void  equinox_gtk_clist_get_header_index    (GtkCList     *clist,
                                                 GtkWidget    *button,
                                                 gint         *column_index,
                                                 gint         *columns);

G_GNUC_INTERNAL void  equinox_get_parent_bg      (const GtkWidget *widget,
                                                 EquinoxRGB      *color);

G_GNUC_INTERNAL void  equinox_option_menu_get_props         (GtkWidget       *widget,
                                                 GtkRequisition  *indicator_size,
                                                 GtkBorder       *indicator_spacing);

G_GNUC_INTERNAL gboolean          mrn_is_panel_widget            (GtkWidget *widget);


G_GNUC_INTERNAL gboolean  equinox_object_is_a (const GObject * object, const gchar * type_name);
G_GNUC_INTERNAL gboolean  equinox_is_combo_box_entry (GtkWidget * widget);
G_GNUC_INTERNAL gboolean  equinox_is_combo_box (GtkWidget * widget, gboolean as_list);
G_GNUC_INTERNAL gboolean equinox_is_tree_column_header (GtkWidget * widget);
G_GNUC_INTERNAL gboolean  equinox_is_combo (GtkWidget * widget);
G_GNUC_INTERNAL gboolean  equinox_is_in_combo_box (GtkWidget * widget);
G_GNUC_INTERNAL gboolean  equinox_is_toolbar_item (GtkWidget * widget);
G_GNUC_INTERNAL gboolean  equinox_is_panel_widget_item (GtkWidget * widget);
G_GNUC_INTERNAL gboolean  equinox_is_bonobo_dock_item (GtkWidget * widget);
G_GNUC_INTERNAL GtkWidget* equinox_find_combo_box_widget_parent (GtkWidget * widget);
G_GNUC_INTERNAL gboolean  equinox_widget_is_ltr (GtkWidget *widget);

G_GNUC_INTERNAL void  equinox_gdk_color_to_cairo (const GdkColor *c, EquinoxRGB *cc);
G_GNUC_INTERNAL void  equinox_mix_color (const EquinoxRGB * a, const EquinoxRGB * b, gdouble mix_factor, EquinoxRGB * c);

G_GNUC_INTERNAL void  equinox_set_source_rgb (cairo_t *cr, const EquinoxRGB *color);
G_GNUC_INTERNAL void  equinox_set_source_rgba (cairo_t *cr, const EquinoxRGB *color, double alpha);

G_GNUC_INTERNAL void  equinox_pattern_add_color_rgb (cairo_pattern_t *pattern, double offset, const EquinoxRGB *color);
G_GNUC_INTERNAL void  equinox_pattern_add_color_rgba (cairo_pattern_t *pattern, double offset, const EquinoxRGB *color, double alpha);

#endif /* SUPPORT_H */
