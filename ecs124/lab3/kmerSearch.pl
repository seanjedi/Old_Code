#!/pkg/bin/perl -w
# Program kmerfirst.pl
# This program finds all the overlapping k-mers of the input string. It builds
# an associative array where each key is one distinct k-mer in the string,
# and the associated value is the starting position where that
#k-mer is FIRST found.  Compare this to kmer2.pl

open (IN, "query.txt");
open Out, ">kmerSearchOut.txt";
$dna = <IN>;
$temp = $dna;
close IN;

@qarray = split(//, $dna);

# print "Input the string\n";
# $dna = <>;
chomp $dna;
$k = 4;
chomp $k;

%kmer = ();                      # This initializes the hash called kmer.
$i = 1;
while (length($dna) >= $k) {
  $dna =~ m/(.{$k})/; 
 # print "$1, $i \n";
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
  $dna = substr($dna, 1, length($dna) - 1);
}

# foreach $kmerkey (keys(%kmer)) {
#  print "The first occurrence of string $kmerkey is in position 
#  $kmer{$kmerkey}\n";
# }

open (IN, "perlblastdata.txt");

while ($string = <IN>) {
   chomp $string;

   %stringkmer = ();
   $i = 1;
   $dna = $temp;
    @sarray = split(//, $string);

  while (length($string) >= $k) {


  $string =~ m/(.{$k})/; 
  # print "$1, $i \n";
   if (! defined $stringkmer{$1}) {    #defined is a function that returns true
                                  		# if a value has already been assigned to
                                  		# $kmer{$1}, otherwise it returns false. 
                                  		# the ! character is the negation, so
                                  		# if $kmer{$1} has no value, then it will
                                  		# be assigned the value of $i, the position
                                  		# where the k-mer is first found.
    $index = 0;
    $stringkmer{$1} = $i;     
    if (defined $kmer{$1}) {

      $matched = $1;
    	print Out "Found a match from S in Q!\n";

      while ($qarray[$kmer{$1} + $index - 1] eq $sarray[$stringkmer{$1} + $index - 1]) {
        $index++;
 #       print "$1\n";
  #      print "$qarray[$kmer{$1} + $index - 2]\n";
      }

      printf Out "Longest right match is: %d\n", $index;

      if (($index) > 5) {
        printf Out "Found a good HSP to the right of length: %d\n", $index;
      }

      $index = 0;
      $matched = $1;

      while ($qarray[$kmer{$1} - $index -1] eq $sarray[$stringkmer{$1} - $index -1]) {
        $index++;
   #     print "$1\n";
    #    print "$qarray[$kmer{$1} + $index - 2]\n";
      }

      printf Out "Longest left match is: %d\n", $k + $index -1;

      if (($k + $index) > 6) {
        printf Out "Found a good HSP to the left of length: %d\n", $k + $index -1;
      }
     } 
   }
  $i++;
  $string = substr($string, 1, length($string) - 1);
  } 
}


close IN;
close Out;
