#!/usr/bin/perl

use strict;
use File::Find;

my $prolog = <<EOP;
// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

EOP

find({wanted => \&chOne, no_chdir=>1}, './core');
find({wanted => \&chOne, no_chdir=>1}, './module');
exit(0);

sub chOne($)
{
    my $fname = $File::Find::name;

    return unless $fname =~ m/.*\.(h|hpp|ipp|cpp|idl)$/;

    my $fh; my $text;
    open($fh, "<$fname") or die "$fname: $!";
    read($fh, $text, 10000000);
    close $fh;

    my $old = $text;
    $text =~ s{\A((\/\/.*\n)|(\s+)|(\n+)|(/\*(.|\n)*?\*/))*}{$prolog}m;

    if($text ne $old)
    {
        open($fh, ">$fname") or die "$fname: $!";
        print $fh $text;
        close $fh;
    }
}
