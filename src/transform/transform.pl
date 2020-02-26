#!/usr/local/bin/perl
#
# $Id: transform.pl,v 1.2 2006/12/19 08:33:18 hochstenbach Exp $
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#

use	XML::XPath;
use 	GDBM_File;
use 	POSIX qw(strftime);

unless (@ARGV == 3) {
	print STDERR "usage: $0 xml_file gdbm_file url base_url\n";
	exit(1);
}

my $xml_file = $ARGV[0];
my $db_file  = $ARGV[1];
my $url      = $ARGV[2];
my $base_url = $ENV{"GWBASEURL"};
my $frag_url = $url; $frag_url =~ s/^http:\/\///i;

tie %db, 'GDBM_File', $db_file, &GDBM_WRCREAT, 0644;

local (*IN);

if (! open(IN,$xml_file) ) {
	warn "main - open() failed";
	exit(2);
}

while (<IN>) {
	chomp();
	s/\r//mg;

	last if (/^\s*$/);

	if (/^Last-Modified:\s+/) {
		$db{lmf} = $';
	}
}

my $xp = &parse_xml(*IN);

close(IN);

if (! defined $xp ) {
	warn "main - parse_xml() failed";
	exit(2);
}


if (! &transform($db_file,$url)) {
	warn "main - transform() failed";
	unlink $db_file;
	exit(3);
}

untie %db;

sub	transform {
	my ($db_file,$url) = @_;
	my $ltime   = time;
	my $isotime = strftime "%Y-%m-%dT%H:%M:%SZ" , gmtime;
	my $s;

	$db{frm} = $url;
	$db{mod} = $ltime;
	$db{crt} = $isotime;

	# Granularity..
	$s = &get_str("/Repository/Identify/oai:granularity");
	if (!$s) {
		warn "transform - no granularity";
		return undef;
	}
	$db{grn} = $s;

	# Earliest datestamp..
	$s = &get_str("/Repository/Identify/oai:earliestDatestamp");
	if (!$s) {
		warn "transform - no earliestDatestamp";
		return undef;
	}
	$db{dtf} = $s;

	# Number of sets is default 0..
	$db{nst} = 0;

	# Tab delimited list of sets..	
	$db{set} = "";

	# Number of metadataFormat's..
	$s = &get_str("count(/Repository/ListMetadataFormats/oai:metadataFormat)");
	if (!$s) {
		warn "transform - no metadataFormat";
		return undef;
	}
	$db{nfm} = $s;

	# Identify..
	$s = &get_frag("/Repository/Identify");
	if (!$s) {
		warn "transform - no Identify";
		return undef;
	}
	$s =~ s/<(\/)?oai:/<$1/mg;
	$s =~ s/<baseURL>([^<]+)<\/baseURL>/sprintf("<baseURL>%s\/%s<\/baseURL>", $base_url, $frag_url)/e;
	$s =~ s/<\/Identify>//;
	$db{idf} = $s;

	# Loop all formats and gather data..	
	my @fmt;
	for (my $i = 1; $i <= $db{nfm} ; $i++) {
		my $f;
		$f = &get_str("/Repository/ListMetadataFormats/oai:metadataFormat[$i]/oai:metadataPrefix");
		if (!$f) {
			warn "transform - no metadataFormat[$i]/metadataPrefix";
			return undef;
		}
		push(@fmt, $f);

		$s = &get_frag("/Repository/ListMetadataFormats/oai:metadataFormat[$i]");
		if (!$s) {
			warn "transform - no metadataFormat[$i]/*";
			return undef;
		}

		# Clean the oai: out of the fragment...
		$s =~ s/<(\/)?oai:/<$1/mg;

		$db{"fmt:$f"} = $s;
	}
	$db{fmt} = join("\t",@fmt);

	my $num_id = 0;
	my $fmt_id = {};
	my @dates  = ();
	foreach my $f (@fmt) {
		my $n = &get_str("count(/Repository/ListRecords[\@metadataPrefix='$f']/oai:record)");
		
		for (my $i = 1 ; $i <= $n ; $i++) {
			# Identifier..
			my $id = &get_str("/Repository/ListRecords[\@metadataPrefix='$f']/oai:record[$i]/oai:header/oai:identifier");
			if (!$id) {
				warn "transform - no ListRecords[\@metadataPrefix='$f']/record[$i]/header/identifier";
				return undef;
			}

			my $dat = &get_str("/Repository/ListRecords[\@metadataPrefix='$f']/oai:record[$i]/oai:header/oai:datestamp");

			if (!$dat) {
				warn "transform - no ListRecords[\@metadataPrefix='$f']/record[$i]/header/datestamp";
				return undef;
			}

			my $frag = &get_frag("/Repository/ListRecords[\@metadataPrefix='$f']/oai:record[$i]");

			if (!$frag) {
				warn "transform - no ListRecords[\@metadataPrefix='$f']/record[$i]";
				return undef;
			}

			# Clean the oai: out of the fragment...
			$frag =~ s/<(\/)?oai:/<$1/mg;

			$db{"dat\:$f\:$id"} = $dat;

			$fmt_id->{$id}->{$f} = 1;

			$db{"xml\:$f\:$id"} = $frag;

			push(@dates,$dat);
		}
	}

	# Count the number of unique id's based on the number of unique oai_dc records...
	my $n = 0;
	foreach my $k (keys %db) {
		$n++ if ($k =~ /^xml:oai_dc/);
	}
	$db{nid} = $n;

	foreach my $k (keys %$fmt_id) {
		$db{"fmt:$k"} = join("\t", keys %{$fmt_id->{$k}});
	}

	# Sort the dates and select the lastest...
	my @sdate = sort {
			my $c = $a; my $d = $b;
			$c =~ s/-//mg;
			$d =~ s/-//mg;
			$d <=> $c } @dates;
	
	$db{dtl} = $sdate[0];	

	$db{lck} = 0;

	return 1;
}

sub	get_str {
	my $p = shift;
	return $xp->findvalue($p);
}

sub	get_frag {
	my $p = shift;
	return $xp->findnodes_as_string($p);
}

sub	parse_xml {
	my $x = XML::XPath->new(ioref => $_[0]);

	# Parse and test...
	eval "\$x->find('/');";

	if ($@) {
		warn "parse_xml - xml is invalid : $@";
		return undef;
	}

	return $x;
}
