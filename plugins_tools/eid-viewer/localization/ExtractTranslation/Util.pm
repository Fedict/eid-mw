package ExtractTranslation::Util;

require Exporter;
our @ISA = "Exporter";
our @EXPORT = qw/addtrans/;

use strict;
use warnings;
use feature "signatures";

no warnings "experimental::signatures";

sub addtrans($translations, $source, $lang, $target, $style) {
	if(!exists($translations->{$source})) {
		$translations->{$source} = { $style => { $lang => $target }};
	} elsif(!exists($translations->{$source}{$style})) {
		$translations->{$source}{$style} = { $lang => $target };
	} else {
		$translations->{$source}{$style}{$lang} = $target;
	}
}

1;
