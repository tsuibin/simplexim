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
	@X11_CFLAGS@		\
	@GLIB_CFLAGS@		\
	@GOBJECT_CFLAGS@	\
	$(INCLUDES)			\
	$(NULL)

libIMdkit = $(top_builddir)/IMdkit/libIMdkit.la
AM_LDADD = \
	@X11_LIBS@	\
	@GLIB_LIBS@	\
	@GOBJECT_LIBS@	\
	$(libIMdkit)	\
	$(NULL)


bin_PROGRAMS = simplexim

simplexim_SOURCES = 		\
	main.c			\
	simplexim.c		\
	simplewindow.c 	\
	$(NULL)

simplexim_LDADD = 	\
	$(AM_LDADD)
	$(NULL)



