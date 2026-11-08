#!/usr/bin/perl
#
# nkf test program for nkf 2.0
#    Shinji KONO <kono@ie.u-ryukyu.ac.jp>
# Sun Aug 18 12:25:40 JST 1996
# Sun Nov  8 00:16:06 JST 1998
# Sun Sep  8 14:04:02 JST 2002
#
# This is useful when you add new patch on nkf.
# Since this test is too strict, faileurs may not mean
# wrong conversion. 
#
# nkf 2.0 utf8
# nkf 1.5 differs on MIME decoding
# nkf 1.4 passes Basic Conversion tests
# nkf PDS version passes Basic Conversion tests  using "nkf -iB -oB "
#

use utf8;
use Encode qw(decode FB_CROAK);

if ($ARGV[0]) {
    $nkf = $ARGV[0];
} else {
    $nkf = 'MSWin32' eq $^O ? ".\\nkf" : "./nkf";
}
# $nkf = "doscmd nkf.exe";

# If you want to see the testing process, set next flag.

$error_count = 0;
$diff=1;

sub source_encoding_test {
    my $file = 'nkf32dll.c';
    open my $fh, '<:raw', $file or die "Cannot open $file: $!\n";
    my $bytes = do { local $/; <$fh> };
    defined $bytes or die "Cannot read $file: $!\n";
    close $fh or die "Cannot close $file: $!\n";

    my $has_bom = substr($bytes, 0, 3) eq "\xEF\xBB\xBF";
    my $source = eval { decode('UTF-8', $bytes, FB_CROAK) };
    die "$file is not valid UTF-8: $@" if $@;
    die "$file contains a Unicode replacement character\n"
        if index($source, "\x{FFFD}") >= 0;

    my %expected = (
        'version comment' => 'こちらのバージョンも更新してください。',
        'copyright notice' => '連絡先： （株）富士通研究所　ソフト３研　市川　至',
        'license notice' => 'このソースのいかなる複写，改変，修正も許諾します。ただし、',
        'patch credits' => 'パッチ制作者',
        'options comment' => 'オプションが 空白で区切られている場合に options を複数呼び出す',
        'encoding label' => 'char *code[] = {"シフトJIS","EUC","ISO-2022-JP","UTF-8","UTF-16LE","UTF-16BE"};',
    );
    for my $description (sort keys %expected) {
        die "$file is missing its $description\n"
            if index($source, $expected{$description}) < 0;
    }
    die "$file needs a UTF-8 BOM for MSVC\n" unless $has_bom;
    print "Windows DLL source encoding ... Ok\n";
}

source_encoding_test();

# &library_test0();
sub test {
    &command_test(@_);
}

sub command_test {
    local ($nkf,$in,@ans) = @_;
    local ($result);

    $result = '';
    open(OUT,"> nkf.in");
    binmode OUT;
    print OUT $in;
    close(OUT);
    system("$nkf <nkf.in >nkf.out");   # easy
    open(IN,"< nkf.out");
    binmode IN;
    while(<IN>) {
	$result .= $_;
    }

    $result =~ s/ //g if $nkf =~ /-\w+m[NS]/o;
    foreach $ans (@ans) {
	$ans =~ s/ //g if $nkf =~ /-\w+m[NS]/o;
        if ($result eq $ans) {
	    print "Ok\n";
	    return $result;
	}
    }
    $ans = $ans[0];
    print "Fail\n";
    system "mv nkf.in nkf.in.$error_count.bad";
    system "mv nkf.out nkf.out.$error_count.bad";
    open(OUT,"> nkf.expect.$error_count.bad");
    binmode OUT;
    print OUT $ans;
    close(OUT);
    $error_count++;
    if ($diff) {
	open(R,"|od -c >tmp.result.bad"); binmode R; print R $result; close(R);
	open(R,"|od -c >tmp.expect.bad"); binmode R; print R $ans; close(R);
	system "diff -c tmp.result.bad tmp.expect.bad";
    }
    return $result;
}

sub command_tests {
    my @tests = @_;
    my ($in, $out, $ans);

    for (my $i = 0; $i <= $#tests; $i += 3){
	my $nkf = $tests[$i];
	$in = $tests[$i+1];
	$ans = $tests[$i+2];
	$out = '';
	open(OUT, "> nkf.in");
	binmode OUT;
	print OUT $in;
	close(OUT);
	system("$nkf <nkf.in >nkf.out");   # easy
	open(IN,"< nkf.out");
	binmode IN;
	while (<IN>) {
	    $out .= $_;
	}
	close(IN);
	$out =~ s/ //g if $nkf =~ /-\w+m[NS]/o;
	$ans =~ s/ //g if $nkf =~ /-\w+m[NS]/o;
        if ($out ne $ans) {
	    last;
	}
    }
    if ($out eq $ans) {
	print "Ok\n";
	return;
    }
    print "Fail\n";
    system "mv nkf.in nkf.in.$error_count.bad";
    system "mv nkf.out nkf.out.$error_count.bad";
    open(OUT,"> nkf.expect.$error_count.bad");
    binmode OUT;
    print OUT $ans;
    close(OUT);
    $error_count++;
    if ($diff) {
	open(R,"|od -c >tmp.result.bad"); binmode R; print R $out; close(R);
	open(R,"|od -c >tmp.expect.bad"); binmode R; print R $ans; close(R);
	system "diff -c tmp.result.bad tmp.expect.bad";
    }
    return;
}

do "./nkf_test.pl";
unlink "nkf.in";
unlink "nkf.out";

if ($error_count > 1) {
    printf("%d errors were found.\n", $error_count);
    exit(1);
} elsif ($error_count == 1) {
    printf("1 error was found.\n");
    exit(1);
} else {
    printf("All tests are succeeded.\n");
    exit(0);
}
