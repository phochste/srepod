#!/usr/local/bin/perl
#
# $Id: validrepo.pl,v 1.2 2006/12/19 08:33:18 hochstenbach Exp $
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#

use     GDBM_File;

unless (@ARGV == 1) {
	print STDERR "usage: $0 gdbm_file\n";
	exit(1);
}

my $db_file = $ARGV[0];

unless (-r $db_file) {
	warn "$db_file not readable";
	exit(2);
}

my @ERRORS = ();

tie %db, 'GDBM_File', $db_file, &GDBM_READER, 0640;

if (! &is_iso($db{crt})) {
	push(@ERRORS,"crt - is not an iso date");
}

if (! length $db{lmf}) {
	push(@ERRORS,"lmf - is not a valid last modification date");
}

if ($db{lck} !~ /^\d$/) {
	push(@ERRORS,"lck - is not a valid lock status");
}

if ($db{mod} !~ /^\D+$/) {
	push(@ERRORS,"mod - is not a valid modification date");
}

if ($db{frm} !~ /^http:\/\/\S+$/) {
	push(@ERRORS,"frm - is not a vallid network location");
}

if ($db{grn} ne "YYYY-MM-DD") {
	push(@ERRORS,"grn - is not a valid granularity");
}

if (! &is_yyyymmdd($db{dtf})) {
	push(@ERRORS,"dtf - is not a valid datestamp");
}

if (! &is_yyyymmdd($db{dtl})) {
	push(@ERRORS,"dtl - is not a valid datestamp");
}
	
my $n = 0;
foreach my $k (keys %db) {
	$n++ if ($k =~ /^xml:oai_dc/);
}

if ($db{nid} != $n) {
	push(@ERRORS,"nid - doesn't match the number of unique id's");
}

if ($db{nst} !~ /^\d+$/) {
	push(@ERRORS,"nst - is not a number");
}

if ($db{nfm} !~ /^\d+$/) {
	push(@ERRORS,"nfm - is not a number");
}

if ($db{idf} !~ /<Identify>(.*)(?!<\/Identify>)$/s) {
	push(@ERRORS,"idf - is not valid");
}

if (&is_tabdelimited($db{set}) != $db{nst}) {
	push(@ERRORS,"set - does not match nst");
}

if (&is_tabdelimited($db{fmt}) != $db{nfm}) {
	push(@ERRORS,"fmt - does not match nfm");
}

foreach my $k (split(/\t/,$db{fmt})) {
	if($db{"fmt:$k"} =~ /^\s*$/) {
		push(@ERRORS,"fmt:$k - is empty");
	}
}

foreach my $k (keys %db) {
	next unless ($k =~ /^xml:oai_dc:(\S+)$/);

	my $id = $1;

	if ($db{"fmt:$id"} =~ /^\s*$/) {
		push(@ERRORS,"fmt:$id - is empty");
	}

	if ($db{"dat:oai_dc:$id"} =~ /^\s*$/) {
		push(@ERRORS,"dat:oai_dc:$id - is empty");
	}
}

if ( @ERRORS > 0 ) {
	foreach (@ERRORS) {
		print STDERR "error : $_\n";
	}
	exit(@ERRORS);
}

exit(0);

sub	contains {
	my ($str,$key) = @_;
	foreach (split(/\t/,$str)) {
		return 1 if ($_ eq $key);
	}
	return 0;
}

sub	is_tabdelimited {
	my $str = shift;
	my @d = split(/\t/,$str);
	my $n = @d;

	return $n;
}

sub	is_yyyymmdd {
	my $str = shift;

	if ($str =~ /^\d{4}-\d{2}-\d{2}$/) {
		return 1;
	} else {
		return 0;
	}
}

sub	is_iso {
	my $str = shift;

	if ($str =~ /^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z$/) {
		return 1;
	} else {
		return 0;
	}
}
