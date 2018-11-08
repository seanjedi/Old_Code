print "Input string:\n";
$line = <>;
if($line =~ m/(TTGACA[ATCG]{18}TATATT)|(TTTACA[ATCG]{18}TATGTT)/)
{
	print "Found!\n";
}
else
{
	print "Not Found!\n";
}
