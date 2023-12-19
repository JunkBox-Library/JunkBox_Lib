
SUBJ = all-basic all-graph all-extend all-math all-brep all-mesh

all: $(SUBJ)


all-basic:
	(cd BasicLib && make)

all-math: all-basic
	(cd MathLib && make)

all-graph: all-basic all-math
	(cd GraphLib && make)

all-brep: all-basic all-math
	(cd BREPLib && make)

all-extend: all-basic all-math all-brep
	(cd ExtendLib && make)

all-mesh: all-basic all-math all-brep all-graph all-extend 
	(cd MeshLib && make)



##########################################################################################################
#
# for Clean
#
clean:
	-rm -f *~ *.tmp *.tgz *.tar.gz
	-(cd BasicLib  && make clean)
	-(cd ExtendLib && make clean)
	-(cd GraphLib  && make clean)
	-(cd MathLib   && make clean)
	-(cd BREPLib   && make clean)
	-(cd MeshLib   && make clean)


clean_deps:
	-rm -rf .deps
	-(cd BasicLib  && make clean_deps)
	-(cd ExtendLib && make clean_deps)
	-(cd GraphLib  && make clean_deps)
	-(cd MathLib   && make clean_deps)
	-(cd BREPLib   && make clean_deps)
	-(cd MeshLib   && make clean_deps)


