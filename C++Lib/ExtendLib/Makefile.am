#
#
#

AM_CFLAGS = -fPIC -W -Wall -I../BasicLib/Lib -I../BasicLib -I../MathLib -I../BREPLib -I./xLib 
AM_CFLAGS += @DEBUG@ -DCPLUSPLUS

AM_CFLAGS += -Wno-stringop-truncation -Wno-stringop-overflow 
AM_CFLAGS += -Wno-class-memaccess -Wno-format-truncation


SRC   =  BVHTool.cpp LogRingBuffer.cpp

CSRC  = xLib/asn1_tool.c xLib/protocol.c xLib/llsd_tool.c xLib/bvh_tool.c xLib/sip_tool.c xLib/http_tool.c xLib/https_tool.c xLib/ipaddr_tool.c 
CSRC += xLib/smtp_tool.c xLib/mime_tool.c xLib/bdb_tool.c xLib/dh_tool.c xLib/ssl_tool.c xLib/gz_tool.c xLib/ldap_tool.c xLib/isnet.c 


CC  = g++

AM_CXXFLAGS = $(AM_CFLAGS)


lib_LIBRARIES = libextendpp.a 

libextendpp_a_SOURCES = $(CSRC) $(SRC)


xLib/asn1_tool.c:
	cd xLib && make


all:
	ln -sf $(lib_LIBRARIES) libextend++.a

clean:
	-rm -f *.o *~ *.a
	-(cd xLib && make clean)


clean_deps: 
	-rm -rf .deps 




