package WriteTranslation::macXliff;

use feature "signatures";

use warnings;
use strict;

use XML::Twig;

no warnings "experimental::signatures";

sub perform($translations, $directory) {
	my @files = glob("\"$directory/*.xcloc/Localized Contents/*.xliff\"");
	foreach my $file(@files) {
		my $lang = basename($file, ".xliff");
		next if($lang eq "en");
		XML::Twig->new(twig_roots => {
				"trans-unit" => \&update_translation
			},
			twig_print_outside_roots => 1,
		)->parsefile_inplace($file);

		sub update_translation($twig, $transunit) {
			my $source = $transunit->first_child_text('source');
			next unless defined($source) && length($source) > 0;
			if(!defined($translations->{$source}{$lang})) {
				warn "ignoring untranslated string '$source' in $lang";
				next;
			}
			$transunit->set_field("target" => $translations->{$source}{$lang});
			$transunit->print;
		}
	}
}

1;
