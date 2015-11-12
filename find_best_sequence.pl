use strict;
use warnings;

use Data::Dumper;

package QuerySequence;

#~ my $error_threshold = shift;
#~ my $blocs = shift;

sub test_queries {
	my $errors = shift;
	my $queries = shift;
	foreach my $q (@$queries) {
		if (!defined $errors->{$q->[0]} && !defined $errors->{$q->[1]}) {
			return 1;
		}
	}
	return 0;
}

sub find_with_queries {
	my $error_threshold = shift;
	my $block_count = shift;
	my $queries = shift;
	if ($error_threshold == 1) {
		for (my $i0 = 0; $i0 < $block_count; $i0++) {
			if (test_queries({$i0 => 1}, $queries) == 0) {
				#~ print "Not found with $i0\n";
				return 0;
			}
		}
	}
	elsif ($error_threshold == 2) {
		for (my $i0 = 0; $i0 < $block_count; $i0++) {
			for (my $i1 = $i0+1; $i1 < $block_count; $i1++) {
				if (test_queries({$i0 => 1, $i1 => 1}, $queries) == 0) {
					#~ print "Not found with errors on ($i0, $i1)\n";
					return 0;
				}
			}
		}
	}
	elsif ($error_threshold == 3) {
		for (my $i0 = 0; $i0 < $block_count; $i0++) {
			for (my $i1 = $i0+1; $i1 < $block_count; $i1++) {
				for (my $i2 = $i1+1; $i2 < $block_count; $i2++) {
					if (test_queries({$i0 => 1, $i1 => 1, $i2 => 1}, $queries) == 0) {
						#~ print "Not found with errors on ($i0, $i1, $i2)\n";
						return 0;
					}
				}
			}
		}
	}
	return 1;
}

sub generate_all_possible_queries {
	my $block_count = shift;
	my @queries = ();
	for (my $i0 = 0; $i0 < $block_count; $i0++) {
		for (my $i1 = $i0+1; $i1 < $block_count; $i1++) {
			push @queries, [$i0, $i1];
		}
	}
	return \@queries;
}

sub query_to_string {
	my $queries = shift;
	my $str = '';
	foreach my $q (@$queries) {
		$str .= $q->[0].','.$q->[1].';';
	}
	return $str;
}


sub query_to_pretty_string {
	my $queries = shift;
	my $str = '';
	foreach my $q (@$queries) {
		$str .= ',('.$q->[0].', '.$q->[1].')';
	}
	if (length $str) {
		return substr($str, 1);
	}
	return $str;
}

sub find_min_query_set {
	my $error_threshold = shift;
	my $block_count = shift;
	my $queries = shift;
	my $dyn_programming_matrix = shift;
	if (find_with_queries($error_threshold, $block_count, $queries) == 0) {
		return [];
	}
	my @results = ($queries);
	my %result_set = (query_to_string($queries) => 1);
	
	for (my $i = 0; $i < scalar @$queries; $i++) {
		my @sub_queries = (@{$queries});
		splice(@sub_queries, $i, 1);
		my $sub_query_str = query_to_string(\@sub_queries);
		my $sub_result = undef;
		if (defined $dyn_programming_matrix->{$sub_query_str}) {
			$sub_result = $dyn_programming_matrix->{$sub_query_str};
		}
		else {
			$sub_result = find_min_query_set($error_threshold, $block_count, \@sub_queries, $dyn_programming_matrix);
			$dyn_programming_matrix->{$sub_query_str} = $sub_result;
		}
		foreach my $query_res (@$sub_result) {
			my $key = query_to_string($query_res);
			if (defined $result_set{$key}) {
				next;
			}
			if (scalar @{$results[0]} == scalar @{$query_res}) {
				push @results, $query_res;
				$result_set{$key} = 1;
			}
			elsif (scalar @{$results[0]} > scalar @{$query_res}) {
				@results = ($query_res);
				%result_set = ($key => 1);
			}
		}
	}
	return \@results;
}

sub display_result {
	my $results = shift;
	print scalar @$results, " results found!\n";
	foreach my $query (@$results) {
		print query_to_pretty_string($query), "\n";
	}
}

#~ Test query order

#~ my $query = [[0,1], [1,2], [3, 4], [0,2], [0,3], [0,5],  [0,6]];
#~ print find_with_queries(1, [[2,3], [0, 1]]);
#~ my $results = find_min_query_set(2, $query, {});
#~ my $results = find_min_query_set($error_threshold, 5, generate_all_possible_queries(5), {});
#~ display_result($results);

1;
