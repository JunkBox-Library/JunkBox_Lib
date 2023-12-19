#

#
#
SUBJ = udp_chat_inp_tiny udp_chat_disp_tiny udp_chat_server_debug udp_chat_oinp udp_chat_server udp_chat_disp udp_chat_inp 

#
all: $(SUBJ)

#
CC   = gcc
AR   = ar

XLIBD = /usr/X11R6/lib
XINCD = /usr/X11R6/include

SSLINC = /usr/local/ssl/include 

LIB_DIR = ../../../JunkBox_Lib

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
#
#
#

udp_chat_server_debug: udp_chat_server_debug.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@ -lcrypt

udp_chat_server: udp_chat_server.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@ -lcrypt

udp_chat_disp: udp_chat_disp.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@ -lcrypt

udp_chat_inp: udp_chat_inp.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@ -lcrypt

udp_chat_oinp: udp_chat_oinp.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@ -lcrypt

udp_chat_inp_tiny: udp_chat_inp_tiny.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@ -lcrypt

udp_chat_disp_tiny: udp_chat_disp_tiny.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@ -lcrypt




$(LIB_BSC):
	(cd $(LIB_BSC_DIR) && make)


$(LIB_GRA):
	(cd $(LIB_GRA_DIR) && make)

	
$(LIB_EXT):
	(cd $(LIB_EXT_DIR) && make)




