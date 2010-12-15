#!/bin/sh
# 
# Installs buildbot_slave on OSX, creates a buildslave user, 
# creates and starts a buildbot slave instance
#
# How to call:
# sudo ./buildbot_slave_env.sh <buildbotmaster>:<buildbotmasterport> <slavename> <password> <youremailaddress>
# 
# Tip: If you are behind a proxy, use this:
# env http_proxy=http://proxy:8080 sudo -E ./buildbot_slave_env.sh <buildbotmaster>:<buildbotmasterport> <slavename> <password> <youremailaddress>
################################################
# Configuration

buildbotslavefile=buildbot-slave-0.8.2.zip
buildbotslavedir=buildbot-slave-0.8.2
buildbotslaveurl=http://dl.dropbox.com/u/2715381/buildbot/$buildbotslavefile

# End configuration
################################################
################################################
# Create tmp dir
################################################
mkdir /tmp/build_slave_env 
cd /tmp/build_slave_env

################################################
# Create buildslave user
################################################
echo -- Create user buildslave
if ! dscl . -read /Users/buildslave; then
  maxid=$(dscl . -list /users UniqueID | awk '{print $2}' | sort -ug | tail -1)
  userid=$((maxid+1))
  dscl . -create /Users/buildslave
  dscl . -create /Users/buildslave RealName buildslave
  dscl . -create /Users/buildslave PrimaryGroupID 20
  dscl . -create /Users/buildslave UniqueID "$userid"
  dscl . -create /Users/buildslave NFSHomeDirectory /Users/buildslave
  createhomedir -c -u buildslave
  echo    user /Users/buildslave created
else
  echo    user /Users/buildlsave already exists
fi

################################################
# Install buildbot_slave
################################################
echo -- Installing buildbot_slave
echo     Download from $buildbotslaveurl
curl $buildbotslaveurl -O
unzip -o $buildbotslavefile
cd $buildbotslavedir
python setup.py build
python setup.py install

################################################
# Create buildslave
################################################
su - buildslave -c '
cat > ~/.profile <<EOF
export PATH=/usr/bin:/bin:/Developer/usr/bin:/usr/local/bin
export LANG="en_US.UTF-8"
EOF
'
su - buildslave -c "
cd
mkdir workshop
/usr/local/bin/buildslave create-slave workshop $1 $2 $3
cd workshop
echo $4 > info/admin
hostinfo > info/host
"

################################################
# make buildslave autostart
################################################
sudo cat > /Library/LaunchDaemons/net.sourceforge.buildbot.slave.snowie.autostart <<EOD
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
        <key>StandardOutPath</key>
        <string>twistd.log</string>
        <key>StandardErrorPath</key>
        <string>twistd-err.log</string>
        <key>EnvironmentVariables</key>
        <dict>
                <key>PATH</key>
                <string>/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin:/usr/X11/bin:/Developer/usr/bin</string>
                <key>PYTHONPATH</key>
                <string>/System/Library/Frameworks/Python.framework/Versions/2.6/Extras/lib/python2.6/site-packages</string>
        </dict>
        <key>GroupName</key>
        <string>daemon</string>
        <key>KeepAlive</key>
        <dict>
                <key>SuccessfulExit</key>
                <false/>
        </dict>
        <key>Label</key>
        <string>net.sourceforge.buildbot.slave.snowie</string>
        <key>ProgramArguments</key>
        <array>
                <string>/usr/bin/twistd</string>
                <string>-no</string>
                <string>-y</string>
                <string>./buildbot.tac</string>
        </array>
        <key>RunAtLoad</key>
        <true/>
        <key>UserName</key>
        <string>buildslave</string>
        <key>WorkingDirectory</key>
        <string>/Users/buildslave/workshop</string>
</dict>
</plist>
EOD

launchctl start net.sourceforge.buildbot.slave.snowie
