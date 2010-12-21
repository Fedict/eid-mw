#!/bin/sh

# find the original script that this is a symlink to
EIDVIEWER_LAUNCH_SCRIPT=`readlink $0`

# extract its directory ; this is where the eid-viewer java classes are installed
EIDVIEWER_HOME=`dirname $EIDVIEWER_LAUNCH_SCRIPT`

# cd to that directory and start the main jar
cd $EIDVIEWER_HOME && java -jar eid-viewer-gui-4.0.0-SNAPSHOT.jar
