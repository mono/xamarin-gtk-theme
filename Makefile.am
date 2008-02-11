INCLUDES = $(GTK_CFLAGS)

enginedir = $(libdir)/gtk-2.0/$(GTK_VERSION)/engines
engine_LTLIBRARIES = libmurrine.la

EXTRA_DIST = ./src/config.h.in

libmurrine_la_SOURCES = \
	./src/animation.c \
	./src/animation.h \
	./src/cairo-support.c \
	./src/cairo-support.h \
	./src/config.h \
	./src/support.c \
	./src/support.h \
	./src/murrine_rc_style.c \
	./src/murrine_rc_style.h \
	./src/murrine_style.c \
	./src/murrine_style.h \
	./src/murrine_theme_main.c \
	./src/murrine_draw.c \
	./src/murrine_draw_rgba.c \
	./src/murrine_draw.h \
	./src/murrine_types.h

libmurrine_la_LDFLAGS = -module -avoid-version -no-undefined
libmurrine_la_LIBADD =  $(GTK_LIBS)

