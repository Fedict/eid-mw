#!/bin/sh

EXT_NAME=belgiumeid
BUILDS_DIR=builds
BUILD_DIR=_build

cd `dirname $0`

VERSION=`cat VERSION`
XPI_NAME=${EXT_NAME}-${VERSION}.xpi
CURRENT_NAME=${EXT_NAME}-CURRENT.xpi
OPTFORCE=0

while getopts fh optionname
do
  case "$optionname" in
   h) echo "Usage: $0"
      echo "Options:"
      echo " -f: overwrite existing build"
      echo " -h: print this help message"
      exit 1;;
   f) OPTFORCE=1;;
esac
done

if [ -e ${BUILDS_DIR}/${XPI_NAME} -a $OPTFORCE -ne 1  ]; then
  echo "A build for version ${VERSION} already exists. Run '$0 -f' to override."
  exit 1
fi
  
if [ -e ${BUILD_DIR} ]; then
  echo "The build dir '${BUILD_DIR}' exists. Please remove the directory and run again."
  exit 1
fi
mkdir ${BUILD_DIR}
cp -r $EXT_NAME/* ${BUILD_DIR}
find ${BUILD_DIR} -path "*.svn" -type d -print0 | xargs -0 /bin/rm -rf

cd ${BUILD_DIR}
zip -r $XPI_NAME .
mkdir ../${BUILDS_DIR} 2> /dev/null
cp -f $XPI_NAME ../${BUILDS_DIR}

cd ../${BUILDS_DIR}
rm ${CURRENT_NAME}
ln -s ${XPI_NAME} ${CURRENT_NAME}

cd ..

rm -rf ${BUILD_DIR}
