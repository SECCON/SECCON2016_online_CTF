#!/usr/bin/perl

use strict;
use warnings;
use utf8;
use DBI;

my $dbh = DBI->connect( 'DBI:mysql:keiba:localhost', 'keiba_ro', '44d8156302d2308bdb8ef2b30f7b68ba', {mysql_enable_utf8 => 1} );

sleep 1;

my $USER = <STDIN> || exit(2);
my $PASS = <STDIN> || exit(3);
chomp $USER;
chomp $PASS;

print STDERR "$USER:$PASS\n";
$USER = decode('utf8', $USER);
$PASS = decode('utf8', $PASS);

# SQLi
my $sql = "SELECT * FROM `☹☺☻` WHERE name='$USER' AND pass='$PASS'";
my @ret = $dbh->selectrow_array($sql);
if(@ret){
	exit(0);
} else {
	exit(1);
}
