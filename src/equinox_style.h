/* Equinox theme engine
 * Copyright (C) 2008 Matthieu James.
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
 * Written by Owen Taylor <otaylor@redhat.com>
 * and by Alexander Larsson <alexl@redhat.com>
 * Modified by Richard Stellingwerff <remenic@gmail.com>
 * Modified by Andrea Cimitan <andrea.cimitan@gmail.com>
 * Modified by Eric Matthews <echm2007@gmail.com>
 * Modified by Matthieu James <matthieu.james@free.fr>
 */
#include <gtk/gtkstyle.h>

#ifndef EQUINOX_STYLE_H
#define EQUINOX_STYLE_H

#include "animation.h"
#include "equinox_types.h"

typedef struct _EquinoxStyle EquinoxStyle;
typedef struct _EquinoxStyleClass EquinoxStyleClass;

G_GNUC_INTERNAL extern GType equinox_type_style;

#define EQUINOX_TYPE_STYLE              equinox_type_style
#define EQUINOX_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), EQUINOX_TYPE_STYLE, EquinoxStyle))
#define EQUINOX_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), EQUINOX_TYPE_STYLE, EquinoxStyleClass))
#define EQUINOX_IS_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), EQUINOX_TYPE_STYLE))
#define EQUINOX_IS_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), EQUINOX_TYPE_STYLE))
#define EQUINOX_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), EQUINOX_TYPE_STYLE, EquinoxStyleClass))

struct _EquinoxStyle
{
	GtkStyle parent_instance;

	EquinoxColors colors;

	gdouble curvature;
	guint8 menubarstyle;
	guint8 menubarborders;
	guint8 menubaritemstyle;
	guint8 toolbarstyle;
	guint8 buttonstyle;
	guint8 menuitemstyle;
	guint8 listviewheaderstyle;
	guint8 listviewitemstyle;
	guint8 scrollbarstyle;
	guint8 scrollbartrough;
	guint8 scrollbarmargin;
	guint8 scrollbarpadding[4];
	GdkColor scrollbar_color;
	gboolean has_scrollbar_color;
	guint8 scalesliderstyle;
	guint8 checkradiostyle;
	guint8 progressbarstyle;
	guint8 separatorstyle;
	guint8 resizegripstyle;
	guint8 textstyle;
	double textshade;
	gboolean animation;
	gdouble arrowsize;

};

struct _EquinoxStyleClass
{
  GtkStyleClass parent_class;
};


G_GNUC_INTERNAL void equinox_style_register_type (GTypeModule *module);

#endif /* CLEARLOOKS_STYLE_H */







