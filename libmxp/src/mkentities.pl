#!/usr/bin/perl

# This script generates a list of all entities recognozed by libMXP.
#
# Copyright (c) 2004 Tomas Mecir

open (INFILE, "entities.txt") or die "Cannot open entities.txt: $!";
open (OUTFILE, ">entitylist.h") or die "Cannot create output file entitylist.h: $!";

print OUTFILE "/* Auto-generated from entities.txt by mkentities.pl */\n\n";
print OUTFILE "#ifndef ENTITYLIST_H\n#define ENTITYLIST_H\n\n";
print OUTFILE "const char *ENTITY_NAMES[] = { \n";

while (<INFILE>)
{
  if (/^Name: (\S+)\s+Value: (\d+)$/)
  {
    print OUTFILE "  \"$1\",\n";
  }
}
print OUTFILE "};\n\n";

close INFILE;
open (INFILE, "entities.txt") or die "Cannot open entities.txt: $!";

print OUTFILE "int ENTITY_DEF[] = {\n";

$count = 0;

while (<INFILE>)
{
  if (/^Name: (\S+)\s+Value: (\d+)$/)
  {
    print OUTFILE "  $2,\n";

    $count++;
  }
}

print OUTFILE "};\n\n";

print OUTFILE "#define NUM_MXP_ENTITIES $count\n\n";

print OUTFILE "#endif\n";

close INFILE;
close OUTFILE;

