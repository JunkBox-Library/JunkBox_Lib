
SUBJ = all-lib

if DISABLE_WIN 
    SUBJ += all-xlib
endif

if ENABLE_GLIB 
    SUBJ += all-glib 
endif
SUBJ += all-tools

all: $(SUBJ)


all-lib:
	(cd Lib   && make)

all-glib: all-lib
	(cd gLib  && make)

all-xlib: all-lib
	(cd xLib  && make)

all-tools: all-lib 
	(cd Tools && make)



##########################################################################################################
#
# for Clean
#
clean:
	-rm -f *~ *.tmp *.tgz *.tar.gz
	-(cd Exam  && make clean)
	-(cd Tools && make clean)
	-(cd xLib  && make clean)
	-(cd gLib  && make clean)
	-(cd Lib   && make clean)


clean_deps:
	-rm -rf .deps
	-(cd xLib  && make clean_deps)
	-(cd Tools && make clean_deps)
if ENABLE_GLIB
	-(cd gLib  && make clean_deps)
endif
	-(cd Lib   && make clean_deps)



##########################################################################################################
#
# for Distribute
#
DIST_DIR = ../Dist
DIST_WRK = ../.temp_Dist
DIST_EXT = tar.gz

EXTRA_DIST = Exam Lib xLib gLib Tools README.utf8

PKG_NAME = $(PACKAGE_NAME)-$(PACKAGE_VERSION)


pkg:
	make clean
	make dist
	zcat  $(PKG_NAME).tar.gz | tar xfp -
	chmod -R o-rwx $(PKG_NAME)
	(cd $(PKG_NAME) && find . -name ".deps"|xargs rm -rf)
	ln -s $(PKG_NAME) JunkBox_Lib
	tar cfp - $(PKG_NAME) JunkBox_Lib | gzip > ../$(PKG_NAME).$(DIST_EXT)
	rm -f JunkBox_Lib
	rm -rf $(PKG_NAME)
	rm -f $(PKG_NAME).$(DIST_EXT)


    
dist-pkg release:
	mkdir -p $(DIST_DIR)
	mkdir -p $(DIST_WRK)
	(cd $(DIST_WRK) && cp -Rpd ../JunkBox_Lib $(PKG_NAME))
	(cd $(DIST_WRK) && find . -name ".svn" | xargs rm -rf)
	(cd $(DIST_WRK) && cd $(PKG_NAME) && make pkg)
	(cd $(DIST_WRK) && rm -rf $(PKG_NAME))
	(cd $(DIST_WRK) && zcat $(PKG_NAME).$(DIST_EXT) | tar xfp -)
	(cd $(DIST_WRK) && rm -rf $(PKG_NAME).$(DIST_EXT))
	(cd $(DIST_WRK) && chmod -R o-rwx .)
	(cd $(DIST_WRK) && tar cfp - * | gzip > $(PKG_NAME).$(DIST_EXT))
	(cd $(DIST_WRK) && rm -rf $(PKG_NAME) JunkBox_Lib)
	mv $(DIST_WRK)/$(PKG_NAME).$(DIST_EXT) $(DIST_DIR)
    
