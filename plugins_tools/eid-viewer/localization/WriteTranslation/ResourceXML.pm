package WriteTranslation::ResourceXML;

use feature "signatures";

use warnings;
use strict;

no warnings "experimental::signatures";

use XML::Twig;

my %map;
my $trans;
my $lang;

sub update_translation($twig, $data) {
	my $source = $map{$data->{att}->{name}};
	next unless defined($source) && length($source) > 0;
	if(!defined($trans->{$source}{$lang})) {
		warn "ignoring untranslated string '$source' in $lang";
		next;
	}
	$data->set_field("value" => $trans->{$source}{$lang});
	$data->print;
}

sub add_map($twig, $data) {
	$map{$data->{att}->{name}} = $data->field("value");
}

sub perform($translations, $directory) {
	$trans = $translations;
	my @german_files = glob("\"$directory/*.de.resx\"");
	foreach my $german(@german_files) {
		%map = ();
		my @parts = split /\./, basename($german);
		my $untranslated = join("/", dirname($german), join(".", $parts[0], $parts[2]));
		XML::Twig->new(twig_roots => { data => \&add_map })->parsefile($untranslated);
		foreach $lang(qw/de fr nl/) {
			XML::Twig->new(twig_roots => { data => \&update_translation },
				twig_print_outside_roots => 1,
			)->parsefile_inplace(join("/", dirname($german), join(".", $parts[0], $lang, $parts[2])));
		}
	}
}

1;
