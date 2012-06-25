INCLUDES = $(GTK_CFLAGS) $(pixman_CFLAGS)

SUBDIRS = schema
DIST_SUBDIRS = $(SUBDIRS) po

themedir = $(datadir)/themes/Xamarin/gtk-2.0
enginedir = $(libdir)/gtk-2.0/$(GTK_VERSION)/engines
engine_LTLIBRARIES = libxamarin.la

theme_DATA = rc/gtkrc

EXTRA_DIST = ./src/config.h.in \
        ./rc/gtkrc
	./COPYING.2.1

libxamarin_la_SOURCES = \
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
	./src/murrine_types.h \
	./src/exponential-blur.c \
	./src/exponential-blur.h \
	./src/gaussian-blur.c \
	./src/gaussian-blur.h \
	./src/raico-blur.c \
	./src/raico-blur.h \
	./src/stack-blur.c \
	./src/stack-blur.h

libxamarin_la_LDFLAGS = -module -avoid-version -no-undefined
libxamarin_la_LIBADD =  $(GTK_LIBS) $(pixman_LIBS) -lm

