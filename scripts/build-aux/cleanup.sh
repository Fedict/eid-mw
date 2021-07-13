#!/bin/bash

if [ -z "$DIST_LOC" ]; then
	echo "E: DIST_LOC is unset" >&2
	exit 1
fi

echo "continuous -- before:"
du -sh $DIST_LOC

for filetype in rpm deb tar.gz asc zip; do
	find $DIST_LOC/continuous -name "*.$filetype" -type f -mtime +550 -print0 | xargs -0 rm
done

echo "continuous -- after:"
du -sh $DIST_LOC

echo "repo, RPM -- before:"
du -sh $REPO_LOC/rpm

find $REPO_LOC/candidate $REPO_LOC/continuous -name "*.rpm" -type f -mtime +550 -print0 | xargs -0 rm

echo "repo, RPM -- after:"
du -sh $REPO_LOC/rpm
echo "repo, deb -- before:"
du -sh $REPREPRO_BASE_DIR
find $REPREPRO_BASE_DIR/repository/old-versions -type f -mtime +550 -print0 | xargs -0 rm
echo "repo, deb -- after:"
du -sh $REPREPRO_BASE_DIR
