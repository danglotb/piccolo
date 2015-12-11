#ifndef OPTIMALQUERYSEQUENCETEMPLATE_H
#define OPTIMALQUERYSEQUENCETEMPLATE_H

#include "VariadicList.h"
#include <algorithm>

namespace meta_prog {

template <unsigned int BlockA, unsigned int BlockB>
struct BlockQuery {

		static constexpr unsigned int blockA = BlockA;
		static constexpr unsigned int blockB = BlockB;

		static_assert(BlockA < BlockB, "Invalid block query");

		static constexpr unsigned int allowedInbetweenErrors = BlockB - BlockA - 1;

};

namespace priv {

template <class T>
constexpr T const& min(T const& a, T const& b) {
	return a > b ? b : a;
}
template <class T>
constexpr T const& max(T const& a, T const& b) {
	return a < b ? b : a;
}

template <class, class>
struct query_comp;

template <unsigned int BlockA_1, unsigned int BlockB_1, unsigned int BlockA_2, unsigned int BlockB_2>
struct query_comp<BlockQuery<BlockA_1, BlockB_1>, BlockQuery<BlockA_2, BlockB_2>> : std::integral_constant<int,
		(BlockA_1 < BlockA_2) || (BlockA_1 == BlockA_2 && BlockB_1 < BlockB_2) ? -1 :
		(BlockA_1 == BlockA_2 && BlockB_1 == BlockB_2) ? 0 : 1> {};
}

template <class...> // Should contain BlockQuery structures only
struct QuerySequence {};

namespace priv {

template <class, class>
struct query_sequence_comp;

template <class BlockQuery1, class BlockQuery2, class... BlockQueries1, class... BlockQueries2>
struct query_sequence_comp<QuerySequence<BlockQuery1, BlockQueries1...>, QuerySequence<BlockQuery2, BlockQueries2...>> : std::integral_constant<bool,
		query_comp<BlockQuery1, BlockQuery2>::value == -1 ? false :
		query_comp<BlockQuery1, BlockQuery2>::value == 1 ? true :
		query_sequence_comp<QuerySequence<BlockQueries1...>, QuerySequence<BlockQueries2...>>::value> {};

template <class BlockQuery1, class BlockQuery2>
struct query_sequence_comp<QuerySequence<BlockQuery1>, QuerySequence<BlockQuery2>> : std::integral_constant<bool,
		(query_comp<BlockQuery1, BlockQuery2>::value > 0)> {};

template <>
struct query_sequence_comp<QuerySequence<>, QuerySequence<>> : std::integral_constant<bool, false> {};
}

template <class BlockQuery_t, unsigned int Error>
struct OffsetBlockQueryAmount : std::integral_constant<unsigned int,
		1u + 2*priv::min(BlockQuery_t::allowedInbetweenErrors, Error)> {};

template <class...> // Should contain QuerySequence structures only
struct SetOfQuerySequence {};

/// Should contain QuerySequence structures only
/// The first parameter is the query sequence to detect miRNA with 0 errors
/// The second parameter is the query sequence to detect miRNA with 1 error
/// ...
/// The n-th parameter is the query sequence to detect miRNA with n errors
/// This list of query sequences must form a partition
template <class...>
struct OptimalQuerySequence {};

template <class OptimalQuerySequence_t>
struct merge_optimal_query_sequence;
template <class... QuerySequences_t>
struct merge_optimal_query_sequence<OptimalQuerySequence<QuerySequences_t...>> {
	typedef typename vlist::concat<QuerySequences_t...>::type type;
};
template <>
struct merge_optimal_query_sequence<OptimalQuerySequence<>> {
	typedef QuerySequence<> type;
};

template <class OptimalQuerySequence_t>
struct score_optimal_query_sequence;

namespace priv {

template <class QuerySequence_t, unsigned int Error>
struct score_query_sequence_helper;
template <class BlockQuery_t, unsigned int Error, class ...BlockQueries_t>
struct score_query_sequence_helper<QuerySequence<BlockQuery_t, BlockQueries_t...>, Error> : std::integral_constant<unsigned int,
		OffsetBlockQueryAmount<BlockQuery_t, Error>::value + score_query_sequence_helper<QuerySequence<BlockQueries_t...>, Error>::value> {};
template <unsigned int Error>
struct score_query_sequence_helper<QuerySequence<>, Error> : std::integral_constant<unsigned int, 0u> {};

template <class OptimalQuerySequence_t, unsigned int ErrorCount>
struct score_optimal_query_sequence_helper;

template <class QuerySequence_t, unsigned int ErrorCount, class ...QuerySequences_t>
struct score_optimal_query_sequence_helper<OptimalQuerySequence<QuerySequence_t, QuerySequences_t...>, ErrorCount> : std::integral_constant<unsigned int,
		score_query_sequence_helper<QuerySequence_t, ErrorCount>::value +
		score_optimal_query_sequence_helper<OptimalQuerySequence<QuerySequences_t...>, ErrorCount>::value> {};
template <unsigned int ErrorCount>
struct score_optimal_query_sequence_helper<OptimalQuerySequence<>, ErrorCount> : std::integral_constant<unsigned int, 0u> {};

}

template <class OptimalQuerySequence_t>
struct score_optimal_query_sequence : priv::score_optimal_query_sequence_helper<OptimalQuerySequence_t,
		vlist::size<OptimalQuerySequence_t>::value-1> {};
template <>
struct score_optimal_query_sequence<OptimalQuerySequence<>> : std::integral_constant<unsigned int, 0u> {};

// ===================================================================================
//		Generate sequence that query every pair of blocks
// ===================================================================================

namespace priv {

template <int I, int J, int B, class query_seq>
struct longuest_sequence_inner_loop {
		typedef typename vlist::concat<query_seq, QuerySequence<BlockQuery<I, J>>>::type new_query_seq;

		typedef typename longuest_sequence_inner_loop<I, J+1, B, new_query_seq>::type type;
};

template <int I, int B, class query_seq>
struct longuest_sequence_inner_loop<I, B, B, query_seq> {
		typedef query_seq type;
};

template <int I, int B, class query_seq>
struct longuest_sequence_loop {
		typedef typename longuest_sequence_inner_loop<I, I+1, B, query_seq>::type new_query_seq;

		typedef typename longuest_sequence_loop<I+1, B, new_query_seq>::type type;

};
template <int B, class query_seq>
struct longuest_sequence_loop<B, B, query_seq> {
		typedef query_seq type;
};

}

template <unsigned int BlockCount>
struct longuest_sequence {

		typedef typename priv::longuest_sequence_loop<0, BlockCount, QuerySequence<>>::type type;

};

// ===================================================================================
//		Returns all possible error distributions
// ===================================================================================

template <unsigned int ErrorId>
struct Error : std::integral_constant<unsigned int, ErrorId> {};

template <class... Args>
class ErrorDistribution {};
template <class... Args>
class ErrorDistributionList {};

namespace priv {

template <unsigned int... ErrorId>
struct integral_list {};

template <unsigned int NestedLoopCount, bool LoopContinues>
struct generate_errors_loop;

template <unsigned int NestedLoopCount>
struct generate_errors_loop<NestedLoopCount, true> {
		template <class error_seq, unsigned int BlockCount, unsigned int InnermostId, unsigned int... Ids>
		struct run_t {
				typedef typename generate_errors_loop<NestedLoopCount-1, (min(InnermostId+1, BlockCount) < BlockCount)>::
				template run_t<error_seq, BlockCount, min(InnermostId+1, BlockCount), InnermostId, Ids...>::type this_inner_loop_result;

				typedef typename generate_errors_loop<NestedLoopCount, (InnermostId+1 < BlockCount)>::
				template run_t<error_seq, BlockCount, InnermostId+1, Ids...>::type this_loop_result;

				typedef typename vlist::concat<this_inner_loop_result, this_loop_result>::type type;
		};
		template <class error_seq, unsigned int BlockCount>
		struct init_t {
				typedef typename generate_errors_loop<NestedLoopCount-1, true>::template run_t<error_seq, BlockCount, 0u>::type type;
		};
};
template <unsigned int NestedLoopCount>
struct generate_errors_loop<NestedLoopCount, false> {
		template <class error_seq, unsigned int BlockCount, unsigned int InnermostId, unsigned int... Ids>
		struct run_t {
				typedef error_seq type;
		};
		template <class error_seq, unsigned int BlockCount>
		struct init_t {
				typedef error_seq type;
		};
};
template <>
struct generate_errors_loop<0u, true> {
		template <class error_seq, unsigned int BlockCount, unsigned int InnermostId, unsigned int... Ids>
		struct run_t {
				typedef typename vlist::concat<error_seq, ErrorDistributionList<ErrorDistribution<Error<InnermostId>, Error<Ids>...>>>::type next_it_error_seq;
				typedef typename generate_errors_loop<0u, (InnermostId+1 < BlockCount)>::template run_t<next_it_error_seq, BlockCount, InnermostId+1, Ids...>::type type;
		};
		template <class error_seq, unsigned int BlockCount>
		struct init_t {
				typedef error_seq type;
		};
};
template <>
struct generate_errors_loop<0u, false> {
		template <class error_seq, unsigned int BlockCount, unsigned int InnermostId, unsigned int... Ids>
		struct run_t {
				typedef error_seq type;
		};
		template <class error_seq, unsigned int BlockCount>
		struct init_t {
				typedef error_seq type;
		};
};

}

template <unsigned int ErrorCount, unsigned int BlockCount>
struct generate_errors {
	typedef typename priv::generate_errors_loop<ErrorCount, (BlockCount > 0)>::template init_t<ErrorDistributionList<>, BlockCount>::type type;
};

// ===================================================================================
//		Tests if a query sequence is complete for a given ErrorDistribution
// ===================================================================================

template <class QuerySequence_t, class ErrorDistribution_t>
struct check_query_sequence_against_error_distribution {
		template <class BlockQuery_t>
		struct predicate : std::integral_constant<bool, !vlist::contains<ErrorDistribution_t, Error<BlockQuery_t::blockA>>::value &&
				!vlist::contains<ErrorDistribution_t, Error<BlockQuery_t::blockB>>::value> {};

		static constexpr bool value = vlist::any_of<QuerySequence_t, predicate>::value;
};

// ===================================================================================
//		Tests if a query sequence is complete for a given ErrorDistributionList
// ===================================================================================

template <class QuerySequence_t, class ErrorDistributionList_t>
struct check_query_sequence {
		template <class ErrorDistribution_t>
		struct predicate : std::integral_constant<bool, check_query_sequence_against_error_distribution<QuerySequence_t, ErrorDistribution_t>::value> {};

		static constexpr bool value = vlist::all_of<ErrorDistributionList_t, predicate>::value;
};

// ===================================================================================
//		Finds the minimum query sequence for a given ErrorCount
// ===================================================================================

template <unsigned int ErrorCount, unsigned int BlockCount>
struct compute_minimum_query_sequence;

namespace priv {

template <unsigned int ErrorCount, unsigned int BlockCount, class QuerySequence_t,
		  bool isValid = check_query_sequence<QuerySequence_t, typename generate_errors<ErrorCount, BlockCount>::type>::value>
struct find_min_query_set;

template <unsigned int ErrorCount, unsigned int BlockCount, class QuerySequence_t>
struct find_min_query_set<ErrorCount, BlockCount, QuerySequence_t, false> {
		typedef SetOfQuerySequence<> type;
		static constexpr unsigned int optimal_query_sequence_size = 0u;
};

template <class MinSetOfQuerySequence_t, bool remove_duplicates = false>
struct find_min_query_set_loop_updated_result_helper {
		typedef MinSetOfQuerySequence_t type;
};
template <class MinSetOfQuerySequence_t>
struct find_min_query_set_loop_updated_result_helper<MinSetOfQuerySequence_t, true> {
		typedef typename vlist::remove_duplicates<MinSetOfQuerySequence_t, query_sequence_comp>::type type;
};

template <class MinSetOfQuerySequence_t, unsigned int MinQuerySequenceSize, class NewSetOfQuerySequence_t, unsigned int NewQuerySequenceSize>
struct find_min_query_set_loop_updated_result {
	private:
		typedef typename vlist::concat<MinSetOfQuerySequence_t, NewSetOfQuerySequence_t>::type concatenated_result;
//		typedef typename vlist::remove_duplicates<concatenated_result, query_sequence_comp>::type merged_result;

		typedef vlist::variadic_list<MinSetOfQuerySequence_t, // We don't update
		NewSetOfQuerySequence_t, // We replace it by the new set
		concatenated_result // We merge the two because they have the same size
		> PossibleOutcomes;

		static constexpr unsigned int outcome_id = MinQuerySequenceSize == NewQuerySequenceSize ? 2u :
																(NewQuerySequenceSize < MinQuerySequenceSize && NewQuerySequenceSize != 0u) ? 1u : 0u;
		typedef typename vlist::get_at<PossibleOutcomes, outcome_id>::type Outcome;
	public:
		typedef typename find_min_query_set_loop_updated_result_helper<Outcome, outcome_id == 2>::type type;
		static constexpr unsigned int optimal_query_sequence_size = (MinQuerySequenceSize == NewQuerySequenceSize ||
				(NewQuerySequenceSize < MinQuerySequenceSize && NewQuerySequenceSize != 0u)) ? NewQuerySequenceSize : MinQuerySequenceSize;
};

template <unsigned int ErrorCount, unsigned int BlockCount, class QuerySequence_t, unsigned int Iteration, unsigned int IterationEnd,
		  class MinSetOfQuerySequence_t, unsigned int MinQuerySequenceSize>
struct find_min_query_set_loop {
	private:
		typedef typename vlist::remove_at<QuerySequence_t, Iteration>::type sub_query_sequence;

		typedef find_min_query_set<ErrorCount, BlockCount, sub_query_sequence> Recursion;
		typedef typename Recursion::type NewSetOfQuerySequence_t;
		static constexpr unsigned int NewQuerySequenceSize = Recursion::optimal_query_sequence_size;


		typedef find_min_query_set_loop_updated_result<MinSetOfQuerySequence_t, MinQuerySequenceSize, NewSetOfQuerySequence_t, NewQuerySequenceSize> Update;

		typedef typename Update::type NewMinSetOfQuerySequence_t;
		static constexpr unsigned int new_optimal_query_sequence_size = Update::optimal_query_sequence_size;

		typedef find_min_query_set_loop<ErrorCount, BlockCount, QuerySequence_t, Iteration+1, IterationEnd,
		NewMinSetOfQuerySequence_t, new_optimal_query_sequence_size> NextIteration;
	public:
		typedef typename NextIteration::type type;
		static constexpr unsigned int optimal_query_sequence_size = NextIteration::optimal_query_sequence_size;
};

template <unsigned int ErrorCount, unsigned int BlockCount, class QuerySequence_t, unsigned int IterationEnd,
		  class MinSetOfQuerySequence_t, unsigned int MinQuerySequenceSize>
struct find_min_query_set_loop<ErrorCount, BlockCount, QuerySequence_t, IterationEnd, IterationEnd, MinSetOfQuerySequence_t, MinQuerySequenceSize> {

		typedef MinSetOfQuerySequence_t type;
		static constexpr unsigned int optimal_query_sequence_size = MinQuerySequenceSize;
};

template <unsigned int ErrorCount, unsigned int BlockCount, class QuerySequence_t>
struct find_min_query_set<ErrorCount, BlockCount, QuerySequence_t, true> {
	private:
		typedef find_min_query_set_loop<ErrorCount, BlockCount, QuerySequence_t, 0u, vlist::size<QuerySequence_t>::value,
		SetOfQuerySequence<QuerySequence_t>, vlist::size<QuerySequence_t>::value> result;
	public:
		typedef typename result::type type;
		static constexpr unsigned int optimal_query_sequence_size = result::optimal_query_sequence_size;

};

// For 0 errors
template <class SetOfQuerySequence_t, class QuerySequence_t>
struct compute_minimum_query_sequence_0_helper;

template <class SetOfQuerySequence_t, class BlockQuery_t, class... BlockQueries>
struct compute_minimum_query_sequence_0_helper<SetOfQuerySequence_t, QuerySequence<BlockQuery_t, BlockQueries...>> {
	typedef typename compute_minimum_query_sequence_0_helper<
		typename vlist::concat<SetOfQuerySequence_t, SetOfQuerySequence<QuerySequence<BlockQuery_t>>>::type,
		QuerySequence<BlockQueries...>
		>::type type;
};
template <class SetOfQuerySequence_t>
struct compute_minimum_query_sequence_0_helper<SetOfQuerySequence_t, QuerySequence<>> {
	typedef SetOfQuerySequence_t type;
};
}

template <unsigned int ErrorCount, unsigned int BlockCount>
struct compute_minimum_query_sequence {
	private:
		typedef priv::find_min_query_set<ErrorCount, BlockCount, typename longuest_sequence<BlockCount>::type> result;
	public:
		typedef typename result::type type;
		static constexpr unsigned int optimal_query_sequence_size = result::optimal_query_sequence_size;
};

template <unsigned int BlockCount>
struct compute_minimum_query_sequence<0, BlockCount> {
	private:
		typedef typename longuest_sequence<BlockCount>::type sub_result;

	public:
		typedef typename priv::compute_minimum_query_sequence_0_helper<SetOfQuerySequence<>, sub_result>::type type;
		static constexpr unsigned int optimal_query_sequence_size = vlist::size<sub_result>::value;
};

// =============================================================================================================
//		Finds ONE optimal query sequence with early stops to detect miRNA with up to ErrorCount errors
// =============================================================================================================

template <unsigned int ErrorCount, unsigned int BlockCount>
struct compute_optimal_query_sequence;

namespace priv {

template <class list_of_min_query_seq, unsigned int Error, unsigned int ErrorCount, unsigned int BlockCount>
struct build_list_of_min_query_for_errors {
	private:
		typedef typename compute_minimum_query_sequence<Error, BlockCount>::type this_result;
	public:
		typedef typename build_list_of_min_query_for_errors<
			typename vlist::concat<list_of_min_query_seq, vlist::variadic_list<this_result>>::type,
			Error+1,
			ErrorCount,
			BlockCount
		>::type type;
};
template <class list_of_min_query_seq, unsigned int ErrorCount, unsigned int BlockCount>
struct build_list_of_min_query_for_errors<list_of_min_query_seq, ErrorCount, ErrorCount, BlockCount> {
	private:
		typedef typename compute_minimum_query_sequence<ErrorCount, BlockCount>::type this_result;
	public:
		typedef typename vlist::concat<list_of_min_query_seq, vlist::variadic_list<this_result>>::type type;
};

template <class OptimalQuerySequence_t, class PreviouslyFoundOptimalQuerySequence_t>
struct optimal_query_sequence_update_min {
	private:
		static constexpr bool PreviouslyFoundOptimalQuerySequence_IsEmpty = std::is_same<PreviouslyFoundOptimalQuerySequence_t, OptimalQuerySequence<>>::value;
		static constexpr unsigned int new_score = score_optimal_query_sequence<OptimalQuerySequence_t>::value;
		static constexpr unsigned int old_score = score_optimal_query_sequence<PreviouslyFoundOptimalQuerySequence_t>::value;
		static constexpr bool should_update = new_score < old_score || PreviouslyFoundOptimalQuerySequence_IsEmpty;
	public:
		typedef typename std::conditional<should_update, OptimalQuerySequence_t, PreviouslyFoundOptimalQuerySequence_t>::type type;
};

template <unsigned int Error, unsigned int ErrorCount, unsigned int BlockCount>
struct optimal_query_sequence_helper {

		typedef typename build_list_of_min_query_for_errors<vlist::variadic_list<>, 0u, ErrorCount, BlockCount>::type ListOfSetOfMinQuerySequence_t;
		typedef typename vlist::get_at<ListOfSetOfMinQuerySequence_t, Error>::type SetOfMinQuerySequence_Error_t;

		template <class OptimalQuerySequence_t, class FoundOptimalQuerySequence_t, class QuerySequence_t,
				  bool = vlist::includes<QuerySequence_t, typename merge_optimal_query_sequence<OptimalQuerySequence_t>::type>::value>
		struct main_loop_if_contained_t {
			private:
				typedef typename vlist::difference<QuerySequence_t,
												   typename merge_optimal_query_sequence<OptimalQuerySequence_t>::type
												  >::type TruncatedQuerySeq;
				typedef typename vlist::concat<OptimalQuerySequence_t, OptimalQuerySequence<TruncatedQuerySeq>>::type new_optimal_query_sequence_t;
			public:
				typedef typename optimal_query_sequence_helper<Error+1, ErrorCount, BlockCount>::
						template init_main_loop_t<new_optimal_query_sequence_t, FoundOptimalQuerySequence_t>::type type; // Best of FoundOptimalQuerySequence_t
				// and the newly calculated sequence
		};

		template <class OptimalQuerySequence_t, class FoundOptimalQuerySequence_t, class QuerySequence_t>
		struct main_loop_if_contained_t<OptimalQuerySequence_t, FoundOptimalQuerySequence_t, QuerySequence_t, false> {
				typedef FoundOptimalQuerySequence_t type;
		};

		template <class OptimalQuerySequence_t, class FoundOptimalQuerySequence_t, class SetOfMinQuerySequence_t>
		struct main_loop_t;

		template <class OptimalQuerySequence_t, class FoundOptimalQuerySequence_t, class QuerySequence_t, class ...QuerySequences_t>
		struct main_loop_t<OptimalQuerySequence_t, FoundOptimalQuerySequence_t, SetOfQuerySequence<QuerySequence_t, QuerySequences_t...>> {
			private:
				typedef typename main_loop_if_contained_t<OptimalQuerySequence_t, FoundOptimalQuerySequence_t, QuerySequence_t>::type new_optimal;
			public:
				typedef typename main_loop_t<OptimalQuerySequence_t, new_optimal, SetOfQuerySequence<QuerySequences_t...>>::type type;
		};
		template <class OptimalQuerySequence_t, class FoundOptimalQuerySequence_t>
		struct main_loop_t<OptimalQuerySequence_t, FoundOptimalQuerySequence_t, SetOfQuerySequence<>> {
			typedef FoundOptimalQuerySequence_t type;
		};

		template <class OptimalQuerySequence_t, class FoundOptimalQuerySequence_t>
		struct init_main_loop_t {
				typedef typename main_loop_t<OptimalQuerySequence_t, FoundOptimalQuerySequence_t, SetOfMinQuerySequence_Error_t>::type type;
		};

};

template <unsigned int ErrorCount, unsigned int BlockCount>
struct optimal_query_sequence_helper<ErrorCount, ErrorCount, BlockCount> {

		typedef typename build_list_of_min_query_for_errors<vlist::variadic_list<>, 0u, ErrorCount, BlockCount>::type ListOfSetOfMinQuerySequence_t;
		typedef typename vlist::get_at<ListOfSetOfMinQuerySequence_t, ErrorCount>::type SetOfMinQuerySequence_Error_t;

		template <class OptimalQuerySequence_t, class FoundOptimalQuerySequence_t, class QuerySequence_t,
				  bool = vlist::includes<QuerySequence_t, typename merge_optimal_query_sequence<OptimalQuerySequence_t>::type>::value>
		struct main_loop_if_contained_t {
			private:
				typedef typename vlist::difference<QuerySequence_t,
												   typename merge_optimal_query_sequence<OptimalQuerySequence_t>::type
												  >::type TruncatedQuerySeq;
				typedef typename vlist::concat<OptimalQuerySequence_t, OptimalQuerySequence<TruncatedQuerySeq>>::type new_optimal_query_sequence_t;
			public:
				typedef typename optimal_query_sequence_update_min<new_optimal_query_sequence_t, FoundOptimalQuerySequence_t>::type type; // Best of FoundOptimalQuerySequence_t
				// and the newly calculated sequence
		};

		template <class OptimalQuerySequence_t, class FoundOptimalQuerySequence_t, class QuerySequence_t>
		struct main_loop_if_contained_t<OptimalQuerySequence_t, FoundOptimalQuerySequence_t, QuerySequence_t, false> {
				typedef FoundOptimalQuerySequence_t type;
		};

		template <class OptimalQuerySequence_t, class FoundOptimalQuerySequence_t, class SetOfMinQuerySequence_t>
		struct main_loop_t;

		template <class OptimalQuerySequence_t, class FoundOptimalQuerySequence_t, class QuerySequence_t, class ...QuerySequences_t>
		struct main_loop_t<OptimalQuerySequence_t, FoundOptimalQuerySequence_t, SetOfQuerySequence<QuerySequence_t, QuerySequences_t...>> {
			private:
				typedef typename main_loop_if_contained_t<OptimalQuerySequence_t, FoundOptimalQuerySequence_t, QuerySequence_t>::type new_optimal;
			public:
				typedef typename main_loop_t<OptimalQuerySequence_t, new_optimal, SetOfQuerySequence<QuerySequences_t...>>::type type;
		};
		template <class OptimalQuerySequence_t, class FoundOptimalQuerySequence_t>
		struct main_loop_t<OptimalQuerySequence_t, FoundOptimalQuerySequence_t, SetOfQuerySequence<>> {
			typedef FoundOptimalQuerySequence_t type;
		};

		template <class OptimalQuerySequence_t, class FoundOptimalQuerySequence_t>
		struct init_main_loop_t {
				typedef typename main_loop_t<OptimalQuerySequence_t, FoundOptimalQuerySequence_t, SetOfMinQuerySequence_Error_t>::type type;
		};

};

template <unsigned int ErrorCount, unsigned int BlockCount>
struct compute_optimal_query_sequence_helper {
	typedef typename optimal_query_sequence_helper<0u, ErrorCount, BlockCount>::
			template init_main_loop_t<OptimalQuerySequence<>, OptimalQuerySequence<>>::type type;
};

}

template <unsigned int ErrorCount, unsigned int BlockCount>
struct compute_optimal_query_sequence {
		typedef typename priv::compute_optimal_query_sequence_helper<ErrorCount, BlockCount>::type type;
};

}

// =============================================================================================================
//		Sets the optimal query sequence to C++ data structures
// =============================================================================================================

namespace meta_prog {

template <unsigned int T>
using ErrorThreshold = std::integral_constant<unsigned int, T>;

template <class...> // List of ErrorThreshold
struct ErrorThresholdList;

//template <bool Cond, class R = void>
//using enable_if_t = typename std::enable_if<Cond, R>::type;

namespace priv {

template <class ErrorThresholdList_t>
struct assign_error_threshold_helper;
template <class ErrorThreshold_t, class... ErrorThresholds_t>
struct assign_error_threshold_helper<ErrorThresholdList<ErrorThreshold_t, ErrorThresholds_t...>> {
		template <class Assigner_t>
		static void assign(Assigner_t& assigner) {
			assigner.addErrorCutoff(ErrorThreshold_t::value);
			assign_error_threshold_helper<ErrorThresholdList<ErrorThresholds_t...>>::assign(assigner);
		}
};
template <>
struct assign_error_threshold_helper<ErrorThresholdList<>> {
		template <class Assigner_t>
		static void assign(Assigner_t&) {}
};

template <class BlockQuery_t, unsigned int Offset, bool>
struct assign_block_query_helper_exec {
        static constexpr unsigned int assignCount = 1u;

        template <class Assigner_t>
        static void assign(Assigner_t& assigner) {
            assigner.addQuery(BlockQuery_t::blockA, BlockQuery_t::blockB, Offset);
        }
};

template <class BlockQuery_t, unsigned int Offset>
struct assign_block_query_helper_exec<BlockQuery_t, Offset, false> {
        static constexpr unsigned int assignCount = 0u;

        template <class Assigner_t>
        static void assign(Assigner_t&) {}
};

template <class BlockMeta_t, class BlockQuery_t, unsigned int ErrorCutOffBegin, unsigned int ErrorCutOffEnd>
struct assign_block_query_helper_loop {
	private:
		static constexpr unsigned int Offset = BlockMeta_t::offsetAt(BlockQuery_t::blockB) - BlockMeta_t::offsetAt(BlockQuery_t::blockA);
		static constexpr unsigned int Offset_1 = Offset - ErrorCutOffBegin;
		static constexpr unsigned int Offset_2 = Offset + ErrorCutOffBegin;

        typedef assign_block_query_helper_exec<BlockQuery_t, Offset_1, true> FirstAssignment;
        typedef assign_block_query_helper_exec<BlockQuery_t, Offset_2, Offset_1 != Offset_2> SecondAssignment;
        typedef assign_block_query_helper_loop<BlockMeta_t, BlockQuery_t, ErrorCutOffBegin+1, ErrorCutOffEnd> LastAssignment;

	public:
		static constexpr unsigned int assignCount = FirstAssignment::assignCount + SecondAssignment::assignCount + LastAssignment::assignCount;

		template <class Assigner_t>
		static void assign(Assigner_t& assigner) {
			FirstAssignment::assign(assigner);
			SecondAssignment::assign(assigner);
			LastAssignment::assign(assigner);
		}
};

template <class BlockMeta_t, class BlockQuery_t, unsigned int ErrorCutOffEnd>
struct assign_block_query_helper_loop<BlockMeta_t, BlockQuery_t, ErrorCutOffEnd, ErrorCutOffEnd> {
	public:
		static constexpr unsigned int assignCount = 0u;

		template <class Assigner_t>
		static void assign(Assigner_t&) {}
};

template <class BlockMeta_t, class BlockQuery_t, unsigned int ErrorCutOffBegin, unsigned int ErrorCutOffEnd>
struct assign_block_query_helper {
	private:
		static constexpr unsigned int _begin = min(BlockQuery_t::allowedInbetweenErrors, ErrorCutOffBegin);
		static constexpr unsigned int _end = BlockQuery_t::allowedInbetweenErrors < ErrorCutOffBegin ?
													_begin :
													min(BlockQuery_t::allowedInbetweenErrors+1, ErrorCutOffEnd);
        typedef assign_block_query_helper_loop<BlockMeta_t, BlockQuery_t, _begin, _end> Assignment;

	public:
		static constexpr unsigned int assignCount = Assignment::assignCount;

		template <class Assigner_t>
		static void assign(Assigner_t& assigner) {
			Assignment::assign(assigner);
		}
};

template <class BlockMeta_t, class QuerySequence_t, unsigned int ErrorCutOffBegin, unsigned int ErrorCutOffEnd>
struct assign_query_sequence_helper {
	private:
		typedef assign_block_query_helper<BlockMeta_t, typename vlist::first<QuerySequence_t>::type,
        ErrorCutOffBegin, ErrorCutOffEnd> HeadAssignment;
		typedef assign_query_sequence_helper<BlockMeta_t, typename vlist::remove_first<QuerySequence_t>::type,
        ErrorCutOffBegin, ErrorCutOffEnd> TailAssignment;
	public:
        static constexpr unsigned int assignCount = HeadAssignment::assignCount + TailAssignment::assignCount;

		template <class Assigner_t>
		static void assign(Assigner_t& assigner) {
            HeadAssignment::assign(assigner);
            TailAssignment::assign(assigner);
		}
};

template <class BlockMeta_t, unsigned int ErrorCutOffBegin, unsigned int ErrorCutOffEnd>
struct assign_query_sequence_helper<BlockMeta_t, QuerySequence<>, ErrorCutOffBegin, ErrorCutOffEnd>  {
		static constexpr unsigned int assignCount = 0u;

		template <class Assigner_t>
		static void assign(Assigner_t&) {}
};

template <class BlockMeta_t, class OptimalQuerySequence_t, unsigned int ErrorCutOff, unsigned int ErrorCutOffEnd,
          class ErrorThresholdList_t>
struct assign_optimal_query_sequence_helper {
private:
		typedef typename vlist::truncate<OptimalQuerySequence_t, ErrorCutOff>::type already_done_optimal_sequences;
		typedef typename merge_optimal_query_sequence<already_done_optimal_sequences>::type already_done_sequences;
		typedef typename vlist::get_at<OptimalQuerySequence_t, ErrorCutOff>::type query_sequence_t;

        typedef assign_query_sequence_helper<BlockMeta_t, already_done_sequences, ErrorCutOff, ErrorCutOff+1> First;
        typedef assign_query_sequence_helper<BlockMeta_t, query_sequence_t, 0u, ErrorCutOff+1> Second;

		static constexpr unsigned int thisRoundAssignCount = First::assignCount + Second::assignCount;

		typedef typename vlist::concat<ErrorThresholdList_t, ErrorThresholdList<ErrorThreshold<thisRoundAssignCount +
				vlist::last<ErrorThresholdList_t>::type::value>>>::type
				NewErrorThresholdList_t;

		typedef assign_optimal_query_sequence_helper<BlockMeta_t, OptimalQuerySequence_t, ErrorCutOff+1, ErrorCutOffEnd,
        NewErrorThresholdList_t> Final;

public:
	template <class Assigner_t>
	static void assign(Assigner_t& assigner) {
		First::assign(assigner);
		Second::assign(assigner);
		Final::assign(assigner);
	}
};

template <class BlockMeta_t, class OptimalQuerySequence_t, unsigned int ErrorCutOffEnd, class ErrorThresholdList_t>
struct assign_optimal_query_sequence_helper<BlockMeta_t, OptimalQuerySequence_t, ErrorCutOffEnd, ErrorCutOffEnd, ErrorThresholdList_t> {
	private:

	public:
		template <class Assigner_t>
		static void assign(Assigner_t& assigner) {
			assign_error_threshold_helper<typename vlist::remove_first<ErrorThresholdList_t>::type>::assign(assigner);
		}
};

}
}


/**
 * struct Assigner {
 *
 *		void reserveQueryMemory(unsigned int amount);
 *		void addQuery(unsigned int blockIdA, unsigned int blockIdB, unsigned int blockOffset);
 *		void addErrorCutoff(unsigned int queryPastEndIndex);
 *
 * };
 */
template <class BlockMeta_t, class OptimalSequence_t>
struct assign_optimal_query_sequence {

	public:
		template <class Assigner_t>
		static void assign(Assigner_t& assigner) {
            assigner.reserveQueryMemory(meta_prog::score_optimal_query_sequence<OptimalSequence_t>::value);
            meta_prog::priv::assign_optimal_query_sequence_helper<BlockMeta_t, OptimalSequence_t, 0u, vlist::size<OptimalSequence_t>::value,
                    meta_prog::ErrorThresholdList<meta_prog::ErrorThreshold<0u>>>::assign(assigner);
		}

};

template <class BlockMeta_t, unsigned int ErrorCount>
struct compute_and_assign_optimal_query_sequence {

    private:
        typedef typename meta_prog::compute_optimal_query_sequence<ErrorCount, BlockMeta_t::block_count>::type OptimalSequence;

    public:
        template <class Assigner_t>
        static void assign(Assigner_t& assigner) {
            assign_optimal_query_sequence<BlockMeta_t, OptimalSequence>::assign(assigner);
        }

};

/**
  *
  * TEMPLATE GLOBAL ALIGNMENT
  *
  * */

template<class BlockMeta_t, class QuerySequence_t>
struct assign_global_loop {
private:
    typedef assign_global_exec


};

template<class BlockMeta_t, class QuerySequence_t>
struct assign_global_sequence {

private:
        typedef assign_global_sequence_loop<BlockMeta_t, QuerySequence_t, 0, 0> assign_global_loop;

public:
        template <class Assigner_t>
        static void assign(Assigner_t& assigner) {
            assign_global_loop::assign(assigner);
        }

};

#endif // OPTIMALQUERYSEQUENCETEMPLATE_H
