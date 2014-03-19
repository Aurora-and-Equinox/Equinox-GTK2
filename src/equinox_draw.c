#include "equinox_draw.h"
#include "equinox_style.h"
#include "equinox_types.h"

#include "support.h"

#include <cairo.h>
#include <math.h>

#define M_PI 3.14159265358979323846
#define M_SQRT3OVER2  0.8660254037844386467637
#define ARROW_WIDTH 7
#define ARROW_HEIGHT 7
#define RESIZE_GRIP_SIZE 8
#define SCALE_TROUGH_SIZE 4
#define HANDLE_RADIUS 2.5
// Static Equinox Functions



static void clearlooks_rounded_rectangle (cairo_t * cr, double x, double y, double w, double h, double radius, uint8 corners) {

    if (radius < 0.01 || (corners == EQX_CORNER_NONE)) {
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

static void
equinox_rounded_rectangle_closed (cairo_t *cr,
                                  double x, double y, double w, double h,
                                  int radius, uint8 corners)
{
	radius < 2 ? cairo_rectangle (cr, x, y, w, h) :
	             clearlooks_rounded_rectangle (cr, x, y, w, h, radius, corners);
}

static void nodoka_rounded_rectangle_inverted (cairo_t * cr, double x, double y, double w, double h, double r, uint8 corners) {

	cairo_translate (cr, x, y);

	if (corners & EQX_CORNER_TOPLEFT)
		cairo_move_to (cr, 0, -r);
	else
		cairo_move_to (cr, 0, 0);

	if (corners & EQX_CORNER_BOTTOMLEFT)
		cairo_arc (cr, r, h + r, r, M_PI * 1.0, M_PI * 1.5);
	else
		cairo_line_to (cr, 0, h);

	if (corners & EQX_CORNER_BOTTOMRIGHT)
		cairo_arc (cr, w - r, h + r, r, M_PI * 1.5, M_PI * 2.0);
	else
		cairo_line_to (cr, w, h);

	if (corners & EQX_CORNER_TOPRIGHT)
		cairo_arc (cr, w - r, -r, r, M_PI * 0.0, M_PI * 0.5);
	else
		cairo_line_to (cr, w, 0);

	if (corners & EQX_CORNER_TOPLEFT)
		cairo_arc (cr, r, -r, r, M_PI * 0.5, M_PI * 1.0);
	else
		cairo_line_to (cr, 0, 0);

	cairo_translate (cr, -x, -y);

}


static void
rotate_mirror_translate (cairo_t * cr, double radius, double x, double y,
			 boolean mirror_horizontally,
			 boolean mirror_vertically)
{

	cairo_matrix_t matrix_result;

	double r_cos = cos (radius);
	double r_sin = sin (radius);

	cairo_matrix_init (&matrix_result, mirror_horizontally ? -r_cos : r_cos,
		     mirror_horizontally ? -r_sin : r_sin,
		     mirror_vertically ? -r_sin : r_sin,
		     mirror_vertically ? -r_cos : r_cos, x, y);

	cairo_set_matrix (cr, &matrix_result);
}

static void  equinox_rectangle (cairo_t *cr, EquinoxCairoOps ops,
				double x, double y, double w, double h,
				const EquinoxRGB *color, double alpha) {
	cairo_rectangle (cr, x, y, w, h);
	equinox_set_source_rgba (cr, color, alpha);
	if (ops == EQX_CAIRO_STROKE)
		cairo_stroke (cr);
	else
		cairo_fill (cr);
}

static void  equinox_rounded_rectangle (cairo_t *cr, EquinoxCairoOps ops,
				double x, double y, double w, double h,
				double radius, uint8 corners,
				const EquinoxRGB *color, double alpha) {
	clearlooks_rounded_rectangle (cr, x, y, w, h, radius, corners);
	equinox_set_source_rgba (cr, color, alpha);
	if (ops == EQX_CAIRO_STROKE)
		cairo_stroke (cr);
	else
		cairo_fill (cr);
}

static void  equinox_rounded_gradient (cairo_t *cr, EquinoxCairoOps ops,
				double x, double y, double w, double h,
				double radius, uint8 corners,
				cairo_pattern_t *pattern) {
	cairo_set_source (cr, pattern);
	clearlooks_rounded_rectangle (cr, x, y, w, h, radius, corners);
	if (ops == EQX_CAIRO_STROKE)
		cairo_stroke (cr);
	else
		cairo_fill (cr);
	cairo_pattern_destroy (pattern);
}

static void  equinox_rectangle_gradient (cairo_t *cr, EquinoxCairoOps ops,
				double x, double y, double w, double h,
				cairo_pattern_t *pattern) {
	cairo_set_source (cr, pattern);
	cairo_rectangle (cr, x, y, w, h);
	if (ops == EQX_CAIRO_STROKE)
		cairo_stroke (cr);
	else
		cairo_fill (cr);
	cairo_pattern_destroy (pattern);
}

static void  equinox_arc (cairo_t *cr, EquinoxCairoOps ops,
				double xc, double yc, double radius, double angle1, double angle2,
				const EquinoxRGB *color, double alpha) {
	equinox_set_source_rgba (cr, color, alpha);
	cairo_arc (cr, xc, yc, radius, angle1, angle2);
	if (ops == EQX_CAIRO_STROKE)
		cairo_stroke (cr);
	else
		cairo_fill (cr);
}

static void  equinox_arc_gradient (cairo_t *cr, EquinoxCairoOps ops,
				double xc, double yc, double radius, double angle1, double angle2,
				cairo_pattern_t *pattern) {
	cairo_set_source (cr, pattern);
	cairo_arc (cr, xc, yc, radius, angle1, angle2);
	if (ops == EQX_CAIRO_STROKE)
		cairo_stroke (cr);
	else
		cairo_fill (cr);
	cairo_pattern_destroy (pattern);
}

static void equinox_draw_inset_circle (cairo_t * cr, double xc, double yc, double radius, const EquinoxRGB *color, int orientation) {
	EquinoxRGB dark, middle, light;
	cairo_pattern_t *pattern;

	double lightness = equinox_get_lightness(color);

	/*equinox_shade (color, &dark, 0.85 - lightness / 3);
	equinox_shade (color, &middle, 1.0);
	equinox_shade (color, &light, 1.05 + lightness / 3);*/

	equinox_shade (color, &dark, 0.40);
	equinox_shade (color, &middle, 0.95);
	equinox_shade (color, &light, 1.30);


	if (orientation == 0)
		pattern = cairo_pattern_create_linear (0, yc - radius, 0, yc + radius);
	else
		pattern = cairo_pattern_create_linear (xc - radius, 0, xc + radius, 0);
	equinox_pattern_add_color_rgba (pattern, 0.0, &dark, 0.85);
	equinox_pattern_add_color_rgba (pattern, 0.6, &middle, 0.85);
	equinox_pattern_add_color_rgba (pattern, 1.0, &light, 0.85);
	equinox_arc_gradient (cr, EQX_CAIRO_FILL, xc, yc, radius, 0, M_PI * 2, pattern);

	if (orientation == 0)
		pattern = cairo_pattern_create_linear (0, yc - radius, 0, yc + radius);
	else
		pattern = cairo_pattern_create_linear (xc - radius, 0, xc + radius, 0);
	equinox_pattern_add_color_rgba (pattern, 0.5, &dark, 0.25);
	equinox_pattern_add_color_rgba (pattern, 1.0, &light, 0.25);
	equinox_arc_gradient (cr, EQX_CAIRO_STROKE, xc, yc, radius - 0.5, 0, M_PI * 2, pattern);
}

static void equinox_draw_etched_shadow (cairo_t * cr, double x, double y, double width,
			    double height, double radius, uint8 corners,
			    const EquinoxRGB*  bg_color, double highlight_modifier) {

	EquinoxRGB shadow;
	EquinoxRGB highlight;
	cairo_pattern_t *pattern;

	equinox_shade (bg_color, &shadow, 0.82);
	equinox_shade (bg_color, &highlight, 1.25 * highlight_modifier);

	// the highlight
	pattern = cairo_pattern_create_linear (0, y, 0, y + height);
	equinox_pattern_add_color_rgba (pattern, 0.75, bg_color, 0.0);
	equinox_pattern_add_color_rgba (pattern, 0.95, &highlight, 0.64);
	equinox_rounded_gradient (cr, EQX_CAIRO_STROKE, x, y, width, height, radius, corners, pattern);

	// the shadow
	pattern = cairo_pattern_create_linear (0, y, 0, y + height);
	equinox_pattern_add_color_rgba (pattern, 0.0, &shadow, 0.32);
	equinox_pattern_add_color_rgba (pattern, 1.0, bg_color, 0.06);
	equinox_rounded_gradient (cr, EQX_CAIRO_STROKE, x + 0.5, y, width - 1, height, radius, corners, pattern);
}

//simple drop shadow
static void
equinox_draw_shadow (cairo_t * cr, double x, double y, double width,
		    double height, double radius, uint8 corners,
		    const EquinoxRGB*  bg_color, double alpha) {

	EquinoxRGB shadow;
	cairo_pattern_t *pattern;

	if (alpha < 1) {
		equinox_shade (bg_color, &shadow, 0.885);
		pattern = cairo_pattern_create_linear (0, y, 0, y + height + 1);
		equinox_pattern_add_color_rgba (pattern, 0.0, &shadow, 0.0);
		equinox_pattern_add_color_rgba (pattern, 0.4, &shadow, 0.75 * alpha);
		equinox_pattern_add_color_rgba (pattern, 1.0 , &shadow, alpha);
	} else {
		equinox_shade (bg_color, &shadow, 0.90);
		pattern = cairo_pattern_create_linear (0, y, 0, y + height);
		equinox_pattern_add_color_rgb (pattern, 0.0, bg_color);
		equinox_pattern_add_color_rgb (pattern, 1.0, &shadow);
	}
	equinox_rounded_gradient (cr, EQX_CAIRO_STROKE, x + 0.5, y + 1, width - 1, height - 1, radius, corners, pattern);
}


//shaded border
static void equinox_draw_border (cairo_t * cr, double x, double y, double width,
		    double height, double radius, uint8 corners,
		    const EquinoxRGB*  border, double lighter, double darker) {
	cairo_pattern_t *pattern;
	EquinoxRGB border_darker, border_lighter;
	equinox_shade_shift (border, &border_darker, darker);
	equinox_shade_shift (border, &border_lighter, lighter);

	pattern = cairo_pattern_create_linear (0, y, 0, y + height);
	equinox_pattern_add_color_rgb (pattern, 0.0, &border_lighter);
	equinox_pattern_add_color_rgb (pattern, 0.5, border);
	equinox_pattern_add_color_rgb (pattern, 1.0, &border_darker);
	equinox_rounded_gradient (cr, EQX_CAIRO_STROKE, x, y, width, height, radius, corners, pattern);
}

static void equinox_draw_etched_border (cairo_t * cr, double x, double y, double width,
		    double height, double radius, uint8 corners,
		    const EquinoxRGB*  border, double lighter, double darker) {
	cairo_pattern_t *pattern;
	EquinoxRGB border_darker, border_lighter;
	equinox_shade_shift (border, &border_darker, darker);
	equinox_shade_shift (border, &border_lighter, lighter);

	pattern = cairo_pattern_create_linear (0, y, 0, y + height);
	equinox_pattern_add_color_rgb (pattern, (radius/2+1)/height, &border_darker);
	equinox_pattern_add_color_rgb (pattern, (radius/2+1)/height, border);
	equinox_pattern_add_color_rgba (pattern, (height-radius/2-1)/height, border, 0.90);
	equinox_pattern_add_color_rgba (pattern, 1.0, &border_lighter, 0.65);
	equinox_rounded_gradient (cr, EQX_CAIRO_STROKE, x, y, width, height, radius, corners, pattern);
}

static void equinox_draw_stepper_shadow (cairo_t * cr, double x, double y, double width,
		    double height, double radius, uint8 corners,
		    const EquinoxRGB*  shadow) {
	equinox_set_source_rgba (cr, shadow, 0.2);
	if (radius >= 1)
		nodoka_rounded_rectangle_inverted (cr, x, y, width, height, radius, corners);
	else
		cairo_rectangle (cr, x, y, width, height);
	cairo_fill (cr);
	equinox_set_source_rgba (cr, shadow, 0.7);
	if (radius >= 1)
		nodoka_rounded_rectangle_inverted (cr, x, y+1, width, height-2, radius, corners);
	else
		cairo_rectangle (cr, x, x, width, height -1);
	cairo_fill (cr);
}

static void equinox_draw_inner_glow (cairo_t * cr, double x, double y, double width,
		    double height, double radius, uint8 corners,
		    const EquinoxRGB*  highlight) {

	cairo_pattern_t *pattern = cairo_pattern_create_radial (width/2, height/1.1, 0, width/2, height/1.1, width/2);;
	EquinoxRGB glow;

	equinox_shade_shift (highlight, &glow, 1.10);
	equinox_pattern_add_color_rgba (pattern, 0.0, &glow, 0.85);
	equinox_pattern_add_color_rgba (pattern, 1.0, &glow, 0.0);
	equinox_rounded_gradient (cr, EQX_CAIRO_FILL, x, y, width - (2 * x), height - (2 * y), radius, corners, pattern);
}

// Widget Functions


/*
  Draws the buttons, toolbar buttons and toggle buttons
  Varries based on disabled, active, prelight and focus states
*/
void
equinox_draw_button (cairo_t * cr,
		    const EquinoxColors * colors,
		    const WidgetParameters * widget,
		    int x, int y, int width, int height, int buttonstyle)
{


	//Fill color, highlight color and border color
	EquinoxRGB fill;
	EquinoxRGB fill_darker, mid_highlight;
	EquinoxRGB highlight;
	EquinoxRGB border;
	cairo_pattern_t *pattern;
	cairo_translate (cr, x, y);

	fill = colors->bg[widget->state_type];
#ifdef HAVE_ANIMATION
	EquinoxRGB fill_old;
	fill_old = colors->bg[widget->prev_state_type];
#endif

	double lightness = equinox_get_lightness(&colors->bg[GTK_STATE_NORMAL]);
	double highlight_scale = lightness < 0.6 ? 0.19*(0.6-lightness):0;//0.15*(0.902-lightness);
	boolean focus_border = lightness > 0.6;

	if (widget->active && widget->prelight)
		equinox_shade (&fill, &fill, 0.78);

#ifdef HAVE_ANIMATION
	if (widget->active && widget->prev_state_type == GTK_STATE_PRELIGHT)
		equinox_shade (&fill_old, &fill_old, 0.78);
	equinox_mix_color (&fill_old, &fill, widget->trans, &fill);
#endif

	//Highlight
	if (widget->disabled)
		equinox_shade_shift (&fill, &highlight, 1.05 + 0.25*highlight_scale);
	else if (widget->active && !widget->prelight)
		equinox_shade_shift (&fill, &highlight, 1.05 + 0.5*highlight_scale);
	else
		equinox_shade_shift (&fill, &highlight, 1.06 + highlight_scale); // 1.10

	//border color a blend of bg color and button color
	equinox_mix_color (&colors->shade[widget->disabled?4:8], &fill, 0.30, &border);
	equinox_tweak_saturation (&fill,&border);

	int roundness = MIN (widget->curvature, (height - 4) / 2.0);

	//Draw the fill
	if (buttonstyle == 1) {
		equinox_shade (&fill, &fill_darker, 0.88); // 0.82
		equinox_shade (&fill, &highlight, lightness < 0.5 ? 1.5 : 1.225); // 1.175

		equinox_mix_color (&fill_darker, &highlight, 0.85, &highlight); // 0.65
		equinox_mix_color (&fill_darker, &highlight, 0.4, &mid_highlight); // 0.45

		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		equinox_pattern_add_color_rgb (pattern, 0.0, &highlight);
		equinox_pattern_add_color_rgb (pattern, 0.5, &mid_highlight);
		equinox_pattern_add_color_rgb (pattern, 0.5, &fill_darker);
		equinox_pattern_add_color_rgb (pattern, 1.0, &fill);
  	} else {
		if (widget->active) {
			equinox_shade_shift (&fill, &fill_darker, widget->disabled? 0.96 : 0.93); // 0.95 : 0.91
			equinox_mix_color (&fill, &highlight, 0.10 + 0.10*highlight_scale, &mid_highlight);
		} else {
			equinox_shade_shift (&fill, &fill_darker, widget->disabled? 0.96 : 0.91); // 0.95 : 0.88
			equinox_mix_color (&fill, &highlight, 0.10 + 0.65*highlight_scale, &mid_highlight);
		}

		pattern = cairo_pattern_create_linear (2, 2, 2, height-4);
		equinox_pattern_add_color_rgb (pattern, 0.0, &highlight);
		equinox_pattern_add_color_rgb (pattern, 0.5, &mid_highlight);
		equinox_pattern_add_color_rgb (pattern, 0.5, &fill);
		equinox_pattern_add_color_rgb (pattern, 1.0, &fill_darker);
	}
	equinox_rounded_gradient (cr, EQX_CAIRO_FILL, 2, 2, width-4, height-4, roundness - 1, widget->corners, pattern);

	//Check if pressed...
	if (widget->active) {
		//"pressed" add an inner shadow
		EquinoxRGB shadow;

		equinox_shade (&border, &shadow, 0.75);
		if (widget->focus && focus_border) {
			equinox_mix_color (&shadow, &colors->spot[2], 0.85, &shadow);
			equinox_shade_shift (&shadow, &shadow, lightness < 0.5 ? 1.5 : 1.2);
		}

		cairo_save (cr);
		clearlooks_rounded_rectangle (cr, 2, 2, width-4, height-4, roundness - 0.5, widget->corners);
		cairo_clip (cr);

		equinox_rounded_rectangle (cr, EQX_CAIRO_STROKE, 2, 2.5, width - 4, height - 3, roundness-0.5, widget->corners, &shadow, widget->disabled ? 0.18 : 0.24);
		equinox_rounded_rectangle (cr, EQX_CAIRO_STROKE, 3, 3.5, width - 6 , height - 6, roundness-1.0, widget->corners, &shadow, widget->disabled? 0.06 : 0.08);
		//equinox_rounded_rectangle (cr, EQX_CAIRO_STROKE, 4, 5.5, width - 8, height - 3, roundness-2.0, widget->corners, &shadow, 0.07);

		cairo_restore (cr);
	}
	else {
		//If not pressed add a 1px highlight along the left and top
		equinox_shade_shift (&highlight, &highlight, widget->disabled? 1.05 : 1.1);

		pattern = cairo_pattern_create_linear (2, 2, 2, height-4);
		equinox_pattern_add_color_rgb  (pattern, 0.0, &highlight);
		equinox_pattern_add_color_rgba (pattern, 1.0, &highlight, 0.12);
		equinox_rounded_gradient (cr, EQX_CAIRO_STROKE, 2.5, 2.5, width - 5, height - 5, roundness - 1, widget->corners, pattern);
		cairo_stroke (cr);
	}

	//the etching/shadow and border
	if (widget->active) {
		equinox_draw_etched_shadow (cr, 0.5, 0.5, width - 1, height - 1.5, roundness + 1, widget->corners, &widget->parentbg, 1.0);
		if (widget->focus && focus_border) {
			equinox_mix_color (&border, &colors->spot[1], 0.85, &border);
			equinox_shade_shift (&border, &border, 0.85);
		}
		equinox_draw_etched_border (cr, 1.5, 1.5, width - 3, height - 3, roundness, widget->corners, &border, widget->disabled ? 1.02 : 1.05, widget->disabled ? 0.85 : 0.95);
	}
	else {
		if (widget->focus && focus_border) {
			equinox_mix_color (&border, &colors->spot[2], 0.85, &border);
			equinox_shade_shift (&border, &border, lightness < 0.5 ? 1.5 : 1.2);
		} else if (widget->is_default)
			equinox_shade_shift (&border, &border, 0.86);
		equinox_draw_border (cr, 1.5, 1.5, width - 3, height - 3, roundness, widget->corners, &border, widget->disabled ? 1.08 : 1.18, widget->disabled ? 0.82 : 0.86);
		equinox_draw_shadow (cr, 0.5, 0.5, width - 1, height - 1, roundness + 1.5, widget->corners, &border, widget->disabled ? 0.25 : widget->is_default ? 0.14 : 0.25);
		if (widget->focus && focus_border)
			equinox_rounded_rectangle (cr, EQX_CAIRO_STROKE, 0.5, 0.5, width - 1, height - 0.1, widget->curvature + 2, widget->corners, &border, 0.08);
	}

	//Focus line
	if (widget->focus && !focus_border) {
		equinox_rounded_rectangle (cr, EQX_CAIRO_STROKE, 2.5, 2.5, width - 5, height - 5, widget->curvature - 1, widget->corners, &colors->spot[1], widget->active ? 0.5 : 0.75);
		equinox_rounded_rectangle (cr, EQX_CAIRO_STROKE, 3.5, 3.5, width - 7, height - 7, widget->curvature - 2, widget->corners, &colors->spot[1], widget->active ? 0.15 : 0.25);
		//equinox_draw_shadow (cr, 3.5, 3.5, width - 7, height - 7, roundness + 1.5, widget->corners, &colors->spot[1], widget->is_default ? 0.14 : 0.25);

		/*pattern = cairo_pattern_create_linear (4, 0, width - 8, 0);
		equinox_pattern_add_color_rgba (pattern, 0.0, &colors->spot[1], 0.0);
		equinox_pattern_add_color_rgba (pattern, 0.25, &colors->spot[1], 1.0);
		equinox_pattern_add_color_rgba (pattern, 0.75, &colors->spot[1], 1.0);
		equinox_pattern_add_color_rgba (pattern, 1.0, &colors->spot[1], 0.0);
		cairo_set_source (cr, pattern);
		cairo_move_to (cr, 4, height - y_offset);
		cairo_line_to (cr, width - 8, height - y_offset);
		cairo_stroke (cr);
		cairo_pattern_destroy (pattern);
		pattern = cairo_pattern_create_linear (4, 0, width - 8, 0);
		equinox_pattern_add_color_rgba (pattern, 0.0, &colors->spot[1], 0.0);
		equinox_pattern_add_color_rgba (pattern, 0.35, &colors->spot[1], 0.30);
		equinox_pattern_add_color_rgba (pattern, 0.65, &colors->spot[1], 0.30);
		equinox_pattern_add_color_rgba (pattern, 1.0, &colors->spot[1], 0.0);
		cairo_set_source (cr, pattern);
		cairo_move_to (cr, 4, height - y_offset - 1);
		cairo_line_to (cr, width - 8, height - y_offset - 1);
		cairo_stroke (cr);
		cairo_pattern_destroy (pattern);*/
	}

}

void
equinox_draw_entry (cairo_t * cr,
		   const EquinoxColors * colors,
		   const WidgetParameters * widget,
		   const EntryParameters * entry,
		   int x, int y, int width, int height, int toolbarstyle)
{

	EquinoxRGB fill;
	cairo_pattern_t *pattern;
	EquinoxRGB color1, color2, parentbg1, parentbg2;
	double lightness = equinox_get_lightness(&widget->parentbg);
	boolean focus_border = (lightness > 0.6);

	int roundness = MIN (widget->curvature, (height - 4) / 2.0);

	// Fill the background (shouldn't have to)
	if (entry->isToolbarItem && !entry->isComboButton) {
		if (toolbarstyle==0 || toolbarstyle==2) {
			equinox_set_source_rgb (cr, &widget->parentbg);
			cairo_rectangle (cr, 0, 0, width, height);
			cairo_fill (cr);
			parentbg1 = widget->parentbg;
			parentbg2 = widget->parentbg;
		} else {
			equinox_shade (&widget->parentbg, &parentbg1, 0.92);
			equinox_shade (&widget->parentbg, &parentbg2, 0.84);
			pattern = cairo_pattern_create_linear (0, 0, 0, height);
			equinox_pattern_add_color_rgb (pattern, 0.0, &parentbg1);
			equinox_pattern_add_color_rgb (pattern, 1.0, &parentbg2);
			equinox_rectangle_gradient (cr, EQX_CAIRO_FILL, 0, 0, width, height, pattern);
		}
	} else {
		equinox_set_source_rgb (cr, &widget->parentbg);
		cairo_rectangle (cr, 0, 0, width, height);
		cairo_fill (cr);
		parentbg1 = widget->parentbg;
		parentbg2 = widget->parentbg;
	}

  cairo_translate (cr, x + 0.5, y + 0.5);
	equinox_rounded_rectangle (cr, EQX_CAIRO_FILL, 0.5, 0.5, width - 2, height - 2, roundness - 0.5, widget->corners, &colors->base[widget->state_type], 1.0);

	//highlight
	equinox_shade (&parentbg2, &color2, equinox_get_lightness(&colors->bg[GTK_STATE_NORMAL]) > 0.6 ? 1.08 : 1.2);
	pattern = cairo_pattern_create_linear (0, 0, 0, height);
	equinox_pattern_add_color_rgba (pattern, 0.0, &color2, 0.0);
	equinox_pattern_add_color_rgba (pattern, 1.0, &color2, 0.8);
	equinox_rounded_gradient (cr, EQX_CAIRO_STROKE, 0, 1, width - 1, height - 2, roundness + 1, widget->corners, pattern);

	//inner shadow
	double width_offset = (entry->type != EQX_ENTRY_NORMAL) ? 1.0 : 0.0;
	double x_offset = (entry->isComboButton || entry->type == EQX_ENTRY_SPINBUTTON) ? -1.0 : 0.0;
	if (widget->focus && focus_border) {
		equinox_mix_color (&colors->base[widget->state_type], &colors->spot[1], 0.15, &color1);
	} else {
		equinox_mix_color (&colors->base[widget->state_type], &widget->parentbg, 0.35, &color1);
	}
	equinox_shade (&color1, &color1, (widget->disabled ? 0.92 : 0.78) - 0.01 * 1 / lightness);
	pattern = cairo_pattern_create_linear (0, 2, 0, height - 4);
	equinox_pattern_add_color_rgba (pattern, 0.0, &color1, 0.64);
	equinox_pattern_add_color_rgba (pattern, 1.0, &color1, 0.32);
	equinox_rounded_gradient (cr, EQX_CAIRO_STROKE, 1 + x_offset, 2, width - 3 + width_offset, height - 5, roundness - 1, widget->corners, pattern);
	equinox_rounded_rectangle (cr, EQX_CAIRO_STROKE, 2 + (2*x_offset), 3, width - 5 + (2*width_offset), height - 7, roundness - 2, widget->corners, &color1, 0.12);

	//border
	if (widget->focus && focus_border) {
		equinox_mix_color (&parentbg1, &colors->spot[2], 0.85, &color1);
		equinox_mix_color (&parentbg2, &colors->spot[2], 0.45, &color2);
	} else {
		equinox_shade (&parentbg1, &color1, (widget->disabled ? 0.75 : 0.56) - 0.03 * 1 / lightness);
		equinox_shade (&parentbg2, &color2, (widget->disabled ? 0.92 : 0.88) - 0.03 * 1 / lightness);
	}
	pattern = cairo_pattern_create_linear (0, 0, 0, height);
	equinox_pattern_add_color_rgb (pattern, 0.0, &color1);
	equinox_pattern_add_color_rgb (pattern, 1.0, &color2);
	equinox_rounded_gradient (cr, EQX_CAIRO_STROKE, 0, 1, width - 1, height - 3, roundness, widget->corners, pattern);

	//Focus line
	if (widget->focus && !focus_border && !entry->isSpinButton) {
		if (entry->isComboButton) {
			equinox_rounded_rectangle (cr, EQX_CAIRO_STROKE, 0, 2, width - 2, height - 5, widget->curvature - 1, widget->corners, &colors->spot[1], 0.75);
			equinox_rounded_rectangle (cr, EQX_CAIRO_STROKE, 0, 3, width - 2, height - 7, widget->curvature - 2, widget->corners, &colors->spot[1], 0.25);
		} else {
			equinox_rounded_rectangle (cr, EQX_CAIRO_STROKE, 1, 2, width - 3, height - 5, widget->curvature - 1, widget->corners, &colors->spot[1], 0.75);
			equinox_rounded_rectangle (cr, EQX_CAIRO_STROKE, 2, 3, width - 5, height - 7, widget->curvature - 2, widget->corners, &colors->spot[1], 0.25);
		}
		/*double y_offset = equinox_get_lightness(&colors->bg[GTK_STATE_NORMAL]) > 0.6 ? 2 : 3;
		pattern = cairo_pattern_create_linear (4, 0, width - 8, 0);
		equinox_pattern_add_color_rgba (pattern, 0.0, &colors->spot[1], 0.0);
		equinox_pattern_add_color_rgba (pattern, 0.15, &colors->spot[1], 1.0);
		equinox_pattern_add_color_rgba (pattern, 0.5, &colors->spot[1], 1.0);
		equinox_pattern_add_color_rgba (pattern, 0.8, &colors->spot[1], 0.0);
		cairo_set_source (cr, pattern);
		cairo_move_to (cr, 4, height - y_offset);
		cairo_line_to (cr, width - 8, height - y_offset);
		cairo_stroke (cr);
		cairo_pattern_destroy (pattern);
		pattern = cairo_pattern_create_linear (4, 0, width - 8, 0);
		equinox_pattern_add_color_rgba (pattern, 0.0, &colors->spot[1], 0.0);
		equinox_pattern_add_color_rgba (pattern, 0.25, &colors->spot[1], 0.25);
		equinox_pattern_add_color_rgba (pattern, 0.35, &colors->spot[1], 0.25);
		equinox_pattern_add_color_rgba (pattern, 0.8, &colors->spot[1], 0.0);
		cairo_set_source (cr, pattern);
		cairo_move_to (cr, 4, height - y_offset - 1);
		cairo_line_to (cr, width - 8, height - y_offset - 1);
		cairo_stroke (cr);
		if (y_offset > 2) {
			cairo_move_to (cr, 4, height - y_offset + 1);
			cairo_line_to (cr, width - 8, height - y_offset + 1);
			cairo_stroke (cr);
		}
		cairo_pattern_destroy (pattern);*/
	}

}


void
equinox_draw_scale_trough (cairo_t * cr,
			  const EquinoxColors * colors,
			  const WidgetParameters * widget,
			  const SliderParameters * slider,
			  int x, int y, int width, int height)
{

	int fx, fy, fw, fh; // Coordinates and dimensions of fill area
	int tx, ty, tw, th; // Coordinates and dimensions of trough
	int fill_size = slider->fill_size;
	const double ROUNDNESS = SCALE_TROUGH_SIZE / 2;
	double lightness = equinox_get_lightness(&widget->parentbg);

	if (slider->horizontal) {
		tx = 1;
		ty = (height - SCALE_TROUGH_SIZE) / 2;
		tw = width - 2;
		th = SCALE_TROUGH_SIZE;
		if (fill_size > tw)
			fill_size = tw;
		fw = fill_size;
		fh = th;
		fx = slider->inverted ? tx + tw - fw: tx;
		fy = ty;
	} else {
		tx = (width - SCALE_TROUGH_SIZE) / 2;
		ty = 1;
		tw = SCALE_TROUGH_SIZE;
		th = height - 2;
		if (fill_size > th)
			fill_size = th;
		fw = tw;
		fh = fill_size;
		fx = tx;
		fy = slider->inverted ? ty + th - fh : ty;
	}

	cairo_translate (cr, x, y);
	//equinox_rectangle (cr, EQX_CAIRO_STROKE, 0.5, 0.5, width-1, height-1, &colors->shade[5], 1.0);

	EquinoxRGB fill = widget->parentbg;
	EquinoxRGB lighter, darker, mid;
	EquinoxRGB border;
	cairo_pattern_t *pattern;

	equinox_shade (&fill, &fill, 0.88 - 0.075 * 1 / lightness);
	equinox_shade (&fill, &border, 0.35);

	// Unfilled trough
	equinox_shade (&fill, &darker, 0.70);
	equinox_shade (&fill, &mid, 0.95);

	if (slider->horizontal)
		pattern = cairo_pattern_create_linear (0, ty, 0, ty + th);
	else
		pattern = cairo_pattern_create_linear (tx, 0, tx + tw, 0);
	equinox_pattern_add_color_rgb (pattern, 0.00, &border);
	equinox_pattern_add_color_rgb (pattern, 0.08, &darker);
	equinox_pattern_add_color_rgb (pattern, 0.80, &mid);
	equinox_pattern_add_color_rgb (pattern, 1.00, &fill);
	equinox_rounded_gradient (cr, EQX_CAIRO_FILL, tx, ty, tw, th, ROUNDNESS, EQX_CORNER_ALL, pattern);

	if (fill_size > 0) {

		//filled trough
		fill = border = colors->base[GTK_STATE_SELECTED];
		equinox_shade_shift (&fill, &lighter, 1.10);
		equinox_shade_shift (&border, &darker, 0.70);

		if (slider->inverted)
			pattern = cairo_pattern_create_linear (slider->horizontal ? fw : 0, slider->horizontal ? 0 : fh, 0, 0);
		else
			pattern = cairo_pattern_create_linear (0, 0, slider->horizontal ? tw - 2 : 0, slider->horizontal ? 0 : th - 2);
		equinox_pattern_add_color_rgb (pattern, 0.0, &darker);
		equinox_pattern_add_color_rgb (pattern, 1.0, &lighter);
		equinox_rounded_gradient (cr, EQX_CAIRO_FILL, fx, fy, fw, fh, ROUNDNESS, EQX_CORNER_ALL, pattern);

		// Shading
		equinox_shade_shift (&fill, &fill, 1.5);
		equinox_shade_shift (&colors->base[GTK_STATE_SELECTED], &darker, 0.5);

		if (slider->horizontal)
			pattern = cairo_pattern_create_linear (0, ty, 0, ty + th);
		else
			pattern = cairo_pattern_create_linear (tx, 0, tx + tw, 0);
		equinox_pattern_add_color_rgba (pattern, 0.0, &darker, 0.5);
		equinox_pattern_add_color_rgba (pattern, 1.0, &fill, 0.5);
		equinox_rounded_gradient (cr, EQX_CAIRO_FILL, fx, fy, fw, fh, ROUNDNESS, EQX_CORNER_ALL, pattern);
	}

	equinox_draw_etched_shadow (cr, tx - 0.5, ty - 0.5, tw + 1, th + 1, ROUNDNESS + 1, EQX_CORNER_ALL, &widget->parentbg, 0.9);

}


void equinox_draw_scale_slider (cairo_t * cr,
			   const EquinoxColors * colors,
			   const WidgetParameters * widget,
			   const SliderParameters * slider,
			   int x, int y, int width, int height, int scalesliderstyle) {

	EquinoxRGB border;
	EquinoxRGB fill;
	EquinoxRGB highlight, mid_highlight, fill_darker;
	cairo_pattern_t *pattern;
	int cx, cy, radius;

	double lightness = equinox_get_lightness(&widget->parentbg);

	// Rotate if not horizontal
	if (!(slider->horizontal)) {
		int tmp = height;
		rotate_mirror_translate (cr, M_PI / 2, x, y, FALSE, FALSE);
		height = width;
		width = tmp;
	} else
		cairo_translate (cr, x, y);

	radius = height / 2;
	cx = width / 2;
	cy = height / 2;

    fill  = colors->bg[widget->state_type];
	//equinox_shade (&fill, &border, 0.7);
	equinox_mix_color (&widget->parentbg, &fill, 0.45, &border);
	double lightness_sum = equinox_get_lightness(&fill) + lightness;
	double lightness_delta = equinox_get_lightness(&fill) - lightness;
	equinox_shade (&border, &border, 0.82 - lightness_delta * lightness_sum);

	// Shadow
	EquinoxRGB shadow;
	equinox_shade (&widget->parentbg, &shadow, 0.65);
	equinox_arc (cr, EQX_CAIRO_STROKE, slider->horizontal ? cx : cx + 0.5, slider->horizontal ? cy + 0.5 : cy, radius - 1.0, 0, M_PI * 2, &shadow, 0.32);
	equinox_arc (cr, EQX_CAIRO_STROKE, slider->horizontal ? cx : cx + 1.5, slider->horizontal ? cy + 1.5 : cy, radius - 0.5, 0, M_PI * 2, &shadow, 0.06);

	if (scalesliderstyle == 1) {
		// Draw fill
		equinox_shade (&fill, &highlight, lightness < 0.5 ? 1.25 : 1.1);
		equinox_shade (&fill, &fill_darker, 0.82);
		pattern = cairo_pattern_create_radial (slider->horizontal ? cx : cx + radius / 2, slider->horizontal ? cy + radius / 2 : cy, radius / 3,
					slider->horizontal ? cx : cx + radius / 2, slider->horizontal ? cy + radius / 2 : cy, radius);
		equinox_pattern_add_color_rgb (pattern, 0.0, &highlight);
		equinox_pattern_add_color_rgb (pattern, 1.0, &fill_darker);
		equinox_arc_gradient (cr, EQX_CAIRO_FILL, cx, cy, radius - 1.5, 0, M_PI * 2, pattern);

		// Fill highlight
		double ex, ey, ew, eh;
		// center of ellipse
		ex = slider->horizontal ? cx : 1 + radius / 2.0;
		ey = slider->horizontal ? 1 + radius / 2.0 : cy;
		// ellipse dimensions
		if (slider->horizontal) {
			ew = 2.0 * width / 3.0;
			eh = 1.0 + height / 3.0;
		} else {
			ew = 1.0 + width / 3.0;
			eh = 2.0 * height / 3.0;
		}

		cairo_save(cr);
		cairo_translate(cr, ex, ey);  // make (ex, ey) == (0, 0)
		cairo_scale(cr, ew / 2.0, eh / 2.0);  // for width: ew / 2.0 == 1.0 and for height: eh / 2.0 == 1.0
		equinox_shade (&fill, &highlight, lightness < 0.5 ? 1.8 : 1.2);
		pattern = cairo_pattern_create_linear (0, 0, slider->horizontal ? 0 : 1.0, slider->horizontal ? 1.0 : 0);
		equinox_pattern_add_color_rgba (pattern, 0.0, &highlight, 0.6);
		equinox_pattern_add_color_rgba (pattern, 0.8, &highlight, 0.0);
		cairo_arc(cr, 0.0, 0.0, 1.0, 0.0, 2 * M_PI);
		cairo_set_source( cr, pattern);
		cairo_fill_preserve(cr);
		cairo_restore(cr);
		cairo_stroke(cr);
	} else {
		//Draw fill
		equinox_shade (&fill, &highlight, 1.12);
		equinox_shade (&fill, &fill_darker, 0.82);
		pattern = cairo_pattern_create_radial (slider->horizontal ? cx : cx - radius / 2, slider->horizontal ? cy - radius / 2 : cy, radius / 3,
					slider->horizontal ? cx : cx - radius / 2, slider->horizontal ? cy - radius / 2 : cy, radius);
		equinox_pattern_add_color_rgb (pattern, 0.0, &highlight);
		equinox_pattern_add_color_rgb (pattern, 0.4, &fill);
		equinox_pattern_add_color_rgb (pattern, 1.0, &fill_darker);
		equinox_arc_gradient (cr, EQX_CAIRO_FILL, cx, cy, radius - 1.5, 0, M_PI * 2, pattern);
	}

	// Bullet
	if (scalesliderstyle >= 2) {
		equinox_shade (&border, &border, 0.95);
		equinox_shade (&fill_darker, &fill_darker, 0.95);
		// Fill
		double r = radius - 4;
		pattern = cairo_pattern_create_linear (cx - r, cy - r, slider->horizontal ? cx - r : cx + r, slider->horizontal ? cy + r : cy - r);
		equinox_pattern_add_color_rgb (pattern, 0.0, &fill_darker);
		equinox_pattern_add_color_rgba (pattern, 1.0, &fill_darker, 0.5);
		equinox_arc_gradient (cr, EQX_CAIRO_FILL, cx, cy, r, 0, M_PI * 2, pattern);
		// Border
		r = radius - 4.5;
		pattern = cairo_pattern_create_linear (cx - r, cy - r, slider->horizontal ? cx - r : cx + r, slider->horizontal ? cy + r : cy - r);
		equinox_pattern_add_color_rgba (pattern, 0.0, &border, 0.95);
		equinox_pattern_add_color_rgba (pattern, 1.0, &highlight, 0.6);
		equinox_arc_gradient (cr, EQX_CAIRO_STROKE, cx, cy, r, 0, M_PI * 2, pattern);
	}

	// Stroke highlight
	equinox_shade (&fill, &highlight, 1.35);
	pattern = cairo_pattern_create_linear (3, 3, slider->horizontal ? 3 : width - 6, slider->horizontal ? height - 6 : 3);
	equinox_pattern_add_color_rgba (pattern, 0.0, &highlight, (scalesliderstyle == 1) ? 0.0 : 1.0);
	equinox_pattern_add_color_rgba (pattern, 1.0, &highlight, (scalesliderstyle == 1) ? 0.5 : 0.0);
	equinox_arc_gradient (cr, EQX_CAIRO_STROKE, cx, cy, radius - 2.5, 0, M_PI * 2, pattern);

	// Border
	EquinoxRGB border_darker;
	if (lightness > 0.6) {
		/*if (widget->focus) {
			equinox_mix_color (&border, &colors->spot[1], 0.85, &border);
			equinox_shade_shift (&border, &border, lightness < 0.5 ? 1.15 : 1.05);
			equinox_shade_shift (&border, &border_darker, 0.72);
			pattern = cairo_pattern_create_linear (2, 2, slider->horizontal ? 2 : width - 4, slider->horizontal ? height - 4 : 2);
			equinox_pattern_add_color_rgba (pattern, 0.0, &border, 0.12);
			equinox_pattern_add_color_rgba (pattern, 1.0, &border_darker, 0.24);
			equinox_arc_gradient (cr, EQX_CAIRO_STROKE, cx, cy, radius - 0.5, 0, M_PI * 2, pattern);
		} else {*/
			equinox_shade_shift (&border, &border_darker, 0.5);
		//}
		pattern = cairo_pattern_create_linear (2, 2, slider->horizontal ? 2 : width - 4, slider->horizontal ? height - 4 : 2);
		equinox_pattern_add_color_rgb (pattern, 0.0, &border);
		equinox_pattern_add_color_rgb (pattern, 1.0, &border_darker);
		equinox_arc_gradient (cr, EQX_CAIRO_STROKE, cx, cy, radius - 1.5, 0, M_PI * 2, pattern);
	} else {
		equinox_shade_shift (&border, &border_darker, 0.5);
		pattern = cairo_pattern_create_linear (2, 2, slider->horizontal ? 2 : width - 4, slider->horizontal ? height - 4 : 2);
		equinox_pattern_add_color_rgb (pattern, 0.0, &border);
		equinox_pattern_add_color_rgb (pattern, 1.0, &border_darker);
		equinox_arc_gradient (cr, EQX_CAIRO_STROKE, cx, cy, radius - 1.5, 0, M_PI * 2, pattern);
		/*if (widget->focus) {
			equinox_mix_color (&border, &colors->spot[1], 0.85, &border);
			equinox_shade_shift (&border, &border_darker, 0.85);
			pattern = cairo_pattern_create_linear (2, 2, slider->horizontal ? 2 : width - 4, slider->horizontal ? height - 4 : 2);
			equinox_pattern_add_color_rgba (pattern, 0.0, &border, 0.1);
			equinox_pattern_add_color_rgba (pattern, 1.0, &border_darker, 1.0);
			equinox_arc_gradient (cr, EQX_CAIRO_STROKE, cx, cy, radius - 2.5 , 0, M_PI * 2, pattern);
		}*/
	}

}


void
equinox_draw_progressbar_trough (cairo_t * cr,
				const EquinoxColors * colors,
				const WidgetParameters * widget,
				const ProgressBarParameters * progressbar,
				int x, int y, int width, int height, int progressbarstyle)
{
	EquinoxRGB bg_darker;
	cairo_pattern_t *pattern;
	double lightness = equinox_get_lightness(&widget->parentbg);

	if (progressbar->orientation == EQX_ORIENTATION_LEFT_TO_RIGHT) {
		rotate_mirror_translate (cr, 0, x, y, FALSE, FALSE);
	}
	else if (progressbar->orientation == EQX_ORIENTATION_RIGHT_TO_LEFT) {
		rotate_mirror_translate (cr, 0, x + width, y, TRUE, FALSE);
	}
	else {			//vertical swap width and height
		int tmp = height;

		height = width;
		width = tmp;

		if (progressbar->orientation == EQX_ORIENTATION_TOP_TO_BOTTOM) {
			rotate_mirror_translate (cr, M_PI / 2, x, y, FALSE, FALSE);
		}
		else {
			rotate_mirror_translate (cr, M_PI / 2, x, y + width, TRUE, FALSE);
		}

	}

	int roundness = MIN (widget->curvature, (height-4.0)/2.0);

	cairo_set_source_rgb (cr, widget->parentbg.r, widget->parentbg.g, widget->parentbg.b);
	cairo_rectangle(cr, x, y, width, height);
	cairo_fill(cr);

	equinox_mix_color (&colors->shade[8], &colors->spot[2], 0.15, &bg_darker);

	equinox_draw_shadow (cr, x+1, y+0.5, width - 2, height - 1, roundness+2, widget->corners, &bg_darker, 0.125 + 0.025 * 1 / lightness);
	equinox_draw_shadow (cr, x+2, y+1.5, width - 4, height - 3, roundness+1, widget->corners, &bg_darker, 0.4 + 0.05 * 1 / lightness);

	cairo_set_source_rgba (cr, bg_darker.r, bg_darker.g, bg_darker.b, 0.10);
	clearlooks_rounded_rectangle (cr, x+3, y+2.5, width-6, height-6, roundness+1, widget->corners);
	cairo_stroke(cr);

	clearlooks_rounded_rectangle (cr, x+1.0, y+3.0, width-2, height-2, roundness+1, widget->corners);

	cairo_clip(cr);

	cairo_translate (cr, 1, 1);
	width -= 2;
	height -= 2;

	EquinoxRGB highlight, highlight_mid;
	equinox_shade (&colors->bg[GTK_STATE_NORMAL], &bg_darker, 0.87);
	equinox_shade (&colors->bg[GTK_STATE_NORMAL], &highlight_mid, 1.2);
	equinox_shade (&colors->bg[GTK_STATE_NORMAL], &highlight, 1.12);

	//equinox_mix_color (&bg_darker, &highlight, 0.50, &highlight);
	//equinox_mix_color (&bg_darker, &highlight, 0.7, &highlight_mid);

	pattern = cairo_pattern_create_linear (0, 0, 0, height);
	if (progressbarstyle == 1) {
		equinox_pattern_add_color_rgb (pattern, 0.0, &highlight);
		equinox_pattern_add_color_rgb (pattern, 0.5, &highlight_mid);
		equinox_pattern_add_color_rgb (pattern, 0.5, &bg_darker);
		equinox_pattern_add_color_rgb (pattern, 1.0, &colors->bg[0]);
	} else {
		equinox_pattern_add_color_rgb (pattern, 0.0, &highlight_mid);
		equinox_pattern_add_color_rgb (pattern, 0.3, &highlight);
		equinox_pattern_add_color_rgb (pattern, 1.0, &bg_darker);
	}
	equinox_rounded_gradient (cr, EQX_CAIRO_FILL, 2, 1, width - 4, height - 2, roundness+1, widget->corners, pattern);

	//highlight
	if (progressbarstyle == 0) {
		equinox_shade (&colors->bg[0], &highlight, 1.5);
		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		equinox_pattern_add_color_rgba (pattern, 0.0, &highlight, 0.6);
		equinox_pattern_add_color_rgba (pattern, 0.3, &highlight, 0.2);
		equinox_pattern_add_color_rgba (pattern, 0.75, &highlight, 0.0);
		equinox_rounded_gradient (cr, EQX_CAIRO_STROKE, 2.5, 2, width - 5, height-4, roundness, widget->corners, pattern);
	}
}

void
equinox_draw_progressbar_fill (cairo_t * cr,
			      const EquinoxColors * colors,
			      const WidgetParameters * widget,
			      const ProgressBarParameters * progressbar,
			      int x, int y, int width, int height,
			      gint offset, int progressbarstyle) {
	double tile_pos = 0;
	double stroke_width;
	int x_step, tmp;

	EquinoxRGB shadow, highlight = colors->bg[GTK_STATE_PRELIGHT];
	cairo_pattern_t *pattern;

	double lightness = equinox_get_lightness(&colors->spot[1]);
	double parent_lightness = equinox_get_lightness(&widget->parentbg);

	if (progressbar->orientation == EQX_ORIENTATION_LEFT_TO_RIGHT)
		rotate_mirror_translate (cr, 0, x, y, FALSE, FALSE);
	else if (progressbar->orientation == EQX_ORIENTATION_RIGHT_TO_LEFT)
		rotate_mirror_translate (cr, 0, x + width, y, TRUE, FALSE);
	else { //vertical swap width and height
		tmp = height;

		height = width;
		width = tmp;

		//trying a little fix
		x = x + 1;
		y = y - 1;
		width += 2;
		height -= 2;

		if (progressbar->orientation == EQX_ORIENTATION_TOP_TO_BOTTOM)
			rotate_mirror_translate (cr, M_PI / 2, x, y, FALSE, FALSE);
		else
			rotate_mirror_translate (cr, M_PI / 2, x, y + width, TRUE, FALSE);
	}

	int roundness = MIN (widget->curvature, (height-3.0)/2.0);
	int yd = 0;
	if ((2 * roundness > width) && roundness > 0) {
		int h = height * sin((M_PI * (width)) / (4*roundness));
		roundness = round((width-4) / 2.0);
		yd = 0.5 + (height - h) / 2;
		height = h;
	}

	stroke_width = height * 2;
	x_step = (((float) stroke_width / 10) * offset);

	// Drop shadow
	equinox_shade(&widget->parentbg, &shadow, 0.6);
	equinox_rounded_rectangle (cr, EQX_CAIRO_FILL, 2, 1, width-2, height-1, roundness+1, widget->corners, &shadow, 0.2);


	cairo_save (cr);
	equinox_rounded_rectangle_closed (cr, 2, 1 + yd, width-4, height-1, roundness, widget->corners);
	cairo_clip (cr);

	cairo_rectangle (cr, 2, 1, width-4, height-2);

	EquinoxRGB spot_darker, spot_lighter;

	equinox_shade_shift (&colors->spot[1], &spot_darker, 0.75);
	equinox_shade_shift (&colors->spot[1], &spot_lighter, 1.08);

	pattern = cairo_pattern_create_linear (0, 0, 0, height);
	if (progressbarstyle == 0) { //Gradient highlight
		equinox_pattern_add_color_rgb (pattern, 0.0, &spot_lighter);
		equinox_pattern_add_color_rgb (pattern, 0.45, &colors->spot[1]);
		equinox_pattern_add_color_rgb (pattern, 1.0, &spot_darker);
	} else {
		equinox_pattern_add_color_rgb (pattern, 0.0, &spot_darker);
		equinox_pattern_add_color_rgb (pattern, 0.85, &colors->spot[1]);
		equinox_pattern_add_color_rgb (pattern, 1.0, &spot_lighter);
	}
	equinox_rounded_gradient (cr, EQX_CAIRO_FILL, 0.5, 0 + yd, width - 1, height, roundness-1, widget->corners, pattern);

	// Draw strokes
	while (tile_pos <= width + x_step - 2) {
		cairo_move_to (cr, stroke_width / 2 - x_step, 0);
		cairo_line_to (cr, stroke_width - x_step, 0);
		cairo_line_to (cr, stroke_width / 2 - x_step, height);
		cairo_line_to (cr, -x_step, height);

		cairo_translate (cr, stroke_width, 0);
		tile_pos += stroke_width;
	}
	equinox_shade_shift (&spot_darker, &spot_darker, 0.95);
	equinox_set_source_rgba (cr, &spot_darker, (progressbarstyle == 1 ? 1.0 : 0.7) - lightness*0.6);
	cairo_fill (cr);

	cairo_restore (cr);

	//Glassy highlight
	if (progressbarstyle == 1) {
		equinox_shade_shift (&colors->spot[1], &highlight, 1.25 + (1.0 - lightness) * 0.25);
		equinox_shade_shift (&colors->spot[1], &shadow, 0.7);

		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		equinox_pattern_add_color_rgba (pattern, 0.0, &highlight, 0.55);
		equinox_pattern_add_color_rgba (pattern, 0.5, &highlight, 0.35);
		equinox_pattern_add_color_rgba (pattern, 0.5, &highlight, 0.0);
		equinox_pattern_add_color_rgba (pattern, 1.0, &highlight, 0.5);
		equinox_rounded_gradient (cr, EQX_CAIRO_FILL, 1, 1 + yd, width-2, height-2*yd, roundness-0.5, widget->corners, pattern);
	}

	// Border
	EquinoxRGB border_darker, border_lighter;
	equinox_shade_shift (&colors->spot[2], &border_darker, 1.1);
	if (parent_lightness < 0.4) {
		equinox_shade_shift (&colors->spot[1], &border_lighter, 1.4);
	} else {
		equinox_shade_shift (&colors->spot[2], &border_lighter, 1.25);
	}
	pattern = cairo_pattern_create_linear (0, 0.5, 0, height - 0.5);
	equinox_pattern_add_color_rgba (pattern, 0.0, &border_lighter, lightness * 0.5);
	equinox_pattern_add_color_rgba (pattern, 1.0, &border_darker, lightness * 0.75);
	equinox_rounded_gradient (cr, EQX_CAIRO_STROKE, 2.5, 1.5 + yd, width - 5, height - 2, roundness-0.5, widget->corners, pattern);
}

void equinox_draw_menubar (cairo_t * cr,
		     const EquinoxColors * colors,
		     const WidgetParameters * widget,
		     int x, int y, int width, int height, int menubarstyle, int menubarborders) {

	EquinoxRGB shadow, highlight;
	cairo_pattern_t *pattern;

	cairo_translate (cr, x, y);
	cairo_rectangle (cr, 0, 0, width, height);

	// Gradient menubar
	if (menubarstyle == 1) {
		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		equinox_pattern_add_color_rgb (pattern, 0.0, &colors->bg[0]);
		equinox_pattern_add_color_rgb (pattern, 1.0, &colors->shade[2]);
		equinox_rectangle_gradient (cr, EQX_CAIRO_FILL, 0, 0, width, height, pattern);
	} else {
		//flat menubar
		equinox_rectangle (cr, EQX_CAIRO_FILL, 0, 0, width, height, &colors->bg[0], 1.0);
	}

	// Draw bottom line
	if (menubarborders == 1 || menubarborders == 3) {
		equinox_shade (&colors->bg[GTK_STATE_NORMAL], &shadow, 0.8);
		cairo_move_to (cr, 0, height - 0.5);
		cairo_line_to (cr, width, height - 0.5);
		equinox_set_source_rgb (cr, &shadow);
		cairo_stroke (cr);
	}
	if (menubarborders >= 2) {
		equinox_shade (&colors->bg[GTK_STATE_NORMAL], &shadow, 0.9);
		pattern = cairo_pattern_create_linear (0, 0, width/2, 0);
		equinox_pattern_add_color_rgba (pattern, 0.0, &shadow, 1.0);
		equinox_pattern_add_color_rgba (pattern, 1.0, &shadow, 0.0);
		cairo_move_to (cr, 0, 0.5);
		cairo_line_to (cr, width/2, 0.5);
		cairo_set_source (cr, pattern);
		cairo_stroke (cr);
		equinox_shade (&colors->bg[GTK_STATE_NORMAL], &highlight, 1.10);
		pattern = cairo_pattern_create_linear (0, 1, width/2, 1);
		equinox_pattern_add_color_rgba (pattern, 0.0, &highlight, 1.0);
		equinox_pattern_add_color_rgba (pattern, 1.0, &highlight, 0.0);
		cairo_move_to (cr, 0, 1.5);
		cairo_line_to (cr, width/2, 1.5);
		cairo_set_source (cr, pattern);
		cairo_stroke (cr);
	}
}

void equinox_draw_panel (cairo_t * cr,
		     const EquinoxColors * colors,
		     const WidgetParameters * widget,
		     int x, int y, int width, int height) {

	EquinoxRGB color;
	EquinoxRGB color2;

	cairo_translate (cr, x, y);
	cairo_rectangle (cr, 0, 0, width, height);

	equinox_shade_shift (&colors->bg[0], &color, 0.85);
	equinox_shade_shift (&colors->bg[0], &color2, 1.85);

	// Gradient panel
	//if (menubarstyle == 1) {
		cairo_pattern_t *pattern = cairo_pattern_create_linear (0, 0, 0, height);
		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		equinox_pattern_add_color_rgb (pattern, 0.0, &color2 );
		equinox_pattern_add_color_rgb (pattern, 1.0, &color);
		equinox_rectangle_gradient (cr, EQX_CAIRO_FILL, 0, 0, width, height, pattern);
	//} else {
		//flat panel
		//equinox_rectangle (cr, EQX_CAIRO_FILL, 0, 0, width, height, &colors->bg[0], 1.0);
	//}

	// Draw bottom line
	equinox_shade_shift (&colors->bg[0], &color, 0.95);
	cairo_move_to (cr, 0, height - 0.5);
	cairo_line_to (cr, width, height - 0.5);
	equinox_set_source_rgb (cr, &color);
	cairo_stroke (cr);
	// Draw top line
	equinox_shade_shift (&colors->bg[0], &color, 1.1);
	cairo_move_to (cr, 0, 0.5);
	cairo_line_to (cr, width, 0.5);
	equinox_set_source_rgb (cr, &color);
	cairo_stroke (cr);
}

static void equinox_get_frame_gap_clip (int x, int y, int width, int height,
			   const FrameParameters * frame,
			   EquinoxRectangle * bevel, EquinoxRectangle * border) {
  if (frame->gap_side == EQX_GAP_TOP) {
    EQUINOX_RECTANGLE_SET ((*bevel), 0.5 + frame->gap_x - 1 +3, -0.5, frame->gap_width - 3, 2.0 + 2);
    EQUINOX_RECTANGLE_SET ((*border), 0.5 + frame->gap_x - 1, -0.5, frame->gap_width - 2 + 2, 2.0 + 1);
  } else if (frame->gap_side == EQX_GAP_BOTTOM) {
    EQUINOX_RECTANGLE_SET ((*bevel), 1.5 + frame->gap_x - 3, height - 2.5, frame->gap_width - 3, 2.0);
    EQUINOX_RECTANGLE_SET ((*border), 0.5 + frame->gap_x - 3 + 2, height - 1.5 - 1, frame->gap_width - 2 + 2, 2.0);
  } else if (frame->gap_side == EQX_GAP_LEFT) {
    EQUINOX_RECTANGLE_SET ((*bevel), -0.5, 1.5 + frame->gap_x, 2.0, frame->gap_width - 3);
    EQUINOX_RECTANGLE_SET ((*border), -0.5 + 1, 0.5 + frame->gap_x - 1, 2.0, frame->gap_width - 2 + 2);
  } else if (frame->gap_side == EQX_GAP_RIGHT) {
    EQUINOX_RECTANGLE_SET ((*bevel), width - 2.5, 1.5 + frame->gap_x, 2.0, frame->gap_width - 3);
    EQUINOX_RECTANGLE_SET ((*border), width - 1.5 - 2, 0.5 + frame->gap_x - 1, 2.0, frame->gap_width - 2 + 2);
  }
}


void equinox_draw_frame (cairo_t * cr,
		   const EquinoxColors * colors,
		   const WidgetParameters * widget,
		   const FrameParameters * frame,
		   int x, int y, int width, int height) {

	EquinoxRGB *border = frame->border;
	EquinoxRectangle bevel_clip;
	EquinoxRectangle frame_clip;


	double curvature = MIN (widget->curvature, (int)MIN ((width - 2.0) / 2.0, (height - 2.0) / 2.0));

	if (frame->shadow == EQX_SHADOW_NONE)
		return;

	if (frame->gap_x != -1)
		equinox_get_frame_gap_clip (x, y, width, height, frame, &bevel_clip, &frame_clip);

	cairo_translate (cr, x + 0.5, y + 0.5);

	// Set clip for the frame
	if (frame->gap_x != -1) {
		// Set clip for gap
		cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);
		cairo_rectangle (cr, -0.5, -0.5, width, height);
		if (frame->use_fill)
			cairo_rectangle (cr, frame_clip.x, frame_clip.y, frame_clip.width, frame_clip.height); //ie for notebooks
		else
			cairo_rectangle (cr, bevel_clip.x, bevel_clip.y, bevel_clip.width, bevel_clip.height); //for other frames
		cairo_clip (cr);
	}

	// Draw frame
	if (frame->shadow == EQX_SHADOW_IN || frame->shadow == EQX_SHADOW_ETCHED_IN) {
		if (frame->fill_bg) {
			EquinoxRGB bg;
			equinox_shade(&colors->bg[GTK_STATE_NORMAL], &bg, 0.97);
			equinox_rounded_rectangle (cr, EQX_CAIRO_FILL, 1.5, 1.5, width-3, height-3, curvature, widget->corners, &bg, 1.0);
		}

		EquinoxRGB highlight, shadow;

		// Shadow
		equinox_shade (&colors->bg[GTK_STATE_NORMAL], &highlight, 1.065);
		equinox_shade (&colors->shade[3], &shadow, 0.80);
		equinox_draw_etched_border (cr, 1, 1, width - 3, height - 3, widget->curvature, widget->corners, &colors->shade[3], 1.10, 0.86);
		equinox_rounded_rectangle (cr, EQX_CAIRO_STROKE, 1.5, 2, width - 4, height - 4, curvature - 0.5, widget->corners, &shadow, 0.23);
		equinox_rounded_rectangle (cr, EQX_CAIRO_STROKE, 2.5, 3, width-6, height - 6, curvature - 0.5, widget->corners, &shadow, 0.05);

		// Bottom Highlight
		cairo_pattern_t *pattern = cairo_pattern_create_linear (0, 0, 0, height-1);
		equinox_pattern_add_color_rgb (pattern, 0.5, &colors->bg[GTK_STATE_NORMAL]);
		equinox_pattern_add_color_rgb (pattern, 1.0, &highlight);
		equinox_rounded_gradient (cr, EQX_CAIRO_STROKE, 0, 0, width - 1, height - 1, curvature + 1, widget->corners, pattern);

	} else {

		// Shadow out
		if (frame->fill_bg) {
			EquinoxRGB bg;
			if (frame->use_fill) {
				bg.r = frame->fill->r;
				bg.g = frame->fill->g;
				bg.b = frame->fill->b;
			} else
				equinox_shade (&colors->bg[GTK_STATE_NORMAL], &bg, 1.04);
			equinox_rounded_rectangle (cr, EQX_CAIRO_FILL, 1.5, 1.5, width - 3, height - 3, curvature, widget->corners, &bg, 1.0);
		}

		EquinoxRGB shadow;
		equinox_shade (border, &shadow, 0.85);
		equinox_rounded_rectangle (cr, EQX_CAIRO_STROKE, 0.5, 1, width - 2 , height - 2, curvature + 1, widget->corners, &shadow, 0.3);

		if (frame->use_fill)
			equinox_rounded_rectangle (cr, EQX_CAIRO_STROKE, 1, 1, width - 3 , height - 3, curvature, widget->corners, border, 1.0);
		else
			equinox_draw_border (cr, 1, 1, width - 3, height - 3, widget->curvature, widget->corners, &colors->shade[4], 1.09, 0.90);

		// Highlight
		EquinoxRGB highlight;
		cairo_save (cr);
		clearlooks_rounded_rectangle (cr, 1.5, 1.5, width - 4, height - 4, curvature, widget->corners);
		cairo_clip (cr);
		equinox_shade (&colors->bg[GTK_STATE_NORMAL], &highlight, 1.06);
		equinox_rounded_rectangle (cr, EQX_CAIRO_STROKE, 1.5, 2, width - 4, height - 3.5, curvature - 1, widget->corners, &highlight, 1.0);
		cairo_restore (cr);
	}
}



void
equinox_draw_tab (cairo_t * cr,
		 const EquinoxColors * colors,
		 const WidgetParameters * widget,
		 const TabParameters * tab,
		 int x, int y, int width, int height)
{
	EquinoxRGB *fill = (EquinoxRGB *) &colors->bg[widget->state_type];
	EquinoxRGB border1;
	EquinoxRGB focus, highlight;

	equinox_shade (&colors->bg[widget->state_type], &border1, 0.68);

	double lightness = equinox_get_lightness(&widget->parentbg);

	cairo_pattern_t *pattern;

	// Make the tabs slightly bigger than they should be, to create a gap
	if (tab->gap_side == EQX_GAP_TOP || tab->gap_side == EQX_GAP_BOTTOM) {
		height += 1;
		width -= 2;
		if (tab->gap_side == EQX_GAP_TOP) {
			rotate_mirror_translate (cr, 0, x + 0.5, y + 0.5, FALSE, TRUE);
			cairo_translate (cr, 1.0, -height + 3);	// gap at the other side
		} else
			cairo_translate (cr, x + 1.5, y + 1.5);
  	} else {
		width += 1;
		height -= 2;

		if (tab->gap_side == EQX_GAP_LEFT) {
			int tmp = width;
			width = height;
			height = tmp;
			rotate_mirror_translate (cr, M_PI * 1.5, x + 0.5, y + 0.5, TRUE, FALSE);
			cairo_translate (cr, 1, 3 - height);	// gap at the other side
		} else {
			int tmp = width;
			width = height;
			height = tmp;
			rotate_mirror_translate (cr, M_PI * 0.5, x + 0.5, y + 0.5, FALSE, FALSE);
			cairo_translate (cr, 1.0, 1.0);	// gap at the other side
		}
	}

	int roundness = MIN (widget->curvature, (width - 2) / 2);

	cairo_save (cr);

	if (widget->active) {

		cairo_rectangle(cr, 0, 0, width, height - 1.5);
		cairo_clip(cr);

	    // Draw Fill and shade
		EquinoxRGB fill_shaded1, fill_shaded2;
		equinox_shade (fill, &fill_shaded1, 0.925);
		equinox_shade (fill, &fill_shaded2, 0.90);
		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		equinox_pattern_add_color_rgb (pattern, 0.0, &fill_shaded1);
		equinox_pattern_add_color_rgb (pattern, 0.5, &fill_shaded2);
		equinox_rounded_gradient (cr, EQX_CAIRO_FILL, 0.5, 0.5, width - 2, height, roundness, EQX_CORNER_TOPLEFT | EQX_CORNER_TOPRIGHT, pattern);

		// Shadow
		EquinoxRGB shadowing;
		equinox_shade (&border1, &shadowing, 0.90);
		pattern = cairo_pattern_create_linear (0, height-4, 0, height);
		equinox_pattern_add_color_rgba (pattern, 0.0, &shadowing, 0.0);
		equinox_pattern_add_color_rgba (pattern, 1.0, &shadowing, 0.26);
		equinox_rounded_gradient (cr, EQX_CAIRO_FILL, 0.5, 0.5, width - 1, height - 2, roundness-1, EQX_CORNER_TOPLEFT | EQX_CORNER_TOPRIGHT, pattern);

	} else {

		// Shadow
		EquinoxRGB shadow;
		equinox_shade (&border1, &shadow, 0.88);
		if ((tab->gap_side == EQX_GAP_RIGHT) || (tab->gap_side == EQX_GAP_LEFT)) //left tabs
			equinox_rounded_rectangle (cr, EQX_CAIRO_FILL, 0, -1, width, height, roundness + 1, EQX_CORNER_TOPLEFT | EQX_CORNER_TOPRIGHT, &shadow, 0.3);
		else if (tab->gap_side == EQX_GAP_TOP)//bottom tabs
			equinox_rounded_rectangle (cr, EQX_CAIRO_FILL, -1, -1, width + 1, height + 1, roundness + 1, EQX_CORNER_TOPLEFT | EQX_CORNER_TOPRIGHT, &shadow, 0.3);
		else //top
			equinox_rounded_rectangle (cr, EQX_CAIRO_FILL, -1, 0, width + 1, height, roundness + 1, EQX_CORNER_TOPLEFT | EQX_CORNER_TOPRIGHT, &shadow, 0.3);

		cairo_rectangle(cr, -0.5, -0.5, width + 1, height + 1);
		cairo_clip(cr);

		// Draw Fill and shade
	    equinox_shade (fill, &highlight, 1.06);
		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		equinox_pattern_add_color_rgb (pattern, 0.0, &highlight);
		equinox_pattern_add_color_rgb (pattern, 0.5, fill);
		equinox_rounded_gradient (cr, EQX_CAIRO_FILL, 0.5, 0.5, width - 2, height + 3, roundness, EQX_CORNER_TOPLEFT | EQX_CORNER_TOPRIGHT, pattern);
	}

	cairo_restore (cr);

	// Highlight
	equinox_shade (fill, &highlight, equinox_get_lightness (&colors->bg[GTK_STATE_NORMAL]) > 0.6 ? 1.125 : 1.25);
	pattern = cairo_pattern_create_linear (0, 0, 0, height);
	equinox_pattern_add_color_rgba (pattern, 0.0, &highlight, 0.5);
	equinox_pattern_add_color_rgba (pattern, 0.5, &highlight, 0.15);
	equinox_pattern_add_color_rgba (pattern, 1.0, &highlight, 0.0);
	equinox_rounded_gradient (cr, EQX_CAIRO_STROKE, 1, 1, width - 3, height, roundness-1, EQX_CORNER_TOPLEFT | EQX_CORNER_TOPRIGHT, pattern);

	// Border
	cairo_rectangle(cr, -0.5, -0.5, width + 1, height);
	cairo_clip(cr);
	equinox_rounded_rectangle (cr, EQX_CAIRO_STROKE, 0, 0, width - 1, height + 3, roundness, EQX_CORNER_TOPLEFT | EQX_CORNER_TOPRIGHT, &border1, 1.0);
	cairo_reset_clip(cr);

	// Focus
	if (widget->focus && !widget->active) {
		double offset = equinox_get_lightness(&colors->bg[GTK_STATE_NORMAL]) > 0.6 ? 0 : 1;

		cairo_rectangle(cr, 0, 0, width, height);
		cairo_clip(cr);

		if (offset == 1) {
			focus = colors->spot[1];
		} else {
			equinox_shade (&colors->spot[1], &focus, 0.8);
		}
		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		equinox_pattern_add_color_rgba (pattern, 0.0, &focus, 1.0);
		equinox_pattern_add_color_rgba (pattern, 1.0, &focus, 0.0);
		equinox_rounded_gradient (cr, EQX_CAIRO_STROKE, offset, offset, width - 1 - (2*offset), height + (2*roundness), roundness, EQX_CORNER_TOPLEFT | EQX_CORNER_TOPRIGHT, pattern);
		focus = colors->spot[1];
		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		equinox_pattern_add_color_rgba (pattern, 0.0, &focus, 0.3);
		equinox_pattern_add_color_rgba (pattern, 1.0, &focus, 0.0);
		equinox_rounded_gradient (cr, EQX_CAIRO_STROKE, offset+1, offset+1, width - 3 - (2*offset), height + (2*roundness), roundness, EQX_CORNER_TOPLEFT | EQX_CORNER_TOPRIGHT, pattern);
	}
}

void
equinox_draw_separator (cairo_t * cr,
		       const EquinoxColors * colors,
		       const WidgetParameters * widget,
		       const SeparatorParameters * separator,
		       int x, int y, int width, int height, int separatorstyle)
{

	EquinoxRGB dark = widget->parentbg;
	EquinoxRGB light;

	cairo_pattern_t *pattern;

	equinox_shade (&dark, &light, separatorstyle >= 2 ? 1.5 : 1.15);
	equinox_shade (&dark, &dark, 0.68);

	cairo_translate (cr, x + 0.5, y + 0.5);
	cairo_set_line_width (cr, 1.0);

	if (separator->horizontal) {
    if (separatorstyle != 2) {
			cairo_move_to (cr, 0.0, 0.0);
			cairo_line_to (cr, width, 0.0);
			if (separatorstyle == 1 || separatorstyle == 3) {
				pattern = cairo_pattern_create_linear (0, 0, width, 0);
				equinox_pattern_add_color_rgba (pattern, 0.0, &dark, 0.0);
				equinox_pattern_add_color_rgba (pattern, 0.25, &dark, 0.6);
				equinox_pattern_add_color_rgba (pattern, 0.75, &dark, 0.6);
				equinox_pattern_add_color_rgba (pattern, 1.0, &dark, 0.0);
				cairo_set_source (cr, pattern);
			} else {
				cairo_set_source_rgba (cr, dark.r, dark.g, dark.b, 0.6);
			}
			cairo_stroke (cr);
		}
		if (separatorstyle != 3) {
			cairo_move_to (cr, 0.0, 1.0);
			cairo_line_to (cr, width, 1.0);
			if (separatorstyle == 1 || separatorstyle == 2) {
				pattern = cairo_pattern_create_linear (0, 1, width, 1);
				equinox_pattern_add_color_rgba (pattern, 0.0, &light, 0.0);
				equinox_pattern_add_color_rgba (pattern, 0.25, &light, 0.6);
				equinox_pattern_add_color_rgba (pattern, 0.75, &light, 0.6);
				equinox_pattern_add_color_rgba (pattern, 1.0, &light, 0.0);
				cairo_set_source (cr, pattern);
			} else {
				cairo_set_source_rgba (cr, light.r, light.g, light.b, 0.6);
			}
			cairo_stroke (cr);
		}
  } else {
    if (separatorstyle != 2) {
			cairo_move_to (cr, 0.0, 0.0);
			cairo_line_to (cr, 0.0, height);
			if (separatorstyle == 1 || separatorstyle == 3) {
				pattern = cairo_pattern_create_linear (0, 0, 0, height);
				equinox_pattern_add_color_rgba (pattern, 0.0, &dark, 0.0);
				if (separator->inToolbar) {
					equinox_pattern_add_color_rgba (pattern, 0.45, &dark, 0.7);
					equinox_pattern_add_color_rgba (pattern, 0.55, &dark, 0.7);
				} else {
					equinox_pattern_add_color_rgba (pattern, 0.1, &dark, 0.6);
					equinox_pattern_add_color_rgba (pattern, 0.9, &dark, 0.6);
				}
				equinox_pattern_add_color_rgba (pattern, 1.0, &dark, 0.0);
				cairo_set_source (cr, pattern);
			} else {
				cairo_set_source_rgba (cr, dark.r, dark.g, dark.b, 0.6);
			}
			cairo_stroke (cr);
		}
		if (separatorstyle != 3) {
			cairo_move_to (cr, 1.0, 0.0);
			cairo_line_to (cr, 1.0, height);
			if (separatorstyle == 1 || separatorstyle == 2) {
				pattern = cairo_pattern_create_linear (1, 0, 1, height);
				equinox_pattern_add_color_rgba (pattern, 0.0, &light, 0.0);
				if (separator->inToolbar) {
					equinox_pattern_add_color_rgba (pattern, 0.45, &light, 0.7);
					equinox_pattern_add_color_rgba (pattern, 0.55, &light, 0.7);
				} else {
					equinox_pattern_add_color_rgba (pattern, 0.9, &light, 0.6);
					equinox_pattern_add_color_rgba (pattern, 0.1, &light, 0.6);
				}
				equinox_pattern_add_color_rgba (pattern, 1.0, &light, 0.0);
				cairo_set_source (cr, pattern);
			} else {
				cairo_set_source_rgba (cr, light.r, light.g, light.b, 0.6);
			}
			cairo_stroke (cr);
		}
  }
  if (separatorstyle > 0) {
		cairo_pattern_destroy (pattern);
	}
}

void
equinox_draw_list_view_header (cairo_t * cr,
			      const EquinoxColors * colors,
			      const WidgetParameters * widget,
			      const ListViewHeaderParameters * header,
			      int x, int y, int width, int height, int listviewheaderstyle)
{
	EquinoxRGB fill;

	if (header->sorted) {
		fill = colors->bg[GTK_STATE_ACTIVE];
		//If prelight match the sorted row prelight brightness with that of normal prelight
		if (widget->state_type == GTK_STATE_PRELIGHT) {
			//Increase lightness for sorted prelight by the difference between prelight and nomral lightness
			double prelight_lightness = equinox_get_lightness (&colors->bg[GTK_STATE_PRELIGHT]);
			double normal_lightness = equinox_get_lightness (&colors->bg[GTK_STATE_NORMAL]);
			equinox_set_lightness (&fill, equinox_get_lightness(&fill) + prelight_lightness - normal_lightness);
		}
	}
	else {
		fill = colors->bg[widget->state_type];
	}
#ifdef HAVE_ANIMATION
	EquinoxRGB fill_old = colors->bg[widget->prev_state_type];

	if (header->sorted && widget->prev_state_type != GTK_STATE_PRELIGHT)
		fill_old = colors->bg[GTK_STATE_ACTIVE];

	if (header->sorted && widget->prev_state_type == GTK_STATE_PRELIGHT) {
		fill_old = colors->bg[GTK_STATE_ACTIVE];
		//Increase lightness for sorted prelight by the difference between prelight and nomral lightness
		double prelight_lightness = equinox_get_lightness (&colors->bg[GTK_STATE_PRELIGHT]);
		double normal_lightness = equinox_get_lightness (&colors->bg[GTK_STATE_NORMAL]);
		equinox_set_lightness (&fill_old, equinox_get_lightness(&fill_old) + prelight_lightness - normal_lightness);

		equinox_match_lightness (&colors->bg[GTK_STATE_PRELIGHT], &fill_old);
	}
	equinox_mix_color (&fill_old, &fill, widget->trans, &fill);
#endif
	EquinoxRGB *border = (EquinoxRGB *) &colors->shade[4];

	cairo_translate (cr, x, y);

	cairo_pattern_t *pattern;
	EquinoxRGB fill_darker, highlight, highlight_mid;


	//equinox_rectangle (cr, EQX_CAIRO_FILL, 0, 0, width, height, &colors->base[0], 1.0);

	if (listviewheaderstyle == 1) {

		equinox_shade_shift (&fill, &highlight, 1.225); // 1.4
		equinox_shade_shift (&fill, &fill_darker, 0.90);
		equinox_mix_color (&fill_darker, &highlight, 0.4, &highlight_mid); // 0.55

		//Draw the fill
		pattern = cairo_pattern_create_linear (0, 0, 0, height-1);
		equinox_pattern_add_color_rgb (pattern, 0.0, &highlight);
		equinox_pattern_add_color_rgb (pattern, 0.5, &highlight_mid);
		equinox_pattern_add_color_rgb (pattern, 0.5, &fill_darker);
		equinox_pattern_add_color_rgb (pattern, 0.90, &fill);
		equinox_rectangle_gradient (cr, EQX_CAIRO_FILL, 0, 0, width, height, pattern);

		//Inner glow
		if (widget->prelight || widget->active) {
			pattern = cairo_pattern_create_radial (width/2, height/1.2, 0, width/2, height/1.2, width/2);
			equinox_pattern_add_color_rgba (pattern, 0.0, &highlight, 0.80);
			equinox_pattern_add_color_rgba (pattern, 1.0, &highlight, 0.0);
			cairo_set_source (cr, pattern);
			cairo_rectangle (cr, 0, 0, width, height);
			cairo_fill (cr);
			cairo_pattern_destroy (pattern);
		}


		if (header->order == EQX_ORDER_FIRST) {
			cairo_move_to (cr, 0.5, height - 1);
			cairo_line_to (cr, 0.5, 0.5);
		}
		else
			cairo_move_to (cr, 0.0, 0.5);
		cairo_line_to (cr, width, 0.5);

		if (header->order == EQX_ORDER_LAST) {
			cairo_move_to (cr, width, 0.5);
			cairo_line_to (cr, width, height - 1);
		}
		cairo_set_source_rgba (cr, highlight.r, highlight.g, highlight.b, 0.5);
		cairo_stroke (cr);

	} else {

		equinox_shade_shift (&fill, &highlight, 1.05);
		equinox_shade_shift (&fill, &fill_darker, 0.92);

		//Draw the fill
		pattern = cairo_pattern_create_linear (0, 0, 0, height-1);
		equinox_pattern_add_color_rgb (pattern, 0.0, &highlight);
		equinox_pattern_add_color_rgb (pattern, 0.56, &fill);
		equinox_pattern_add_color_rgb (pattern, 1.0, &fill_darker);
		equinox_rectangle_gradient (cr, EQX_CAIRO_FILL, 0, 0, width, height, pattern);

	}

	// Draw bottom border
	EquinoxRGB shadow;
	double widget_lightness = equinox_get_lightness (&colors->bg[GTK_STATE_NORMAL]);
	double parent_lightness = equinox_get_lightness (&widget->parentbg);
	equinox_shade (border, &shadow, widget_lightness - parent_lightness > 0.3 ? 0.75 - (widget_lightness - parent_lightness) : 0.9);
	cairo_move_to (cr, 0.0, height - 0.5);
	cairo_line_to (cr, width, height - 0.5);
	cairo_set_source_rgb (cr, shadow.r, shadow.g, shadow.b);
	cairo_stroke (cr);

	// Draw separator and resize grip
	EquinoxRGB separator;

	if (header->order == EQX_ORDER_LAST && header->resizable) {
		equinox_shade (border, &separator, 0.45);
		cairo_set_source_rgba (cr, separator.r, separator.g, separator.b, 0.42);
		cairo_move_to (cr, width -0.5, 0.5);
		cairo_line_to (cr, width -0.5,  height - 0.5);
		cairo_stroke (cr);

		equinox_shade (&fill, &separator, 1.40);
		cairo_set_source_rgba (cr, separator.r, separator.g, separator.b, 0.55);
		cairo_move_to (cr, 0.5, 0.5);
		cairo_line_to (cr, 0.5,  height - 1.5);

		/*cairo_move_to (cr, width -0.5, 0.5);
		cairo_line_to (cr, width -0.5,  height - 1.5);*/
		cairo_stroke (cr);
	}
	else {
		if (header->order != EQX_ORDER_LAST) {
			equinox_shade (border, &separator, 0.45);

			pattern = cairo_pattern_create_linear (0, 0, 0, height);
			cairo_pattern_add_color_stop_rgba (pattern, 0.0, separator.r, separator.g, separator.b, 0.35);
			cairo_pattern_add_color_stop_rgba (pattern, 0.3, separator.r, separator.g, separator.b, 0.70);
			cairo_pattern_add_color_stop_rgba (pattern, 0.6, separator.r, separator.g, separator.b, 0.70);
			cairo_pattern_add_color_stop_rgba (pattern, 1.0, separator.r, separator.g, separator.b, 0.25);
			cairo_set_source (cr, pattern);
			//cairo_set_source_rgba (cr, separator.r, separator.g, separator.b, 0.52);
			cairo_move_to (cr, width -0.5, 0.5);
			cairo_line_to (cr, width -0.5,  height - 1.5);
			cairo_stroke (cr);
			cairo_pattern_destroy (pattern);

			equinox_shade (&fill, &separator, 1.4);
			cairo_set_source_rgba (cr, separator.r, separator.g, separator.b, 0.35);
			cairo_move_to (cr, width -1.5, 0.5);
			cairo_line_to (cr, width -1.5,  height - 1.5);
			cairo_stroke (cr);

		}
		if (header->order != EQX_ORDER_FIRST) {
			equinox_shade (&fill, &separator, 1.4);
			cairo_set_source_rgba (cr, separator.r, separator.g, separator.b, 0.35);
			cairo_move_to (cr, 0.5, 0.5);
			cairo_line_to (cr, 0.5,  height - 1.5);
			cairo_stroke (cr);
		}
	}
}

/* We can't draw transparent things here, since it will be called on the same
 * surface multiple times, when placed on a handlebox_bin or dockitem_bin */
void
equinox_draw_toolbar (cairo_t * cr,
		     const EquinoxColors * colors,
		     const WidgetParameters * widget,
		     int x, int y, int width, int height, int toolbarstyle)
{
    EquinoxRGB fill_shadow, stroke_highlight, stroke_shadow;

    cairo_translate (cr, x, y);

    cairo_pattern_t *pattern = cairo_pattern_create_linear (0, 0, 0, height);

	if ((toolbarstyle == 1) || (toolbarstyle == 3) || (toolbarstyle == 5)) {
		// Gradient fill
		equinox_mix_color (&colors->bg[0], &colors->shade[3], 0.8, &fill_shadow);
		equinox_pattern_add_color_rgb (pattern, 0.0, &colors->shade[2]);
		equinox_pattern_add_color_rgb (pattern, 1.0, &fill_shadow);
		equinox_rectangle_gradient (cr, EQX_CAIRO_FILL, 0, 0, width, height, pattern);

		equinox_shade (&colors->shade[2], &stroke_highlight, 1.12);
		stroke_shadow = colors->shade[4];
	} else {
		// Solid fill
		equinox_rectangle (cr, EQX_CAIRO_FILL, 0, 0, width, height, &colors->bg[0], 1.0);

		equinox_shade (&colors->bg[0], &stroke_highlight, 1.075);
		equinox_shade (&colors->bg[0], &stroke_shadow, 0.875);
	}

	if (toolbarstyle != 2 && toolbarstyle != 3) {
		if (toolbarstyle <= 1) {
			// Highlight
			cairo_move_to (cr, 0, 0.5);
			cairo_line_to (cr, width, 0.5);
			equinox_set_source_rgb (cr, &stroke_highlight);
			cairo_stroke (cr);
		}

		// Shadow
		cairo_move_to (cr, 0, height - 0.5);
		cairo_line_to (cr, width, height - 0.5);
		equinox_set_source_rgb (cr, &stroke_shadow);
		cairo_stroke (cr);
	}

}

void
equinox_draw_menubar_item (cairo_t * cr,
		      const EquinoxColors * colors,
		      const WidgetParameters * widget,
		      int x, int y, int width, int height, int menubaritemstyle, boolean in_panel)
{

	cairo_pattern_t *pattern;

	cairo_translate (cr, x, y);

	EquinoxRGB fill;

	if (menubaritemstyle == 0) {
		equinox_rounded_rectangle (cr, EQX_CAIRO_FILL, 0, 0, width, height, widget->curvature, widget->corners, &colors->bg[GTK_STATE_SELECTED], 1.0);
	} else if (menubaritemstyle == 1) {
		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		equinox_pattern_add_color_rgba (pattern, 0.0, &colors->bg[GTK_STATE_SELECTED], 0.2);
		equinox_pattern_add_color_rgba (pattern, 1.0, &colors->bg[GTK_STATE_SELECTED], 1.0);
		equinox_rounded_gradient (cr, EQX_CAIRO_FILL, 0, 0, width, height, widget->curvature, widget->corners, pattern);
	} else if (menubaritemstyle == 2) {
		equinox_shade (&colors->bg[GTK_STATE_SELECTED], &fill, 0.6);
		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		equinox_pattern_add_color_rgb (pattern, 0.0, &fill);
		equinox_pattern_add_color_rgb (pattern, 1.0, &colors->bg[GTK_STATE_SELECTED]);
		equinox_rounded_gradient (cr, EQX_CAIRO_FILL, 0, 0, width-1, height, widget->curvature, widget->corners, pattern);
		EquinoxRGB border;
		equinox_shade (&colors->bg[GTK_STATE_SELECTED], &border, 0.5);
		cairo_set_source_rgba (cr, border.r, border.g, border.b, 0.4);
		cairo_move_to (cr, 0.5, 0.5);
		cairo_line_to (cr, 0.5,  height - 1);
		cairo_stroke (cr);
		cairo_set_source_rgba (cr, border.r, border.g, border.b, 0.2);
		cairo_move_to (cr, 1.5, 0.5);
		cairo_line_to (cr, 1.5,  height - 1);
		cairo_stroke (cr);
		cairo_set_source_rgba (cr, border.r, border.g, border.b, 0.4);
		cairo_move_to (cr, 0.5, 0.5);
		cairo_line_to (cr, width -1,  0.5);
		cairo_stroke (cr);
		equinox_shade (&colors->bg[GTK_STATE_SELECTED], &border, 1.5);
		cairo_set_source_rgba (cr, border.r, border.g, border.b, 0.25);
		cairo_move_to (cr, width - 0.5, 0.5);
		cairo_line_to (cr, width - 0.5,  height - 1);
		cairo_stroke (cr);
	} else if (menubaritemstyle == 3) {
		equinox_shade (&colors->bg[GTK_STATE_SELECTED], &fill, 1.15);
		pattern = cairo_pattern_create_linear (0, height - 4, 0, height);
		equinox_pattern_add_color_rgb (pattern, 0.0, &fill);
		equinox_pattern_add_color_rgb (pattern, 1.0, &colors->bg[GTK_STATE_SELECTED]);
		equinox_rounded_gradient (cr, EQX_CAIRO_FILL, 0, height - 4, width, 4, widget->curvature, widget->corners, pattern);
	} else { // old original design
		EquinoxRGB light = widget->parentbg;
		EquinoxRGB shadow, border;
		fill = colors->bg[GTK_STATE_SELECTED];
		equinox_shade_shift (&fill, &border, 0.7);
		equinox_shade_shift (&light, &light, 1.1);
		equinox_shade_shift (&fill, &shadow, 0.9);
		// Item background
		pattern = cairo_pattern_create_linear (0, 1, 0, height);
		equinox_pattern_add_color_rgba (pattern, 0.0, &fill, 0.6);
		equinox_pattern_add_color_rgba (pattern, 1.0, &fill, 1.0);
		equinox_rounded_gradient (cr, EQX_CAIRO_FILL, 0, 1, width-1, height-1, widget->curvature, widget->corners, pattern);
		// Item border
		pattern = cairo_pattern_create_linear (0, 4, 0, height);
		equinox_pattern_add_color_rgba (pattern, 0.0, &border, 0.0);
		equinox_pattern_add_color_rgba (pattern, 0.5, &border, 0.7);
		equinox_rectangle_gradient (cr, EQX_CAIRO_STROKE, 0.5, 0, width-2, height+1, pattern);
		if (!in_panel) {
			// Shadow
			pattern = cairo_pattern_create_linear (0, 0, 0, height);
			equinox_pattern_add_color_rgba (pattern, 0.25, &shadow, 0.0);
			equinox_pattern_add_color_rgba (pattern, 1.0, &shadow, 0.4);
			equinox_rectangle_gradient (cr, EQX_CAIRO_STROKE, 1.5, 0, width-4, height-1, pattern);
			// Highlight
			pattern = cairo_pattern_create_linear (0, 6, 0, height);
			equinox_pattern_add_color_rgba (pattern, 0.0, &light, 0.0);
			equinox_pattern_add_color_rgba (pattern, 0.5, &light, 0.4);
			equinox_rectangle_gradient (cr, EQX_CAIRO_STROKE, width-1, 0, 1, height+1, pattern);
		}
	}
}


void equinox_draw_menuitem (cairo_t * cr,
		      const EquinoxColors * colors,
		      const WidgetParameters * widget,
		      int x, int y, int width, int height, int menuitemstyle) {
	EquinoxRGB fill = colors->spot[1];
	EquinoxRGB fill_shade, fill_light;
	EquinoxRGB border;

	double lightness = equinox_get_lightness(&widget->parentbg);


	cairo_pattern_t *pattern;

	if (menuitemstyle == 1) {
		EquinoxRGB fill_glow;
		equinox_shade_shift (&fill, &fill_shade, 0.98 - lightness * 0.05);
		equinox_shade_shift (&fill, &fill_glow, 1.05 + (1.0 - lightness) * 0.1);

		cairo_translate (cr, x, y);

		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		equinox_pattern_add_color_rgb (pattern, 0.0, &fill);
		equinox_pattern_add_color_rgb (pattern, 0.5, &fill_glow);
		equinox_pattern_add_color_rgb (pattern, 0.5, &fill_shade);
		equinox_pattern_add_color_rgb (pattern, 1.0, &fill);
	} else {
		equinox_shade_shift (&fill, &fill_shade, 0.92);
		equinox_shade_shift (&fill, &fill_light, 1.05);

		cairo_translate (cr, x, y);

		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		equinox_pattern_add_color_rgb (pattern, 0.0, &fill_light);
		equinox_pattern_add_color_rgb (pattern, 1.0, &fill_shade);
	}
	equinox_rounded_gradient (cr, EQX_CAIRO_FILL, 0, 0, width, height, widget->curvature, widget->corners, pattern);

	// Highlight
	equinox_shade (&fill, &border, 1.05);
	if (widget->curvature == 0) {
		cairo_move_to (cr, 0.5, 0.5);
		cairo_line_to (cr, width, 0.5);
		cairo_set_source_rgb (cr, border.r, border.g, border.b);
		cairo_stroke (cr);

		pattern = cairo_pattern_create_linear (0, 1, 0, height - 2);
		equinox_pattern_add_color_rgba (pattern, 0.0, &border, 1.0);
		equinox_pattern_add_color_rgba (pattern, 1.0, &border, 0.0);
		cairo_move_to (cr, 0.5, 1.5);
		cairo_line_to (cr, 0.5, height - 2);
		cairo_set_source (cr, pattern);
		cairo_stroke (cr);
		cairo_move_to (cr, width - 0.5, 1.5);
		cairo_line_to (cr, width - 0.5, height - 1);
		cairo_set_source (cr, pattern);
		cairo_stroke (cr);
	} else {
		EquinoxRGB border_shade;
		equinox_shade (&fill_shade, &border_shade, lightness < 0.5 ? 0.5 : 0.92);
		equinox_rounded_rectangle (cr, EQX_CAIRO_STROKE, 0.5, 0.5, width-1, height-1, widget->curvature, widget->corners, &border_shade, 1.0);
		pattern = cairo_pattern_create_linear (0, 1, 0, height - 2);
		equinox_pattern_add_color_rgba (pattern, 0.0, &border, 1.0);
		equinox_pattern_add_color_rgba (pattern, 1.0, &border, 0.1);
		equinox_rounded_gradient (cr, EQX_CAIRO_STROKE, 1.5, 1.5, width-3, height-3, widget->curvature - 1, widget->corners, pattern);
	}

}



void
equinox_draw_scrollbar_trough (cairo_t * cr,
			      const EquinoxColors * colors,
			      const WidgetParameters * widget,
			      const ScrollBarParameters * scrollbar,
			      int x, int y, int width, int height, int scrollbarstyle, int scrollbartrough)
{
	EquinoxRGB bg;
	EquinoxRGB shadow, highlight;
	cairo_pattern_t *pattern;

	if (scrollbar->horizontal) {
		int tmp = height;

		rotate_mirror_translate (cr, M_PI / 2, x, y, FALSE, FALSE);
		height = width;
		width = tmp;
	}
	else
		cairo_translate (cr, x, y);

	if (scrollbartrough == 1) {
		bg = colors->base[GTK_STATE_NORMAL];
	} else {
		bg = widget->parentbg;
	}
	if (scrollbarstyle == 4) {
		equinox_rectangle (cr, EQX_CAIRO_FILL, 0, 0, width, height, &bg, 1.0);
	} else {
		equinox_shade (&bg, &shadow, 0.92);
		equinox_shade (&bg, &highlight, 1.0);
		equinox_shade (&bg, &bg, 0.98);
		pattern = cairo_pattern_create_linear (0, 0, width, 0);
		equinox_pattern_add_color_rgb (pattern, 0.0, &shadow);
		equinox_pattern_add_color_rgb (pattern, 0.22, &bg);
		equinox_pattern_add_color_rgb (pattern, 0.55, &highlight);
		equinox_pattern_add_color_rgb (pattern, 0.82, &bg);
		equinox_pattern_add_color_rgb (pattern, 1.0, &shadow);
		equinox_rectangle_gradient (cr, EQX_CAIRO_FILL, 0, 0, width, height, pattern);
	}
}

void equinox_draw_scrollbar_stepper (cairo_t * cr, const EquinoxColors * colors,
							   const WidgetParameters * widget,
							   const ScrollBarParameters * scrollbar,
							   //const ScrollBarStepperParameters * stepper,
							   int x, int y, int width, int height, int scrollbarstyle, int scrollbartrough) {
/*	EquinoxRGB fill;
	EquinoxRGB border_normal;
	EquinoxRGB top_shade, bottom_shade;

	if (scrollbarstyle == 1) {
		equinox_shade (&colors->bg[GTK_STATE_NORMAL], &fill, 1.1);
		equinox_shade (&colors->shade[5], &border_normal, 0.95);

		// Border
		border_normal.r = border_normal.r * (0.5) + fill.r * 0.4;
		border_normal.g = border_normal.g * (0.5) + fill.g * 0.4;
		border_normal.b = border_normal.b * (0.5) + fill.b * 0.4;

		cairo_save (cr);

		if (!(scrollbar->horizontal))
			rotate_mirror_translate (cr, 0, x, y, FALSE, FALSE);
		else {
			int tmp = height;

			rotate_mirror_translate (cr, M_PI / 2, x, y, FALSE, FALSE);
			height = width;
			width = tmp;
		}
		cairo_set_line_width (cr, 1.0);

		boolean first_curved = FALSE;
		boolean second_curved = FALSE;
		if (scrollbar->steppers & EQX_STEPPER_B) {
			if (stepper->stepper == EQX_STEPPER_B)
				first_curved = TRUE;
		} else {
			if (stepper->stepper == EQX_STEPPER_A)
				first_curved = TRUE;
		}
		if (scrollbar->steppers & EQX_STEPPER_C) {
			if (stepper->stepper == EQX_STEPPER_C)
				second_curved = TRUE;
		} else {
			if (stepper->stepper == EQX_STEPPER_D)
				second_curved = TRUE;
		}

		//Shadow
		EquinoxRGB shadow;
		equinox_shade (&widget->parentbg, &shadow, 0.75);
		if (first_curved)
			equinox_draw_stepper_shadow(cr, 1.5, 2, width - 2, height, widget->curvature, widget->corners, &shadow);
		if (second_curved)
			equinox_draw_stepper_shadow(cr, 1.5, -2, width - 2, height, widget->curvature, widget->corners, &shadow);

		cairo_pattern_t *pattern;

		equinox_shade (&fill, &top_shade, 1.12);
		equinox_shade (&fill, &bottom_shade, 0.85);
		pattern = cairo_pattern_create_linear (0, 0, width, 0);
		equinox_pattern_add_color_rgb (pattern, 0.0, &top_shade);
		equinox_pattern_add_color_rgb (pattern, 0.6, &fill);
		equinox_pattern_add_color_rgb (pattern, 1.0, &bottom_shade);
		cairo_set_source (cr, pattern);
		cairo_pattern_destroy (pattern);

		// Button
		if (widget->curvature >= 1)
			nodoka_rounded_rectangle_inverted (cr, 1, 0, width - 1, height, widget->curvature, widget->corners);
		else
			cairo_rectangle (cr, 0, 0, width, height);
		cairo_fill (cr);

		// Top border
		if (stepper->stepper == EQX_STEPPER_B || (stepper->stepper == EQX_STEPPER_D && !second_curved)) {
			cairo_move_to (cr, 0.5, 0.5);
			cairo_line_to (cr, width - 1, 0.5);
			cairo_set_source_rgb (cr, border_normal.r, border_normal.g, border_normal.b);
			cairo_stroke (cr);
		}
		// Bottom border
		if (stepper->stepper == EQX_STEPPER_D) {
			cairo_move_to (cr, 0.5, height - 0.5);
			cairo_line_to (cr, width - 1, height - 0.5);
			cairo_set_source_rgb (cr, border_normal.r, border_normal.g, border_normal.b);
			cairo_stroke (cr);
		}
		// Left border
		cairo_move_to (cr, 0.5, second_curved ? -3.5 : 0.5);
		cairo_line_to (cr, 0.5, height + (first_curved || second_curved ? 3 : -1));
		cairo_set_source_rgb (cr, border_normal.r, border_normal.g, border_normal.b);
		cairo_stroke (cr);

		cairo_restore (cr);
	}*/
}


void equinox_draw_scrollbar_slider (cairo_t * cr,
			      const EquinoxColors * colors,
			      const WidgetParameters * widget,
			      const ScrollBarParameters * scrollbar,
			      int x, int y, int width, int height, int scrollbarstyle, int scrollbarmargin) {

	EquinoxRGB bg;
	EquinoxRGB bg_lighter, bg_mid, bg_darker;
	EquinoxRGB border;

	if (widget->prelight)
		bg = colors->bg[GTK_STATE_PRELIGHT];
	else
		bg = colors->bg[GTK_STATE_ACTIVE];

	equinox_shade (&bg, &border, 0.725);

	//begin drawing
	if (scrollbar->horizontal) {
		/*y++;
		height--;*/
		cairo_translate (cr, x, y);
	} else {
		/*x++;
		width--;*/
		int tmp = height;
		height = width;
		width = tmp;
		rotate_mirror_translate (cr, M_PI / 2, x, y, FALSE, FALSE);
	}

	int roundness = MIN (widget->curvature, height/2.0);
	uint8 corners = EQX_CORNER_NONE;
	// Use of scrollbarpadding parameter
	/*if (scrollbarpadding[2] >= scrollbarpadding[3] && scrollbarpadding[3] == 0)
		corners = EQX_CORNER_TOPLEFT | EQX_CORNER_TOPRIGHT;
	if (scrollbarpadding[3] >= scrollbarpadding[2] && scrollbarpadding[2] == 0)
		corners = corners | EQX_CORNER_BOTTOMLEFT | EQX_CORNER_BOTTOMRIGHT;*/

	// Use of scrollbarmargin parameter
	/*if (scrollbarmargin > 0)
		corners = corners | EQX_CORNER_BOTTOMLEFT | EQX_CORNER_BOTTOMRIGHT;
	else
		corners = EQX_CORNER_ALL;*/

	// Draw fill
	if (scrollbarstyle == 4) {
		// Use of scrollbarpadding parameter
		//equinox_rounded_rectangle (cr, EQX_CAIRO_FILL, scrollbarpadding[0], scrollbarpadding[2], width - (scrollbarpadding[0]+scrollbarpadding[1]), height - (scrollbarpadding[2]+scrollbarpadding[3]), roundness, corners, &bg, 1.0);

		// Use of scrollbarmargin parameter
		//equinox_rounded_rectangle (cr, EQX_CAIRO_FILL, 0, 0, width, height - scrollbarmargin, roundness, corners, &bg, 1.0);
		int offset = round(height / 3);
		equinox_rounded_rectangle (cr, EQX_CAIRO_FILL, 0, offset, width, height - 2 * offset, roundness, widget->corners, &bg, 1.0);
	} else {
		// Draw shadow
		EquinoxRGB shadow;
		equinox_shade (&colors->base[GTK_STATE_NORMAL], &shadow, 0.85);
		equinox_rounded_rectangle (cr, EQX_CAIRO_FILL, 0, 0, width , height, roundness, widget->corners, &shadow, 0.5);

		cairo_pattern_t *pattern;
		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		if (scrollbarstyle == 1 || scrollbarstyle == 3) {
			equinox_shade (&bg, &bg_darker, 0.85); // 0.76
			equinox_shade (&bg, &bg_lighter, 1.24); // 1.24

			equinox_mix_color (&bg_darker, &bg_lighter, 0.9, &bg_lighter); // 0.80
			equinox_mix_color (&bg_darker, &bg_lighter, 0.4, &bg_mid); // 0.55

			equinox_pattern_add_color_rgb (pattern, 0.0, &bg_lighter);
			equinox_pattern_add_color_rgb (pattern, 0.5, &bg_mid);
			equinox_pattern_add_color_rgb (pattern, 0.5, &bg_darker);
			equinox_pattern_add_color_rgb (pattern, 1.0, &bg);
		} else {
			equinox_shade_shift (&bg, &bg_lighter, 1.12);
			equinox_shade_shift (&bg, &bg_darker, 0.92);

			equinox_pattern_add_color_rgb (pattern, 0.0, &bg_lighter);
			equinox_pattern_add_color_rgb (pattern, 0.5, &bg);
			equinox_pattern_add_color_rgb (pattern, 1.0, &bg_darker);
		}
		equinox_rounded_gradient (cr, EQX_CAIRO_FILL, 1.5, 0.5, width - 3, height - 1, roundness, widget->corners, pattern);

		// Draw highlight
		EquinoxRGB highlight;
		equinox_shade_shift (&bg, &highlight, 1.25);

		pattern = cairo_pattern_create_linear (1, 1, 1 , height - 2);
		equinox_pattern_add_color_rgba (pattern, 0.0, &highlight, 0.75);
		equinox_pattern_add_color_rgba (pattern, 0.9, &highlight, 0.0);
		equinox_rounded_gradient (cr, EQX_CAIRO_STROKE, 2.5, 1.5, width - 5, height - 3, roundness - 1, widget->corners, pattern);

		// Draw border
		EquinoxRGB border_lighter;
		equinox_shade_shift (&border, &border, 0.68);
		equinox_shade_shift (&border, &border_lighter, 1.30);

		pattern = cairo_pattern_create_linear (0, 0, width, 0);
		equinox_pattern_add_color_rgb (pattern, 0.0, &border_lighter);
		equinox_pattern_add_color_rgb (pattern, 0.5, &border);
		equinox_pattern_add_color_rgb (pattern, 1.0, &border_lighter);
		equinox_rounded_gradient (cr, EQX_CAIRO_STROKE, 1.5, 0.5, width - 3, height - 1, roundness, widget->corners, pattern);

		//grips
		if (scrollbarstyle >= 2) {
			equinox_draw_inset_circle (cr, width / 2, height / 2, HANDLE_RADIUS, &bg, (scrollbar->horizontal) ? 0 : 1);
		}
	}
}

void
equinox_draw_menu_frame (cairo_t * cr,
			const EquinoxColors * colors,
			const WidgetParameters * widget,
			int x, int y, int width, int height)
{
		EquinoxRGB *border = (EquinoxRGB*)&colors->bg[GTK_STATE_ACTIVE]; //(EquinoxRGB*)&colors->shade[6];
		//equinox_shade_shift (&colors->bg[GTK_STATE_NORMAL], border, 0.76);

		cairo_translate (cr, x + 0.5, y + 0.5);
		cairo_rectangle (cr, 0, 0, width - 1, height - 1);
		cairo_set_source_rgb (cr, border->r, border->g, border->b);
		cairo_stroke (cr);
}

void equinox_draw_handle (cairo_t * cr,
		    const EquinoxColors * colors,
		    const WidgetParameters * widget,
		    const HandleParameters * handle,
		    int x, int y, int width, int height) {

	//equinox_draw_inset_circle (cr, x + width / 2, y + height / 2 + (2 * HANDLE_RADIUS + 3), HANDLE_RADIUS, &widget->parentbg, 0);
	equinox_draw_inset_circle (cr, x + width / 2, y + height / 2, HANDLE_RADIUS, &widget->parentbg, 0);
	//equinox_draw_inset_circle (cr, x + width / 2, y + height / 2 - (2 * HANDLE_RADIUS + 3), HANDLE_RADIUS, &widget->parentbg, 0);
}


static void _equinox_draw_arrow (cairo_t * cr, EquinoxRGB * color,
		    float alpha, EquinoxDirection dir, EquinoxArrowType type,
		    double x, double y, double width, double height)
{
	double rotate;

	if (dir == EQX_DIRECTION_LEFT)
		rotate = M_PI * 1.5;
	else if (dir == EQX_DIRECTION_RIGHT)
		rotate = M_PI * 0.5;
	else if (dir == EQX_DIRECTION_UP)
		rotate = M_PI;
	else
		rotate = 0;

	rotate_mirror_translate (cr, rotate, x, y, FALSE, FALSE);

	cairo_translate (cr, -0.5, 0.5);
	cairo_move_to (cr, -ARROW_WIDTH / 2, -ARROW_HEIGHT / 2);
	cairo_line_to (cr, 0, ARROW_HEIGHT / 2);
	cairo_line_to (cr, ARROW_WIDTH / 2, -ARROW_HEIGHT / 2);

	if (alpha < 1) {
		equinox_set_source_rgba (cr, color, alpha);
		cairo_fill (cr);
	} else {
		EquinoxRGB bg_lighter;

		equinox_shade (color, &bg_lighter, 1.15);
		cairo_pattern_t *pattern;
		pattern = cairo_pattern_create_linear (0, -height / 2, 0, height / 2);
		equinox_pattern_add_color_rgba (pattern, 0.0, color, alpha);
		equinox_pattern_add_color_rgba (pattern, 1.0, &bg_lighter, alpha);
		cairo_set_source (cr, pattern);
		cairo_fill (cr);
	}

}

void equinox_draw_arrow (cairo_t * cr,
		   const EquinoxColors * colors,
		   const WidgetParameters * widget,
		   const ArrowParameters * arrow,
		   int x, int y, int width, int height) {

	EquinoxRGB color;

	color = colors->text[widget->state_type];

	if (color.r != colors->bg[GTK_STATE_NORMAL].r || color.g != colors->bg[GTK_STATE_NORMAL].g || color.b != colors->bg[GTK_STATE_NORMAL].b) {
		gdouble tx, ty;

		if (arrow->direction == EQX_DIRECTION_DOWN || arrow->direction == EQX_DIRECTION_UP) {
			tx = (x + width / 2);
			ty = (y + height / 2) + 0.5;
		} else {
			tx = (x + width / 2) + 0.5;
			ty = (y + height / 2);
		}

		_equinox_draw_arrow (cr, &color, 1.0, arrow->direction, arrow->type, tx, ty, width, height);
	}
}


void
equinox_draw_resize_grip (cairo_t * cr,
			 const EquinoxColors * colors,
			 const WidgetParameters * widget,
			 const ResizeGripParameters * grip,
			 int x, int y, int width, int height, int resizegripstyle)
{
	if (resizegripstyle == 0) {
		equinox_draw_inset_circle (cr, x + width / 2, y + height / 2, MAX(height/2 - 5, HANDLE_RADIUS), &colors->bg[GTK_STATE_NORMAL], 0);
	} else {
		int OFFSET = 2;
		//cairo_translate (cr, 0.5, 0.5);
		EquinoxRGB fill;
		EquinoxRGB highlight;
		cairo_pattern_t *pattern;
		cairo_move_to (cr, width - OFFSET, height - (RESIZE_GRIP_SIZE + OFFSET));
		cairo_line_to (cr, width - OFFSET, height - OFFSET);
		cairo_line_to (cr, width - (RESIZE_GRIP_SIZE + OFFSET), height - OFFSET);
		equinox_shade (&widget->parentbg, &fill, 0.65);
		pattern = cairo_pattern_create_linear (0, height - (RESIZE_GRIP_SIZE + OFFSET), 0, height - OFFSET);
		equinox_pattern_add_color_rgba (pattern, 0.0, &fill, 0.9);
		equinox_pattern_add_color_rgba (pattern, 1.0, &fill, 0.35);
		cairo_set_source (cr, pattern);
		cairo_fill (cr);
		// Highlight
		cairo_move_to (cr, width + 0.5 - OFFSET, height - (RESIZE_GRIP_SIZE + OFFSET + 0.5));
		cairo_line_to (cr, width + 0.5 - OFFSET, height + 0.5 - OFFSET);
		cairo_line_to (cr, width - (RESIZE_GRIP_SIZE + OFFSET + 0.5), height + 0.5 - OFFSET);
		cairo_line_to (cr, width + 0.5 - OFFSET, height - (RESIZE_GRIP_SIZE + OFFSET + 0.5));
		equinox_shade_shift (&colors->bg[GTK_STATE_NORMAL], &highlight, 1.15);
		pattern = cairo_pattern_create_linear (0, height - (RESIZE_GRIP_SIZE + OFFSET), 0, height - OFFSET);
		equinox_pattern_add_color_rgba (pattern, 0.0, &highlight, 0.2);
		equinox_pattern_add_color_rgba (pattern, 1.0, &highlight, 0.8);
		cairo_set_source (cr, pattern);
		cairo_stroke (cr);
	}

}

void
equinox_draw_menu_checkbutton (cairo_t * cr,
			      const EquinoxColors * colors,
			      const WidgetParameters * widget,
			      const OptionParameters * status,
			      int x, int y, int width, int height)
{

  width = height = 13;
  cairo_translate (cr, x, y);

  if (status->draw_bullet) {
    if (status->inconsistent) {
      cairo_rectangle (cr, 4, 6, 7, 3);
    }
    else {
      cairo_scale (cr, width / 17.0, height / 17.0);
			cairo_translate (cr, -2.0, 0.0);
			cairo_translate(cr, 0.75,1.75);
			cairo_move_to (cr, 1.0, 8.0);
			cairo_rel_line_to (cr, 2.0, -2.50);
			cairo_rel_line_to (cr, 3.5, 2.75);
			cairo_rel_line_to (cr, 5.25, -8.5);
			cairo_rel_line_to (cr, 1.95, 0.0);
			cairo_rel_line_to (cr, -6.95, 12.5);
			cairo_close_path (cr);
    }
    cairo_set_source_rgb (cr, colors->text[widget->state_type].r, colors->text[widget->state_type].g, colors->text[widget->state_type].b);
    cairo_fill (cr);
	}
}

void equinox_draw_cell_checkbutton (cairo_t * cr,
			      const EquinoxColors * colors,
			      const WidgetParameters * widget,
			      const OptionParameters * status,
			      int x, int y, int width, int height) {
	const double ROUNDNESS = 1.5;
	EquinoxRGB border;
	EquinoxRGB bullet = colors->text[GTK_STATE_SELECTED];
	EquinoxRGB bg;
	width = height = 13;

	cairo_translate (cr, x, y);

	if (status->draw_bullet) {
		bg = colors->spot[1];
		equinox_mix_color (&colors->spot[1], &colors->spot[2], 0.50, &bg);
		border = colors->spot[2];
		equinox_shade (&border, &border, 0.60);
	} else {
		bg = colors->base[widget->state_type];
		equinox_shade (&bg, &bg, 1.175);
		border = colors->text[GTK_STATE_NORMAL];
	}

	//Applies to all other states
	if (widget->disabled) {
		bg = colors->bg[widget->state_type];
		//lighten border
		border = colors->shade[4];
		//change bullet to insensitive color
		bullet = colors->text[widget->state_type];
	}

	// Draw the rectangle for the checkbox itself
	equinox_rounded_rectangle (cr, EQX_CAIRO_FILL, 1.0, 1.0, width - 2, height - 2, ROUNDNESS, widget->corners, &bg, 1.0);
	equinox_rounded_rectangle (cr, EQX_CAIRO_STROKE, 0.5, 0.5, width - 1, height - 1, ROUNDNESS + 1, widget->corners, &border, 1.0);


	if (status->draw_bullet) {
		if (status->inconsistent)
			cairo_rectangle (cr, 4, 6, 7, 3);
		else {
	  		cairo_scale (cr, width / 19.0, height / 20.0);
			cairo_translate(cr, 2.1, 4.75);
			cairo_move_to (cr, 1.0, 8.0);
			cairo_rel_line_to (cr, 2.0, -2.50);
			cairo_rel_line_to (cr, 3.5, 2.75);
			cairo_rel_line_to (cr, 5.25, -8.5);
			cairo_rel_line_to (cr, 1.95, 0.0);
			cairo_rel_line_to (cr, -6.95, 12.5);
			cairo_close_path (cr);
		}
		equinox_set_source_rgb (cr, &bullet);
		cairo_fill (cr);
	}
}


void equinox_draw_checkbutton (cairo_t * cr,
			 const EquinoxColors * colors,
			 const WidgetParameters * widget,
			 const OptionParameters * status,
			 int x, int y, int width, int height, int checkradiostyle) {

	const double ROUNDNESS = 1.75;
	EquinoxRGB border;
	EquinoxRGB bullet = colors->text[GTK_STATE_SELECTED];
	EquinoxRGB bg;
	EquinoxRGB shadow;
	EquinoxRGB highlight;
	cairo_pattern_t *pattern;

	width = height = 15;

	cairo_translate (cr, x, y);

#ifdef HAVE_ANIMATION
	EquinoxRGB bg_old, border_old;
#endif
	double lightness = equinox_get_lightness(&widget->parentbg);
	//double border_scale = lightness < 0.5 ? (lightness*3.5) :1;
	double border_scale = (widget->disabled ? 0.72 : 0.56) - 0.03 * 1 / lightness;
	double highlight_scale = 0.19 - 0.025 * 1 / lightness;

	if (status->draw_bullet) {
		bg  = colors->base[GTK_STATE_SELECTED];
#ifdef HAVE_ANIMATION
		bg_old = colors->base[GTK_STATE_ACTIVE];
		equinox_mix_color (&colors->shade[8], &bg, 0.18, &border_old);
#endif
		equinox_mix_color (&widget->parentbg, &colors->base[GTK_STATE_SELECTED], lightness < 0.6 ? 0.15 : 0.45, &border);
		equinox_shade (&border, &border, border_scale );
	}
	else {
		bg = colors->base[widget->state_type];
#ifdef HAVE_ANIMATION
		bg_old  = colors->base[GTK_STATE_NORMAL];
		equinox_shade (&colors->shade[8], &border_old, border_scale);
#endif
		equinox_shade (&widget->parentbg, &border, border_scale);
	}

#ifdef HAVE_ANIMATION
	equinox_mix_color (&bg_old, &bg, widget->trans, &bg);
	equinox_mix_color (&border_old, &border, widget->trans, &border);
#endif

	if (widget->prelight) {
		equinox_shade_shift (&bg, &bg, 1.09);
		equinox_shade_shift (&bg, &highlight, 1.2);
	} else if (widget->disabled) {
		bg = colors->bg[widget->state_type];
		//border = colors->shade[5];
		equinox_shade (&colors->bg[widget->state_type], &border, border_scale);
		bullet = colors->text[widget->state_type];
		equinox_shade_shift (&bg, &highlight, 1.10);
	} else
		equinox_shade_shift (&bg, &highlight, 1.13);

	equinox_shade (&border, &shadow, 0.875);

	//Fill
	if (status->draw_bullet) {

		EquinoxRGB bg_darker, mid_highlight;

		if (checkradiostyle == 1) {
			equinox_shade_shift (&bg, &bg_darker, widget->disabled? 0.92 : 0.90);
			equinox_mix_color (&bg, &highlight, 0.50, &mid_highlight);
			pattern = cairo_pattern_create_linear (3, 3, 3, height-4);
			equinox_pattern_add_color_rgb (pattern, 0.0, &highlight);
			equinox_pattern_add_color_rgb (pattern, 0.5, &mid_highlight);
			equinox_pattern_add_color_rgb (pattern, 0.5, &bg);
			equinox_pattern_add_color_rgb (pattern, 1.0, &bg_darker);
		} else {
			equinox_shade_shift (&bg, &highlight, 1.15);
			equinox_shade_shift (&bg, &bg_darker, widget->disabled? 0.92 : 0.82);
			pattern = cairo_pattern_create_linear (2, 2, 2, height-4);
			equinox_pattern_add_color_rgb (pattern, 0.0, &highlight);
			equinox_pattern_add_color_rgb (pattern, 0.55, &bg);
			equinox_pattern_add_color_rgb (pattern, 1.0, &bg_darker);
		}
		equinox_rounded_gradient (cr, EQX_CAIRO_FILL, 2.0, 2.0, width - 4, height - 4, ROUNDNESS-0.5, widget->corners, pattern);

		// Stroke highlight
		equinox_shade_shift (&highlight, &highlight, widget->disabled ? 1.05 : 1.08);

		pattern = cairo_pattern_create_linear (0, 2.5, 0, height-2);
		equinox_pattern_add_color_rgb  (pattern, 0.0, &highlight);
		equinox_pattern_add_color_rgba (pattern, 1.0, &highlight, 0.0);
		equinox_rounded_gradient (cr, EQX_CAIRO_STROKE, 2.5, 2.5, width - 5, height - 5, ROUNDNESS-0.5, widget->corners, pattern);

		// Inner glow
		if ((widget->prelight || widget->disabled) && checkradiostyle == 1) {
			equinox_shade_shift (&highlight, &highlight, 1.10);
			pattern = cairo_pattern_create_radial (width/2, height/1.1, 0, width/2, height/1.1, width/2);
			equinox_pattern_add_color_rgba (pattern, 0.0, &highlight, 0.85);
			equinox_pattern_add_color_rgba (pattern, 1.0, &highlight, 0.0);
			equinox_rounded_gradient (cr, EQX_CAIRO_FILL, 2.0, 2.0, width - 4, height - 4, ROUNDNESS-0.5, widget->corners, pattern);
			equinox_draw_inner_glow (cr, 2, 2, width, height, ROUNDNESS, widget->corners, &highlight);
		}


	} else {

		equinox_rounded_rectangle (cr, EQX_CAIRO_FILL, 2.0, 2.0, width - 4, height - 4, ROUNDNESS-1, widget->corners, &bg, 1.0);

		// Inner shadow
		cairo_save (cr);
		clearlooks_rounded_rectangle (cr, 2.5, 2.5, width - 5, height - 5, ROUNDNESS - 1, widget->corners);
		cairo_clip (cr);
		equinox_rectangle (cr, EQX_CAIRO_STROKE, 2, 2.5, width - 4, height - 3, &shadow, 0.64); // previous alpha  : 0.32
		equinox_rectangle (cr, EQX_CAIRO_STROKE, 3, 3.5, width - 6, height - 5, &shadow, 0.12); // previous alpha  : 0.06
		cairo_restore (cr);
  }


	if (status->draw_bullet) {
		// Draw the shadow inset for unchecked shadow for checked
		equinox_draw_shadow (cr, 0.5, 0.5, width - 1, height - 1, ROUNDNESS + 0.5, widget->corners, &border, 0.32);
		equinox_draw_shadow (cr, 0.5, 1.0, width - 2, height - 0.5, ROUNDNESS + 2, widget->corners, &border, 0.08);
		// Border
		equinox_draw_border (cr, 1.5, 1.5, width - 3, height - 3, ROUNDNESS, widget->corners, &border, 1.18, 0.85);
	} else {
		// Draw the shadow inset for unchecked shadow for checked
		equinox_draw_etched_shadow (cr, 0.5, 0.5, width - 1, height - 1.5, ROUNDNESS + 1, widget->corners, &widget->parentbg, 1.0);
		// Border
		equinox_draw_etched_border (cr, 1.5, 1.5, width - 3, height - 3, ROUNDNESS, widget->corners, &border, 1.25, 0.85);
	}


	cairo_save(cr);
	if (!widget->disabled) {
		if (status->inconsistent) {
			cairo_translate(cr, 0.0, 0.5);
			cairo_rectangle (cr, 4, 6, 7, 3);
		} else {
			cairo_scale (cr, width / 19.0, height / 20.0);
			cairo_translate(cr, 2.1, 5.25);
			cairo_move_to (cr, 1.0, 8.0);
			cairo_rel_line_to (cr, 2.0, -2.50);
			cairo_rel_line_to (cr, 3.5, 2.75);
			cairo_rel_line_to (cr, 5.25, -8.5);
			cairo_rel_line_to (cr, 1.95, 0.0);
			cairo_rel_line_to (cr, -6.95, 12.5);
			  cairo_close_path (cr);
		}
    }
	if (status->draw_bullet)
		equinox_set_source_rgba (cr, &shadow, 0.55 * widget->trans);
	else
		equinox_set_source_rgba (cr, &shadow, 0.55 * (1 - widget->trans));
	cairo_set_line_width(cr, 0.50);
	cairo_stroke (cr);
	cairo_restore(cr);

	if (status->inconsistent) {
		cairo_rectangle (cr, 4, 6, 7, 3);
	} else {
		cairo_scale (cr, width / 22.0, height / 22.0); // 19, 20
		cairo_translate(cr, 3.75, 5.25); // 2.1, 4.75
		cairo_move_to (cr, 1.0, 8.0);
		cairo_rel_line_to (cr, 2.0, -2.50);
		cairo_rel_line_to (cr, 3.5, 2.75);
		cairo_rel_line_to (cr, 5.25, -8.5);
		cairo_rel_line_to (cr, 1.95, 0.0);
		cairo_rel_line_to (cr, -6.95, 12.5);
		cairo_close_path (cr);
	}
	if (status->draw_bullet)
    	equinox_set_source_rgba (cr, &bullet, widget->trans);
    else
     	equinox_set_source_rgba (cr, &bullet, (1 - widget->trans));
	cairo_fill (cr);

}

void
equinox_draw_menu_radiobutton (cairo_t * cr,
			      const EquinoxColors * colors,
			      const WidgetParameters * widget,
			      const OptionParameters * status,
			      int x, int y, int width, int height)
{

  EquinoxRGB bullet;

  width = height = 15;
  double center = width / 2.0 - 1.5;

  cairo_translate (cr, x-2.0, y-1.0);

  //selected text color for check mark as it goes over selected color
  bullet = colors->text[widget->state_type];

	/* Draw the rectangle for the checkbox itself */
  cairo_arc (cr, center + 1.5, center + 1.5, center, 0, M_PI * 2);

  cairo_set_source_rgb (cr, bullet.r, bullet.g, bullet.b);
  cairo_stroke (cr);

  if (status->draw_bullet) {
    if (status->inconsistent) {
      cairo_rectangle (cr, 4, 6, 7, 3);
    }
    else {
      cairo_arc (cr, center + 1.5, center + 1.5, center - 2.5, 0, M_PI * 2);
    }

    cairo_set_source_rgb (cr, bullet.r, bullet.g, bullet.b);
    cairo_fill (cr);
  }
}

void
equinox_draw_cell_radiobutton (cairo_t * cr,
			      const EquinoxColors * colors,
			      const WidgetParameters * widget,
			      const OptionParameters * status,
			      int x, int y, int width, int height)
{

  EquinoxRGB border;
  EquinoxRGB bullet;
  EquinoxRGB bg;

  width = height = 15;
  double center = width / 2.0 - 1.5;

  cairo_translate (cr, x, y);

  //selected text color for check mark as it goes over selected color
  bullet = colors->text[GTK_STATE_SELECTED];

  if (status->draw_bullet) {
    bg = colors->spot[1];
	equinox_mix_color (&colors->spot[1], &colors->spot[2], 0.50, &bg);
    border = colors->spot[2];
	equinox_shade (&border, &border, 0.60);

  }
  else {
    bg = colors->base[widget->state_type];
    equinox_shade (&bg, &bg, 1.175);
    border = colors->text[GTK_STATE_NORMAL];
  }

  //Applies to all other states
  if (widget->disabled) {
		equinox_mix_color (&colors->bg[widget->state_type], &bg, 0.10, &bg);
    //lighten border
    border = colors->shade[4];
    //change bullet to insensitive color
    bullet = colors->text[widget->state_type];
  }


  /* Draw the rectangle for the checkbox itself */
  cairo_arc (cr, center + 1.5, center + 1.5, center, 0, M_PI * 2);

  cairo_set_source_rgb (cr, bg.r, bg.g, bg.b);
  cairo_fill_preserve (cr);

  cairo_set_source_rgb (cr, border.r, border.g, border.b);
  cairo_stroke (cr);


  if (status->draw_bullet) {
    if (status->inconsistent) {
      cairo_rectangle (cr, 4, 6, 7, 3);
    }
    else {
      cairo_arc (cr, center + 1.5, center + 1.5, center - 2.75, 0, M_PI * 2);
    }

    cairo_set_source_rgb (cr, bullet.r, bullet.g, bullet.b);
    cairo_fill (cr);
  }
}



void equinox_draw_radiobutton (cairo_t * cr,
			 const EquinoxColors * colors,
			 const WidgetParameters * widget,
			 const OptionParameters * status,
			 int x, int y, int width, int height, int checkradiostyle) {

	EquinoxRGB border;
	EquinoxRGB bullet = colors->text[GTK_STATE_SELECTED];
	EquinoxRGB bg;
	EquinoxRGB shadow;
	EquinoxRGB highlight;
	cairo_pattern_t *pattern;

	width = height = 15;
	double center = width / 2.0 - 1.5;

	cairo_translate (cr, x, y);

#ifdef HAVE_ANIMATION
	EquinoxRGB bg_old, border_old;
#endif
	double lightness = equinox_get_lightness(&widget->parentbg);
	//double border_scale = lightness < 0.5 ? 0.72 : 0.56;
	double border_scale = (widget->disabled ? 0.72 : 0.56) - 0.03 * 1 / lightness;
	double highlight_scale = 0.19 - 0.025 * 1 / lightness;

	if (status->draw_bullet) {
		bg  = colors->base[GTK_STATE_SELECTED];
#ifdef HAVE_ANIMATION
		bg_old = colors->base[GTK_STATE_ACTIVE];
		equinox_mix_color (&colors->shade[8], &bg, 0.18, &border_old);
#endif
		equinox_mix_color (&widget->parentbg, &colors->base[GTK_STATE_SELECTED], lightness < 0.6 ? 0.15 : 0.45, &border);
		equinox_shade (&border, &border, 0.56);
	} else {
		bg = colors->base[widget->state_type];
#ifdef HAVE_ANIMATION
		bg_old  = colors->base[GTK_STATE_NORMAL];
		equinox_shade (&colors->shade[8], &border_old, border_scale);
#endif
		equinox_shade (&widget->parentbg, &border, border_scale);
	}

#ifdef HAVE_ANIMATION
	equinox_mix_color (&bg_old, &bg, widget->trans, &bg);
	equinox_mix_color (&border_old, &border, widget->trans, &border);
#endif

	if (widget->prelight) {
		equinox_shade_shift (&bg, &bg, 1.09);
		equinox_shade_shift (&bg, &highlight, 1.2);
	} else if (widget->disabled) {
		bg = colors->bg[widget->state_type];
		equinox_shade (&colors->bg[widget->state_type], &border, border_scale);
		bullet = colors->text[widget->state_type];
		equinox_shade_shift (&bg, &highlight, 1.10);
	} else
		equinox_shade_shift (&bg, &highlight, 1.13);

	equinox_shade (&border, &shadow, 0.875);

	// Fill
	if (status->draw_bullet) {
		EquinoxRGB bg_darker, mid_highlight;

		if (checkradiostyle == 1) {
			equinox_shade_shift (&bg, &bg_darker, widget->disabled? 0.92 : 0.90);
			equinox_mix_color (&bg, &highlight, 0.60, &mid_highlight);
			pattern = cairo_pattern_create_linear (3, 3, 3, height-4);
			equinox_pattern_add_color_rgb (pattern, 0.0, &highlight);
			equinox_pattern_add_color_rgb (pattern, 0.5, &mid_highlight);
			equinox_pattern_add_color_rgb (pattern, 0.5, &bg);
			equinox_pattern_add_color_rgb (pattern, 1.0, &bg_darker);
		} else {
			equinox_shade_shift (&bg, &highlight, 1.15);
			equinox_shade_shift (&bg, &bg_darker, widget->disabled? 0.90 : 0.86);
			pattern = cairo_pattern_create_linear (2, 2, 2, height-4);
			equinox_pattern_add_color_rgb (pattern, 0.0, &highlight);
			equinox_pattern_add_color_rgb (pattern, 0.55, &bg);
			equinox_pattern_add_color_rgb (pattern, 1.0, &bg_darker);
		}
		equinox_arc_gradient (cr, EQX_CAIRO_FILL, center + 1.5, center + 1.5, center -0.5, 0, M_PI * 2, pattern);

		// Stroke highlight
		equinox_shade_shift (&highlight, &highlight, widget->disabled ? 1.06 : 1.08);

		pattern = cairo_pattern_create_linear (0, 2, 0, height-2);
		equinox_pattern_add_color_rgb (pattern, 0.0, &highlight);
		equinox_pattern_add_color_rgba (pattern, 0.8, &highlight, 0.0);
		equinox_arc_gradient (cr, EQX_CAIRO_STROKE, center + 1.5, center + 1.5, center - 1, 0, M_PI * 2, pattern);

		// Inner glow
		if ((widget->prelight || widget->disabled) && checkradiostyle == 1) {
			equinox_shade_shift (&highlight, &highlight, 1.10);
			pattern = cairo_pattern_create_radial (width/2, height/1.1, 0, width/2, height/1.1, width/2);
			equinox_pattern_add_color_rgba (pattern, 0.0, &highlight, 0.85);
			equinox_pattern_add_color_rgba (pattern, 1.0, &highlight, 0.0);
	  		equinox_arc_gradient (cr, EQX_CAIRO_FILL, center + 1.5, center + 1.5, center -0.5, 0, M_PI * 2, pattern);
		}

	}
	else {
		equinox_arc (cr, EQX_CAIRO_FILL, center + 1.5, center + 1.5, center -0.5, 0, M_PI * 2, &bg, 1.0);

		// Inner shadow
		pattern = cairo_pattern_create_linear (0, 2, 0, height-2);
		equinox_pattern_add_color_rgba (pattern, 0.3, &shadow, 0.32);
		equinox_pattern_add_color_rgba (pattern, 1.0, &highlight, 0.0);
		equinox_arc_gradient (cr, EQX_CAIRO_STROKE, center + 1.5, center + 1.5, center - 1, M_PI * 0.75, M_PI * 0.25, pattern);
		pattern = cairo_pattern_create_linear (0, 2, 0, height-4);
		equinox_pattern_add_color_rgba (pattern, 0.3, &shadow, 0.06);
		equinox_pattern_add_color_rgba (pattern, 1.0, &highlight, 0.0);
		equinox_arc_gradient (cr, EQX_CAIRO_STROKE, center + 1.5, center + 1.5, center - 2, M_PI * 0.75, M_PI * 0.25, pattern);
	}

	//Border
	EquinoxRGB border_darker, border_lighter;
	pattern = cairo_pattern_create_linear (0, 2, 0, height-2);
	if (status->draw_bullet) {
		equinox_shade_shift (&border, &border_darker, 0.85);
		equinox_shade_shift (&border, &border_lighter, widget->disabled ? 1.20 : 1.35);
		equinox_pattern_add_color_rgb (pattern, 0.0, &border_lighter);
		equinox_pattern_add_color_rgb (pattern, 0.5, &border);
		equinox_pattern_add_color_rgb (pattern, 1.0, &border_darker);
	} else {
		equinox_shade_shift (&border, &border_darker, 0.78);
		equinox_shade_shift (&border, &border_lighter, 1.35);
		equinox_pattern_add_color_rgb (pattern, 0.0, &border_darker);
		equinox_pattern_add_color_rgb (pattern, 0.5, &border);
		equinox_pattern_add_color_rgb (pattern, 1.0, &border_lighter);
	}
	equinox_arc_gradient (cr, EQX_CAIRO_STROKE, center + 1.5, center + 1.5, center , 0, M_PI * 2, pattern);

   	// Draw the shadow : inset for unchecked shadow for checked
	if (status->draw_bullet) {
		equinox_arc (cr, EQX_CAIRO_STROKE, center + 1.5, center + 2.0, center + 0.5, 0, M_PI * 2, &shadow, 0.24);
		equinox_arc (cr, EQX_CAIRO_STROKE, center + 1.5, center + 3.5, center, 0, M_PI * 2, &shadow, 0.06);
	} else {
		EquinoxRGB shadow1;
		EquinoxRGB highlight1;

		equinox_shade_shift (&widget->parentbg, &shadow1, 0.8);
		equinox_shade_shift (&widget->parentbg, &highlight1, 1.26);

		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		equinox_pattern_add_color_rgba (pattern, 0.0, &shadow1, 0.40);
		equinox_pattern_add_color_rgba (pattern, 0.5, &widget->parentbg, 0.0);
		equinox_pattern_add_color_rgba (pattern, 1.0, &highlight1, 0.40);
		equinox_arc_gradient (cr, EQX_CAIRO_STROKE, center + 1.5, center + 1.5, center + 1, 0, M_PI * 2, pattern);
	}

	// Bullet
	cairo_save (cr);
	if (!widget->disabled) {
		cairo_translate (cr, 0.0, 0.2);
		if (status->inconsistent)
			clearlooks_rounded_rectangle(cr, 4, 6, 7, 3, 1.5, EQX_CORNER_ALL);
		else
			cairo_arc (cr, center + 1.5, center + 1.5, center - 3.25, 0, M_PI * 2);
	}
	if (status->draw_bullet)
		equinox_set_source_rgba (cr, &shadow, 0.55 * widget->trans);
	else
		equinox_set_source_rgba (cr, &shadow, 0.55 * (1 - widget->trans));
	cairo_set_line_width(cr, 0.50);
	cairo_stroke (cr);
	cairo_restore (cr);

	if (status->inconsistent)
		clearlooks_rounded_rectangle(cr, 4, 6, 7, 3, 1.5, EQX_CORNER_ALL);
	else
		cairo_arc (cr, center + 1.5, center + 1.5, center - 3.25, 0, M_PI * 2);
	if (status->draw_bullet)
		equinox_set_source_rgba (cr, &bullet, widget->trans);
	else
		equinox_set_source_rgba (cr, &bullet, (1 - widget->trans));
	cairo_fill (cr);
}




void
equinox_draw_tooltip (cairo_t * cr,
		     const EquinoxColors * colors,
		     const WidgetParameters * widget,
		     int x, int y, int width, int height)
{
	const EquinoxRGB *border = &colors->text[GTK_STATE_NORMAL];
	const EquinoxRGB *highlight = &colors->text[GTK_STATE_SELECTED];
	EquinoxRGB darker;
	cairo_pattern_t *pattern;

	cairo_translate (cr, x, y);


	cairo_rectangle(cr, 0, 0, width, height);

	equinox_shade_shift (&colors->bg[GTK_STATE_NORMAL], &darker, 0.90);

	pattern = cairo_pattern_create_linear (0, 0, width*0.75, height);
	cairo_pattern_add_color_stop_rgb (pattern, 0.0, colors->bg[GTK_STATE_NORMAL].r, colors->bg[GTK_STATE_NORMAL].g, colors->bg[GTK_STATE_NORMAL].b);
	cairo_pattern_add_color_stop_rgb (pattern, 1.0, darker.r, darker.g, darker.b);
	cairo_set_source (cr, pattern);
	cairo_pattern_destroy (pattern);

	cairo_fill(cr);

	//Highlight
	/*pattern = cairo_pattern_create_linear (0, 0, width*0.08, height*0.75);
	cairo_pattern_add_color_stop_rgba (pattern, 0.0, highlight->r, highlight->g, highlight->b, 0.40);
	cairo_pattern_add_color_stop_rgba (pattern, 1.0, highlight->r, highlight->g, highlight->b, 0.0);
	cairo_set_source (cr, pattern);
	cairo_pattern_destroy (pattern);

	cairo_move_to  (cr, 0, 0);
	cairo_line_to  (cr, 0, height);
	cairo_line_to  (cr, 5, height);
	cairo_curve_to (cr, 5, height, 0, 0, width, 0);
	cairo_fill(cr);*/


	//Stroke
	cairo_set_source_rgb (cr, border->r, border->g, border->b);
	cairo_rectangle (cr, 0.5, 0.5, width - 1, height - 1);
	cairo_stroke (cr);
}



void equinox_draw_list_selection (cairo_t * cr,
			    const EquinoxColors * colors,
			    const WidgetParameters * widget,
			    int x, int y, int width, int height) {

	EquinoxRGB fill_color, fill_shade, fill_light;
	EquinoxRGB border;

	cairo_pattern_t *pattern;

	if (widget->focus)
		fill_color = colors->base[widget->state_type];
	else
		fill_color = colors->base[GTK_STATE_ACTIVE];

	double lightness = equinox_get_lightness(&fill_color);

	equinox_shade_shift (&fill_color, &fill_shade, 0.95);
	equinox_shade_shift (&fill_color, &fill_light, 1.05);

	cairo_translate (cr, x, y);

	pattern = cairo_pattern_create_linear (0, 0, 0, height);
	equinox_pattern_add_color_rgb (pattern, 0.0, &fill_light);
	equinox_pattern_add_color_rgb (pattern, 1.0, &fill_shade);
	equinox_rectangle_gradient (cr, EQX_CAIRO_FILL, 0, 0, width, height, pattern);

	// Highlight
	equinox_shade (&fill_light, &border, 1.0125 + ((1 - lightness) * 0.1));
	cairo_move_to (cr, 0.5, 0.5);
	cairo_line_to (cr, width - 1, 0.5);
	cairo_set_source_rgb (cr, border.r, border.g, border.b);
	cairo_stroke (cr);

	// Shadow
	/*equinox_shade (&fill_shade, &border, 0.95);
	cairo_move_to (cr, 0.5, height - 0.5);
	cairo_line_to (cr, width - 1, height - 0.5);
	cairo_set_source_rgb (cr, border.r, border.g, border.b);
	cairo_stroke (cr);*/
}

void equinox_draw_list_item (cairo_t * cr,
			    const EquinoxColors * colors,
			    const WidgetParameters * widget,
			    int x, int y, int width, int height, int listviewitemstyle) {

	EquinoxRGB fill, fill_shade, fill_middle;

	fill = colors->base[GTK_STATE_NORMAL];

	if (listviewitemstyle == 1) {
		cairo_pattern_t *pattern;

		equinox_shade_shift (&fill, &fill_shade, 0.92);
		equinox_mix_color (&fill_shade, &fill, 0.78, &fill_middle);
		cairo_translate (cr, x, y);

		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		equinox_pattern_add_color_rgb (pattern, 0.0, &fill_shade);
		equinox_pattern_add_color_rgb (pattern, 0.15 * 20 / height, &fill_middle);
		equinox_pattern_add_color_rgb (pattern, 0.3 * 20 / height, &fill);
		equinox_rectangle_gradient (cr, EQX_CAIRO_FILL, 0, 0, width, height, pattern);

	}
}
