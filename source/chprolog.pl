#!/usr/bin/perl

use strict;
use File::Find;

my $prolog = <<EOP;
/* This file is part of the the dci project. Copyright (C) 2013-2023 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

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
