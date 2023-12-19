#
#
#SUBJ = db
SUBJ = zlib2 zlib


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

CFLAGS  = -fPIC -DHAVE_CONFIG_H -I../sl_relay -I$(LIB_DIR) -I$(LIB_BSC_DIR) -I$(LIB_EXT_DIR) -I$(SLR_DIR) $(DEBUG)

#SLIB = -L$(LIB_BSC_DIR) -lbasic -lm -ldb-4.2
SLIB = -L$(LIB_BSC_DIR) -lbasic -lm 
ELIB = -L$(LIB_EXT_DIR) -lextend -lz
SLLB =
#
#
#

.h.c:
	$(TH) $@


.c.o:
	$(CC) $< $(CFLAGS) -c -O2 


clean:
	rm -f *.o *~ $(SUBJ) conf/*~ 


#
#
#
#
zlib: zlib.o $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

zlib2: zlib2.o $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz








$(LIB_BSC):
	(cd $(LIB_BSC_DIR) && make)


$(LIB_EXT):
	(cd $(LIB_EXT_DIR) && make)




install: sl_relay
	install -m 0755 sl_relay /usr/local/bin
	mkdir -p   /usr/local/etc/sl_proxy
	chmod 0755 /usr/local/etc/sl_proxy
	install -m 0744 conf/hosts_relay.allow.sample   /usr/local/etc/sl_proxy/hosts_relay.allow.sample
	install -m 0744 conf/sl_relay.conf.sample /usr/local/etc/sl_proxy
	install -m 0744 conf/CA_lindenlab.pem     /usr/local/etc/sl_proxy
	mkdir -p     /var/sl_proxy
	chown nobody /var/sl_proxy
	chmod 0750   /var/sl_proxy
	install -m 0755 conf/sl_relay.init /etc/init.d/sl_relay


clean-install:
	rm -f  /usr/local/bin/sl_relay
	rm -rf /usr/local/etc/sl_proxy
	rm -f  /etc/init.d/sl_proxy
	rm -rf /var/sl_proxy






