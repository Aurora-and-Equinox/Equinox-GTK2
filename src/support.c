#include "support.h"

G_GNUC_INTERNAL void
equinox_rgb_to_hls (gdouble *r,
                    gdouble *g,
                    gdouble *b)
{
	gdouble min;
	gdouble max;
	gdouble red;
	gdouble green;
	gdouble blue;
	gdouble h, l, s;
	gdouble delta;

	red = *r;
	green = *g;
	blue = *b;

	if (red > green)
	{
		if (red > blue)
			max = red;
		else
			max = blue;

		if (green < blue)
			min = green;
		else
			min = blue;
	}
	else
	{
		if (green > blue)
			max = green;
		else
			max = blue;

		if (red < blue)
			min = red;
		else
			min = blue;
	}

	l = (max+min)/2;
	if (fabs (max-min) < 0.0001)
	{
		h = 0;
		s = 0;
	}
	else
	{
		if (l <= 0.5)
			s = (max-min)/(max+min);
		else
			s = (max-min)/(2-max-min);

		delta = max -min;
		if (red == max)
			h = (green-blue)/delta;
		else if (green == max)
			h = 2+(blue-red)/delta;
		else if (blue == max)
			h = 4+(red-green)/delta;

		h *= 60;
		if (h < 0.0)
			h += 360;
	}

	*r = h;
	*g = l;
	*b = s;
}

G_GNUC_INTERNAL void
equinox_hls_to_rgb (gdouble *h,
                    gdouble *l,
                    gdouble *s)
{
	gdouble hue;
	gdouble lightness;
	gdouble saturation;
	gdouble m1, m2;
	gdouble r, g, b;

	lightness = *l;
	saturation = *s;

	if (lightness <= 0.5)
		m2 = lightness*(1+saturation);
	else
		m2 = lightness+saturation-lightness*saturation;

	m1 = 2*lightness-m2;

	if (saturation == 0)
	{
		*h = lightness;
		*l = lightness;
		*s = lightness;
	}
	else
	{
		hue = *h+120;
		while (hue > 360)
			hue -= 360;
		while (hue < 0)
			hue += 360;

		if (hue < 60)
			r = m1+(m2-m1)*hue/60;
		else if (hue < 180)
			r = m2;
		else if (hue < 240)
			r = m1+(m2-m1)*(240-hue)/60;
		else
			r = m1;

		hue = *h;
		while (hue > 360)
			hue -= 360;
		while (hue < 0)
			hue += 360;

		if (hue < 60)
			g = m1+(m2-m1)*hue/60;
		else if (hue < 180)
			g = m2;
		else if (hue < 240)
			g = m1+(m2-m1)*(240-hue)/60;
		else
			g = m1;

		hue = *h-120;
		while (hue > 360)
			hue -= 360;
		while (hue < 0)
			hue += 360;

		if (hue < 60)
			b = m1+(m2-m1)*hue/60;
		else if (hue < 180)
			b = m2;
		else if (hue < 240)
			b = m1+(m2-m1)*(240-hue)/60;
		else
			b = m1;

		*h = r;
		*l = g;
		*s = b;
	}
}

GtkTextDirection
equinox_get_direction (GtkWidget *widget)
{
	GtkTextDirection dir;

	if (widget)
		dir = gtk_widget_get_direction (widget);
	else
		dir = GTK_TEXT_DIR_LTR;

	return dir;
}


/* Widget Type Lookups/Macros

   Based on/modified from functions in
   Smooth-Engine.
*/
gboolean
equinox_object_is_a (const GObject * object, const gchar * type_name)
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
equinox_is_combo_box_entry (GtkWidget * widget)
{
  gboolean result = FALSE;

  if ((widget) && (widget->parent))
    {
      if (EQUINOX_IS_COMBO_BOX_ENTRY (widget->parent))
	result = TRUE;
      else
	result = equinox_is_combo_box_entry (widget->parent);
    }
  return result;
}

static gboolean
equinox_combo_box_is_using_list (GtkWidget * widget)
{
  gboolean result = FALSE;

  if (EQUINOX_IS_COMBO_BOX (widget))
    {
      gboolean *tmp = NULL;

      gtk_widget_style_get (widget, "appears-as-list", &result, NULL);

      if (tmp)
	result = *tmp;
    }

  return result;
}

gboolean
equinox_is_combo_box (GtkWidget * widget, gboolean as_list)
{
  gboolean result = FALSE;

  if ((widget) && (widget->parent))
    {
      if (EQUINOX_IS_COMBO_BOX (widget->parent))
        {
          if (as_list)
            result = (equinox_combo_box_is_using_list(widget->parent));
          else
            result = (!equinox_combo_box_is_using_list(widget->parent));
        }
      else
	result = equinox_is_combo_box (widget->parent, as_list);
    }
  return result;
}

gboolean
equinox_is_combo (GtkWidget * widget)
{
  gboolean result = FALSE;

  if ((widget) && (widget->parent))
    {
      if (EQUINOX_IS_COMBO (widget->parent))
	result = TRUE;
      else
	result = equinox_is_combo (widget->parent);
    }
  return result;
}

gboolean
equinox_is_in_combo_box (GtkWidget * widget)
{
  return ((equinox_is_combo (widget) || equinox_is_combo_box (widget, TRUE) || equinox_is_combo_box_entry (widget)));
}

gboolean
equinox_is_tree_column_header (GtkWidget * widget)
{
  gboolean result = FALSE;

  if ((widget) && (widget->parent))
    {
      if (EQUINOX_IS_TREE_VIEW (widget->parent))
	result = TRUE;
      else
	result = equinox_is_tree_column_header (widget->parent);
    }
  return result;
}

gboolean
equinox_is_toolbar_item (GtkWidget * widget)
{
  gboolean result = FALSE;

  if ((widget) && (widget->parent)) {
    if ((EQUINOX_IS_BONOBO_TOOLBAR (widget->parent))
	|| (EQUINOX_IS_BONOBO_DOCK_ITEM (widget->parent))
	|| (EQUINOX_IS_EGG_TOOLBAR (widget->parent))
	|| (EQUINOX_IS_TOOLBAR (widget->parent))
	|| (EQUINOX_IS_HANDLE_BOX (widget->parent)))
      result = TRUE;
    else
      result = equinox_is_toolbar_item (widget->parent);
  }
  return result;
}

gboolean
equinox_is_panel_widget_item (GtkWidget * widget)
{
  gboolean result = FALSE;

  if ((widget) && (widget->parent))
    {
      if (EQUINOX_IS_PANEL_WIDGET (widget->parent))
	result = TRUE;
      else
	result = equinox_is_panel_widget_item (widget->parent);
    }
  return result;
}

gboolean
equinox_is_bonobo_dock_item (GtkWidget * widget)
{
  gboolean result = FALSE;

  if ((widget))
    {
      if (EQUINOX_IS_BONOBO_DOCK_ITEM(widget) || EQUINOX_IS_BONOBO_DOCK_ITEM (widget->parent))
	result = TRUE;
      else if (EQUINOX_IS_BOX(widget) || EQUINOX_IS_BOX(widget->parent))
        {
          GtkContainer *box = EQUINOX_IS_BOX(widget)?GTK_CONTAINER(widget):GTK_CONTAINER(widget->parent);
          GList *children = NULL, *child = NULL;

          children = gtk_container_get_children(box);

          for (child = g_list_first(children); child; child = g_list_next(child))
            {
	      if (EQUINOX_IS_BONOBO_DOCK_ITEM_GRIP(child->data))
	        {
	          result = TRUE;
	          child = NULL;
	        }
            }

          if (children)
  	    g_list_free(children);
	}
    }
  return result;
}

static GtkWidget *
equinox_find_combo_box_entry_widget (GtkWidget * widget)
{
  GtkWidget *result = NULL;

  if (widget)
    {
      if (EQUINOX_IS_COMBO_BOX_ENTRY (widget))
	result = widget;
      else
	result = equinox_find_combo_box_entry_widget (widget->parent);
    }

  return result;
}

static GtkWidget *
equinox_find_combo_box_widget (GtkWidget * widget, gboolean as_list)
{
  GtkWidget *result = NULL;

  if (widget)
    {
      if (EQUINOX_IS_COMBO_BOX (widget))
        {
          if (as_list)
            result = (equinox_combo_box_is_using_list(widget))?widget:NULL;
          else
            result = (!equinox_combo_box_is_using_list(widget))?widget:NULL;
        }
      else
	result = equinox_find_combo_box_widget (widget->parent, as_list);
    }
  return result;
}

static GtkWidget *
equinox_find_combo_widget (GtkWidget * widget)
{
  GtkWidget *result = NULL;

  if (widget)
    {
      if (EQUINOX_IS_COMBO (widget))
	result = widget;
      else
	result = equinox_find_combo_widget(widget->parent);
    }
  return result;
}

GtkWidget*
equinox_find_combo_box_widget_parent (GtkWidget * widget)
{
   GtkWidget *result = NULL;

   if (!result)
     result = equinox_find_combo_widget(widget);

   if (!result)
     result = equinox_find_combo_box_widget(widget, TRUE);

   if (!result)
     result = equinox_find_combo_box_entry_widget(widget);

  return result;
}


gboolean
equinox_widget_is_ltr (GtkWidget *widget)
{
	GtkTextDirection dir = GTK_TEXT_DIR_NONE;

	if (EQUINOX_IS_WIDGET (widget))
		dir = gtk_widget_get_direction (widget);

	if (dir == GTK_TEXT_DIR_NONE)
		dir = gtk_widget_get_default_direction ();

	if (dir == GTK_TEXT_DIR_RTL)
		return FALSE;
	else
		return TRUE;
}




/***********************************************
 * ge_hsb_from_color -
 *
 *   Get HSB values from RGB values.
 *
 *   Modified from Smooth but originated in GTK+
 ***********************************************/
static void
equinox_hsb_from_color (const EquinoxRGB *color,
                        gdouble *hue,
                        gdouble *saturation,
                        gdouble *brightness)
{
	gdouble min, max, delta;
	gdouble red, green, blue;

	red = color->r;
	green = color->g;
	blue = color->b;

	if (red > green)
	{
		max = MAX(red, blue);
		min = MIN(green, blue);
	}
	else
	{
		max = MAX(green, blue);
		min = MIN(red, blue);
	}

	*brightness = (max + min) / 2;

	if (fabs(max - min) < 0.0001)
	{
		*hue = 0;
		*saturation = 0;
	}
	else
	{
		if (*brightness <= 0.5)
			*saturation = (max - min) / (max + min);
		else
			*saturation = (max - min) / (2 - max - min);

		delta = max -min;

		if (red == max)
			*hue = (green - blue) / delta;
		else if (green == max)
			*hue = 2 + (blue - red) / delta;
		else if (blue == max)
			*hue = 4 + (red - green) / delta;

		*hue *= 60;
		if (*hue < 0.0)
			*hue += 360;
	}
}

/***********************************************
 * ge_color_from_hsb -
 *
 *   Get RGB values from HSB values.
 *
 *   Modified from Smooth but originated in GTK+
 ***********************************************/
#define MODULA(number, divisor) (((gint)number % divisor) + (number - (gint)number))
static void
equinox_color_from_hsb (gdouble hue,
                        gdouble saturation,
                        gdouble brightness,
                        EquinoxRGB *color)
{
	gint i;
	gdouble hue_shift[3], color_shift[3];
	gdouble m1, m2, m3;

	if (!color) return;

	if (brightness <= 0.5)
		m2 = brightness * (1 + saturation);
	else
		m2 = brightness + saturation - brightness * saturation;

	m1 = 2 * brightness - m2;

	hue_shift[0] = hue + 120;
	hue_shift[1] = hue;
	hue_shift[2] = hue - 120;

	color_shift[0] = color_shift[1] = color_shift[2] = brightness;

	i = (saturation == 0)?3:0;

	for (; i < 3; i++)
	{
		m3 = hue_shift[i];

		if (m3 > 360)
			m3 = MODULA(m3, 360);
		else if (m3 < 0)
			m3 = 360 - MODULA(ABS(m3), 360);

		if (m3 < 60)
			color_shift[i] = m1 + (m2 - m1) * m3 / 60;
		else if (m3 < 180)
			color_shift[i] = m2;
		else if (m3 < 240)
			color_shift[i] = m1 + (m2 - m1) * (240 - m3) / 60;
		else
			color_shift[i] = m1;
	}

	color->r = color_shift[0];
	color->g = color_shift[1];
	color->b = color_shift[2];
}

void
equinox_shade (const EquinoxRGB *base, EquinoxRGB *composite, double shade_ratio)
{
	gdouble hue = 0;
	gdouble saturation = 0;
	gdouble brightness = 0;

	g_return_if_fail (base && composite);

	equinox_hsb_from_color (base, &hue, &saturation, &brightness);

	brightness = MIN(brightness*shade_ratio, 1.0);
	brightness = MAX(brightness, 0.0);

	equinox_color_from_hsb (hue, saturation, brightness, composite);
}

void
equinox_shade_shift (const EquinoxRGB *base, EquinoxRGB *composite, double shade_ratio)
{
	gdouble hue = 0;
	gdouble saturation = 0;
	gdouble brightness = 0;

	g_return_if_fail (base && composite);

	equinox_hsb_from_color (base, &hue, &saturation, &brightness);

	brightness = MIN(brightness*shade_ratio, 1.0);
	brightness = MAX(brightness, 0.0);
	hue += shade_ratio >= 1.0 ? -2.75*shade_ratio : -12*shade_ratio + 12;

	equinox_color_from_hsb (hue, saturation, brightness, composite);
}

void
murrine_shade (const EquinoxRGB *a, float k, EquinoxRGB *b)
{
	double red;
	double green;
	double blue;

	red   = a->r;
	green = a->g;
	blue  = a->b;

	if (k == 1.0)
	{
		b->r = red;
		b->g = green;
		b->b = blue;
		return;
	}

	equinox_rgb_to_hls (&red, &green, &blue);

	green *= k;
	if (green > 1.0)
		green = 1.0;
	else if (green < 0.0)
		green = 0.0;

	blue *= k;
	if (blue > 1.0)
		blue = 1.0;
	else if (blue < 0.0)
		blue = 0.0;

	equinox_hls_to_rgb (&red, &green, &blue);

	b->r = red;
	b->g = green;
	b->b = blue;
}

void
equinox_hue_shift (const EquinoxRGB *base, EquinoxRGB *composite, double shift_amount)
{
	gdouble hue = 0;
	gdouble saturation = 0;
	gdouble brightness = 0;

	g_return_if_fail (base && composite);

	equinox_hsb_from_color (base, &hue, &saturation, &brightness);

	hue += shift_amount;

	equinox_color_from_hsb (hue, saturation, brightness, composite);
}

void
equinox_match_lightness (const EquinoxRGB * a, EquinoxRGB * b)
{

	gdouble hue = 0;
	gdouble saturation = 0;
	gdouble brightness = 0;

	equinox_hsb_from_color (b, &hue, &saturation, &brightness);
	equinox_color_from_hsb (hue, saturation, equinox_get_lightness(a), b);

}

void
equinox_set_lightness (EquinoxRGB * a, double new_lightness)
{

	gdouble hue = 0;
	gdouble saturation = 0;
	gdouble brightness = 0;

	equinox_hsb_from_color (a, &hue, &saturation, &brightness);
	equinox_color_from_hsb (hue, saturation, new_lightness, a);

}

void
equinox_tweak_saturation (const EquinoxRGB * a, EquinoxRGB * b)
{

	gdouble hue = 0;
	gdouble hue2 = 0;

	gdouble saturation = 0;
	gdouble saturation2 = 0;

	gdouble brightness = 0;
	gdouble brightness2 = 0;

	equinox_hsb_from_color (b, &hue, &saturation, &brightness);
 	equinox_hsb_from_color (a, &hue2, &saturation2, &brightness2);


	equinox_color_from_hsb (hue, (saturation+saturation2)/2.0, brightness, b);

}


gdouble
equinox_get_lightness (const EquinoxRGB * a)
{

	gdouble min, max;

	if (a->r > a->g)
	{
		max = MAX(a->r, a->b);
		min = MIN(a->g, a->b);
	}
	else
	{
		max = MAX(a->g, a->b);
		min = MIN(a->r, a->b);
	}

	return (max + min) / 2.0;
}


void
equinox_mix_color (const EquinoxRGB * a, const EquinoxRGB * b, gdouble mix_factor, EquinoxRGB * c)
{
	g_return_if_fail (a && b && c);

	c->r = a->r * (1-mix_factor) + b->r * mix_factor;
	c->g = a->g * (1-mix_factor) + b->g * mix_factor;
	c->b = a->b * (1-mix_factor) + b->b * mix_factor;
}


void equinox_gtk_treeview_get_header_index (GtkTreeView *tv, GtkWidget *header,
                                    gint *column_index, gint *columns,
                                    gboolean *resizable, gboolean *sorted)
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
			*sorted = column->show_sort_indicator;
		}
		if ( column->visible )
			(*columns)++;
	} while ((list = g_list_next(list)));

	g_list_free (list_start);
}


void equinox_gtk_clist_get_header_index (GtkCList *clist, GtkWidget *button,
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


void
equinox_gdk_color_to_cairo (const GdkColor *c, EquinoxRGB *cc)
{
	gdouble r, g, b;

	g_return_if_fail (c && cc);

	r = c->red / 65535.0;
	g = c->green / 65535.0;
	b = c->blue / 65535.0;

	cc->r = r;
	cc->g = g;
	cc->b = b;
}

void
equinox_gdk_color_to_rgb (GdkColor *c, double *r, double *g, double *b)
{
	*r = (double)c->red   /  (double)65535;
	*g = (double)c->green /  (double)65535;
	*b = (double)c->blue  /  (double)65535;
}

void
equinox_get_parent_bg (const GtkWidget *widget, EquinoxRGB *color)
{
	GtkStateType state_type;
	const GtkWidget *parent;
	GdkColor *gcolor;

	if (widget == NULL)
		return;


	parent = widget->parent;

	while (parent && GTK_WIDGET_NO_WINDOW (parent)
                       && !( (GTK_IS_NOTEBOOK (parent) && (gtk_notebook_get_show_tabs ((GtkNotebook*)parent) || gtk_notebook_get_show_border((GtkNotebook*)parent)))
                            || GTK_IS_TOOLBAR(parent)
                            || (GTK_IS_FRAME(parent) && (gtk_frame_get_shadow_type(GTK_FRAME(parent)) != GTK_SHADOW_NONE)) )
		)
	{
		parent = parent->parent;
	}

	if (parent == NULL)
		return;

	if (GTK_IS_MENU_BAR(parent))
		parent = widget;

	state_type = GTK_WIDGET_STATE (parent);

	gcolor = &parent->style->bg[state_type];

	equinox_gdk_color_to_rgb (gcolor, &color->r, &color->g, &color->b);

    if (GTK_IS_FRAME (parent)) {
    GtkShadowType shadow = gtk_frame_get_shadow_type(GTK_FRAME(parent));
         if (shadow == (GTK_SHADOW_IN || GTK_SHADOW_ETCHED_IN))
         	equinox_shade (color, color, 0.97);
         else if (shadow == (GTK_SHADOW_OUT || GTK_SHADOW_ETCHED_OUT))
            equinox_shade (color, color, 1.03);
    }
}

void  equinox_set_source_rgb (cairo_t *cr, const EquinoxRGB *color) {
	equinox_set_source_rgba (cr, color, 1.0);
}

void  equinox_set_source_rgba (cairo_t *cr, const EquinoxRGB *color, double alpha) {
	if (alpha == 1.0)
		cairo_set_source_rgb (cr, color->r, color->g, color->b);
	else
		cairo_set_source_rgba (cr, color->r, color->g, color->b, alpha);
}

void  equinox_pattern_add_color_rgb (cairo_pattern_t *pattern, double offset, const EquinoxRGB *color) {
	equinox_pattern_add_color_rgba (pattern, offset, color, 1.0);
}

void  equinox_pattern_add_color_rgba (cairo_pattern_t *pattern, double offset, const EquinoxRGB *color, double alpha) {
	if (alpha == 1.0)
		cairo_pattern_add_color_stop_rgb (pattern, offset, color->r, color->g, color->b);
	else
		cairo_pattern_add_color_stop_rgba (pattern, offset, color->r, color->g, color->b, alpha);
}

