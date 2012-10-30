#!/usr/bin/perl

# This script generated a color-file from a rgb.txt file that comes with
# XFree86. This file contains all color names that libMXP recognizes.
# Note that this list is probably bigger than what other implementations
# support, so server MXP implementations should test with those before relying
# on using all those color constants.
#
# Copyright (c) 2004 Tomas Mecir

open (INFILE, "rgb.txt") or die "Cannot open rgb.txt: $!";
open (OUTFILE, ">colorlist.h") or die "Cannot create output file colorlist.h: $!";

print OUTFILE "/* Auto-generated from rgb.txt by mkcolors.pl */\n\n";
print OUTFILE "#ifndef COLORLIST_H\n#define COLORLIST_H\n\n";
print OUTFILE "#include \"libmxp.h\"\n\n";
print OUTFILE "const char *COLOR_NAMES[] = { \n";

while (<INFILE>)
{
  # only colors whose names don't contain whitespaces
  if (/^\s*(\d+)\s+(\d+)\s+(\d+)\s+(\S+) *$/)
  {
    $cname = lc $4;
    print OUTFILE "  \"$cname\",\n";
  }
}
print OUTFILE "};\n\n";

close INFILE;
open (INFILE, "rgb.txt") or die "Cannot open rgb.txt: $!";

print OUTFILE "RGB COLOR_DEF[] = {\n";

$count = 0;

while (<INFILE>)
{
  if (/^\s*(\d+)\s+(\d+)\s+(\d+)\s+(\S+) *$/)
  {
    print OUTFILE "  {$1, $2, $3},\n";

    $count++;
  }
}

print OUTFILE "};\n\n";

print OUTFILE "#define NUM_MXP_COLORS $count\n\n";

print OUTFILE "#endif\n";

close INFILE;
close OUTFILE;

