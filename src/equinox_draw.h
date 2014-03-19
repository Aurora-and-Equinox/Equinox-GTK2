#ifndef EQUINOX_DRAW_H
#define EQUINOX_DRAW_H

#include "equinox_types.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <cairo.h>

G_GNUC_INTERNAL void equinox_draw_button           (cairo_t *cr,
                                       const EquinoxColors      *colors,
                                       const WidgetParameters      *widget,
                                       int x, int y, int width, int height, int buttonstyle);

G_GNUC_INTERNAL void equinox_draw_scale_trough     (cairo_t *cr,
                                       const EquinoxColors      *colors,
                                       const WidgetParameters      *widget,
                                       const SliderParameters      *slider,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void equinox_draw_progressbar_trough (cairo_t *cr,
                                       const EquinoxColors      *colors,
                                       const WidgetParameters      *widget,
                                  		const ProgressBarParameters *progressbar,
                                       int x, int y, int width, int height, int progressbarstyle);


G_GNUC_INTERNAL void equinox_draw_progressbar_fill (cairo_t *cr,
                                       const EquinoxColors      *colors,
                                       const WidgetParameters      *widget,
                                       const ProgressBarParameters *progressbar,
                                       int x, int y, int width, int height, gint offset, int progressbarstyle);

G_GNUC_INTERNAL void equinox_draw_scale_slider    (cairo_t *cr,
                                       const EquinoxColors     *colors,
                                       const WidgetParameters     *widget,
                                       const SliderParameters     *slider,
                                       int x, int y, int width, int height, int scalesliderstyle);

G_GNUC_INTERNAL void equinox_draw_entry            (cairo_t *cr,
                                       const EquinoxColors     *colors,
                                       const WidgetParameters     *widget,
                                       const EntryParameters      *entry,
                                       int x, int y, int width, int height, int toolbarstyle);

G_GNUC_INTERNAL void equinox_draw_menubar          (cairo_t *cr,
                                       const EquinoxColors     *colors,
                                       const WidgetParameters     *widget,
                                       int x, int y, int width, int height,
                                       int menubarstyle, int menubarborders);

G_GNUC_INTERNAL void equinox_draw_panel          (cairo_t *cr,
                                       const EquinoxColors     *colors,
                                       const WidgetParameters     *widget,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void equinox_draw_tab              (cairo_t *cr,
                                       const EquinoxColors     *colors,
                                       const WidgetParameters     *widget,
                                       const TabParameters        *tab,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void equinox_draw_frame            (cairo_t *cr,
                                       const EquinoxColors     *colors,
                                       const WidgetParameters     *widget,
                                       const FrameParameters      *frame,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void equinox_draw_separator        (cairo_t *cr,
                                       const EquinoxColors     *colors,
                                       const WidgetParameters     *widget,
                                       const SeparatorParameters  *separator,
                                       int x, int y, int width, int height, int separatorstyle);

G_GNUC_INTERNAL void equinox_draw_list_view_header (cairo_t *cr,
                                       const EquinoxColors         *colors,
                                       const WidgetParameters         *widget,
                                       const ListViewHeaderParameters *header,
                                       int x, int y, int width, int height, int listviewheaderstyle);

G_GNUC_INTERNAL void equinox_draw_toolbar          (cairo_t *cr,
                                       const EquinoxColors         *colors,
                                       const WidgetParameters         *widget,
                                       int x, int y, int width, int height, int toolbarstyle);

G_GNUC_INTERNAL void equinox_draw_menuitem         (cairo_t *cr,
                                       const EquinoxColors         *colors,
                                       const WidgetParameters         *widget,
                                       int x, int y, int width, int height, int menuitemstyle);

G_GNUC_INTERNAL void equinox_draw_menubar_item     (cairo_t *cr,
                                       const EquinoxColors         *colors,
                                       const WidgetParameters         *widget,
                                       int x, int y, int width, int height, int menubaritemstyle, boolean in_panel);

G_GNUC_INTERNAL void equinox_draw_scrollbar_slider (cairo_t *cr,
                                       const EquinoxColors          *colors,
                                       const WidgetParameters          *widget,
                                       const ScrollBarParameters       *scrollbar,
                                       int x, int y, int width, int height, int scrollbarstyle, int scrollbarmargin);

G_GNUC_INTERNAL void equinox_draw_scrollbar_stepper (cairo_t * cr, const EquinoxColors * colors,
									 	const WidgetParameters * widget,
										const ScrollBarParameters * scrollbar,
										//const ScrollBarStepperParameters * stepper,
										int x, int y, int width, int height, int scrollbarstyle, int scrollbartrough);

G_GNUC_INTERNAL void equinox_draw_scrollbar_trough (cairo_t *cr,
                                       const EquinoxColors          *colors,
                                       const WidgetParameters          *widget,
                                       const ScrollBarParameters       *scrollbar,
                                       int x, int y, int width, int height, int scrollbarstyle, int scrollbartrough);

G_GNUC_INTERNAL void equinox_draw_menu_frame       (cairo_t *cr,
                                       const EquinoxColors          *colors,
                                       const WidgetParameters          *widget,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void equinox_draw_handle           (cairo_t *cr,
                                       const EquinoxColors          *colors,
                                       const WidgetParameters          *widget,
                                       const HandleParameters          *handle,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void equinox_draw_arrow            (cairo_t *cr,
                                       const EquinoxColors          *colors,
                                       const WidgetParameters          *widget,
                                       const ArrowParameters           *arrow,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void equinox_draw_resize_grip      (cairo_t *cr,
                                       const EquinoxColors          *colors,
                                       const WidgetParameters          *widget,
                                       const ResizeGripParameters      *grip,
                                       int x, int y, int width, int height, int resizegripstyle);

G_GNUC_INTERNAL void ubuntulooks_draw_tooltip      (cairo_t *cr,
                                       const EquinoxColors          *colors,
                                       const WidgetParameters          *widget,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void equinox_draw_checkbutton 		(cairo_t *cr,
                             				const EquinoxColors         *colors,
                             				const WidgetParameters      *widget,
                             				const OptionParameters      *status,
                             				int x, int y, int width, int height, int checkradiostyle);

G_GNUC_INTERNAL void equinox_draw_radiobutton 		(cairo_t *cr,
                             				const EquinoxColors         *colors,
                             				const WidgetParameters      *widget,
                             				const OptionParameters      *status,
                             				int x, int y, int width, int height, int checkradiostyle);

G_GNUC_INTERNAL void equinox_draw_menu_checkbutton 		(cairo_t *cr,
                             				const EquinoxColors         *colors,
                             				const WidgetParameters      *widget,
                             				const OptionParameters      *status,
                             				int x, int y, int width, int height);

G_GNUC_INTERNAL void equinox_draw_cell_checkbutton 		(cairo_t *cr,
                             				const EquinoxColors         *colors,
                             				const WidgetParameters      *widget,
                             				const OptionParameters      *status,
                             				int x, int y, int width, int height);


G_GNUC_INTERNAL void equinox_draw_menu_radiobutton 		(cairo_t *cr,
                             				const EquinoxColors         *colors,
                             				const WidgetParameters      *widget,
                             				const OptionParameters      *status,
                             				int x, int y, int width, int height);

G_GNUC_INTERNAL void equinox_draw_cell_radiobutton 		(cairo_t *cr,
                             				const EquinoxColors         *colors,
                             				const WidgetParameters      *widget,
                             				const OptionParameters      *status,
                             				int x, int y, int width, int height);

G_GNUC_INTERNAL void equinox_draw_list_selection (cairo_t * cr,
                                    const EquinoxColors * colors,
                                    const WidgetParameters * widget,
                                    int x, int y, int width, int height);

G_GNUC_INTERNAL void equinox_draw_list_item (cairo_t * cr,
									const EquinoxColors * colors,
									const WidgetParameters * widget,
									int x, int y, int width, int height, int listviewitemstyle);

G_GNUC_INTERNAL void equinox_draw_tooltip (cairo_t * cr,
                                    const EquinoxColors * colors,
                                    const WidgetParameters * widget,
                                    int x, int y, int width, int height);


#endif /* EQUINOX_DRAW_H */
