#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include "equinox_style.h"
#include "equinox_rc_style.h"
#include "equinox_draw.h"
#include "support.h"

static EquinoxStyleClass *equinox_style_class;
static GtkStyleClass *equinox_parent_class;


#define DETAIL(xx)   ((detail) && (!strcmp(xx, detail)))

#define DRAW_ARGS    GtkStyle       *style, \
                     GdkWindow      *window, \
                     GtkStateType    state_type, \
                     GtkShadowType   shadow_type, \
                     GdkRectangle   *area, \
                     GtkWidget      *widget, \
                     const gchar    *detail, \
                     gint            x, \
                     gint            y, \
                     gint            width, \
                     gint            height

#define CHECK_ARGS					\
  g_return_if_fail (window != NULL);			\
  g_return_if_fail (style != NULL);

#define SANITIZE_SIZE					\
  g_return_if_fail (width  >= -1);			\
  g_return_if_fail (height >= -1);			\
                                                        \
  if ((width == -1) && (height == -1))			\
    gdk_drawable_get_size (window, &width, &height);	\
  else if (width == -1)					\
    gdk_drawable_get_size (window, &width, NULL);	\
  else if (height == -1)				\
    gdk_drawable_get_size (window, NULL, &height);


#ifdef HAVE_ANIMATION
#include "animation.h"
#endif

const static int FRAME_SHADE = 5;

static void
clearlooks_rounded_rectangle (cairo_t * cr,
			      double x, double y, double w, double h,
			      double radius, uint8 corners)
{
  if (radius < 0.01) {
    cairo_rectangle (cr, x, y, w, h);
    return;
  }

  if (corners & EQX_CORNER_TOPLEFT)
    cairo_move_to (cr, x + radius, y);
  else
    cairo_move_to (cr, x, y);

  if (corners & EQX_CORNER_TOPRIGHT)
    cairo_arc (cr, x + w - radius, y + radius, radius, M_PI * 1.5, M_PI * 2);
  else
    cairo_line_to (cr, x + w, y);

  if (corners & EQX_CORNER_BOTTOMRIGHT)
    cairo_arc (cr, x + w - radius, y + h - radius, radius, 0, M_PI * 0.5);
  else
    cairo_line_to (cr, x + w, y + h);

  if (corners & EQX_CORNER_BOTTOMLEFT)
    cairo_arc (cr, x + radius, y + h - radius, radius, M_PI * 0.5, M_PI);
  else
    cairo_line_to (cr, x, y + h);

  if (corners & EQX_CORNER_TOPLEFT)
    cairo_arc (cr, x + radius, y + radius, radius, M_PI, M_PI * 1.5);
  else
    cairo_line_to (cr, x, y);
}

static cairo_t *
equinox_begin_paint (GdkDrawable * window, GdkRectangle * area)
{
  cairo_t *cr;

  g_return_val_if_fail (window != NULL, NULL);

  cr = (cairo_t *) gdk_cairo_create (window);
  cairo_set_line_width (cr, 1.0);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
  cairo_set_line_join (cr, CAIRO_LINE_JOIN_MITER);

  if (area) {
    cairo_rectangle (cr, area->x, area->y, area->width, area->height);
    cairo_clip_preserve (cr);
    cairo_new_path (cr);
  }

  return cr;
}


static void
equinox_set_widget_parameters (const GtkWidget * widget,
			      const GtkStyle * style,
			      GtkStateType state_type,
			      WidgetParameters * params)
{

  params->active = (state_type == GTK_STATE_ACTIVE);
  params->prelight = (state_type == GTK_STATE_PRELIGHT);
  params->disabled = (state_type == GTK_STATE_INSENSITIVE);
  params->state_type = (EquinoxStateType) state_type;
  params->corners = EQX_CORNER_ALL;
  params->curvature = EQUINOX_STYLE (style)->curvature;
  params->focus = widget && GTK_WIDGET_HAS_FOCUS (widget);
  params->is_default = widget && GTK_WIDGET_HAS_DEFAULT (widget);
  params->trans = 1.0;
  params->prev_state_type = state_type;
  params->ltr = equinox_widget_is_ltr ((GtkWidget*)widget);
  //params->composited = gtk_widget_is_composited ((GtkWidget*)widget);

  if (!params->active && widget && GTK_IS_TOGGLE_BUTTON (widget))
    params->active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));

  params->xthickness = style->xthickness;
  params->ythickness = style->ythickness;

  if (widget)
    equinox_get_parent_bg (widget, &params->parentbg);


}

static void
equinox_style_draw_flat_box (DRAW_ARGS) {

	if (DETAIL ("tooltip")) {
		WidgetParameters params;
		EquinoxStyle *equinox_style;
		EquinoxColors *colors;
		cairo_t *cr;

		CHECK_ARGS
		SANITIZE_SIZE
		equinox_style = EQUINOX_STYLE (style);
		equinox_set_widget_parameters (widget, style, state_type, &params);
		colors = &equinox_style->colors;
		cr = equinox_begin_paint (window, area);

		equinox_draw_tooltip (cr, colors, &params, x, y, width, height);
		cairo_destroy (cr);
	} else if (DETAIL ("text")) {
	} else if (detail && state_type == GTK_STATE_SELECTED && (!strncmp ("cell_even", detail, 9) || !strncmp ("cell_odd", detail, 8))) {
		WidgetParameters params;
		EquinoxStyle *equinox_style;
		EquinoxColors *colors;
		cairo_t *cr;

		CHECK_ARGS
		SANITIZE_SIZE
		equinox_style = EQUINOX_STYLE (style);

		equinox_set_widget_parameters (widget, style, state_type, &params);
		colors = &equinox_style->colors;

		cr = equinox_begin_paint (window, area);
		equinox_draw_list_selection (cr, colors, &params, x, y, width, height);
		cairo_destroy (cr);
	} else if (DETAIL ("expander") || DETAIL ("checkbutton")) {
		WidgetParameters params;
		EquinoxStyle *equinox_style;
		EquinoxColors *colors;
		cairo_t *cr;

		CHECK_ARGS
		SANITIZE_SIZE
		equinox_style = EQUINOX_STYLE (style);

		equinox_set_widget_parameters (widget, style, state_type, &params);
		colors = &equinox_style->colors;
		cr = equinox_begin_paint (window, area);

		cairo_translate (cr, x, y);

		// Focus colour lighter than parentbg
		EquinoxRGB focus_color;
		equinox_shade (&params.parentbg, &focus_color, equinox_get_lightness(&params.parentbg) < 0.6 ? 1.24 : 1.05);
		cairo_set_source_rgb (cr, focus_color.r, focus_color.g, focus_color.b);
		clearlooks_rounded_rectangle (cr, 0, 0, width, height, params.curvature + 2, EQX_CORNER_ALL);
		cairo_fill (cr);

		cairo_destroy (cr);
	} else if (detail && (strncmp ("cell_even", detail, 9) || strncmp ("cell_odd", detail, 8)) && widget && EQUINOX_IS_TREE_VIEW(widget)) {
		WidgetParameters params;
		EquinoxStyle *equinox_style;
		EquinoxColors *colors;
		cairo_t *cr;

		CHECK_ARGS
		SANITIZE_SIZE
		equinox_style = EQUINOX_STYLE (style);
		if (equinox_style->listviewitemstyle == 1 && GTK_IS_TREE_VIEW(widget) && gtk_tree_view_get_rules_hint(GTK_TREE_VIEW(widget))) {
			equinox_set_widget_parameters (widget, style, state_type, &params);
			colors = &equinox_style->colors;

			cr = equinox_begin_paint (window, area);
			equinox_draw_list_item (cr, colors, &params, x, y, width, height, equinox_style->listviewitemstyle);
			cairo_destroy (cr);
		} else  {
			equinox_parent_class->draw_flat_box (style, window, state_type,	shadow_type, area, widget, detail, x, y, width, height);
		}
	} else {
		equinox_parent_class->draw_flat_box (style, window, state_type,	shadow_type, area, widget, detail, x, y, width, height);
	}
}

static void equinox_style_draw_shadow (DRAW_ARGS) {
	EquinoxStyle *equinox_style = EQUINOX_STYLE (style);
	EquinoxColors *colors = &equinox_style->colors;
	cairo_t *cr;

	CHECK_ARGS
	SANITIZE_SIZE
	cr = equinox_begin_paint (window, area);


	if (((DETAIL ("entry") && !(widget && EQUINOX_IS_TREE_VIEW (widget->parent))) ||
	    (DETAIL ("frame") && equinox_is_in_combo_box (widget))) && shadow_type != GTK_SHADOW_NONE) {
		WidgetParameters params;
		EntryParameters entry;
		//entry.isCombo = FALSE;
		entry.type = EQX_ENTRY_NORMAL;
		entry.isComboButton = FALSE;
		entry.isSpinButton = FALSE;
		if (equinox_is_toolbar_item (widget))
			entry.isToolbarItem = TRUE;
		else
			entry.isToolbarItem = FALSE;

		equinox_set_widget_parameters (widget, style, state_type, &params);

		if (EQUINOX_IS_ENTRY (widget))
			params.state_type = GTK_WIDGET_STATE (widget);

		params.active = (params.state_type == GTK_STATE_ACTIVE);
		params.prelight = (params.state_type == GTK_STATE_PRELIGHT);
		params.disabled = (params.state_type == GTK_STATE_INSENSITIVE);
		//params.curvature = 0;

		if (widget && (EQUINOX_IS_COMBO (widget->parent) ||
				EQUINOX_IS_COMBO_BOX_ENTRY (widget->parent) ||
				EQUINOX_IS_SPIN_BUTTON (widget))) {
			//As the enter key for a GtkCombo makes the list appear, have the focus drawn for the button to.
			if (EQUINOX_IS_COMBO (widget->parent))
				gtk_widget_queue_draw (((GtkCombo*)widget->parent)->button);


			width += style->xthickness;
			if (!params.ltr)
				x -= style->xthickness;

			if (params.ltr)
				params.corners = EQX_CORNER_TOPLEFT | EQX_CORNER_BOTTOMLEFT;
			else
				params.corners = EQX_CORNER_TOPRIGHT | EQX_CORNER_BOTTOMRIGHT;

			if (EQUINOX_IS_SPIN_BUTTON (widget))
				entry.type = EQX_ENTRY_SPINBUTTON;
			else
				entry.type = EQUINOX_IS_COMBO (widget->parent) ? EQX_ENTRY_COMBO : EQX_ENTRY_COMBO_BOX;

			//entry.isCombo = TRUE;
		}

		equinox_draw_entry (cr, colors, &params, &entry, x, y, width, height, equinox_style->toolbarstyle);
	} else if (DETAIL ("frame") && widget && EQUINOX_IS_STATUSBAR (widget->parent) && shadow_type != GTK_SHADOW_NONE) {
		WidgetParameters params;
		equinox_set_widget_parameters (widget, style, state_type, &params);
		SeparatorParameters separator;
		separator.horizontal = TRUE;

		cairo_save(cr);
		cairo_translate (cr, x, y);

		cairo_set_source_rgb (cr, colors->bg[GTK_STATE_NORMAL].r, colors->bg[GTK_STATE_NORMAL].g, colors->bg[GTK_STATE_NORMAL].b);

		cairo_rectangle (cr, 0, 0, width, height);
		cairo_fill (cr);

		cairo_restore(cr);
		// Uncomment the following line to draw a separator at the top of the statusbar
		//equinox_draw_separator (cr, colors, &params, &separator, x, y, width, 2);
	} else if (widget && widget->parent && widget->parent->parent && EQUINOX_IS_COLOR_BUTTON (widget->parent->parent)) {
		EquinoxRGB border;
		GdkColor button_color;

		gtk_color_button_get_color ((GtkColorButton *) widget->parent->parent, &button_color);

		border.r = colors->shade[8].r * (0.85) +(button_color.red / (double) 65535) * 0.15;
		border.g = colors->shade[8].g * (0.85) +(button_color.green / (double) 65535) * 0.15;
		border.b = colors->shade[8].b * (0.85) +(button_color.blue / (double) 65535) * 0.15;

		cairo_set_source_rgb (cr, border.r, border.g, border.b);
		clearlooks_rounded_rectangle (cr, x + 1.5, y + 1.5, width - 3, height - 3, 2, EQX_CORNER_ALL);
		cairo_stroke (cr);

		equinox_shade (&border, &border, 0.875);
		cairo_set_source_rgba (cr, border.r, border.g, border.b, 0.2);
		clearlooks_rounded_rectangle (cr, x + 2.5, y + 2.5, width - 3, height - 3, 3, EQX_CORNER_ALL);
		cairo_stroke (cr);
	} else if (DETAIL ("pager") || DETAIL ("pager-frame")) {
    EquinoxRGB border;

		equinox_mix_color (&colors->shade[7], &colors->bg[GTK_STATE_SELECTED], 0.15, &border);
		cairo_set_source_rgb (cr, border.r, border.g, border.b);
		clearlooks_rounded_rectangle (cr, x + 0.5, y + 0.5, width - 1, height - 1, 2, EQX_CORNER_ALL);
		cairo_stroke (cr);
	} else if (DETAIL ("frame") || DETAIL ("scrolled_window") || DETAIL ("viewport") || detail == NULL) {
		if (widget && !g_str_equal ("XfcePanelWindow", gtk_widget_get_name (gtk_widget_get_toplevel (widget))) && shadow_type != GTK_SHADOW_NONE) {
			cairo_rectangle (cr, x + 0.5, y + 0.5, width - 1, height - 1);
			cairo_set_source_rgb (cr, colors->shade[FRAME_SHADE].r, colors->shade[FRAME_SHADE].g, colors->shade[FRAME_SHADE].b);
			cairo_stroke (cr);
		}
	} else if (shadow_type != GTK_SHADOW_NONE) {
		cairo_rectangle (cr, x + 0.5, y + 0.5, width - 1, height - 1);
		cairo_set_source_rgb (cr, colors->shade[FRAME_SHADE].r, colors->shade[FRAME_SHADE].g, colors->shade[FRAME_SHADE].b);
		cairo_stroke (cr);
	}
	cairo_destroy (cr);
}

static void equinox_style_draw_box_gap (DRAW_ARGS, GtkPositionType gap_side, gint gap_x, gint gap_width) {
	EquinoxStyle *equinox_style = EQUINOX_STYLE (style);
	EquinoxColors *colors = &equinox_style->colors;
	cairo_t *cr;

	CHECK_ARGS
	SANITIZE_SIZE
	cr = equinox_begin_paint (window, area);

	if (DETAIL ("notebook")) {
		WidgetParameters params;
		FrameParameters frame;

		frame.shadow = shadow_type;
		frame.gap_side = gap_side;
		//frame.gap_x = gap_x;
		//frame.gap_width = gap_width;
		frame.gap_x = -1;
		frame.gap_width = 0;
		frame.border = &colors->shade[FRAME_SHADE];
		frame.fill = &colors->bg[state_type];
		frame.use_fill = TRUE;
		frame.fill_bg = TRUE;
		equinox_set_widget_parameters (widget, style, state_type, &params);
		params.curvature = MIN(params.curvature, MIN(params.xthickness, params.ythickness) + 1.5);

		//Don't round top corner when the border tab is selected.
		int current_page = gtk_notebook_get_current_page ((GtkNotebook *) widget);
		int num_pages = gtk_notebook_get_n_pages ((GtkNotebook *) widget);

		frame.gap_width -= (current_page == num_pages - 1) ? 2 : 0;

		if (frame.gap_side == EQX_GAP_TOP) {
			if ((current_page == 0) && (num_pages - 1 == 0)) {
					params.corners = EQX_CORNER_BOTTOMRIGHT | EQX_CORNER_BOTTOMLEFT;
			} else if (current_page == 0) {
				if(params.ltr)
					params.corners =EQX_CORNER_BOTTOMRIGHT | EQX_CORNER_BOTTOMLEFT | EQX_CORNER_TOPRIGHT;
				else
					params.corners =EQX_CORNER_BOTTOMRIGHT | EQX_CORNER_BOTTOMLEFT | EQX_CORNER_TOPLEFT;
			} else if (current_page == num_pages - 1) {
				if(params.ltr)
					params.corners = EQX_CORNER_BOTTOMRIGHT | EQX_CORNER_BOTTOMLEFT | EQX_CORNER_TOPLEFT;
				else
					params.corners =EQX_CORNER_BOTTOMRIGHT | EQX_CORNER_BOTTOMLEFT | EQX_CORNER_TOPRIGHT;
			} else
				params.corners = EQX_CORNER_ALL;
		} else if (frame.gap_side == EQX_GAP_BOTTOM) {
			if ((current_page == 0) && (num_pages - 1 == 0)) {
				params.corners = EQX_CORNER_TOPRIGHT | EQX_CORNER_TOPLEFT;
			} else if (current_page == 0) {
				if(params.ltr)
					params.corners = EQX_CORNER_TOPRIGHT | EQX_CORNER_TOPLEFT | EQX_CORNER_BOTTOMRIGHT;
				else
					params.corners = EQX_CORNER_TOPRIGHT | EQX_CORNER_BOTTOMLEFT | EQX_CORNER_TOPLEFT;
			} else if (current_page == num_pages - 1) {
				if(params.ltr)
					params.corners = EQX_CORNER_TOPRIGHT | EQX_CORNER_BOTTOMLEFT | EQX_CORNER_TOPLEFT;
				else
					params.corners = EQX_CORNER_TOPRIGHT | EQX_CORNER_TOPLEFT | EQX_CORNER_BOTTOMRIGHT;
			} else
				params.corners = EQX_CORNER_ALL;
		} else if (frame.gap_side == EQX_GAP_LEFT) {
			if ((current_page == 0) && (num_pages - 1 == 0)) {
				params.corners = EQX_CORNER_TOPRIGHT | EQX_CORNER_BOTTOMRIGHT;
			} else if (current_page == 0) {
				params.corners = EQX_CORNER_BOTTOMLEFT | EQX_CORNER_TOPRIGHT | EQX_CORNER_BOTTOMRIGHT;
			} else if (current_page == num_pages - 1) {
				params.corners = EQX_CORNER_TOPRIGHT | EQX_CORNER_TOPLEFT | EQX_CORNER_BOTTOMRIGHT;
			} else
				params.corners = EQX_CORNER_ALL;
		} else {			//Right
			if ((current_page == 0) && (num_pages - 1 == 0)) {
				params.corners = EQX_CORNER_TOPLEFT | EQX_CORNER_BOTTOMLEFT;
			} else if (current_page == 0) {
				params.corners = EQX_CORNER_BOTTOMLEFT | EQX_CORNER_TOPLEFT | EQX_CORNER_BOTTOMRIGHT;
			} else if (current_page == num_pages - 1) {
				params.corners = EQX_CORNER_TOPRIGHT | EQX_CORNER_TOPLEFT | EQX_CORNER_BOTTOMLEFT;
			} else
				params.corners = EQX_CORNER_ALL;
		}

		/*if (params.curvature < 2)
			params.corners = EQX_CORNER_NONE;*/

		if (gtk_notebook_get_show_border ((GtkNotebook*)widget) || gtk_notebook_get_show_tabs ((GtkNotebook*)widget))
			equinox_draw_frame (cr, colors, &params, &frame, x, y, width, height);

	} else
		equinox_parent_class->draw_box_gap (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height, gap_side, gap_x, gap_width);

  	cairo_destroy (cr);
}

static void equinox_style_draw_extension (DRAW_ARGS, GtkPositionType gap_side) {
	EquinoxStyle *equinox_style = EQUINOX_STYLE (style);
	EquinoxColors *colors = &equinox_style->colors;
	cairo_t *cr;

	CHECK_ARGS
	SANITIZE_SIZE
	cr = equinox_begin_paint (window, area);

	if (DETAIL ("tab"))	{
		WidgetParameters params;
		TabParameters tab;

		equinox_set_widget_parameters (widget, style, state_type, &params);
		tab.gap_side = (EquinoxGapSide) gap_side;

		int current_page = gtk_notebook_get_current_page ((GtkNotebook *) widget);
		int num_of_pages = gtk_notebook_get_n_pages ((GtkNotebook *) widget);

		if (current_page == 0)
			tab.first_tab = (params.ltr || (tab.gap_side == EQX_GAP_LEFT || tab.gap_side == EQX_GAP_RIGHT)) ? TRUE : FALSE;
		else
			tab.first_tab = (params.ltr || (tab.gap_side == EQX_GAP_LEFT || tab.gap_side == EQX_GAP_RIGHT)) ? FALSE : TRUE;
		if (num_of_pages - 1 == current_page)
			tab.last_tab = (params.ltr || (tab.gap_side == EQX_GAP_LEFT || tab.gap_side == EQX_GAP_RIGHT)) ? TRUE : FALSE;
		else
			tab.last_tab = (params.ltr || (tab.gap_side == EQX_GAP_LEFT || tab.gap_side == EQX_GAP_RIGHT)) ? FALSE : TRUE;

		if (num_of_pages == 1)
			tab.last_tab = tab.first_tab = TRUE;

		if (gtk_notebook_get_show_tabs((GtkNotebook*)widget))
			equinox_draw_tab (cr, colors, &params, &tab, x, y, width, height);

	} else {
		equinox_parent_class->draw_extension (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height, gap_side);

	}
	cairo_destroy (cr);
}

static void equinox_style_draw_handle (DRAW_ARGS, GtkOrientation orientation) {

	EquinoxStyle *equinox_style = EQUINOX_STYLE (style);
	EquinoxColors *colors = &equinox_style->colors;
	cairo_t *cr;
	gboolean is_horizontal;

	CHECK_ARGS
	SANITIZE_SIZE
	cr = equinox_begin_paint (window, area);

	WidgetParameters params;
	HandleParameters handle;

	equinox_set_widget_parameters (widget, style, state_type, &params);

	// Evil hack to work around broken orientation for toolbars
	is_horizontal = (width > height);

	if (DETAIL ("handlebox")) {
		handle.type = EQX_HANDLE_TOOLBAR;
		handle.horizontal = is_horizontal;

    // Is this ever true? -Daniel
		if (EQUINOX_IS_TOOLBAR (widget) && shadow_type != GTK_SHADOW_NONE) {
			cairo_save (cr);
			equinox_draw_toolbar (cr, colors, &params, x, y, width, height, equinox_style->toolbarstyle);
			cairo_restore (cr);
		}

		equinox_draw_handle (cr, colors, &params, &handle, x, y, width, height);
	} else if (DETAIL ("paned")) {
		handle.type = EQX_HANDLE_SPLITTER;
		handle.horizontal = orientation == GTK_ORIENTATION_HORIZONTAL;

		equinox_draw_handle (cr, colors, &params, &handle, x, y, width, height);
	} else {
		handle.type = EQX_HANDLE_TOOLBAR;
		handle.horizontal = is_horizontal;

		if (EQUINOX_IS_TOOLBAR (widget) && shadow_type != GTK_SHADOW_NONE) {
			cairo_save (cr);
			equinox_draw_toolbar (cr, colors, &params, x, y, width, height, equinox_style->toolbarstyle);
			cairo_restore (cr);
		}

		equinox_draw_handle (cr, colors, &params, &handle, x, y, width, height);
  }
	cairo_destroy (cr);
}

static void equinox_style_draw_box (DRAW_ARGS) {
	EquinoxStyle *equinox_style = EQUINOX_STYLE (style);
	EquinoxColors *colors = &equinox_style->colors;

	CHECK_ARGS
	SANITIZE_SIZE
	cairo_t *cr = equinox_begin_paint (window, area);


	if (DETAIL ("menubar") && !equinox_is_panel_widget_item(widget)) {
		WidgetParameters params;
		equinox_set_widget_parameters (widget, style, state_type, &params);
		equinox_draw_menubar (cr, colors, &params, x, y, width, height, equinox_style->menubarstyle, equinox_style->menubarborders);
	} else if (DETAIL ("button") && widget && widget->parent && (EQUINOX_IS_TREE_VIEW(widget->parent) || EQUINOX_IS_CLIST (widget->parent)  || equinox_object_is_a (G_OBJECT (widget->parent), "ETree") || /* ECanvas inside ETree */ equinox_object_is_a (G_OBJECT (widget->parent), "ETable") /* ECanvas inside ETable */ )) {
		WidgetParameters params;
		ListViewHeaderParameters header;

		gint columns, column_index;
		gboolean resizable = TRUE;
		gboolean sorted = FALSE;
		columns = 3;
		column_index = 1;

		equinox_set_widget_parameters (widget, style, state_type, &params);
		params.corners = EQX_CORNER_NONE;

		if (EQUINOX_IS_TREE_VIEW (widget->parent))
			equinox_gtk_treeview_get_header_index (GTK_TREE_VIEW (widget->parent), widget, &column_index, &columns, &resizable, &sorted);
		else if (EQUINOX_IS_CLIST (widget->parent))
			equinox_gtk_clist_get_header_index (GTK_CLIST (widget->parent), widget, &column_index, &columns);

		header.resizable = resizable;
		header.sorted = sorted;

		if (column_index == 0)
			header.order = EQX_ORDER_FIRST;
		else if (column_index == columns - 1)
			header.order = EQX_ORDER_LAST;
		else
			header.order = EQX_ORDER_MIDDLE;

    	gtk_style_apply_default_background (style, window, FALSE, state_type, area, x, y, width, height);

#ifdef HAVE_ANIMATION
		if (equinox_style->animation) {
			equinox_animation_connect_button (widget);
			params.prev_state_type = equinox_animation_starting_state (widget);
		}

		if (equinox_style->animation && GTK_IS_BUTTON (widget) && equinox_animation_is_animated (widget)) {
			gfloat elapsed = equinox_animation_elapsed (widget);
			params.prev_state_type = equinox_animation_starting_state (widget);
			params.trans = sqrt (sqrt (MIN(elapsed / ANIMATION_TRANS_TIME, 1.0)));
		}
#endif

		equinox_draw_list_view_header (cr, colors, &params, &header, x, y, width, height, equinox_style->listviewheaderstyle);

	} else if (DETAIL ("button") || DETAIL ("buttondefault")) {
		WidgetParameters params;
		equinox_set_widget_parameters (widget, style, state_type, &params);

#ifdef HAVE_ANIMATION
		if (equinox_style->animation && !(widget && widget->parent && widget->parent->parent && EQUINOX_IS_PANEL_WIDGET(widget->parent->parent))) {
			equinox_animation_connect_button (widget);
			params.prev_state_type = equinox_animation_starting_state (widget);
		}

		if (equinox_style->animation && GTK_IS_BUTTON (widget) && equinox_animation_is_animated (widget)) {
			gfloat elapsed = equinox_animation_elapsed (widget);
			params.prev_state_type = equinox_animation_starting_state (widget);
			params.trans = sqrt (sqrt (MIN(elapsed / ANIMATION_TRANS_TIME, 1.0)));
		}
#endif

		if ((widget && (EQUINOX_IS_COMBO_BOX_ENTRY (widget->parent) || EQUINOX_IS_COMBO (widget->parent)))) {

			//As the enter key for a GtkCombo makes the list appear, have the focus drawn for the button too.
			if (EQUINOX_IS_COMBO (widget->parent))
				params.focus |= GTK_WIDGET_HAS_FOCUS (((GtkCombo*)widget->parent)->entry);

			if (params.ltr) {
				params.corners = EQX_CORNER_TOPRIGHT | EQX_CORNER_BOTTOMRIGHT;
				x -= 2;
				width += 2;
			}
			else {
				params.corners = EQX_CORNER_TOPLEFT | EQX_CORNER_BOTTOMLEFT;
				width += 2;
			}
		}

		if (EQUINOX_IS_TOGGLE_BUTTON (widget) && gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
			params.active = TRUE;

		//
		/*if (EQUINOX_IS_BUTTON (widget) && EQUINOX_IS_FIXED (widget->parent) && widget->allocation.x == -1 &&  widget->allocation.y == -1)
				gtk_style_apply_default_background (widget->parent->style, window, TRUE, GTK_STATE_NORMAL, area, x, y, width, height);*/

		if (EQUINOX_IS_PANEL_WIDGET(widget->parent->parent)) {	//for task list
			params.corners = EQX_CORNER_TOPLEFT | EQX_CORNER_TOPRIGHT;
			height += 3;
		}

		//For special buttons like tab close buttons, reduce curvature to sane amount.
		//Increase width/height as well as focus won't be drawn
		//Haven't confirmed if some apps use x/ythickness = 0 for other reasons and this could backfire.
		if (params.xthickness <= 0 || params.ythickness <= 0) {
			x--;
			y--;
			width+=2;
		  	height+=2;
	  		params.curvature = MIN (params.curvature, 2.5);
		}

		if (EQUINOX_IS_COMBO_BOX_ENTRY (widget->parent) || EQUINOX_IS_COMBO (widget->parent)) {
			EntryParameters entry;
			entry.type = EQUINOX_IS_COMBO_BOX_ENTRY (widget->parent) ? EQX_ENTRY_COMBO_BOX : EQX_ENTRY_COMBO;
			entry.isComboButton = TRUE;
			entry.isSpinButton = FALSE;
			entry.isToolbarItem = TRUE;
			equinox_draw_entry (cr, &equinox_style->colors, &params, &entry, x, y, width, height, equinox_style->toolbarstyle);
		} else {
	  	equinox_draw_button (cr, &equinox_style->colors, &params, x, y, width, height, equinox_style->buttonstyle);
	  }
	} else if (DETAIL ("spinbutton_up") || DETAIL ("spinbutton_down")) {

		WidgetParameters params;
		equinox_set_widget_parameters (widget, style, state_type, &params);

		EntryParameters entry;
		entry.type = EQX_ENTRY_SPINBUTTON;
		entry.isComboButton = FALSE;
		entry.isSpinButton = TRUE;
		if (equinox_is_toolbar_item (widget))
			entry.isToolbarItem = TRUE;
		else
			entry.isToolbarItem = FALSE;

	  	//Only draw button as disabled if entire widget is disabled
	  	if (params.disabled && !(GTK_WIDGET_STATE(widget) == GTK_STATE_INSENSITIVE) ) {
			params.disabled = FALSE;
			params.state_type = GTK_STATE_NORMAL;
		}

		if (params.ltr) {
			x -= 2;
			width += 2;
			params.corners = EQX_CORNER_TOPRIGHT | EQX_CORNER_BOTTOMRIGHT;
		} else {
			width += 2;
			params.corners = EQX_CORNER_TOPLEFT | EQX_CORNER_BOTTOMLEFT;
		}

		/*more faking of real transparancy.*/
		cairo_set_source_rgb (cr, params.parentbg.r, params.parentbg.g, params.parentbg.b);
		cairo_rectangle (cr, x, y, width, height);
		cairo_fill_preserve(cr);
		cairo_clip(cr);

	 	if (DETAIL ("spinbutton_up")) {
			equinox_draw_entry (cr, &equinox_style->colors, &params, &entry, x, y, width, height*2, equinox_style->toolbarstyle);
		} else {
			equinox_draw_entry (cr, &equinox_style->colors, &params, &entry, x, y - height, width, height*2, equinox_style->toolbarstyle);
		}
  	} else if (DETAIL ("spinbutton")) {
	} else if (DETAIL ("trough") && widget && GTK_IS_SCALE (widget)) {
		GtkAdjustment *adjustment = gtk_range_get_adjustment (GTK_RANGE (widget));
		WidgetParameters params;
		SliderParameters slider;
		gint slider_length;

		gtk_widget_style_get (widget, "slider-length", &slider_length, NULL);
		equinox_set_widget_parameters (widget, style, state_type, &params);

		slider.inverted = gtk_range_get_inverted (GTK_RANGE (widget));
		slider.horizontal = (GTK_RANGE (widget)->orientation == GTK_ORIENTATION_HORIZONTAL);
		slider.fill_size = ((slider.horizontal ? width : height) - slider_length) * (1 / ((adjustment->upper - adjustment->lower) / (adjustment->value - adjustment->lower)));
		if (slider.horizontal)
		  slider.inverted = slider.inverted != (equinox_get_direction (widget) == GTK_TEXT_DIR_RTL);

		if (slider.horizontal) {
			  x += slider_length / 2;
			  width -= slider_length;
		} else {
			  y += slider_length / 2;
			  height -= slider_length;
		}

		equinox_draw_scale_trough (cr, &equinox_style->colors, &params, &slider, x, y, width, height);
  	} else if (DETAIL ("trough") && widget && EQUINOX_IS_PROGRESS_BAR (widget)) {
		WidgetParameters params;
		ProgressBarParameters progressbar;
		equinox_set_widget_parameters (widget, style, state_type, &params);
		//params.curvature = 0;

		progressbar.orientation = gtk_progress_bar_get_orientation (GTK_PROGRESS_BAR (widget));

    	equinox_draw_progressbar_trough (cr, colors, &params, &progressbar, x, y, width, height, equinox_style->progressbarstyle);
	} else if ((DETAIL ("trough") && widget && (EQUINOX_IS_VSCROLLBAR (widget) || EQUINOX_IS_HSCROLLBAR (widget))) || DETAIL ("stepper")) {
		WidgetParameters params;
		ScrollBarParameters scrollbar;

		equinox_set_widget_parameters (widget, style, state_type, &params);
		params.curvature = 0;
		scrollbar.horizontal = TRUE;
		//scrollbar.junction = equinox_scrollbar_get_junction (widget);
		//scrollbar.steppers = equinox_scrollbar_visible_steppers (widget);
		if (EQUINOX_IS_RANGE (widget))
			scrollbar.horizontal = GTK_RANGE (widget)->orientation == GTK_ORIENTATION_HORIZONTAL;
		equinox_draw_scrollbar_trough (cr, colors, &params, &scrollbar, x, y, width, height, equinox_style->scrollbarstyle, equinox_style->scrollbartrough);
	} else if (DETAIL ("bar")) {
		WidgetParameters params;
		ProgressBarParameters progressbar;
		gdouble elapsed = 0.0;

#ifdef HAVE_ANIMATION
		if (equinox_style->animation && EQX_IS_PROGRESS_BAR (widget)) {
			gboolean activity_mode = GTK_PROGRESS (widget)->activity_mode;
			if (!activity_mode)
				equinox_animation_progressbar_add ((gpointer) widget);
		}
		elapsed = equinox_animation_elapsed (widget);
#endif

		equinox_set_widget_parameters (widget, style, state_type, &params);
		//params.curvature = 0;


		if (widget && EQUINOX_IS_PROGRESS_BAR (widget)) {
			progressbar.orientation = gtk_progress_bar_get_orientation (GTK_PROGRESS_BAR (widget));
			progressbar.fraction = gtk_progress_bar_get_fraction (GTK_PROGRESS_BAR (widget));
		} else
			progressbar.orientation = EQX_ORIENTATION_LEFT_TO_RIGHT;

		equinox_draw_progressbar_fill (cr, colors, &params, &progressbar, x - 1, y, width + 2, height, 10 - (int) (elapsed * 10) % 10, equinox_style->progressbarstyle);
	} else if (DETAIL ("optionmenu")) {
		WidgetParameters params;
		equinox_set_widget_parameters (widget, style, state_type, &params);

#ifdef HAVE_ANIMATION
		if (equinox_style->animation) {
			equinox_animation_connect_button (widget);
			params.prev_state_type = equinox_animation_starting_state (widget);
		}

		if (equinox_style->animation && GTK_IS_BUTTON (widget) && equinox_animation_is_animated (widget)) {
			gfloat elapsed = equinox_animation_elapsed (widget);
			params.prev_state_type = equinox_animation_starting_state (widget);
			params.trans = sqrt (sqrt (MIN(elapsed / ANIMATION_TRANS_TIME, 1.0)));
		}
#endif

		equinox_draw_button (cr, colors, &params, x, y, width, height, equinox_style->buttonstyle);

	} else if (DETAIL ("menuitem")) {
		WidgetParameters params;

		equinox_set_widget_parameters (widget, style, state_type, &params);
		params.corners = EQX_CORNER_NONE;

		//If parent is menubar draw top corners rounded else draw normal
		if (widget && EQUINOX_IS_MENU_BAR (widget->parent)) {
			params.corners = EQX_CORNER_TOPRIGHT | EQX_CORNER_TOPLEFT;
			params.curvature = (params.curvature > height*0.2) ? height*0.2 : params.curvature;
			height++;			//hide bottom border so we don't get a double border with menu
			equinox_draw_menubar_item (cr, colors, &params, x, y, width, height, equinox_style->menubaritemstyle, EQUINOX_IS_PANEL_WIDGET(widget->parent->parent));
		}
		else {
			params.corners = EQX_CORNER_ALL;
			params.curvature = 0;
			equinox_draw_menuitem (cr, colors, &params, x, y, width, height, equinox_style->menuitemstyle);
		}
	//} else if (DETAIL ("hscrollbar") || DETAIL ("vscrollbar") || DETAIL ("slider") /*|| DETAIL ("stepper")*/) {
	} else if (DETAIL ("hscrollbar") || DETAIL ("vscrollbar") || DETAIL ("slider")) {
		WidgetParameters params;
		ScrollBarParameters scrollbar;

		equinox_set_widget_parameters (widget, style, state_type, &params);

		params.corners = EQX_CORNER_NONE;
		scrollbar.has_color = FALSE;
		scrollbar.horizontal = TRUE;
		scrollbar.style = equinox_style->scrollbarstyle;

		if (widget && GTK_IS_RANGE (widget))
			scrollbar.horizontal = (GTK_RANGE (widget)->orientation == GTK_ORIENTATION_HORIZONTAL);

		if (equinox_style->has_scrollbar_color) {
			equinox_gdk_color_to_rgb (&equinox_style->scrollbar_color, &scrollbar.color.r, &scrollbar.color.g, &scrollbar.color.b);
			scrollbar.has_color = TRUE;
		}

		if (DETAIL ("slider")) {
			if (params.curvature >= 1)
				params.corners = EQX_CORNER_ALL;
			else
				params.corners = EQX_CORNER_NONE;
			equinox_draw_scrollbar_slider (cr, colors, &params, &scrollbar, x, y, width, height, equinox_style->scrollbarstyle, equinox_style->scrollbarmargin);
		}
	} else if (DETAIL ("toolbar") || DETAIL ("handlebox_bin") || DETAIL ("dockitem_bin")) {
		// Only draw the shadows on horizontal toolbars
		if (shadow_type != GTK_SHADOW_NONE && height < 2 * width)
			equinox_draw_toolbar (cr, colors, NULL, x, y, width, height, equinox_style->toolbarstyle);
	} else if (DETAIL ("trough")) {
	} else if (DETAIL ("menu")) {
		WidgetParameters params;
		equinox_set_widget_parameters (widget, style, state_type, &params);
		equinox_draw_menu_frame (cr, colors, &params, x, y, width, height);
	} else {
		equinox_parent_class->draw_box (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height);
	}
	cairo_destroy (cr);
}

static void equinox_style_draw_slider (DRAW_ARGS, GtkOrientation orientation) {
	if (DETAIL ("hscale") || DETAIL ("vscale"))	{
		EquinoxStyle *equinox_style = EQUINOX_STYLE (style);
		EquinoxColors *colors = &equinox_style->colors;
		cairo_t *cr;

		CHECK_ARGS
		SANITIZE_SIZE
		cr = equinox_begin_paint (window, area);

		WidgetParameters params;
		SliderParameters slider;

		equinox_set_widget_parameters (widget, style, state_type, &params);
		params.curvature = MIN (MIN(height,width)*0.147, params.curvature);
		slider.horizontal = TRUE;
    	slider.horizontal = DETAIL ("hscale");

		if (!params.disabled)
			equinox_draw_scale_slider (cr, colors, &params, &slider, x, y, width, height, equinox_style->scalesliderstyle);

		cairo_destroy (cr);
	} else {
		equinox_parent_class->draw_slider (style, window, state_type, shadow_type,
				      area, widget, detail, x, y, width,
				      height, orientation);
	}
}

static void equinox_style_draw_option (DRAW_ARGS) {
	EquinoxStyle *equinox_style = EQUINOX_STYLE (style);
	EquinoxColors *colors = &equinox_style->colors;
	cairo_t *cr;
	WidgetParameters params;

	CHECK_ARGS
	SANITIZE_SIZE
	cr = equinox_begin_paint (window, area);

	equinox_set_widget_parameters (widget, style, state_type, &params);
	OptionParameters option;
	option.draw_bullet = ((shadow_type == GTK_SHADOW_IN) || (shadow_type == GTK_SHADOW_ETCHED_IN));
	option.inconsistent = (shadow_type == GTK_SHADOW_ETCHED_IN);

#ifdef HAVE_ANIMATION
	if (equinox_style->animation)
		equinox_animation_connect_checkbox (widget);

	if (equinox_style->animation && GTK_IS_CHECK_BUTTON (widget) && equinox_animation_is_animated (widget) && !gtk_toggle_button_get_inconsistent (GTK_TOGGLE_BUTTON (widget)))
	{
		gfloat elapsed = equinox_animation_elapsed (widget);
		params.trans = sqrt (sqrt (MIN(elapsed / CHECK_ANIMATION_TIME, 1.0)));
	}
#endif

	if (widget && EQUINOX_IS_MENU (widget->parent)) {
		x--;
		y--;
		equinox_draw_menu_radiobutton (cr, colors, &params, &option, x, y, width, height);
	}
	else if (DETAIL ("cellradio")) {
		x--;
		y--;
		equinox_draw_cell_radiobutton (cr, colors, &params, &option, x, y, width, height);
	}
	else {
		equinox_draw_radiobutton (cr, colors, &params, &option, x, y, width, height, equinox_style->checkradiostyle);
	}
  cairo_destroy (cr);
}

static void equinox_style_draw_check (DRAW_ARGS) {
	EquinoxStyle *equinox_style = EQUINOX_STYLE (style);
	EquinoxColors *colors = &equinox_style->colors;
	cairo_t *cr;
	WidgetParameters params;

	CHECK_ARGS
	SANITIZE_SIZE
	cr = equinox_begin_paint (window, area);

	equinox_set_widget_parameters (widget, style, state_type, &params);
	OptionParameters option;
	option.draw_bullet = ((shadow_type == GTK_SHADOW_IN) || (shadow_type == GTK_SHADOW_ETCHED_IN));
	option.inconsistent = (shadow_type == GTK_SHADOW_ETCHED_IN);

#ifdef HAVE_ANIMATION
	if (equinox_style->animation)
		equinox_animation_connect_checkbox (widget);

	if (equinox_style->animation && GTK_IS_CHECK_BUTTON (widget) && equinox_animation_is_animated (widget) && !gtk_toggle_button_get_inconsistent (GTK_TOGGLE_BUTTON (widget)))
	{
		gfloat elapsed = equinox_animation_elapsed (widget);
		params.trans = sqrt (sqrt (MIN(elapsed / CHECK_ANIMATION_TIME, 1.0)));
	}
#endif

	if (widget && EQUINOX_IS_MENU (widget->parent)) {
		equinox_draw_menu_checkbutton (cr, colors, &params, &option, x, y, width, height);
	}
	else if (DETAIL ("cellcheck")) {
		equinox_draw_cell_checkbutton (cr, colors, &params, &option, x, y, width, height);
	}
	else {
		equinox_draw_checkbutton (cr, colors, &params, &option, x, y, width, height, equinox_style->checkradiostyle);
	}
	cairo_destroy (cr);
}

static void equinox_style_draw_tab (DRAW_ARGS) {
	EquinoxStyle *equinox_style = EQUINOX_STYLE (style);
	EquinoxColors *colors = &equinox_style->colors;
	cairo_t *cr;
	WidgetParameters params;
	ArrowParameters arrow;

	CHECK_ARGS
	SANITIZE_SIZE
	cr = equinox_begin_paint (window, area);

	equinox_set_widget_parameters (widget, style, state_type, &params);
	arrow.type = EQX_ARROW_NORMAL;
	arrow.direction = EQX_DIRECTION_DOWN;
	arrow.size = equinox_style->arrowsize;

	equinox_draw_arrow (cr, colors, &params, &arrow, x, y, width, height);

	cairo_destroy (cr);
}

static void equinox_style_draw_vline (GtkStyle * style,
			 GdkWindow * window,
			 GtkStateType state_type,
			 GdkRectangle * area,
			 GtkWidget * widget,
			 const gchar * detail, gint y1, gint y2, gint x) {
	EquinoxStyle *equinox_style = EQUINOX_STYLE (style);
	EquinoxColors *colors = &equinox_style->colors;
	cairo_t *cr = equinox_begin_paint (window, area);

	SeparatorParameters separator;
	WidgetParameters params;
	separator.horizontal = FALSE;
	if (widget && widget->parent) {
		separator.inToolbar = EQUINOX_IS_TOOLBAR (widget->parent);
	} else {
		separator.inToolbar = FALSE;
	}
	equinox_set_widget_parameters (widget, style, state_type, &params);


	if (widget && widget->parent && widget->parent->parent && widget->parent->parent->parent &&
      EQUINOX_IS_HBOX (widget->parent) &&
      EQUINOX_IS_TOGGLE_BUTTON (widget->parent->parent) &&
      EQUINOX_IS_COMBO_BOX (widget->parent->parent->parent)) {
	}
	else
		//equinox_draw_separator (cr, colors, &params, &separator, x, y1, 2, y2 - y1, equinox_style->separatorstyle);

	cairo_destroy (cr);
}

static void equinox_style_draw_hline (GtkStyle * style,
			 GdkWindow * window,
			 GtkStateType state_type,
			 GdkRectangle * area,
			 GtkWidget * widget,
			 const gchar * detail, gint x1, gint x2, gint y) {
	WidgetParameters params;
	EquinoxStyle *equinox_style = EQUINOX_STYLE (style);
	EquinoxColors *colors = &equinox_style->colors;
	cairo_t *cr = equinox_begin_paint (window, area);

	equinox_set_widget_parameters (widget, style, state_type, &params);
	SeparatorParameters separator;
	separator.horizontal = TRUE;

	equinox_draw_separator (cr, colors, &params, &separator, x1, y, x2 - x1, 2, equinox_style->separatorstyle);

	cairo_destroy (cr);
}

static void equinox_style_draw_shadow_gap (DRAW_ARGS,
			      GtkPositionType gap_side,
			      gint gap_x, gint gap_width) {
	EquinoxStyle *equinox_style = EQUINOX_STYLE (style);
	EquinoxColors *colors = &equinox_style->colors;
	cairo_t *cr;

	CHECK_ARGS
	SANITIZE_SIZE
	cr = equinox_begin_paint (window, area);

	if (DETAIL ("frame")) {
		WidgetParameters params;
		FrameParameters frame;

		frame.shadow = shadow_type;
		frame.gap_side = gap_side;
		frame.gap_x = gap_x;
		frame.gap_width = gap_width;
		frame.border = (EquinoxRGB *) & colors->shade[FRAME_SHADE];
		frame.use_fill = FALSE;
		frame.fill_bg = TRUE;

		equinox_set_widget_parameters (widget, style, state_type, &params);
		params.curvature = MIN(params.curvature, MIN(params.xthickness, params.ythickness) + 1.5);

		equinox_draw_frame (cr, colors, &params, &frame, x-1, y-1, width+2, height+2);
	} else {
		equinox_parent_class->draw_shadow_gap (style, window, state_type,
					  shadow_type, area, widget, detail,
					  x, y, width, height, gap_side,
					  gap_x, gap_width);
	}
	cairo_destroy (cr);
}

static void equinox_style_draw_resize_grip (GtkStyle * style,
			       GdkWindow * window,
			       GtkStateType state_type,
			       GdkRectangle * area,
			       GtkWidget * widget,
			       const gchar * detail,
			       GdkWindowEdge edge,
			       gint x, gint y, gint width, gint height) {
	EquinoxStyle *equinox_style = EQUINOX_STYLE (style);
	EquinoxColors *colors = &equinox_style->colors;
	cairo_t *cr;

	CHECK_ARGS
	SANITIZE_SIZE
	cr = equinox_begin_paint (window, area);

	WidgetParameters params;
	ResizeGripParameters grip;

	equinox_set_widget_parameters (widget, style, state_type, &params);
	grip.edge = (EquinoxWindowEdge) edge;

	/*if (edge != GDK_WINDOW_EDGE_SOUTH_EAST)
		return;			// sorry... need to work on this :P*/

	equinox_draw_resize_grip (cr, colors, &params, &grip, x+2*params.xthickness, y+2*params.ythickness, width, height, equinox_style->resizegripstyle);

	cairo_destroy (cr);
}

static void equinox_style_draw_arrow (GtkStyle * style,
			 GdkWindow * window,
			 GtkStateType state_type,
			 GtkShadowType shadow,
			 GdkRectangle * area,
			 GtkWidget * widget,
			 const gchar * detail,
			 GtkArrowType arrow_type,
			 gboolean fill,
			 gint x, gint y, gint width, gint height) {
	EquinoxStyle *equinox_style = EQUINOX_STYLE (style);
	EquinoxColors *colors = &equinox_style->colors;
	cairo_t *cr = equinox_begin_paint (window, area);

	CHECK_ARGS
	SANITIZE_SIZE

	WidgetParameters params;
	ArrowParameters arrow;

	equinox_set_widget_parameters (widget, style, state_type, &params);
	arrow.type = EQX_ARROW_NORMAL;
	params.state_type = (EquinoxStateType) state_type;
	arrow.direction = (EquinoxDirection) arrow_type;

	if (arrow_type == (GtkArrowType) 4) {
		cairo_destroy (cr);
		return;
	}

	if (widget && widget->parent && widget->parent->parent && widget->parent->parent->parent
	    && GTK_IS_COMBO_BOX (widget->parent->parent->parent) &&
	    !(GTK_IS_COMBO_BOX_ENTRY (widget->parent->parent->parent)))
		x += 1;

	if (arrow.direction == EQX_DIRECTION_RIGHT)
		y += 1;
	else if (arrow.direction == EQX_DIRECTION_UP)
		x -= 1;

	if (DETAIL ("arrow")) {
		arrow.type = EQX_ARROW_COMBO;
	} else if ((DETAIL ("hscrollbar") || DETAIL ("vscrollbar"))) {
		arrow.type = EQX_ARROW_SCROLL;
		if (DETAIL ("vscrollbar")) {
			x++;
			width++;
		} else
			height++;
	} else if (DETAIL ("spinbutton")) {
		arrow.type = EQX_ARROW_SPINBUTTON;
		x += 2;
		if (arrow.direction == EQX_DIRECTION_UP)
			y += 1;
	}

	equinox_draw_arrow (cr, colors, &params, &arrow, x, y, width, height);

	cairo_destroy (cr);
}

static void equinox_style_init_from_rc (GtkStyle * style, GtkRcStyle * rc_style) {
	EquinoxStyle *equinox_style = EQUINOX_STYLE (style);

	equinox_parent_class->init_from_rc (style, rc_style);

	equinox_style->curvature = EQUINOX_RC_STYLE (rc_style)->curvature;
	equinox_style->menubarstyle = EQUINOX_RC_STYLE (rc_style)->menubarstyle;
	equinox_style->menubarborders = EQUINOX_RC_STYLE (rc_style)->menubarborders;
	equinox_style->menubaritemstyle = EQUINOX_RC_STYLE (rc_style)->menubaritemstyle;
	equinox_style->toolbarstyle = EQUINOX_RC_STYLE (rc_style)->toolbarstyle;
	equinox_style->buttonstyle = EQUINOX_RC_STYLE (rc_style)->buttonstyle;
	equinox_style->menuitemstyle = EQUINOX_RC_STYLE (rc_style)->menuitemstyle;
	equinox_style->listviewheaderstyle = EQUINOX_RC_STYLE (rc_style)->listviewheaderstyle;
	equinox_style->listviewitemstyle = EQUINOX_RC_STYLE (rc_style)->listviewitemstyle;
	equinox_style->scrollbarstyle = EQUINOX_RC_STYLE (rc_style)->scrollbarstyle;
	equinox_style->scrollbartrough = EQUINOX_RC_STYLE (rc_style)->scrollbartrough;
	equinox_style->scrollbarmargin = EQUINOX_RC_STYLE (rc_style)->scrollbarmargin;
	equinox_style->scrollbarpadding[0] = EQUINOX_RC_STYLE (rc_style)->scrollbarpadding[0];
	equinox_style->scrollbarpadding[1] = EQUINOX_RC_STYLE (rc_style)->scrollbarpadding[1];
	equinox_style->scrollbarpadding[2] = EQUINOX_RC_STYLE (rc_style)->scrollbarpadding[2];
	equinox_style->scrollbarpadding[3] = EQUINOX_RC_STYLE (rc_style)->scrollbarpadding[3];
	equinox_style->scrollbar_color = EQUINOX_RC_STYLE (rc_style)->scrollbar_color;
	equinox_style->scalesliderstyle = EQUINOX_RC_STYLE (rc_style)->scalesliderstyle;
	equinox_style->checkradiostyle = EQUINOX_RC_STYLE (rc_style)->checkradiostyle;
	equinox_style->progressbarstyle = EQUINOX_RC_STYLE (rc_style)->progressbarstyle;
	equinox_style->separatorstyle = EQUINOX_RC_STYLE (rc_style)->separatorstyle;
	equinox_style->resizegripstyle = EQUINOX_RC_STYLE (rc_style)->resizegripstyle;
	equinox_style->textstyle = EQUINOX_RC_STYLE (rc_style)->textstyle;
	equinox_style->textshade = EQUINOX_RC_STYLE (rc_style)->textshade;
	equinox_style->animation = EQUINOX_RC_STYLE (rc_style)->animation;
	equinox_style->arrowsize = EQUINOX_RC_STYLE (rc_style)->arrowsize;
}

static void equinox_style_realize (GtkStyle * style) {
	EquinoxStyle *equinox_style = EQUINOX_STYLE (style);

		//shade 1,2,6,7 not used.  0,3,4,5,8 used (something better could be done here
		//lots of variation is used for highlights but little in the mid and shadow tones...
		// highlights 1.175, 1.15, 1.1, 1.07, 1.03   and mids 0.82, 0.7, 0.665  and shadow 0.4 ???
		//more spot shades are desired too I think or maybe just adjust defaults...
	double shades[] = { 1.15, 1.04, 0.94, 0.80, 0.70, 0.64, 0.5, 0.45, 0.4 };

	EquinoxRGB spot_color;
	EquinoxRGB bg_normal;
	double contrast;
	int i;

	equinox_parent_class->realize (style);

	contrast = EQUINOX_RC_STYLE (style->rc_style)->contrast;

	equinox_gdk_color_to_cairo (&style->bg[GTK_STATE_NORMAL], &bg_normal);

	/* Lighter to darker */
	for (i = 0; i < 9; i++)	{
		equinox_shade(&bg_normal, &equinox_style->colors.shade[i], (shades[i]-0.7) * contrast + 0.7);
	}

	equinox_gdk_color_to_cairo (&style->bg[GTK_STATE_SELECTED], &spot_color);

	equinox_shade(&spot_color, &equinox_style->colors.spot[0],1.42);
	equinox_shade(&spot_color, &equinox_style->colors.spot[1], 1.00);
	equinox_shade(&spot_color, &equinox_style->colors.spot[2], 0.65);

	for (i=0; i<5; i++)	{
		equinox_gdk_color_to_cairo (&style->bg[i], &equinox_style->colors.bg[i]);
		equinox_gdk_color_to_cairo (&style->base[i], &equinox_style->colors.base[i]);
		equinox_gdk_color_to_cairo (&style->text[i], &equinox_style->colors.text[i]);
		equinox_gdk_color_to_cairo (&style->fg[i], &equinox_style->colors.fg[i]);
	}
}

static void equinox_style_draw_focus (GtkStyle * style, GdkWindow * window,
			 GtkStateType state_type, GdkRectangle * area,
			 GtkWidget * widget, const gchar * detail, gint x,
			 gint y, gint width, gint height) {
	EquinoxStyle *equinox_style = EQUINOX_STYLE (style);
	EquinoxColors *colors = &equinox_style->colors;
	cairo_t *cr;

	CHECK_ARGS
	SANITIZE_SIZE
	cr = gdk_cairo_create (window);

	//As all other focus drawing is taken care of by respective widgets...
	//GtkTextView

	if (DETAIL ("scrolled_window") || DETAIL ("viewport") || detail == NULL
				|| (DETAIL ("button") && widget && (EQUINOX_IS_TREE_VIEW (widget->parent) || EQUINOX_IS_CLIST (widget->parent)))) {

		WidgetParameters params;
		equinox_set_widget_parameters (widget, style, state_type, &params);
		EquinoxRGB focus_color = colors->spot[1];

		cairo_set_line_width (cr, 1.0);
		cairo_set_source_rgba (cr, colors->spot[1].r, colors->spot[1].g, colors->spot[1].b, 0.10);
		clearlooks_rounded_rectangle (cr, x, y, width, height, params.curvature, EQX_CORNER_ALL);
		cairo_fill (cr);
		cairo_rectangle (cr, x+0.5, y+0.5, width-1, height - (DETAIL ("button") ? 2 : 1));
		equinox_set_source_rgba (cr, &colors->spot[1], 0.75);
		cairo_stroke (cr);
	} else if ((DETAIL ("checkbutton") || DETAIL ("expander") || (DETAIL ("trough") && widget && GTK_IS_SCALE (widget))) || (detail && g_str_has_prefix (detail, "treeview"))) {
		WidgetParameters params;
		equinox_set_widget_parameters (widget, style, state_type, &params);
		double roundness;
		if (detail && g_str_has_prefix (detail, "treeview")) {
			roundness = 0;
		} else {
			roundness = params.curvature;
		}

		cairo_translate (cr, x, y);
		cairo_set_line_width (cr, 1.0);

		cairo_set_source_rgba (cr, colors->spot[1].r, colors->spot[1].g, colors->spot[1].b, 0.08);
		clearlooks_rounded_rectangle (cr, 0, 0, width - 1, height - 1, roundness, EQX_CORNER_ALL);
		cairo_fill (cr);

		cairo_set_source_rgba (cr, colors->spot[1].r, colors->spot[1].g, colors->spot[1].b, 0.6);
		clearlooks_rounded_rectangle (cr, 0.5, 0.5, width - 1, height - 1, roundness, EQX_CORNER_ALL);
		cairo_stroke (cr);
	}

	cairo_destroy (cr);
}

static void equinox_style_copy (GtkStyle * style, GtkStyle * src) {
	EquinoxStyle *eqx_style = EQUINOX_STYLE (style);
	EquinoxStyle *eqx_src = EQUINOX_STYLE (src);

	eqx_style->colors = eqx_src->colors;
	eqx_style->curvature = eqx_src->curvature;
	eqx_style->menubarstyle = eqx_src->menubarstyle;
	eqx_style->menubarborders = eqx_src->menubarborders;
	eqx_style->menubaritemstyle = eqx_src->menubaritemstyle;
	eqx_style->toolbarstyle = eqx_src->toolbarstyle;
	eqx_style->buttonstyle = eqx_src->buttonstyle;
	eqx_style->scrollbarstyle = eqx_src->scrollbarstyle;
	eqx_style->scrollbartrough = eqx_src->scrollbartrough;
	eqx_style->scrollbarmargin = eqx_src->scrollbarmargin;
	eqx_style->scrollbarpadding[0] = eqx_src->scrollbarpadding[0];
	eqx_style->scrollbarpadding[1] = eqx_src->scrollbarpadding[1];
	eqx_style->scrollbarpadding[2] = eqx_src->scrollbarpadding[2];
	eqx_style->scrollbarpadding[3] = eqx_src->scrollbarpadding[3];
	eqx_style->scrollbar_color = eqx_src->scrollbar_color;
	eqx_style->checkradiostyle = eqx_src->checkradiostyle;
	eqx_style->listviewheaderstyle = eqx_src->listviewheaderstyle;
	eqx_style->listviewitemstyle = eqx_src->listviewitemstyle;
	eqx_style->progressbarstyle = eqx_src->progressbarstyle;
	eqx_style->separatorstyle = eqx_src->separatorstyle;
	eqx_style->resizegripstyle = eqx_src->resizegripstyle;
	eqx_style->textstyle = eqx_src->textstyle;
	eqx_style->textshade = eqx_src->textshade;
	eqx_style->arrowsize = eqx_src->arrowsize;
	eqx_style->animation = eqx_src->animation;

	equinox_parent_class->copy (style, src);
}

static void equinox_style_unrealize (GtkStyle * style) {
  equinox_parent_class->unrealize (style);
}

static GdkPixbuf *set_transparency (const GdkPixbuf * pixbuf, gdouble alpha_percent) {
	GdkPixbuf *target;
	guchar *data, *current;
	guint x, y, rowstride, height, width;

	g_return_val_if_fail (pixbuf != NULL, NULL);
	g_return_val_if_fail (GDK_IS_PIXBUF (pixbuf), NULL);

	/* Returns a copy of pixbuf with it's non-completely-transparent pixels to
	 have an alpha level "alpha_percent" of their original value. */

	target = gdk_pixbuf_add_alpha (pixbuf, FALSE, 0, 0, 0);

	if (alpha_percent == 1.0)
		return target;
	width = gdk_pixbuf_get_width (target);
	height = gdk_pixbuf_get_height (target);
	rowstride = gdk_pixbuf_get_rowstride (target);
	data = gdk_pixbuf_get_pixels (target);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			/* The "4" is the number of chars per pixel, in this case, RGBA,
			 the 3 means "skip to the alpha" */
			current = data + (y * rowstride) + (x * 4) + 3;
			*(current) = (guchar) (*(current) * alpha_percent);
		}
	}

  return target;
}

static GdkPixbuf *scale_or_ref (GdkPixbuf * src, int width, int height) {
  if (width == gdk_pixbuf_get_width (src) && height == gdk_pixbuf_get_height (src)) {
    return g_object_ref (src);
  } else {
    return gdk_pixbuf_scale_simple (src, width, height, GDK_INTERP_BILINEAR);
  }
}


static void icon_scale_brightness (const GdkPixbuf * src,
		       GdkPixbuf * dest, gfloat brightness) {


  g_return_if_fail (GDK_IS_PIXBUF (src));
  g_return_if_fail (GDK_IS_PIXBUF (dest));
  g_return_if_fail (gdk_pixbuf_get_height (src) == gdk_pixbuf_get_height (dest));
  g_return_if_fail (gdk_pixbuf_get_width (src) == gdk_pixbuf_get_width (dest));
  g_return_if_fail (gdk_pixbuf_get_has_alpha (src) == gdk_pixbuf_get_has_alpha (dest));
  g_return_if_fail (gdk_pixbuf_get_colorspace (src) == gdk_pixbuf_get_colorspace (dest));

  int i, j, t;
  int width, height, has_alpha, src_rowstride, dest_rowstride, bytes_per_pixel;
  guchar *src_line;
  guchar *dest_line;
  guchar *src_pixel;
  guchar *dest_pixel;

  has_alpha = gdk_pixbuf_get_has_alpha (src);
  bytes_per_pixel = has_alpha ? 4 : 3;
  width = gdk_pixbuf_get_width (src);
  height = gdk_pixbuf_get_height (src);
  src_rowstride = gdk_pixbuf_get_rowstride (src);
  dest_rowstride = gdk_pixbuf_get_rowstride (dest);

  src_line = gdk_pixbuf_get_pixels (src);
  dest_line = gdk_pixbuf_get_pixels (dest);

#define CLAMP_UCHAR(v) (t = (v), CLAMP (t, 0, 255))
#define SCALE_BRIGHTNESS(v) ( brightness * (v))

  for (i = 0; i < height; i++) {
    src_pixel = src_line;
    src_line += src_rowstride;
    dest_pixel = dest_line;
    dest_line += dest_rowstride;

    for (j = 0; j < width; j++) {
      dest_pixel[0] = CLAMP_UCHAR (SCALE_BRIGHTNESS (src_pixel[0]));
      dest_pixel[1] = CLAMP_UCHAR (SCALE_BRIGHTNESS (src_pixel[1]));
      dest_pixel[2] = CLAMP_UCHAR (SCALE_BRIGHTNESS (src_pixel[2]));

      if (has_alpha)
	  dest_pixel[3] = src_pixel[3];

      src_pixel += bytes_per_pixel;
      dest_pixel += bytes_per_pixel;
    }
  }
}

static GdkPixbuf *
equinox_style_draw_render_icon (GtkStyle * style,
			       const GtkIconSource * source,
			       GtkTextDirection direction,
			       GtkStateType state,
			       GtkIconSize size,
			       GtkWidget * widget, const char *detail)
{
  int width = 1;
  int height = 1;
  GdkPixbuf *scaled;
  GdkPixbuf *stated;
  GdkPixbuf *base_pixbuf;
  GdkScreen *screen;
  GtkSettings *settings;

  /* Oddly, style can be NULL in this function, because
   * GtkIconSet can be used without a style and if so
   * it uses this function.
   */

  base_pixbuf = gtk_icon_source_get_pixbuf (source);

  g_return_val_if_fail (base_pixbuf != NULL, NULL);

  if (widget && gtk_widget_has_screen (widget)) {
    screen = gtk_widget_get_screen (widget);
    settings = gtk_settings_get_for_screen (screen);
  }
  else if (style->colormap) {
    screen = gdk_colormap_get_screen (style->colormap);
    settings = gtk_settings_get_for_screen (screen);
  }
  else {
    settings = gtk_settings_get_default ();
    GTK_NOTE (MULTIHEAD, g_warning("Using the default screen for gtk_default_equinox_style_draw_render_icon()"));
  }


  if (size != (GtkIconSize) - 1
      && !gtk_icon_size_lookup_for_settings (settings, size, &width, &height)) {
    g_warning (G_STRLOC ": invalid icon size '%d'", size);
    return NULL;
  }

  /* If the size was wildcarded, and we're allowed to scale, then scale; otherwise,
   * leave it alone.
   */
  if (size != (GtkIconSize) - 1
      && gtk_icon_source_get_size_wildcarded (source))
    scaled = scale_or_ref (base_pixbuf, width, height);
  else
    scaled = g_object_ref (base_pixbuf);

  /* If the state was wildcarded, then generate a state. */
  if (gtk_icon_source_get_state_wildcarded (source)) {
    if (state == GTK_STATE_INSENSITIVE) {
      stated = set_transparency (scaled, 0.3);
      gdk_pixbuf_saturate_and_pixelate (stated, stated, 0.125, FALSE);
      g_object_unref (scaled);
    }
    else if (state == GTK_STATE_PRELIGHT) {
      stated = gdk_pixbuf_copy (scaled);

      //Increase brightness
      icon_scale_brightness (scaled, stated, 1.125);
      //gdk_pixbuf_saturate_and_pixelate (stated, stated, 1.05, FALSE);

      g_object_unref (scaled);
    }
    else {
      stated = scaled;
    }
  }
  else
    stated = scaled;

  return stated;
}
static void
equinox_style_init (EquinoxStyle * style)
{
}


static void equinox_style_draw_layout (GtkStyle * style,
		     GdkWindow * window,
	     GtkStateType state_type,
	     gboolean use_text,
	     GdkRectangle * area,
	     GtkWidget * widget,
	     const gchar * detail, gint x, gint y, PangoLayout *layout) {
	GdkGC *gc;

	g_return_if_fail (GTK_IS_STYLE (style));
	g_return_if_fail (window != NULL);

	gc = use_text ? style->text_gc[state_type] : style->fg_gc[state_type];

	if (area)
		gdk_gc_set_clip_rectangle (gc, area);

	if (widget && (state_type == GTK_STATE_INSENSITIVE ||
	    (EQUINOX_STYLE (style)->textstyle != 0 &&
	     state_type != GTK_STATE_PRELIGHT &&
	     !(DETAIL ("cellrenderertext") && state_type == GTK_STATE_NORMAL))))
	{
		EquinoxStyle *equinox_style = EQUINOX_STYLE (style);
		EquinoxColors *colors = &equinox_style->colors;

		WidgetParameters params;

		equinox_set_widget_parameters (widget, style, state_type, &params);

		GdkColor etched;
		EquinoxRGB temp;

		double shade_level = equinox_style->textshade;
		double xos = 0;
		double yos = 0;

		switch (equinox_style->textstyle) {
			case 1:
				yos = +0.5;
 				break;
			case 2:
				yos = -0.5;
				break;
			case 3:
				xos = 0.5;
				yos = 0.5;
				break;
			case 4:
				xos = -0.5;
				yos = -0.5;
				break;
		}

		if (state_type == GTK_STATE_INSENSITIVE) {
			shade_level = 1.24;
			xos = 0.5;
			yos = 0.5;
		}

		if (!gtk_widget_get_has_window (widget)) {
			boolean use_parentbg = TRUE;
			while (widget->parent) {
				if (EQUINOX_IS_BUTTON(widget->parent) ||
				    EQUINOX_IS_TOGGLE_BUTTON(widget->parent) ||
				    EQUINOX_IS_COMBO_BOX(widget->parent) ||
				    EQUINOX_IS_COMBO_BOX_ENTRY(widget->parent) ||
				    EQUINOX_IS_COMBO(widget->parent) ||
				    EQUINOX_IS_OPTION_MENU(widget->parent) ||
				    EQUINOX_IS_NOTEBOOK(widget->parent)) {
					GtkReliefStyle relief = GTK_RELIEF_NORMAL;
					// Check for the shadow type.make
					if (EQUINOX_IS_BUTTON (widget->parent))
						g_object_get (G_OBJECT (widget->parent), "relief", &relief, NULL);

					if (!EQUINOX_IS_CHECK_BUTTON(widget->parent) &&
					    !EQUINOX_IS_RADIO_BUTTON(widget->parent) &&
					    !(relief == GTK_RELIEF_NONE &&
					      (state_type == GTK_STATE_NORMAL ||
					       state_type == GTK_STATE_INSENSITIVE)))
						use_parentbg = FALSE;

					break;
				}
				widget = widget->parent;
			}

			if (use_parentbg)
				murrine_shade (&params.parentbg, shade_level, &temp);
			else if (DETAIL ("cellrenderertext"))
				murrine_shade (&colors->base[state_type], shade_level, &temp);
			else
				murrine_shade (&colors->bg[state_type], shade_level, &temp);
		}
		else if (DETAIL ("cellrenderertext"))
			//murrine_shade (&colors->base[state_type], shade_level, &temp);
			murrine_shade (&colors->base[GTK_STATE_NORMAL], shade_level - 0.12, &temp);
		else
			murrine_shade (&colors->bg[state_type], shade_level, &temp);

		etched.red = (int) (temp.r*65535);
		etched.green = (int) (temp.g*65535);
		etched.blue = (int) (temp.b*65535);

		// with cairo
		cairo_t *cr;
		cr = equinox_begin_paint (window, area);
		cairo_translate (cr, x+xos, y+yos);
		pango_cairo_layout_path (cr, layout);
		equinox_set_source_rgba (cr, &temp, 0.5);
		cairo_stroke (cr);
		cairo_destroy (cr);
	}

	if (DETAIL ("accellabel")) {
		EquinoxStyle *equinox_style = EQUINOX_STYLE (style);
		EquinoxColors *colors = &equinox_style->colors;

		WidgetParameters params;

		equinox_set_widget_parameters (widget, style, state_type, &params);

		GdkColor etched;
		EquinoxRGB temp;

		equinox_mix_color (&colors->fg[state_type], &params.parentbg, state_type != GTK_STATE_PRELIGHT ? 0.4 : 0.2, &temp);

		etched.red = (int) (temp.r*65535);
		etched.green = (int) (temp.g*65535);
		etched.blue = (int) (temp.b*65535);

		gdk_draw_layout_with_colors(window, gc, x, y, layout, &etched, NULL);
	}
	else
		gdk_draw_layout (window, gc, x, y, layout);

	/*if (state_type == GTK_STATE_INSENSITIVE) {
		EquinoxStyle *equinox_style = EQUINOX_STYLE (style);
		EquinoxColors *colors = &equinox_style->colors;

		WidgetParameters params;
		GdkColor etched;
		EquinoxRGB temp;

		equinox_set_widget_parameters (widget, style, state_type, &params);

		double bg_value, text_value;
		bg_value = MAX (MAX (params.parentbg.r, params.parentbg.g), params.parentbg.b);
		text_value = MAX (MAX (colors->text[widget->state].r, colors->text[widget->state].g), colors->text[widget->state].b);

		if(text_value < (bg_value + 0.1*bg_value) ) {
		    if (widget && GTK_WIDGET_NO_WINDOW (widget))
		      equinox_shade (&params.parentbg, &temp, 1.15);
		    else
		      equinox_shade (&colors->bg[widget->state], &temp, 1.15);

		    etched.red = (int) (temp.r * 65535);
		    etched.green = (int) (temp.g * 65535);
		    etched.blue = (int) (temp.b * 65535);

		    gdk_draw_layout_with_colors (window, style->text_gc[state_type], x, y + 1, layout, &etched, NULL);
		}

    	gdk_draw_layout (window, style->text_gc[state_type], x, y, layout);
	} else
		gdk_draw_layout (window, gc, x, y, layout);*/

  	if (area)
    	gdk_gc_set_clip_rectangle (gc, NULL);
}

static void equinox_style_draw_expander (GtkStyle * style,
			    GdkWindow * window,
			    GtkStateType state_type,
			    GdkRectangle * area,
			    GtkWidget * widget,
			    const gchar * detail,
			    gint x, gint y, GtkExpanderStyle expander_style) {
//#define DEFAULT_EXPANDER_SIZE 11
#define DEFAULT_EXPANDER_SIZE 7

	gint expander_size;
	gint line_width;
	double vertical_overshoot;
	int diameter;
	double radius;
	double interp;		/* interpolation factor for center position */
	double x_double_horz, y_double_horz;
	double x_double_vert, y_double_vert;
	double x_double, y_double;
	gint degrees = 0;

	cairo_t *cr = gdk_cairo_create (window);

	if (area) {
		gdk_cairo_rectangle (cr, area);
		cairo_clip (cr);
	}

	/*if (widget &&
	  gtk_widget_class_find_style_property (GTK_WIDGET_GET_CLASS (widget),
						"expander-size")) {
	gtk_widget_style_get (widget, "expander-size", &expander_size, NULL);
	}
	else*/
	expander_size = DEFAULT_EXPANDER_SIZE;

	line_width = 1;

	switch (expander_style) {
		case GTK_EXPANDER_COLLAPSED:
			degrees = (equinox_get_direction (widget) == GTK_TEXT_DIR_RTL) ? 180 : 0;
			interp = 0.0;
			break;
		case GTK_EXPANDER_SEMI_COLLAPSED:
			degrees = (equinox_get_direction (widget) == GTK_TEXT_DIR_RTL) ? 150 : 30;
			interp = 0.25;
			break;
		case GTK_EXPANDER_SEMI_EXPANDED:
			degrees = (equinox_get_direction (widget) == GTK_TEXT_DIR_RTL) ? 120 : 60;
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
	diameter = MAX (3, expander_size - 2 * vertical_overshoot);

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

	cairo_move_to (cr, -radius / 2., -radius / 2.0);
	cairo_line_to (cr,  radius / 2.,  0);
	cairo_line_to (cr, -radius / 2.,  radius / 2.0);
	cairo_close_path (cr);

	cairo_set_line_width (cr, line_width);

	if (state_type == GTK_STATE_INSENSITIVE)
		gdk_cairo_set_source_color (cr, &style->base[GTK_STATE_INSENSITIVE]);
	else if (state_type == GTK_STATE_PRELIGHT)
		gdk_cairo_set_source_color (cr, &style->base[GTK_STATE_NORMAL]);
	else if (state_type == GTK_STATE_ACTIVE)
		gdk_cairo_set_source_color (cr, &style->light[GTK_STATE_ACTIVE]);
	else
		gdk_cairo_set_source_color (cr, &style->fg[GTK_STATE_PRELIGHT]);

	cairo_fill_preserve (cr);

	gdk_cairo_set_source_color (cr, &style->fg[state_type]);
	cairo_stroke (cr);

	cairo_destroy (cr);

}


static void equinox_style_class_init (EquinoxStyleClass * klass) {
  GtkStyleClass *style_class = GTK_STYLE_CLASS (klass);

  equinox_style_class = EQUINOX_STYLE_CLASS (klass);
  equinox_parent_class = g_type_class_peek_parent (klass);

  style_class->copy		          = equinox_style_copy;
  style_class->realize          = equinox_style_realize;
  style_class->unrealize        = equinox_style_unrealize;
  style_class->init_from_rc     = equinox_style_init_from_rc;
  style_class->draw_handle      = equinox_style_draw_handle;
  style_class->draw_slider      = equinox_style_draw_slider;
  style_class->draw_shadow_gap  = equinox_style_draw_shadow_gap;
  style_class->draw_focus       = equinox_style_draw_focus;
  style_class->draw_box         = equinox_style_draw_box;
  style_class->draw_shadow      = equinox_style_draw_shadow;
  style_class->draw_box_gap     = equinox_style_draw_box_gap;
  style_class->draw_extension   = equinox_style_draw_extension;
  style_class->draw_option      = equinox_style_draw_option;
  style_class->draw_check       = equinox_style_draw_check;
  style_class->draw_flat_box    = equinox_style_draw_flat_box;
  style_class->draw_tab         = equinox_style_draw_tab;
  style_class->draw_vline       = equinox_style_draw_vline;
  style_class->draw_hline       = equinox_style_draw_hline;
  style_class->draw_resize_grip = equinox_style_draw_resize_grip;
  style_class->draw_arrow       = equinox_style_draw_arrow;
  style_class->render_icon      = equinox_style_draw_render_icon;
  style_class->draw_layout      = equinox_style_draw_layout;
  style_class->draw_expander    = equinox_style_draw_expander;
}

GType equinox_type_style = 0;

void equinox_style_register_type (GTypeModule * module) {
	static const GTypeInfo object_info = {
		sizeof (EquinoxStyleClass),
		(GBaseInitFunc) NULL,
		(GBaseFinalizeFunc) NULL,
		(GClassInitFunc) equinox_style_class_init,
		NULL,			/* class_finalize */
		NULL,			/* class_data */
		sizeof (EquinoxStyle),
		0,				/* n_preallocs */
		(GInstanceInitFunc) equinox_style_init,
		NULL
	};

	equinox_type_style = g_type_module_register_type (module,
						   GTK_TYPE_STYLE,
						   "EquinoxStyle",
						   &object_info, 0);
}
