package ExtractTranslation::Gettext;

use strict;
use warnings;
use feature "signatures";
use utf8;

no warnings "experimental::signatures";

use File::Basename;
use ExtractTranslation::Util;

sub extract($directory, $translations) {
	my @files = glob("\"$directory/*.po\"");
	foreach my $file(@files) {
		my $lang = basename($file, ".po");
		open my $input, "<:encoding(utf8)", $file;
		my $current_source;
		my $current_target;
		my $current;
		while(my $line = <$input>) {
			if($line =~ /^msgid "(.*)"$/) {
				$current_source = $1;
				$current = \$current_source;
			} elsif($line =~ /^msgstr "(.*)"$/) {
				$current_target = $1;
				$current = \$current_target;
			} elsif($line =~ /^"(.*)"$/) {
				$$current .= $1;
			} elsif($line =~ /^$/) {
				if($current_source ne "") {
					addtrans($translations, $current_source, $lang, $current_target, "gettext");
				}
			} elsif($line =~ /^#/) {
				# ignore comments
				next;
			} else {
				die "Unparseable string: '$line'";
			}
		}
		close $input;
	}
}

1;
