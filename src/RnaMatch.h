#ifndef RNAMATCH_H
#define RNAMATCH_H

#include "RnaIndex.h"

#include "OptimalQuerySequence.h"
#include "RnaAlignment.h"

#include <array>

//typedef unsigned char nt;

class QueryMeta;

class RnaMatch {

protected:
		RnaIndex const& m_index;

		/**
		 * @brief stringToNt Converts an ASCII sequence to nt (integers in 0, 1, 2, 3)
		 */
		static std::vector<nt> stringToNt(std::string const& seq);

        OptimalQuerySequenceBuilder m_querySequence;

		/**
		 * @brief processQuery Process the query q. This means it searches for miRNAs in the index and computes the distance.
		 * @param seq_pos Where we are in the precursor sequence
		 * @param q The query. Contains the IDs of the two blocks, and their hash values.
		 * @param qMeta Meta information associated with the query. Mainly indicates how many nucleotides separate the blocks of the query q.
		 */
        void processQuery(int seq_pos, Query const& q, QueryMeta const& qMeta);

	public:

        struct MiRnaAlignmentResult {
//				unsigned int errorCount = 0u;
//				int sequenceBegin = 0;
//				int sequenceEnd = 0;
				AlignmentResult alignment;

				int begin() const { return alignment.sequenceLocus.begin; }
				int end() const { return alignment.sequenceLocus.end; }

				unsigned int errorCount() const { return alignment.errorCount; }
//				AlignmentResult front, middle, back;

				MiRnaAlignmentResult() {}

				MiRnaAlignmentResult(MiRnaAlignmentResult const&) = default;
				MiRnaAlignmentResult& operator=(MiRnaAlignmentResult const&) = default;

				MiRnaAlignmentResult(MiRnaAlignmentResult&&) = default;
				MiRnaAlignmentResult& operator=(MiRnaAlignmentResult&&) = default;

				void mergeAlignmentResults(AlignmentResult&& front, AlignmentResult&& middle, AlignmentResult&& back,
										   std::size_t blockASize, std::size_t blockBSize, int begin, int end);

				void display(std::ostream& out, std::vector<nt> const& miRna, nt const* seq) const;

				void displayExonerate(std::ostream& out, const std::vector<nt>& miRna, const nt* seq) const;

		};

		typedef std::vector<MiRnaAlignmentResult> RnaResult;

        unsigned int nbAligned = 0;
        unsigned int nbQuerried = 0;
        bool isAligned = false;
        bool isQuerried = false;

    protected:
        std::vector<RnaResult> m_results; // one entry per mi rna
		unsigned int m_minErrorFound;
		nt const* m_seq_begin;
		nt const* m_seq_end;
		RnaAlignment m_aligner;
        bool m_findBest = true;
        const unsigned int m_nbIndel;
		void reset(std::size_t sequence_size, RnaIndex const& new_index);

		/**
		 * @brief processQueryResult. From the results of a query (IDs of miRNAs that match in the index), computes the alignment for each miRNA.
		 * @param seq_pos Where we are in the precursor sequence
		 * @param query The query. Contains the IDs of the two blocks, and their hash values.
		 * @param qMeta Meta information associated with the query. Mainly indicates how many nucleotides separate the blocks of the query q.
		 * @param queryResult The result of the query. Contains the IDs of miRNAs that match our query in the index.
		 */
        template<typename Query_t>
        void processQueryResult(int seq_pos, Query_t const& query, const QueryMeta& qMeta, QueryResult const& queryResult);

		void displayHumanReadableResult(MiRnaEntry const& sequence, uint from, std::ostream& out) const;

		void displayExonerateResult(MiRnaEntry const& sequence, uint from, std::ostream& out) const;

	public:
        RnaMatch(RnaIndex const& index);
        RnaMatch(RnaIndex const& index, unsigned int k);
		RnaMatch(RnaMatch const& index) = default;
		RnaMatch(RnaMatch&& index) = default;

		RnaMatch& operator=(RnaMatch const& index) = default;
		RnaMatch& operator=(RnaMatch&& index) = default;

		/**
		 * @brief match Main function. Runs the entire algorithm on a sequence.
		 * @param best if true, returns only the best results. Returns all results otherwise (with no more than 3 errors).
		 */
        void match(std::string const& sequence, bool best, bool global) { return match(util::stringToNt(sequence), best, global); }
		/**
		 * @brief match Overload of the above function
		 */
        void match(std::vector<nt> const& sequence, bool best, bool global);
		/**
		 * @brief match Overload of the above function
		 */
        void match_small_in_large(nt const* sequence_begin, nt const* sequence_end, bool best);

        void match_global(const nt* sequence_begin, const nt* sequence_end);

		bool displayResult(MiRnaEntry const& sequence, std::ostream& out, bool humanReadable) const;
		bool displayResult(MiRnaEntry const& sequence, uint from, std::ostream& out, bool humanReadable) const;
};

#endif // RNAMATCH_H
