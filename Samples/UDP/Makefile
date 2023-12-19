#

#
#
SUBJ = sample_server server udp_c-0_check udp_s-1 udp_s-20 udp_s-21 udp_s-22 udp_s-23 bbs shttp_s tcp-s tcp-2 tcp-1 udp_c-0 udp_s-0 udp_c-1 udp_s-u2l

#
LIB_DIR = ../../../JunkBox_Lib


#
all: $(SUBJ)

#
CC   = gcc
AR   = ar

XLIBD = /usr/X11R6/lib
XINCD = /usr/X11R6/include

SSLINC = /usr/local/ssl/include 


LIB_BSC_DIR = $(LIB_DIR)/Lib
LIB_GRA_DIR = $(LIB_DIR)/gLib
LIB_EXT_DIR = $(LIB_DIR)/xLib

LIB_BSC = $(LIB_BSC_DIR)/libbasic.a
LIB_GRA = $(LIB_GRA_DIR)/libgraph.a
LIB_EXT = $(LIB_EXT_DIR)/libextend.a


CFLAGS  = -DHAVE_CONFIG_H -I$(LIB_DIR) -I$(LIB_BSC_DIR) -I$(LIB_GRA_DIR) -I$(LIB_EXT_DIR) -I$(XINCD) -I$(SSLINC)

XLIB = -L$(XLIBD) -lX11 
SLIB = -L$(LIB_BSC_DIR) -lbasic -lm
GLIB = -L$(LIB_GRA_DIR) -lgraph $(XLIB)
ELIB = -L$(LIB_EXT_DIR) -lextend
#
#
#

.c.o:
	$(CC) $< $(CFLAGS) -c -O2 


clean:
	rm -f *.o *~ $(SUBJ)


#
$(LIB_BSC):
	(cd $(LIB_BSC_DIR) && make)


$(LIB_GRA):
	(cd $(LIB_GRA_DIR) && make)

	
$(LIB_EXT):
	(cd $(LIB_EXT_DIR) && make)

#
#
#
#
tcp-1: tcp-1.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@ 

tcp-2: tcp-2.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@ -lz

tcp-s: tcp-s.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@

shttp_s: shttp_s.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@ -lz

udp_c-0: udp_c-0.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@

udp_c-1: udp_c-1.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@

udp_s-0: udp_s-0.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@

udp_s-u2l: udp_s-u2l.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@


bbs: bbs.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@

server: server.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@

sample_server: sample_server.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@


udp_s-1: udp_s-1.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@

udp_s-20: udp_s-20.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@ -lcrypt

udp_s-21: udp_s-21.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@

udp_s-22: udp_s-22.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@

udp_s-23: udp_s-23.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@

udp_c-0_check: udp_c-0_check.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@

