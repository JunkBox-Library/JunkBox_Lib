#
#
SUBJ = bvh llm2stl fdump xtools uuid signal base64 awk


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
LIB_GRP_DIR = $(LIB_DIR)/gLib

LIB_BSC = $(LIB_BSC_DIR)/libbasic.a
LIB_EXT = $(LIB_EXT_DIR)/libextend.a
LIB_GRP = $(LIB_GRP_DIR)/libgraph.a

#DEBUG = -DEBUG

CFLAGS  = -fPIC -DHAVE_CONFIG_H -I../sl_relay -I$(LIB_DIR) -I$(LIB_BSC_DIR) -I$(LIB_EXT_DIR) -I$(LIB_GRP_DIR) $(DEBUG)

#SLIB = -L$(LIB_BSC_DIR) -lbasic -lm -ldb-4.2
SLIB = -L$(LIB_BSC_DIR) -lbasic -lm 
ELIB = -L$(LIB_EXT_DIR) -lextend
GLIB = -L$(LIB_GRP_DIR) -lgraph
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
check_salt: check_salt.o $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

bvh: bvh.o $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

llm2stl: llm2stl.o $(LIB_BSC) $(LIB_EXT) $(LIB_GRP)
	$(CC) $(@).o $(ELIB) $(GLIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

fdump: fdump.o $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

xtools: xtools.o $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

awk: awk.o $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

uuid: uuid.o $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

signal: signal.o $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

base64: base64.o $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz

db: db.o $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz -ldb-4.2

db2: db2.o $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz -ldb-4.2

db_lock: db_lock.o $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lssl -lcrypto -lcrypt -lz -ldb-4.2



$(LIB_BSC):
	(cd $(LIB_BSC_DIR) && make)


$(LIB_EXT):
	(cd $(LIB_EXT_DIR) && make)




