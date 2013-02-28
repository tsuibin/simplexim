SUBDIRS =  \
	IMdkit \
	$(NULL)
	
INCLUDES =					\
	-I$(top_srcdir)			\
	-I$(top_srcdir)/IMdkit		\
	-I$(top_builddir)			\
	-I$(top_builddir)/IMdkit	\
	$(NULL)

AM_CFLAGS =				\
	@GLIB_CFLAGS@		\
	@GOBJECT_CFLAGS@	\
	@GTK3_CFLAGS@		\
	@GTK3X11_CFLAGS@	\
	$(INCLUDES)			\
	$(NULL)

libIMdkit = $(top_builddir)/IMdkit/libIMdkit.la
AM_LDADD = \
	@GLIB_LIBS@	\
	@GOBJECT_LIBS@	\
	@GTK3_LIBS@	\
	@GTK3X11_LIBS@	\
	$(libIMdkit)	\
	$(NULL)

# (xims -- XIM Server)
bin_PROGRAMS = dp-xims 

dp_xims_SOURCES = 	\
	main.c		\
	dp-xims.c	\
	dp-xims-window.c \
	$(NULL)

dp_xims_LDADD = 	\
	$(AM_LDADD) 	\
	$(NULL)
