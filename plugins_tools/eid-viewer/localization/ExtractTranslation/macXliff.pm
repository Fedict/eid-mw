package ExtractTranslation::macXliff;

use strict;
use warnings;
use feature "signatures";

no warnings "experimental::signatures";

use File::Basename;
use XML::SimpleObject;
use ExtractTranslation::Util;

sub extract($directory, $translations) {
	my @files = glob("\"$directory/*.xcloc/Localized Contents/*.xliff\"");
	foreach my $file(@files) {
		my $lang = basename($file, ".xliff");
		next if($lang eq "en");
		open my $input, "<:encoding(utf8)", "$file";
		my $xml;
		while(<$input>) {
			$xml .= $_;
		}
		close $input;
		my $xmlobj = XML::SimpleObject->new(XML => $xml);
		my $xliff = $xmlobj->child("xliff");
		foreach my $file($xliff->children("file")) {
			foreach my $unit($file->child("body")->children("trans-unit")) {
				my $source = $unit->child("source");
				next unless defined($source);
				$source = $source->value;
				next unless length($source);
				my $target = $unit->child("target");
				if(defined($target)) {
					addtrans($translations, $source, $lang, $target->value, "macXliff");
				}
			}
		}
	}
}

1;
