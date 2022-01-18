package WriteTranslation::macXliff;

use feature "signatures";

use warnings;
use strict;

use File::Basename;
use XML::Twig;

no warnings "experimental::signatures";

my $trans;
my $lang;

sub do_update($twig, $transunit) {
	my $source = $transunit->first_child_text('source');
	return unless defined($source) && length($source) > 0;
	if(!defined($trans->{$source}{$lang})) {
		warn "ignoring untranslated string '$source' in $lang";
		return;
	}
	$transunit->set_field("target" => $trans->{$source}{$lang});
}

sub update_translation($twig, $transunit) {
	do_update($twig, $transunit);
	$transunit->print;
}

sub perform($translations, $directory) {
	my @files = glob("\"$directory/*.xcloc/Localized Contents/*.xliff\"");
	$trans = $translations;
	foreach my $file(@files) {
		$lang = basename($file, ".xliff");
		next if($lang eq "en");
		XML::Twig->new(twig_roots => {
				"trans-unit" => \&update_translation
			},
			twig_print_outside_roots => 1,
		)->parsefile_inplace($file);

	}
}

1;
