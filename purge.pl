#!/usr/bin/perl -w
# This is a crude script to remove spam in TWiki installations.
# Call it like this from your twiki/data/<WEB> directory.
# 
# 	~/purge.pl LiYan
#
# Copyleft (L) 2005 Joachim Nilsson <jocke()vmlinux!org>
#

$user=$ARGV[0];
$list=`find . -name '*.txt' |xargs grep 'author="$user"'`;
@files=split(/\n/, $list);

foreach (@files)
{
    if (/^(.*):%META.*version="([0-9]+)\.([0-9]+)".*$/)
    {
	$file  = $1;
	$major = $2;
	$minor = $3;
	$rev   = "$2.$3";

	if ($3 == 1)
	{
	    print "No way, crap in rev 1.1 of $file, remove it (y/N)? ";
	    read (STDIN, $yorn, 2);
	    $| = 1;
	    chomp ($yorn);
	    if ($yorn =~ /^[yY].*/)
	    {
		@goners = ($file, "$file,v");
		print "\tDeleting the following files: @goners\n";
		unlink @goners;
	    }
	    else
	    {
		print "\tKeeping $file\n";
	    }
	}
	else
	{
	    $minor--;
	    print "Revert $file:$rev to RCS rev $major.$minor (y/N)? ";
	    read (STDIN, $yorn, 2);
	    $| = 1;
	    chomp ($yorn);
	    if ($yorn =~ /^[yY].*/)
	    {
		system("co -q -f -r$major.$minor $file");
		system("ci -q -l -m\"Reverted to older version due to spam. /Jocke\" $file");
		print "\tReverted $file to RCS version $major.$minor\n";
	    }
	    else
	    {
		print "\tKeeping RCS version $rev of $file\n";
	    }
	}
    }
}


