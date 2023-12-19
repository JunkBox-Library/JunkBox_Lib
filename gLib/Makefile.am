#
#
#

AM_CFLAGS = -fPIC -W -Wall -I../Lib -fno-strict-aliasing

SRC = gdata.c gio.c gmt.c graph.c window.c gdebug.c tiff_tool.c shape_file.c


if ENABLE_X11
    SRC += openX.c
    AM_CFLAGS += $(X_CFLAGS)
else
    AM_CFLAGS += -DDISABLE_X11
endif


if ENABLE_JPEGFNC
    SRC += jpeg_tool.c
else
    AM_CFLAGS += -DDISABLE_JPEG
endif


if ENABLE_OPENJPEGFNC
    SRC += jp2k_tool.c jp2k_tool2.c
else
    AM_CFLAGS += -DDISABLE_OPENJPEG
endif



#AM_CFLAGS += @JPEG_INC_DIR@ @ENABLE_JPEG@ @DEBUG@ 



lib_LIBRARIES = libgraph.a

libgraph_a_SOURCES = $(SRC) 



clean:
	-rm -f *.o *~ libgraph.a 


clean_deps:
	-rm -rf .deps 


