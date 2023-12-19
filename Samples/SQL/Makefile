#
MYSQL_DIR = /usr/local/mysql
MYSQL_LIB_DIR = /usr/lib64/mysql

#
#
SUBJ = sql_test
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

MYSQL_INC_DIR = $(MYSQL_DIR)/include

#DEBUG = -DEBUG

CFLAGS  = -fPIC -DHAVE_CONFIG_H -I$(LIB_DIR) -I$(LIB_BSC_DIR) -I$(LIB_EXT_DIR) -I$(MYSQL_INC_DIR) $(DEBUG)

SLIB = -L$(LIB_BSC_DIR) -lbasic -lm
ELIB = -L$(LIB_EXT_DIR) -lextend

MYSQL_LIB = -L$(MYSQL_LIB_DIR) -lmysqlclient 

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

sql_test: sql_test.o  $(LIB_BSC) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(SLIB) $(MYSQL_LIB) -O2 -o $@  -lssl -lcrypto -lcrypt 




$(LIB_BSC):
	(cd $(LIB_BSC_DIR) && make)

$(LIB_EXT):
	(cd $(LIB_EXT_DIR) && make)


