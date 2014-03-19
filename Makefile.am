INCLUDES = $(GTK_CFLAGS)

enginedir = $(libdir)/gtk-2.0/$(GTK_VERSION)/engines
engine_LTLIBRARIES = libequinox.la

EXTRA_DIST = ./src/config.h.in

libequinox_la_SOURCES =		\
    ./src/equinox_rc_style.c	\
    ./src/equinox_rc_style.h	\
    ./src/equinox_style.c	\
    ./src/equinox_style.h	\
    ./src/equinox_theme_main.c	\
	./src/support.c			\
	./src/animation.c		\
	./src/animation.h		\
	./src/support.h			\
	./src/equinox_draw.c		\
	./src/equinox_draw.h		\
	./src/equinox_types.h	\
	./src/config.h

libequinox_la_LDFLAGS = -module -avoid-version -no-undefined
libequinox_la_LIBADD =  $(GTK_LIBS)
