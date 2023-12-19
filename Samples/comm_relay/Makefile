#
#
SUBJ = web_proxy ssl_relay_rev ssl_relay udp_relay tcp_direct tcp_relay
#
all: $(SUBJ)

#
CC   = gcc
AR   = ar
TH	 = touch

LIB_DIR=../../../JunkBox_Lib
LIB_BSC_DIR = $(LIB_DIR)/Lib
LIB_EXT_DIR = $(LIB_DIR)/xLib

LIB_BSC = $(LIB_BSC_DIR)/libbasic.a
LIB_EXT = $(LIB_EXT_DIR)/libextend.a

#DEBUG = -DEBUG

CFLAGS  = -fPIC -DHAVE_CONFIG_H -I$(LIB_DIR) -I$(LIB_BSC_DIR) -I$(LIB_EXT_DIR) $(DEBUG) -DENABLE_SSL

SLIB = -L$(LIB_BSC_DIR) -lbasic -lm
ELIB = -L$(LIB_EXT_DIR) -L/usr/local/ssl/lib -lextend
#
#
#

.h.c:
	$(TH) $@


.c.o:
	$(CC) $< $(CFLAGS) -c -O2 


clean:
	rm -f *.o *~ $(SUBJ)


#
#
#
#
web_proxy: web_proxy.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lz

tcp_relay: tcp_relay.o  $(LIB_BSC)
	$(CC) $(@).o $(SLIB) -O2 -o $@ 

tcp_direct: tcp_direct.o  $(LIB_BSC)
	$(CC) $(@).o $(SLIB) -O2 -o $@ 

udp_relay: udp_relay.o  $(LIB_BSC)
	$(CC) $(@).o $(SLIB) -O2 -o $@ 

ssl_relay: ssl_relay.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt 

ssl_relay_rev: ssl_relay_rev.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt 

sl_relay: sl_relay.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz




$(LIB_BSC):
	(cd $(LIB_BSC_DIR) && make)

$(LIB_EXT):
	(cd $(LIB_EXT_DIR) && make)


