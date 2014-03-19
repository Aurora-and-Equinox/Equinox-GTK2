#ifndef EQUINOX_TYPES_H
#define EQUINOX_TYPES_H

typedef unsigned char boolean;
typedef unsigned char uint8;


typedef enum {
	EQX_CAIRO_FILL = 0,
	EQX_CAIRO_STROKE = 1
} EquinoxCairoOps;

typedef enum {
	EQX_STATE_NORMAL,
	EQX_STATE_ACTIVE,
	EQX_STATE_SELECTED,
	EQX_STATE_INSENSITIVE
} EquinoxStateType;

typedef enum {
	EQX_CORNER_NONE        = 0,
	EQX_CORNER_TOPLEFT     = 1,
	EQX_CORNER_TOPRIGHT    = 2,
	EQX_CORNER_BOTTOMLEFT  = 4,
	EQX_CORNER_BOTTOMRIGHT = 8,
	EQX_CORNER_ALL         = 15
} EquinoxCorners;

typedef enum {
	EQX_JUNCTION_NONE      = 0,
	EQX_JUNCTION_BEGIN     = 1,
	EQX_JUNCTION_END       = 2
} EquinoxJunction;

typedef enum {
	EQX_STEPPER_UNKNOWN    = 0,
	EQX_STEPPER_A          = 1,
	EQX_STEPPER_B          = 2,
	EQX_STEPPER_C          = 4,
	EQX_STEPPER_D          = 8
} EquinoxStepper;

typedef enum {
	EQX_ORDER_FIRST,
	EQX_ORDER_MIDDLE,
	EQX_ORDER_LAST
} EquinoxOrder;

typedef enum {
	EQX_ORIENTATION_LEFT_TO_RIGHT,
	EQX_ORIENTATION_RIGHT_TO_LEFT,
	EQX_ORIENTATION_BOTTOM_TO_TOP,
	EQX_ORIENTATION_TOP_TO_BOTTOM
} EquinoxOrientation;

typedef enum {
	EQX_GAP_LEFT,
	EQX_GAP_RIGHT,
	EQX_GAP_TOP,
	EQX_GAP_BOTTOM
} EquinoxGapSide;

typedef enum {
	EQX_SHADOW_NONE,
	EQX_SHADOW_IN,
	EQX_SHADOW_OUT,
	EQX_SHADOW_ETCHED_IN,
	EQX_SHADOW_ETCHED_OUT
} EquinoxShadowType;

typedef enum {
	EQX_HANDLE_TOOLBAR,
	EQX_HANDLE_SPLITTER
} EquinoxHandleType;

typedef enum {
	EQX_ARROW_NORMAL,
	EQX_ARROW_SCROLL,
	EQX_ARROW_SPINBUTTON,
	EQX_ARROW_COMBO
} EquinoxArrowType;

typedef enum {
	EQX_DIRECTION_UP,
	EQX_DIRECTION_DOWN,
	EQX_DIRECTION_LEFT,
	EQX_DIRECTION_RIGHT
} EquinoxDirection;


typedef enum {
	EQX_PROGRESSBAR_CONTINUOUS,
	EQX_PROGRESSBAR_DISCRETE
} EquinoxProgressBarStyle;

typedef enum {
	EQX_WINDOW_EDGE_NORTH_WEST,
	EQX_WINDOW_EDGE_NORTH,
	EQX_WINDOW_EDGE_NORTH_EAST,
	EQX_WINDOW_EDGE_WEST,
	EQX_WINDOW_EDGE_EAST,
	EQX_WINDOW_EDGE_SOUTH_WEST,
	EQX_WINDOW_EDGE_SOUTH,
	EQX_WINDOW_EDGE_SOUTH_EAST
} EquinoxWindowEdge;

typedef enum {
	EQX_ENTRY_NORMAL,
	EQX_ENTRY_SPINBUTTON,
	EQX_ENTRY_COMBO_BOX,
	EQX_ENTRY_COMBO
} EquinoxEntryType;

typedef struct {
	double r;
	double g;
	double b;
} EquinoxRGB;

typedef struct {
	double x;
	double y;
	double width;
	double height;
} EquinoxRectangle;

typedef struct {
	EquinoxRGB bg[5];
	EquinoxRGB base[5];
	EquinoxRGB text[5];
	EquinoxRGB fg[5];
	EquinoxRGB shade[9];

	EquinoxRGB highlight[6];
	EquinoxRGB midtone[3];
	EquinoxRGB shadow;

	EquinoxRGB spot[3];

} EquinoxColors;

typedef struct {
	boolean active;
	boolean prelight;
	boolean disabled;
	boolean focus;
	boolean is_default;
	EquinoxStateType state_type;
	uint8 corners;
	uint8 xthickness;
	uint8 ythickness;
	EquinoxRGB parentbg;

	EquinoxStateType prev_state_type;
	double trans;
	boolean ltr;
	//boolean composited;

	// Style
	double curvature;
	/*int glazestyle;
	int glowstyle;
	int lightborderstyle;
	int reliefstyle;
	int roundness;
	double contrast;
	double glow_shade;
	double highlight_shade;
	double lightborder_shade;
	MurrineGradients mrn_gradient;
	MurrineStyles style;*/

} WidgetParameters;

typedef struct {
	boolean inverted;
	boolean horizontal;
	int fill_size;
} SliderParameters;

typedef struct {
	EquinoxOrientation orientation;
	EquinoxProgressBarStyle style;
	double fraction;
} ProgressBarParameters;

typedef struct {
	int linepos;
} OptionMenuParameters;

typedef struct {
	EquinoxShadowType shadow;
	EquinoxGapSide gap_side;
	int gap_x;
	int gap_width;
	EquinoxRGB *border;
	EquinoxRGB *fill;
	boolean use_fill;
	boolean fill_bg;
} FrameParameters;

typedef struct {
	EquinoxGapSide gap_side;
	boolean first_tab;
	boolean last_tab;

} TabParameters;

typedef struct {
	boolean inconsistent;
	boolean draw_bullet;
} OptionParameters;

typedef struct {
	EquinoxCorners    corners;
	EquinoxShadowType shadow;
} ShadowParameters;

typedef struct {
	boolean horizontal;
	boolean inToolbar;
} SeparatorParameters;

typedef struct {
	EquinoxOrder order;
	boolean resizable;
	boolean sorted;
} ListViewHeaderParameters;

typedef struct {
	EquinoxRGB color;
	//EquinoxJunction junction;	/* On which sides the slider junctions */
	//EquinoxStepper steppers;		/* The visible steppers */
	boolean horizontal;
	boolean has_color;
	int	style;
} ScrollBarParameters;

typedef struct {
	EquinoxHandleType type;
	boolean horizontal;
} HandleParameters;

/*typedef struct {
	EquinoxStepper stepper;         // Which stepper to draw
} ScrollBarStepperParameters;*/

typedef struct {
	EquinoxArrowType type;
	EquinoxDirection direction;
	double size;
} ArrowParameters;


typedef struct {
	EquinoxEntryType type;
	boolean isComboButton;
	boolean isSpinButton;
	boolean isToolbarItem;
} EntryParameters;

typedef struct {
	EquinoxWindowEdge edge;
} ResizeGripParameters;

#define EQUINOX_RECTANGLE_SET(rect, _x, _y, _w, _h) rect.x      = _x; \
                                                       rect.y      = _y; \
                                                       rect.width  = _w; \
                                                       rect.height = _h;

#endif /* EQUINOX_TYPES_H */
