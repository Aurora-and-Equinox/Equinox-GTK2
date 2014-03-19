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
 * Modified by Kulyk Nazar <schamane@myeburg.net>
 * Modified by Andrea Cimitan <andrea.cimitan@gmail.com>
 * Modified by Eric Matthews <echm2007@gmail.com>
 * Modified by Matthieu James <matthieu.james@free.fr>
 */

#include <gtk/gtkrc.h>

typedef struct _EquinoxRcStyle EquinoxRcStyle;
typedef struct _EquinoxRcStyleClass EquinoxRcStyleClass;

G_GNUC_INTERNAL extern GType equinox_type_rc_style;

#define EQUINOX_TYPE_RC_STYLE              equinox_type_rc_style
#define EQUINOX_RC_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), EQUINOX_TYPE_RC_STYLE, EquinoxRcStyle))
#define EQUINOX_RC_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), EQUINOX_TYPE_RC_STYLE, EquinoxRcStyleClass))
#define EQUINOX_IS_RC_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), EQUINOX_TYPE_RC_STYLE))
#define EQUINOX_IS_RC_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), EQUINOX_TYPE_RC_STYLE))
#define EQUINOX_RC_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), EQUINOX_TYPE_RC_STYLE, EquinoxRcStyleClass))

typedef enum {
	EQX_FLAG_CONTRAST = 1 << 0,
	EQX_FLAG_CURVATURE = 1 << 1,
	EQX_FLAG_MENUBARSTYLE = 1 << 2,
	EQX_FLAG_TOOLBARSTYLE = 1 << 3,
	EQX_FLAG_MENUBARITEMSTYLE = 1 << 4,
	EQX_FLAG_BUTTONSTYLE = 1 << 5,
	EQX_FLAG_MENUITEMSTYLE = 1 << 6,
	EQX_FLAG_LISTVIEWHEADERSTYLE = 1 << 7,
	EQX_FLAG_SCROLLBARSTYLE = 1 << 8,
	EQX_FLAG_SCROLLBARTROUGH = 1 << 9,
	EQX_FLAG_SCROLLBARMARGIN = 1 << 10,
	EQX_FLAG_SCROLLBARPADDING = 1 << 11,
	EQX_FLAG_SCROLLBAR_COLOR = 1 << 12,
	EQX_FLAG_SCALESLIDERSTYLE = 1 << 13,
	EQX_FLAG_CHECKRADIOSTYLE = 1 << 14,
	EQX_FLAG_PROGRESSBARSTYLE = 1 << 15,
	EQX_FLAG_SEPARATORSTYLE = 1 << 16,
	EQX_FLAG_RESIZEGRIPSTYLE = 1 << 17,
	EQX_FLAG_TEXTSTYLE = 1 << 18,
	EQX_FLAG_TEXTSHADE = 1 << 19,
	EQX_FLAG_ANIMATION = 1 << 20,
	EQX_FLAG_ARROWSIZE = 1 << 21,
	EQX_FLAG_MENUBARBORDERS = 1 << 22,
	EQX_FLAG_LISTVIEWITEMSTYLE = 1 << 23
} EquinoxRcFlags;

struct _EquinoxRcStyle {
	GtkRcStyle parent_instance;

	EquinoxRcFlags flags;

	double contrast;
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
	gboolean has_scrollbar_color;
	GdkColor scrollbar_color;
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

struct _EquinoxRcStyleClass
{
  GtkRcStyleClass parent_class;
};

G_GNUC_INTERNAL void equinox_rc_style_register_type (GTypeModule *module);
