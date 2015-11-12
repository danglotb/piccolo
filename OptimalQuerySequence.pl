use strict;
use warnings;

use Data::Dumper;

use List::Util qw(first max maxstr min minstr reduce shuffle sum);

use BlockMeta;

use QuerySequence;
	
#~  Returns 1 if A is contained in B
sub is_contained {
	my $hashA = shift;
	my $hashB = shift;
	
	foreach my $kA (keys %{$hashA}) {
		if (!defined $hashB->{$kA}) {
			return 0;
		}
	}
	return 1;
}

sub find_query_in_array_query_seq {
	my $query_hash = shift;
	my $error = shift;
	my $array_of_query_seq_hash_per_error = shift;
	if ($error == scalar @$array_of_query_seq_hash_per_error) {
		return [{'current' => $query_hash, 'next' => undef}];
	}
	my $array_of_query_seq_hash = $array_of_query_seq_hash_per_error->[$error];
	my @min_size_results = ();
	my $min_size = -1;
	foreach my $query_seq_hash (@$array_of_query_seq_hash) {
		if (is_contained($query_hash, $query_seq_hash)) {
			my $result = find_query_in_array_query_seq($query_seq_hash, $error+1, $array_of_query_seq_hash_per_error);
			if (scalar @$result > 0) {
				if ($min_size == -1) {
					push @min_size_results, {'current' => $query_hash, 'next' => $result};
					$min_size = scalar keys %{$result->[0]{'current'}};
				}
				elsif (scalar keys %{$result->[0]{'current'}} < $min_size) {
					@min_size_results = ({'current' => $query_hash, 'next' => $result});
					$min_size = scalar keys %{$result->[0]{'current'}};
				}
				elsif (scalar keys %{$result->[0]{'current'}} == $min_size) {
					push @min_size_results, {'current' => $query_hash, 'next' => $result};
				}
			}
		}
	}
	return \@min_size_results;
}

sub create_optimal_result_display {
	my $seq = shift;
	my $exclude = shift;
	
	if (!defined $seq) {
		return [];
	}
	
	my @str_result = ();
	
	foreach my $current_seq (@$seq) {
	
		my @result = ();
		foreach my $k (keys %{$current_seq->{'current'}}) {
			if (!defined $exclude->{$k}) {
				push @result, $k;
			}
		}
		
		if (defined $current_seq->{'next'}) {
			my $subResult = create_optimal_result_display($current_seq->{'next'}, $current_seq->{'current'});
			foreach my $r (@$subResult) {
				push @str_result, [@result, '_', @$r];
			}
		}
		else {
			push @str_result, \@result;
		}
		
	}
	
	return \@str_result;
}

sub display_optimal_result {
	my $optimal_result = shift;
	my $display = create_optimal_result_display($optimal_result, {});
	
	
	foreach my $seq (@$display) {
		my $error = -1;
		my @id_error = ();
		my $i = 0;
		print "# ========================================\n";
		print "# Display sequence\n";
		print "# ========================================\n";
		foreach my $q (@$seq) {
			if ($q eq '_') {
				$error++;
				push @id_error, $i;
				print "# For $error error(s)\n";
			}
			else {
				$i++;
				print "$q\n";
			}
		}
		print "# Error increases at: ", join(', ', @id_error), "\n";
	}
}

sub display_optimal_result_cpp {
	my $optimal_result = shift;
	my $blockMeta = shift;
	my $error_threshold = $blockMeta->{'error_threshold'};
	my $display = create_optimal_result_display($optimal_result, {});
	
	print "// Prints every optimal query sequence. You only need to paste one in the constructor of OptimalQuerySequence\n\n";
	
	foreach my $seq (@$display) {
		my $error = -1;
		my @id_error = ();
		my $i = 0;
		print "// ===============================================================\n";
		print "// Display sequence (In constructor of OptimalQuerySequence)\n";
		print "// ===============================================================\n";
		print "static_assert(std::is_same<BLOCK_SIZES, ", $blockMeta->cpp_display(), ">::value, ",
		"\"The blocks and the optimal query sequence don't match. Please run OptimalQuerySequence.pl and paste the result here.\");\n";
		print 'm_queries.reserve(', (scalar @$seq) - $error_threshold-1, ");\n";
		foreach my $q (@$seq) {
			if ($q eq '_') {
				$error++;
				push @id_error, $i;
				print "// For $error error(s)\n";
			}
			else {
				$i++;
				$q =~ s/(\d+),(\d+):(\d+),(\d+)/$1, $2, $3, $4/;
				print "m_queries.emplace_back($q);\n";
			}
		}
		print "// Error increases at: ", join(', ', @id_error), "\n";
		foreach my $id (@id_error) {
			next if $id == 0;
			print "m_querieEnds.push_back(m_queries.begin()+$id);\n";
		}
		print "m_querieEnds.push_back(m_queries.end());\n";
	}
}

sub find_optimal_query_sequence {
	my $blockMeta = shift;
	
	my @optimal_block_queries_per_error = ();
	
	for (my $error_threshold = 0; $error_threshold <= $blockMeta->{'error_threshold'}; $error_threshold++) {
		my $optimal_queries = QuerySequence::find_optimal_query_set($error_threshold, $blockMeta->block_count);
		
		my @optimal_block_queries = ();
		
		foreach my $query_sequence (@$optimal_queries) {
			my %block_query_seq = ();
			foreach my $q (@$query_sequence) {
				my $block_queries = $blockMeta->generate_block_pairs($q->[0], $q->[1], $error_threshold);
				
				foreach my $bq (keys %{$block_queries}) {
					$block_query_seq{"$q->[0],$q->[1]:$bq"} = $block_queries->{$bq};
				}
			}
			push @optimal_block_queries, \%block_query_seq;
		}
		
		@optimal_block_queries = sort {scalar keys %{$a} <=> scalar keys %{$b}} @optimal_block_queries;
		
		push @optimal_block_queries_per_error, \@optimal_block_queries;
	}
	
	return find_query_in_array_query_seq({}, 0, \@optimal_block_queries_per_error);
}

my $block_str = shift;
my @blocks = split(':', $block_str);

my $error_threshold = shift;

if (!defined $error_threshold) {
	print "\nReturns every optimal sequence of queries for the given blocks and errors.\n\n\t",
	"Usage: OptimalQuerySequence.pl B1:B2:B3:B4:B5 error\n",
	"\t(block sizes separated by ':', followed by the error threshold).\n\n",
	"\tExample: OptimalQuerySequence.pl 4:4:4:4:6 3\nFinds optimal queries for 5 blocks, ",
	"the first 4 blocks of size 4nt, and the 5th of size 6nt, with 3 errors at most.\n";
	die;
}

my $blockMeta = BlockMeta->new(\@blocks, $error_threshold);
my $optimal = QuerySequence::find_optimal_query_set($error_threshold, $blockMeta->block_count);
QuerySequence::display_result($optimal);
#~ my $optimal = find_optimal_query_sequence($blockMeta);
#~ print Dumper($optimal);

#~ display_optimal_result_cpp($optimal, $blockMeta);

