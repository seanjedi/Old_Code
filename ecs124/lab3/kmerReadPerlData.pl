# Program kmerfirst.pl
# This program finds all the overlapping k-mers of the input string. It builds
# an associative array where each key is one distinct k-mer in the string,
# and the associated value is the starting position where that
#k-mer is FIRST found.  Compare this to kmer2.pl

#print "Input the name of the file\n";
#$file = <>;
#chomp $file;
open In2, 'perlblastdata.txt';
open Out, ">kmerReadPerlData.txt";
print "Input the length of the window\n"; 
$k = <>;
chomp $k;

while($dna = <In2>){
chomp $dna;
%kmer = ();                      # This initializes the hash called kmer.
$i = 1;
	while (length($dna) >= $k) {
  	$dna =~ m/(.{$k})/; 
 	 print Out "$1, $i \n";
   	if (! defined $kmer{$1}) {     #defined is a function that returns true
                                  # if a value has already been assigned to
                                  # $kmer{$1}, otherwise it returns false. 
                                  # the ! character is the negation, so
                                  # if $kmer{$1} has no value, then it will
                                  # be assigned the value of $i, the position
                                  # where the k-mer is first found.
    $kmer{$1} = $i;       
   }
 $i++;
  $dna = substr($dna, 1, length($dna) -1);
}

foreach $kmerkey (keys(%kmer)) {
 print Out "The first occurrence of string $kmerkey is in position 
 $kmer{$kmerkey}\n";
	}
}
close(In2);
close(Out);
