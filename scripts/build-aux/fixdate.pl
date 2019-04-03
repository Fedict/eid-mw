#!/usr/bin/perl -w

use strict;
use warnings;

use Dpkg::Changelog::Debian;

my $c = Dpkg::Changelog::Debian->new();

my $changefile;
if(defined($ARGV[0])) {
	$changefile = $ARGV[0];
} else {
	$changefile = "debian/changelog";
}
my $date = undef;
if(defined($ARGV[1])) {
	$date=$ARGV[1];
}

$c->load($changefile);
print "before:\n-------\n";
print $c;

my $author = 'Build Slave <buildslave@eidmw.yourict.net>';

if(!defined($date)) {
	open GIT, "git log --date=rfc \$CI_COMMIT_SHA^..\$CI_COMMIT_SHA |";
	while(<GIT>) {
		chomp;
		if(/^Date:\s+(.*)$/) {
			$date = $1;
		}
	}
	close GIT;
}
$c->{data}[0]{trailer} = " -- $author  $date";
$c->save($changefile);
print "after:\n------\n";
print $c;
