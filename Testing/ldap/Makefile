#
#
SUBJ = ldap


#
all: $(SUBJ)

#
CC   = gcc
#CC   = g++
AR   = ar
TH	 = touch

TOPDIR=../../..

LIB_DIR=$(TOPDIR)/JunkBox_Lib


LIB_BSC_DIR = $(LIB_DIR)/Lib
LIB_EXT_DIR = $(LIB_DIR)/xLib
LIB_GRP_DIR = $(LIB_DIR)/gLib

LIB_BSC = $(LIB_BSC_DIR)/libbasic.a
LIB_EXT = $(LIB_EXT_DIR)/libextend.a
LIB_GRP = $(LIB_GRP_DIR)/libgraph.a

#DEBUG = -DEBUG

CFLAGS  = -fPIC -DHAVE_CONFIG_H -DENABLE_LDAP -DENABLE_SSL -I$(LIB_DIR) -I$(LIB_BSC_DIR) -I$(LIB_EXT_DIR) -I$(LIB_GRP_DIR) $(DEBUG)

#SLIB = -L$(LIB_BSC_DIR) -lbasic -lm -ldb-4.2
SLIB = -L$(LIB_BSC_DIR) -lbasic -lm 
ELIB = -L$(LIB_EXT_DIR) -lextend
GLIB = -L$(LIB_GRP_DIR) -lgraph
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
ldap: ldap.o $(LIB_BSC) $(LIB_EXT) $(LIB_GRP)
	$(CC) $(@).o $(ELIB) $(GLIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz -lldap -llber

