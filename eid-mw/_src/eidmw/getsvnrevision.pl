#!/usr/bin/perl -w

use strict;

sub WriteRevision();
sub WriteRevisionH();

my $numArgs = $#ARGV + 1;

if ($numArgs>0)
{
	my $cmd = "echo " . $ARGV[0] . " > svn_info";
	system($cmd);
}
else
{
	system("svn info | grep Revision > svn_info");
}

open SVN_INFO, "< svn_info" or die "[Error] Could not open svn_info\n";
$_=<SVN_INFO>;
$_ =~ m/([0-9]+)/ or die "[Error] Revision not found\n";
my $Revision=$1;
close SVN_INFO;

WriteRevision();
WriteRevisionH();

##--------------------------------------------------------------------
sub WriteRevision()
{
  open FILE, "> svn_revision" or die "[Error] Could not open svn_revision\n";
  print FILE $Revision."\n";
  close FILE;
}

##--------------------------------------------------------------------
sub WriteRevisionH()
{
  open FILE, "> svn_revision.h" or die "[Error] Could not open svn_revision.h\n";

  print FILE "/* ****************************************************************************\n";
  print FILE "\$\$\$_ZETES_LICENSE_MUST_BE_HERE_BEFORE_DISTRIBUTING_SOURCES_\$\$\$\n";
  print FILE "**************************************************************************** */\n";
  print FILE "#ifndef __SVN_REVISION_H__\n";
  print FILE "#define __SVN_REVISION_H__\n";
  print FILE "\n";
  print FILE "#define SVN_REVISION ".$Revision."\n";
  print FILE "#define SVN_REVISION_STR \"".$Revision."\"\n";
  print FILE "\n";
  print FILE "#endif //__SVN_REVISION_H__\n";

  close FILE;
}

