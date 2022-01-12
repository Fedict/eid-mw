package ExtractTranslation::ResourceXml;

use v5.30;
use strict;
use warnings;
use feature "signatures";
use feature "unicode_strings";

use open ":encoding(UTF-8)";

no warnings "experimental::signatures";

use XML::SimpleObject;
use File::Basename qw/basename dirname/;
use ExtractTranslation::Util;

sub readxml($filename, $dest) {
	open my $file, "<", $filename;
	my $in_comment = 0;
	while(<$file>) {
		if(!$in_comment) {
			if(/<!--/) {
				$in_comment = 1;
				next;
			}
		} else {
			if(/-->/) {
				$in_comment = 0;
			}
			next;
		}
		$$dest .= $_;
	}
	close $file;
}

sub extract($directory, $translations) {
	my @german_files = glob("\"$directory/*.de.resx\"");
	foreach my $german(@german_files) {
		my @parts = split /\./, basename($german);
		my $untranslated = join("/", dirname($german), join(".", $parts[0], $parts[2]));
		my $xml_untranslated = "";
		my %untranslated;

		readxml($untranslated, \$xml_untranslated);
		my $xmlobj = XML::SimpleObject->new(XML => $xml_untranslated);

		my $root = $xmlobj->child("root");
		foreach my $data($root->children("data")) {
			my $value = $data->child("value");
			$untranslated{$data->attribute("name")} = $value->value;
		}
		foreach my $lang("de", "fr", "nl") {
			my $translated = join("/", dirname($german), join(".", $parts[0], $lang, $parts[2]));
			my $xml_translated = "";
			readxml($translated, \$xml_translated);
			$xmlobj = XML::SimpleObject->new(XML => $xml_translated);
			$root = $xmlobj->child("root");
			foreach my $data($root->children("data")) {
				my $value = $data->child("value");
				addtrans($translations, $untranslated{$data->attribute("name")}, $lang, $value->value, "ResourceXML");
			}
		}
	}
}

1;
