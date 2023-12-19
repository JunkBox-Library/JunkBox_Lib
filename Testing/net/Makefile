#
#
SUBJ = http_chunk ipaddr2 ipaddr http_server2 http_server tcp_hell get_http_header get_http udp_count
#
all: $(SUBJ)

#
CC   = gcc
#CC   = g++
AR   = ar
TH	 = touch

LIB_DIR=../../../JunkBox_Lib
LIB_BSC_DIR = $(LIB_DIR)/Lib
LIB_EXT_DIR = $(LIB_DIR)/xLib

LIB_BSC = $(LIB_BSC_DIR)/libbasic.a
LIB_EXT = $(LIB_EXT_DIR)/libextend.a

#DEBUG = -DEBUG

CFLAGS  = -fPIC -DHAVE_CONFIG_H -I../sl_relay -I$(LIB_DIR) -I$(LIB_BSC_DIR) -I$(LIB_EXT_DIR) $(DEBUG)

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
	rm -f *.o *~ $(SUBJ) conf/*~ *.v2s* *.s2v*


#
#
#
#
http_chunk: http_chunk.o  $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

http_server2: http_server2.o  $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

http_server: http_server.o  $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

tcp_hell: tcp_hell.o  $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

get_http_header: get_http_header.o  $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@ -lz 

get_http: get_http.o  $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

udp_count: udp_count.o  $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

ipaddr: ipaddr.o  $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

ipaddr2: ipaddr2.o  $(LIB_BSC) $(LIB_EXT) 
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




