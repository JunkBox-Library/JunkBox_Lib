#

SRC_TOP = ../../../xLib

CC  = g++
AR  = ar
TH  = touch
CP  = cp
LN  = ln -fs



#DEBUG = -DEBUG
DEBUG = 

CFLAGS = -fPIC -W -Wall -DHAVE_CONFIG_H -I../.. -I../../BasicLib/Lib -I../../BasicLib $(DEBUG)


LIB_H    = asn1_tool.h asn1_node.h llsd_tool.h bvh_tool.h bdb_tool.h sip_tool.h gz_tool.h http_tool.h https_tool.h ipaddr_tool.h 
LIB_SRC  = asn1_tool.c asn1_node.c llsd_tool.c bvh_tool.c bdb_tool.c sip_tool.c gz_tool.c http_tool.c https_tool.c ipaddr_tool.c
LIB_H   += isnet.h isnet_server.h isnet_client.h protocol.h smtp_tool.h dh_tool.h ssl_tool.h mime_tool.h
LIB_SRC += isnet.c isnet_server.c isnet_client.c protocol.c smtp_tool.c dh_tool.c ssl_tool.c mime_tool.c

LIB_H   += ldap_tool.h 
LIB_SRC += ldap_tool.c 



all: $(LIB_SRC) $(LIB_H)


clean:
	-rm -f *.o *~ *.h *.c *.a



#
#
#
#

llsd_tool.h:
	$(LN) $(SRC_TOP)/$@ .

bvh_tool.h:
	$(LN) $(SRC_TOP)/$@ .

bdb_tool.h:
	$(LN) $(SRC_TOP)/$@ .

sip_tool.h:
	$(LN) $(SRC_TOP)/$@ .

gz_tool.h:
	$(LN) $(SRC_TOP)/$@ .

http_tool.h:
	$(LN) $(SRC_TOP)/$@ .

https_tool.h:
	$(LN) $(SRC_TOP)/$@ .

ipaddr_tool.h:
	$(LN) $(SRC_TOP)/$@ .

isnet.h:
	$(LN) $(SRC_TOP)/$@ .

isnet_server.h:
	$(LN) $(SRC_TOP)/$@ .

isnet_client.h:
	$(LN) $(SRC_TOP)/$@ .

protocol.h:
	$(LN) $(SRC_TOP)/$@ .

smtp_tool.h:
	$(LN) $(SRC_TOP)/$@ .

mime_tool.h:
	$(LN) $(SRC_TOP)/$@ .

dh_tool.h:
	$(LN) $(SRC_TOP)/$@ .

ssl_tool.h:
	$(LN) $(SRC_TOP)/$@ .

asn1_tool.h:
	$(LN) $(SRC_TOP)/$@ .

asn1_node.h:
	$(LN) $(SRC_TOP)/$@ .

ldap_tool.h:
	$(LN) $(SRC_TOP)/$@ .


llsd_tool.c:
	$(LN) $(SRC_TOP)/$@ .

bvh_tool.c:
	$(LN) $(SRC_TOP)/$@ .

bdb_tool.c:
	$(LN) $(SRC_TOP)/$@ .

sip_tool.c:
	$(LN) $(SRC_TOP)/$@ .

gz_tool.c:
	$(LN) $(SRC_TOP)/$@ .

http_tool.c:
	$(LN) $(SRC_TOP)/$@ .

https_tool.c:
	$(LN) $(SRC_TOP)/$@ .

ipaddr_tool.c:
	$(LN) $(SRC_TOP)/$@ .

isnet.c:
	$(LN) $(SRC_TOP)/$@ .

isnet_server.c:
	$(LN) $(SRC_TOP)/$@ .

isnet_client.c:
	$(LN) $(SRC_TOP)/$@ .

protocol.c:
	$(LN) $(SRC_TOP)/$@ .

smtp_tool.c:
	$(LN) $(SRC_TOP)/$@ .

mime_tool.c:
	$(LN) $(SRC_TOP)/$@ .

dh_tool.c:
	$(LN) $(SRC_TOP)/$@ .

ssl_tool.c:
	$(LN) $(SRC_TOP)/$@ .

asn1_tool.c:
	$(LN) $(SRC_TOP)/$@ .

asn1_node.c:
	$(LN) $(SRC_TOP)/$@ .

ldap_tool.c:
	$(LN) $(SRC_TOP)/$@ .



