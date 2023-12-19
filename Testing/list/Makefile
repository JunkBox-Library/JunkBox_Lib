
TARGET = list list2 list3 list4 tree

all: $(TARGET)

TOP = ../../..


list: list.c $(TOP)/JunkBox_Lib/Lib/libbasic.a
	gcc list.c -o list -I$(TOP)/JunkBox_Lib/Lib -L$(TOP)/JunkBox_Lib/Lib -lbasic

list2: list2.c $(TOP)/JunkBox_Lib/Lib/libbasic.a
	gcc list2.c -o list2 -I$(TOP)/JunkBox_Lib/Lib -L$(TOP)/JunkBox_Lib/Lib -lbasic

list3: list3.c $(TOP)/JunkBox_Lib/Lib/libbasic.a
	gcc list3.c -o list3 -I$(TOP)/JunkBox_Lib/Lib -L$(TOP)/JunkBox_Lib/Lib -lbasic

list4: list4.c $(TOP)/JunkBox_Lib/Lib/libbasic.a $(TOP)/JunkBox_Lib/xLib/libextend.a
	gcc list4.c -o list4 -I$(TOP)/JunkBox_Lib/Lib -I$(TOP)/JunkBox_Lib/xLib -L$(TOP)/JunkBox_Lib/xLib -lextend -L$(TOP)/JunkBox_Lib/Lib -lbasic -lz

tree: tree.c $(TOP)/JunkBox_Lib/Lib/libbasic.a
	gcc tree.c -o tree -I$(TOP)/JunkBox_Lib/Lib -L$(TOP)/JunkBox_Lib/Lib -lbasic


clean:
	rm -f ${TARGET} *.o *~
