#
#
SUBJ = read_srtm
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

CFLAGS  = -fPIC -DHAVE_CONFIG_H -I$(LIB_DIR) -I$(LIB_BSC_DIR) -I$(LIB_EXT_DIR) $(DEBUG)

SLIB = -L$(LIB_BSC_DIR) -lbasic -lm
ELIB = -L$(LIB_EXT_DIR) -lextend
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
read_srtm: read_srtm.o  $(LIB_BSC) $(LIB_EXT)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  -lz




$(LIB_BSC):
	(cd $(LIB_BSC_DIR) && make)

$(LIB_EXT):
	(cd $(LIB_EXT_DIR) && make)


