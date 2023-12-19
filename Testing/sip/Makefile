#
#
SUBJ = sip_test2 sip_test sip_via sip_contact sip_sdp


#
all: $(SUBJ)

#
CC   = gcc
#CC   = g++
AR   = ar
TH	 = touch

TOPDIR=../../..

LIB_DIR=$(TOPDIR)/JunkBox_Lib
SLR_DIR=$(TOPDIR)/sl_proxy/sl_relay


LIB_BSC_DIR = $(LIB_DIR)/Lib
LIB_EXT_DIR = $(LIB_DIR)/xLib

LIB_BSC = $(LIB_BSC_DIR)/libbasic.a
LIB_EXT = $(LIB_EXT_DIR)/libextend.a

#DEBUG = -DEBUG

CFLAGS  = -fPIC -DHAVE_CONFIG_H -I../sl_relay -I$(LIB_DIR) -I$(LIB_BSC_DIR) -I$(LIB_EXT_DIR) $(DEBUG)

#SLIB = -L$(LIB_BSC_DIR) -lbasic -lm -ldb-4.2
SLIB = -L$(LIB_BSC_DIR) -lbasic -lm 
ELIB = -L$(LIB_EXT_DIR) -lextend
SLLB =
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

sip_test: sip_test.o $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

sip_test2: sip_test2.o $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

sip_via: sip_via.o $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

sip_contact: sip_contact.o $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

sip_sdp: sip_sdp.o $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz





$(LIB_BSC):
	(cd $(LIB_BSC_DIR) && make)


$(LIB_EXT):
	(cd $(LIB_EXT_DIR) && make)




