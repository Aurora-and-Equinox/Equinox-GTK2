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
 * Modifies by Matthieu James <matthieu.james@free.fr>
 */

#include "equinox_style.h"
#include "equinox_rc_style.h"

#include "animation.h"

static void      equinox_rc_style_init         (EquinoxRcStyle      *style);
#ifdef HAVE_ANIMATION
static void      equinox_rc_style_finalize     (GObject                *object);
#endif
static void      equinox_rc_style_class_init   (EquinoxRcStyleClass *klass);
static GtkStyle *equinox_rc_style_create_style (GtkRcStyle             *rc_style);
static guint     equinox_rc_style_parse        (GtkRcStyle             *rc_style,
						   GtkSettings            *settings,
						   GScanner               *scanner);
static void      equinox_rc_style_merge        (GtkRcStyle             *dest,
						   GtkRcStyle             *src);


static GtkRcStyleClass *parent_class;

GType equinox_type_rc_style = 0;

enum {
 	TOKEN_CONTRAST,
	TOKEN_CURVATURE,
	TOKEN_MENUBARSTYLE,
	TOKEN_MENUBARBORDERS,
	TOKEN_MENUBARITEMSTYLE,
	TOKEN_TOOLBARSTYLE,
	TOKEN_BUTTONSTYLE,
	TOKEN_MENUITEMSTYLE,
	TOKEN_LISTVIEWHEADERSTYLE,
	TOKEN_LISTVIEWITEMSTYLE,
	TOKEN_SCROLLBARSTYLE,
	TOKEN_SCROLLBARTROUGH,
	TOKEN_SCROLLBARMARGIN,
	TOKEN_SCROLLBARPADDING,
	TOKEN_SCALESLIDERSTYLE,
	TOKEN_CHECKRADIOSTYLE,
	TOKEN_PROGRESSBARSTYLE,
	TOKEN_SEPARATORSTYLE,
	TOKEN_RESIZEGRIPSTYLE,
	TOKEN_TEXTSTYLE,
	TOKEN_TEXTSHADE,
	TOKEN_ANIMATION,
	TOKEN_ARROWSIZE,
	TOKEN_TRUE,
	TOKEN_FALSE
};

static struct
  {
    const gchar        *name;
    guint               token;
  }
theme_symbols[] = {
	{ "contrast", TOKEN_CONTRAST },
	{ "curvature", TOKEN_CURVATURE },
	{ "menubarstyle", TOKEN_MENUBARSTYLE },
	{ "menubarborders", TOKEN_MENUBARBORDERS },
	{ "menubaritemstyle", TOKEN_MENUBARITEMSTYLE },
	{ "toolbarstyle", TOKEN_TOOLBARSTYLE },
	{ "buttonstyle", TOKEN_BUTTONSTYLE },
	{ "menuitemstyle", TOKEN_MENUITEMSTYLE },
	{ "listviewheaderstyle", TOKEN_LISTVIEWHEADERSTYLE },
	{ "listviewitemstyle", TOKEN_LISTVIEWITEMSTYLE },
	{ "scrollbarstyle", TOKEN_SCROLLBARSTYLE },
	{ "scrollbartrough", TOKEN_SCROLLBARTROUGH },
	{ "scrollbarmargin", TOKEN_SCROLLBARMARGIN },
	{ "scrollbarpadding", TOKEN_SCROLLBARPADDING },
	{ "scalesliderstyle", TOKEN_SCALESLIDERSTYLE },
	{ "checkradiostyle", TOKEN_CHECKRADIOSTYLE },
	{ "progressbarstyle", TOKEN_PROGRESSBARSTYLE },
	{ "separatorstyle", TOKEN_SEPARATORSTYLE },
	{ "resizegripstyle", TOKEN_RESIZEGRIPSTYLE },
	{ "textstyle", TOKEN_TEXTSTYLE },
	{ "textshade", TOKEN_TEXTSHADE },
	{ "animation", TOKEN_ANIMATION },
	{ "arrowsize", TOKEN_ARROWSIZE },
	{ "TRUE", TOKEN_TRUE },
	{ "FALSE", TOKEN_FALSE }
};


void equinox_rc_style_register_type (GTypeModule *module) {
	static const GTypeInfo object_info =   {
		sizeof (EquinoxRcStyleClass),
		(GBaseInitFunc) NULL,
		(GBaseFinalizeFunc) NULL,
		(GClassInitFunc) equinox_rc_style_class_init,
		NULL,           /* class_finalize */
		NULL,           /* class_data */
		sizeof (EquinoxRcStyle),
		0,              /* n_preallocs */
		(GInstanceInitFunc) equinox_rc_style_init,
		NULL
		};
	equinox_type_rc_style = g_type_module_register_type (module, GTK_TYPE_RC_STYLE, "EquinoxRcStyle", &object_info, 0);
}

static void equinox_rc_style_init (EquinoxRcStyle *equinox_rc) {
	equinox_rc->contrast = 1.0;
	equinox_rc->curvature = 5.0;
	equinox_rc->menubarstyle = 1;
	equinox_rc->menubarborders = 1;
	equinox_rc->menubaritemstyle = 99;
	equinox_rc->toolbarstyle = 1;
	equinox_rc->buttonstyle = 0;
	equinox_rc->menuitemstyle = 0;
	equinox_rc->listviewheaderstyle = 0;
	equinox_rc->listviewitemstyle = 0;
	equinox_rc->buttonstyle = 0;
	equinox_rc->scrollbarstyle = 1;
	equinox_rc->scrollbartrough = 0;
	equinox_rc->scrollbarmargin = 0;
	equinox_rc->scrollbarpadding[0] = 0;
	equinox_rc->scrollbarpadding[1] = 0;
	equinox_rc->scrollbarpadding[2] = 0;
	equinox_rc->scrollbarpadding[3] = 0;
	equinox_rc->scalesliderstyle = 0;
	equinox_rc->checkradiostyle = 0;
	equinox_rc->progressbarstyle = 0;
	equinox_rc->separatorstyle = 1;
	equinox_rc->resizegripstyle = 0;
	equinox_rc->textstyle = 0;
	equinox_rc->textshade = 1.12;
	equinox_rc->animation = FALSE;
	equinox_rc->arrowsize = 1.0;
}

#ifdef HAVE_ANIMATION
static void
equinox_rc_style_finalize (GObject *object)
{
	/* cleanup all the animation stuff */
	equinox_animation_cleanup ();

	if (G_OBJECT_CLASS (parent_class)->finalize != NULL)
		G_OBJECT_CLASS (parent_class)->finalize(object);
}
#endif


static void
equinox_rc_style_class_init (EquinoxRcStyleClass *klass)
{
  GtkRcStyleClass *rc_style_class = GTK_RC_STYLE_CLASS (klass);
#ifdef HAVE_ANIMATION
  GObjectClass    *g_object_class = G_OBJECT_CLASS (klass);
#endif

  parent_class = g_type_class_peek_parent (klass);

  rc_style_class->parse = equinox_rc_style_parse;
  rc_style_class->create_style = equinox_rc_style_create_style;
  rc_style_class->merge = equinox_rc_style_merge;

#ifdef HAVE_ANIMATION
  g_object_class->finalize = equinox_rc_style_finalize;
#endif
}

static guint
theme_parse_boolean (GtkSettings *settings,
                     GScanner     *scanner,
                     gboolean *retval)
{
  guint token;
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
theme_parse_color(GtkSettings  *settings,
		  GScanner     *scanner,
		  GdkColor     *color)
{
  guint token;

  /* Skip 'blah_color' */
  token = g_scanner_get_next_token(scanner);

  token = g_scanner_get_next_token(scanner);
  if (token != G_TOKEN_EQUAL_SIGN)
    return G_TOKEN_EQUAL_SIGN;

  return gtk_rc_parse_color (scanner, color);
}

static guint
theme_parse_contrast(GtkSettings  *settings,
		     GScanner     *scanner,
		     double       *contrast)
{
  guint token;

  /* Skip 'contrast' */
  token = g_scanner_get_next_token(scanner);

  token = g_scanner_get_next_token(scanner);
  if (token != G_TOKEN_EQUAL_SIGN)
    return G_TOKEN_EQUAL_SIGN;

  token = g_scanner_get_next_token(scanner);
  if (token != G_TOKEN_FLOAT)
    return G_TOKEN_FLOAT;

  *contrast = scanner->value.v_float;

  return G_TOKEN_NONE;
}

static gdouble
theme_parse_curvature(GtkSettings  *settings,
		     GScanner     *scanner,
		     double       *curvature)
{
  guint token;

  /* Skip 'curvature' */
  token = g_scanner_get_next_token(scanner);

  token = g_scanner_get_next_token(scanner);
  if (token != G_TOKEN_EQUAL_SIGN)
    return G_TOKEN_EQUAL_SIGN;


  token = g_scanner_get_next_token(scanner);
  if (token != G_TOKEN_FLOAT) {
	if (token != G_TOKEN_INT)
		return G_TOKEN_FLOAT;
	else
		  *curvature = scanner->value.v_int;
	return G_TOKEN_NONE;
  }

  *curvature = scanner->value.v_float;

  return G_TOKEN_NONE;
}

static guint
theme_parse_int (GtkSettings  *settings,
		         GScanner     *scanner,
		         guint8       *style)
{
  guint token;

  /* Skip 'parameter name' */
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
theme_parse_shade (GtkSettings  *settings,
		         GScanner     *scanner,
		         double       *ratio)
{
  guint token;

  /* Skip 'parameter name' */
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
theme_parse_int_quaternion (GtkSettings *settings,
                      GScanner *scanner,
                      guint8 quaternion[4]) {
	guint	token;

	/* Skip 'blah_border' */
	token = g_scanner_get_next_token(scanner);

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_EQUAL_SIGN)
		return G_TOKEN_EQUAL_SIGN;

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_LEFT_CURLY)
		return G_TOKEN_LEFT_CURLY;

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_INT)
		return G_TOKEN_INT;
	quaternion[0] = scanner->value.v_int;
	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_COMMA)
		return G_TOKEN_COMMA;

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_INT)
		return G_TOKEN_INT;
	quaternion[1] = scanner->value.v_int;
	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_COMMA)
		return G_TOKEN_COMMA;

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_INT)
		return G_TOKEN_INT;
	quaternion[2] = scanner->value.v_int;
	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_COMMA)
		return G_TOKEN_COMMA;

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_INT)
		return G_TOKEN_INT;
	quaternion[3] = scanner->value.v_int;

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_RIGHT_CURLY)
		return G_TOKEN_RIGHT_CURLY;

	/* save those values */

	return G_TOKEN_NONE;
}

static guint
equinox_rc_style_parse (GtkRcStyle *rc_style,
			   GtkSettings  *settings,
			   GScanner   *scanner)

{
  static GQuark scope_id = 0;
  EquinoxRcStyle *equinox_style = EQUINOX_RC_STYLE (rc_style);

  guint old_scope;
  guint token;
  guint i;

  /* Set up a new scope in this scanner. */

  if (!scope_id)
    scope_id = g_quark_from_string("equinox_theme_engine");

  /* If we bail out due to errors, we *don't* reset the scope, so the
   * error messaging code can make sense of our tokens.
   */
  old_scope = g_scanner_set_scope(scanner, scope_id);

  /* Now check if we already added our symbols to this scope
   * (in some previous call to equinox_rc_style_parse for the
   * same scanner.
   */

  if (!g_scanner_lookup_symbol(scanner, theme_symbols[0].name))
    {
      g_scanner_freeze_symbol_table(scanner);
      for (i = 0; i < G_N_ELEMENTS (theme_symbols); i++)
	g_scanner_scope_add_symbol(scanner, scope_id,
				   theme_symbols[i].name,
				   GINT_TO_POINTER(theme_symbols[i].token));
      g_scanner_thaw_symbol_table(scanner);
    }

  /* We're ready to go, now parse the top level */

  token = g_scanner_peek_next_token(scanner);
  while (token != G_TOKEN_RIGHT_CURLY) {
		switch (token) {
			case TOKEN_CONTRAST:
				token = theme_parse_contrast (settings, scanner, &equinox_style->contrast);
				equinox_style->flags |= EQX_FLAG_CONTRAST;
				break;
			case TOKEN_CURVATURE:
				token = theme_parse_curvature (settings, scanner, &equinox_style->curvature);
				equinox_style->flags |= EQX_FLAG_CURVATURE;
				break;
			case TOKEN_MENUBARSTYLE:
				token = theme_parse_int (settings, scanner, &equinox_style->menubarstyle);
				equinox_style->flags |= EQX_FLAG_MENUBARSTYLE;
				break;
			case TOKEN_MENUBARBORDERS:
				token = theme_parse_int (settings, scanner, &equinox_style->menubarborders);
				equinox_style->flags |= EQX_FLAG_MENUBARBORDERS;
				break;
			case TOKEN_MENUBARITEMSTYLE:
				token = theme_parse_int (settings, scanner, &equinox_style->menubaritemstyle);
				equinox_style->flags |= EQX_FLAG_MENUBARITEMSTYLE;
				break;
			case TOKEN_TOOLBARSTYLE:
				token = theme_parse_int (settings, scanner, &equinox_style->toolbarstyle);
				equinox_style->flags |= EQX_FLAG_TOOLBARSTYLE;
				break;
			case TOKEN_BUTTONSTYLE:
				token = theme_parse_int (settings, scanner, &equinox_style->buttonstyle);
				equinox_style->flags |= EQX_FLAG_BUTTONSTYLE;
				break;
			case TOKEN_MENUITEMSTYLE:
				token = theme_parse_int (settings, scanner, &equinox_style->menuitemstyle);
				equinox_style->flags |= EQX_FLAG_MENUITEMSTYLE;
				break;
			case TOKEN_LISTVIEWHEADERSTYLE:
				token = theme_parse_int (settings, scanner, &equinox_style->listviewheaderstyle);
				equinox_style->flags |= EQX_FLAG_LISTVIEWHEADERSTYLE;
				break;
			case TOKEN_LISTVIEWITEMSTYLE:
				token = theme_parse_int (settings, scanner, &equinox_style->listviewitemstyle);
				equinox_style->flags |= EQX_FLAG_LISTVIEWITEMSTYLE;
				break;
			case TOKEN_SCROLLBARSTYLE:
				token = theme_parse_int (settings, scanner, &equinox_style->scrollbarstyle);
				equinox_style->flags |= EQX_FLAG_SCROLLBARSTYLE;
				break;
			case TOKEN_SCROLLBARTROUGH:
				token = theme_parse_int (settings, scanner, &equinox_style->scrollbartrough);
				equinox_style->flags |= EQX_FLAG_SCROLLBARTROUGH;
				break;
			case TOKEN_SCROLLBARMARGIN:
				token = theme_parse_int (settings, scanner, &equinox_style->scrollbarmargin);
				equinox_style->flags |= EQX_FLAG_SCROLLBARMARGIN;
				break;
			case TOKEN_SCROLLBARPADDING:
				token = theme_parse_int_quaternion (settings, scanner, equinox_style->scrollbarpadding);
				equinox_style->flags |= EQX_FLAG_SCROLLBARPADDING;
				break;
			case TOKEN_SCALESLIDERSTYLE:
				token = theme_parse_int (settings, scanner, &equinox_style->scalesliderstyle);
				equinox_style->flags |= EQX_FLAG_SCALESLIDERSTYLE;
				break;
			case TOKEN_CHECKRADIOSTYLE:
				token = theme_parse_int (settings, scanner, &equinox_style->checkradiostyle);
				equinox_style->flags |= EQX_FLAG_CHECKRADIOSTYLE;
				break;
			case TOKEN_PROGRESSBARSTYLE:
				token = theme_parse_int (settings, scanner, &equinox_style->progressbarstyle);
				equinox_style->flags |= EQX_FLAG_PROGRESSBARSTYLE;
				break;
			case TOKEN_SEPARATORSTYLE:
				token = theme_parse_int (settings, scanner, &equinox_style->separatorstyle);
				equinox_style->flags |= EQX_FLAG_SEPARATORSTYLE;
				break;
			case TOKEN_RESIZEGRIPSTYLE:
				token = theme_parse_int (settings, scanner, &equinox_style->resizegripstyle);
				equinox_style->flags |= EQX_FLAG_RESIZEGRIPSTYLE;
				break;
			case TOKEN_TEXTSTYLE:
				token = theme_parse_int (settings, scanner, &equinox_style->textstyle);
				equinox_style->flags |= EQX_FLAG_TEXTSTYLE;
				break;
			case TOKEN_TEXTSHADE:
				token = theme_parse_shade (settings, scanner, &equinox_style->textshade);
				equinox_style->flags |= EQX_FLAG_TEXTSHADE;
				break;
			case TOKEN_ANIMATION:
				token = theme_parse_boolean (settings, scanner, &equinox_style->animation);
				equinox_style->flags |= EQX_FLAG_ANIMATION;
				break;
			case TOKEN_ARROWSIZE:
				token = theme_parse_curvature (settings, scanner, &equinox_style->arrowsize);
				equinox_style->flags |= EQX_FLAG_ARROWSIZE;
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

static void equinox_rc_style_merge (GtkRcStyle *dest, GtkRcStyle *src) {
	EquinoxRcStyle *dest_w, *src_w;
	EquinoxRcFlags flags;

	parent_class->merge (dest, src);

	if (!EQUINOX_IS_RC_STYLE (src))
		return;

	src_w = EQUINOX_RC_STYLE (src);
	dest_w = EQUINOX_RC_STYLE (dest);

	flags = (~dest_w->flags) & src_w->flags;

	if (flags & EQX_FLAG_CONTRAST)
		dest_w->contrast = src_w->contrast;
	if (flags & EQX_FLAG_CURVATURE)
		dest_w->curvature = src_w->curvature;
	if (flags & EQX_FLAG_MENUBARSTYLE)
		dest_w->menubarstyle = src_w->menubarstyle;
	if (flags & EQX_FLAG_MENUBARBORDERS)
		dest_w->menubarborders = src_w->menubarborders;
	if (flags & EQX_FLAG_MENUBARITEMSTYLE)
		dest_w->menubaritemstyle = src_w->menubaritemstyle;
	if (flags & EQX_FLAG_TOOLBARSTYLE)
		dest_w->toolbarstyle = src_w->toolbarstyle;
	if (flags & EQX_FLAG_BUTTONSTYLE)
		dest_w->buttonstyle = src_w->buttonstyle;
	if (flags & EQX_FLAG_MENUITEMSTYLE)
		dest_w->menuitemstyle = src_w->menuitemstyle;
	if (flags & EQX_FLAG_LISTVIEWHEADERSTYLE)
		dest_w->listviewheaderstyle = src_w->listviewheaderstyle;
	if (flags & EQX_FLAG_LISTVIEWITEMSTYLE)
		dest_w->listviewitemstyle = src_w->listviewitemstyle;
	if (flags & EQX_FLAG_SCROLLBARSTYLE)
		dest_w->scrollbarstyle = src_w->scrollbarstyle;
	if (flags & EQX_FLAG_SCROLLBARTROUGH)
		dest_w->scrollbartrough = src_w->scrollbartrough;
	if (flags & EQX_FLAG_SCROLLBARMARGIN)
		dest_w->scrollbarmargin = src_w->scrollbarmargin;
	if (flags & EQX_FLAG_SCROLLBARPADDING)
		dest_w->scrollbarpadding[0] = src_w->scrollbarpadding[0];
		dest_w->scrollbarpadding[1] = src_w->scrollbarpadding[1];
		dest_w->scrollbarpadding[2] = src_w->scrollbarpadding[2];
		dest_w->scrollbarpadding[3] = src_w->scrollbarpadding[3];
	if (flags & EQX_FLAG_SCALESLIDERSTYLE)
		dest_w->scalesliderstyle = src_w->scalesliderstyle;
	if (flags & EQX_FLAG_CHECKRADIOSTYLE)
		dest_w->checkradiostyle = src_w->checkradiostyle;
	if (flags & EQX_FLAG_PROGRESSBARSTYLE)
		dest_w->progressbarstyle = src_w->progressbarstyle;
	if (flags & EQX_FLAG_SEPARATORSTYLE)
		dest_w->separatorstyle = src_w->separatorstyle;
	if (flags & EQX_FLAG_RESIZEGRIPSTYLE)
		dest_w->resizegripstyle = src_w->resizegripstyle;
	if (flags & EQX_FLAG_TEXTSTYLE)
		dest_w->textstyle = src_w->textstyle;
	if (flags & EQX_FLAG_TEXTSHADE)
		dest_w->textshade = src_w->textshade;
	if (flags & EQX_FLAG_ANIMATION)
		dest_w->animation = src_w->animation;
	if (flags & EQX_FLAG_ARROWSIZE)
		dest_w->arrowsize = src_w->arrowsize;

	dest_w->flags |= src_w->flags;
}


/* Create an empty style suitable to this RC style
 */
static GtkStyle *equinox_rc_style_create_style (GtkRcStyle *rc_style) {
  return GTK_STYLE (g_object_new (EQUINOX_TYPE_STYLE, NULL));
}
