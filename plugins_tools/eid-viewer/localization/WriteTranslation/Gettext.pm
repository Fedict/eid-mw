package WriteTranslation::Gettext::Item;

sub new {
	my $class = shift;
	my $arg = shift;
	my $self = [];
	bless $self,$class;
	if(defined($arg)) {
		$self->add_line($arg);
	}
	return $self;
}

sub add_line {
	my ($self, $line) = @_;
	chomp $line;
	push @$self, $line;
}

sub clear {
	my $self = shift;
	while(scalar(@$self)) {
		pop @$self;
	}
}

sub lines {
	my $self = shift;
	return @$self;
}

sub string {
	my $self = shift;
	return join("", @$self);
}

sub print {
	my $self = shift;
	my $output = shift;

	foreach my $line($self->lines) {
		print $output $line . "\n";
	}
}

package WriteTranslation::Gettext::Msgid;

push @ISA, "WriteTranslation::Gettext::Item";

sub print {
	my $self = shift;
	my $output = shift;

	print $output "msgid ";
	$self->SUPER::print($output);
}

sub lines {
	my $self = shift;
	return map { '"' . $_ . '"' } @$self;
}

package WriteTranslation::Gettext::Msgstr;

push @ISA, "WriteTranslation::Gettext::Item";

sub print {
	my $self = shift;
	my $output = shift;

	print $output "msgstr ";
	$self->SUPER::print($output);
	print $output "\n";
}

sub lines {
	my $self = shift;
	return map { '"' . $_ . '"' } @$self;
}

package WriteTranslation::Gettext;

use strict;
use warnings;

use feature "signatures";

no warnings "experimental::signatures";

use File::Basename;

sub perform($translations, $directory) {
	my @files = glob("\"$directory/*.po\"");
	foreach my $file(@files) {
		my $lang = basename($file, ".po");
		open my $input, "< :encoding(UTF-8)", $file;
		my $msgid;
		my $msgstr;
		my $current;
		my @items;
		while(my $line = <$input>) {
			if($line =~ /^msgid "(.*)"$/) {
				$msgid = WriteTranslation::Gettext::Msgid->new($1);
				$current = \$msgid;
			} elsif($line =~ /^msgstr "(.*)"$/) {
				$msgstr = WriteTranslation::Gettext::Msgstr->new($1);
				$current = \$msgstr;
			} elsif($line =~ /^"(.*)"$/) {
				$$current->add_line($1);
			} elsif($line =~ /^$/) {
				if(!defined($msgid) && !defined($msgstr)) {
					push @items, WriteTranslation::Gettext::Item->new($line);
					next;
				}
				if(!defined($msgid) || !defined($msgstr)) {
					die "invalid .po file: missing msgstr or msgid";
				}
				if(exists($translations->{$msgid->string}{$lang})) {
					if($msgstr->string ne $translations->{$msgid->string}{$lang}) {
						$msgstr->clear;
						$msgstr->add_line($translations->{$msgid->string}{$lang});
					}
				}
				push @items, ($msgid, $msgstr);
				$msgid = undef;
				$msgstr = undef;
			} else {
				push @items, WriteTranslation::Gettext::Item->new($line);
			}
		}
		close $input;
		open my $output, "> :encoding(UTF-8)", $file;
		foreach my $item(@items) {
			$item->print($output);
		}
		close $output;
	}
}
