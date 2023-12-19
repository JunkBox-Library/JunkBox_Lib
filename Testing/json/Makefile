#
# Operating System
OS = Linux
#OS = Solaris
#OS = AIX

# CPU
CPU = Intel
#CPU = Motorola

#
#
SUBJ = json_test6 ipynb_conv json_file json2 json json_test5 json_test4 json_test3 json_test2 json_test
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

CFLAGS  = -fPIC -DHAVE_CONFIG_H -I$(LIB_DIR) -I$(LIB_BSC_DIR) -I$(LIB_EXT_DIR) -D$(OS) -D$(CPU) $(DEBUG)

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
ipynb_conv: ipynb_conv.o  $(LIB_BSC)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@   

json_file: json_file.o  $(LIB_BSC)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@ 

json2: json2.o  $(LIB_BSC)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  

json: json.o  $(LIB_BSC)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  

json_test: json_test.o  $(LIB_BSC)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@ 

json_test2: json_test2.o  $(LIB_BSC)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  

json_test3: json_test3.o  $(LIB_BSC)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  

json_test4: json_test4.o  $(LIB_BSC)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@   

json_test5: json_test5.o  $(LIB_BSC)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  

json_test6: json_test6.o  $(LIB_BSC)
	$(CC) $(@).o $(ELIB) $(SLIB) -O2 -o $@  


$(LIB_BSC):
	(cd $(LIB_BSC_DIR) && make)

$(LIB_EXT):
	(cd $(LIB_EXT_DIR) && make)



