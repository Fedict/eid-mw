#
# Regular cron jobs for the eid-mw package
#
0 4	* * *	root	[ -x /usr/bin/eid-mw_maintenance ] && /usr/bin/eid-mw_maintenance
