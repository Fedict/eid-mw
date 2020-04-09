#!/bin/sh
/bin/ps aux | /usr/bin/awk '/pcsd/{print $2}'

