QT4=`readlink -f \`which qmake\``
QT4PATH=${QT4%/*}
QT4PATH=${QT4PATH}/..
JAVACPATH=`which javac 2> /dev/null`
JAVAC=`echo $JAVACPATH | wc -l`
if [[ $JAVAC -eq 1 ]]
then
	JAVAINCPATH=${JAVACPATH%/*}
	JAVAINCPATH=$JAVAINCPATH/../include
fi
./configure --lib+=-L${QT4PATH}/lib/ --include+=${QT4PATH}/include/Qt --include+=${JAVAINCPATH} --include+=${JAVAINCPATH}/linux  BUILD_SDK=1
