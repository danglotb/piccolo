use strict;
use warnings;

use Data::Dumper;

for (my $a = 0; $a < 4; $a++) {
	for (my $b = $a+1; $b < 4; $b++) {
		print "($a, $b) -> ", (7-$a)*($a)/2 + $b-$a-1, "\n";
		print "($a, $b) -> ", $a+$b-($a-1)*($a-2)/2, "\n";
	}
}
