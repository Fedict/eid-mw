export DIST=src/dist
export DIST32=${DIST}/osx32
export DIST64=${DIST}/osx64
export DIST96=${DIST}/osx96
export LIBRARY=Library
export APPSUPPORT=${LIBRARY}/Application Support
export FRAMEWORKS=${LIBRARY}/Frameworks

all : dmg

.PHONY: dmg
dmg:

	mkdir ${DIST96}

# Copy Firefox Add-On from the 64-bit platform
###################################################################################
	mkdir "${DIST96}/${LIBRARY}"
	cp -r 	"${DIST64}/${APPSUPPORT}" \
	      	"${DIST96}/${LIBRARY}"

# Create /Library/Frameworks directory structure according to 64-bit version
###################################################################################
	for dir in `find ${DIST64}/${FRAMEWORKS} -type d -mindepth 1 | sed -e "s|${DIST64}/${FRAMEWORKS}/||"`; do \
	mkdir -p ${DIST96}/${FRAMEWORKS}/$${dir}; \
	done

# Merge Dynamic Libraries
###################################################################################
	for dylib in `find ${DIST64}/${FRAMEWORKS} -name *.dylib | sed -e "s|${DIST64}||"`; do \
		lipo ${DIST64}/$${dylib} \
		${DIST32}/$${dylib} \
		-output ${DIST96}/$${dylib} \
		-create; \
	done

# Merge executables
###################################################################################
	for bin in `find \`find ${DIST64}/${FRAMEWORKS} -name bin\` -type f | sed -e "s|${DIST64}/||"`; do \
	lipo ${DIST64}/$${bin} \
	${DIST32}/$${bin} \
	-output ${DIST96}/$${bin} \
	-create; \
	done

# Remove linux and Windows config
###################################################################################
	find ${DIST96} -path *platform/Linux* -delete
	find ${DIST96} -path *platform/WINNT* -delete

clean:
	rm -Rf ${DIST96}
