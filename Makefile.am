AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = genudfimage
genudfimage_SOURCES = \
  src/genudfimage.c \
  src/basics.c \
  src/conv.c \
  src/util.c \
  src/filst.c \
  src/main.c


src/filst.c: src/filst_te.c

src/filst_te.c: src/m4_filst_te.m4
	m4 -Isrc $^ > $@


#SUBDIRS = src
