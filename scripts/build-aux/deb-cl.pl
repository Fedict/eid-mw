#!/usr/bin/perl -w

use strict;
use warnings;

use Dpkg::Changelog::Debian;
use File::Basename;

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

my $entry = new Dpkg::Changelog::Entry;

if(!defined($date)) {
	open GIT, "git log --date=rfc HEAD^..HEAD |";
	while(<GIT>) {
		chomp;
		if(/^Date:\s+(.*)$/) {
			$date = $1;
		}
	}
	close GIT;
}
open my $vers, dirname($0) . "/genver.sh|";
my $version = <$vers>;
close $vers;
chomp $version;
$entry->{header} = "eid-mw ($version-0" . $ENV{SHORT} . "-1) " . $ENV{TARGET} . "-" . $ENV{DIST} . "; urgency=low";
$entry->{changes} = ["  * Snapshot release"];
$entry->{trailer} = " -- $author  $date";
$entry->normalize;
unshift @{$c->{data}}, $entry;
$c->save($changefile);
print "after:\n------\n";
print $c;
