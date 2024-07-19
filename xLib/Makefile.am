#
#
#

AM_CFLAGS = -fPIC -W -Wall -I../Lib

AM_CFLAGS += -Wno-stringop-truncation -Wno-stringop-overflow
AM_CFLAGS += -Wno-format-truncation 


SRC = asn1_tool.c asn1_node.c llsd_tool.c bvh_tool.c sip_tool.c http_tool.c ipaddr_tool.c protocol.c smtp_tool.c mime_tool.c 



if ENABLE_LDAP_FNC
    SRC += ldap_tool.c
else
    AM_CFLAGS += -DDISABLE_LDAP
endif


if ENABLE_SSL_FNC
    SRC += dh_tool.c ssl_tool.c https_tool.c isnet.c
else
    AM_CFLAGS += -DDISABLE_SSL
endif


if ENABLE_ZLIB_FNC
    SRC += gz_tool.c
else
    AM_CFLAGS += -DDISABLE_ZLIB
endif


if ENABLE_DB_FNC
    SRC += bdb_tool.c
else
    AM_CFLAGS += -DDISABLE_BERKELEYDB
endif


if ENABLE_MYSQL_FNC
    SRC += mysql_tool.c
else
    AM_CFLAGS += -DDISABLE_MYSQL
endif


AM_CFLAGS += @LDAP_INC_DIR@ @SSL_INC_DIR@ @ZLIB_INC_DIR@ @MYSQL_INC_DIR@
AM_CFLAGS += @ENABLE_LDAP@  @ENABLE_SSL@  @ENABLE_ZLIB@  @DEBUG@ 


lib_LIBRARIES = libextend.a

libextend_a_SOURCES = $(SRC)



clean:
	-rm -f *.o *~ libextend.a 


clean_deps:
	-rm -rf .deps

