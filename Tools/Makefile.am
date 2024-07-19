#

LIB_BSC_DIR = ../Lib
LIB_EXT_DIR = ../xLib
LIB_BSC = $(LIB_BSC_DIR)/libbasic.a
LIB_EXT = $(LIB_EXT_DIR)/libextend.a

SUBJ = c2doc fdump printxml llsd_dump base64_filename

if DISABLE_WIN
    SUBJ += check_ip_allow
endif


AM_CFLAGS  = -I. -I$(LIB_BSC_DIR) -I$(LIB_EXT_DIR) @DEBUG@ -Wall

bin_PROGRAMS = $(SUBJ)

c2doc_SOURCES = c2doc.c
c2doc_LDADD = $(LIB_BSC)

fdump_SOURCES = fdump.c
fdump_LDADD = $(LIB_BSC)

base64_filename_SOURCES = base64_filename.c
base64_filename_LDADD = $(LIB_BSC)

printxml_SOURCES = printxml.c
printxml_LDADD = $(LIB_BSC)

check_ip_allow_SOURCES = check_ip_allow.c
check_ip_allow_LDADD = $(LIB_EXT) $(LIB_BSC)

llsd_dump_SOURCES = llsd_dump.c
llsd_dump_LDADD = $(LIB_EXT) $(LIB_BSC)


clean:
	rm -f *.o *~ $(SUBJ)


clean_deps:
	-rm -rf .deps 



$(LIB_BSC):
	(cd ../Lib && make)


$(LIB_EXT):
	(cd ../xLib && make)



