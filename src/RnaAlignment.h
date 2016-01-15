#ifndef RNAALIGNMENT_H
#define RNAALIGNMENT_H

#include <vector>
#include <deque>
#include "Util.h"
#include <iostream>

enum class Operation { Match, Subsitution, Insertion, Deletion };

struct Locus {
		unsigned int begin, end;
		Locus() : begin(0u), end(0u) {}
		Locus(unsigned int b, unsigned int e) : begin(b), end(e) {}
};

struct AlignmentResult {
		Locus sequenceLocus;
		std::deque<Operation> operations;
		unsigned int errorCount = 0u;

		AlignmentResult() {}
		AlignmentResult(AlignmentResult const&) = default;
		AlignmentResult(AlignmentResult&&) = default;

		AlignmentResult& operator=(AlignmentResult const&) = default;
		AlignmentResult& operator=(AlignmentResult&&) = default;

		void clear() {
			sequenceLocus.begin = sequenceLocus.end = 0u;
			operations.clear();
			errorCount = 0u;
		}
};

class RnaAlignment {

		struct AlignmentScore {
				int score = 0;
				Operation operation = Operation::Match;

				AlignmentScore(int score = 0) : score(score) {}
				AlignmentScore(int score, Operation op) : score(score), operation(op) {}
		};

		std::vector<AlignmentScore> m_alignMatrix;
//		typedef std::size_t RnaIndex;
//		typedef std::size_t SequenceIndex;

        typedef unsigned long RnaIndex;
        typedef unsigned long SequenceIndex;

        SequenceIndex /*const*/ m_sequenceLength;
		RnaIndex /*const*/ m_miRnaLength;

		AlignmentScore* scanSequenceRow(RnaIndex rnaId) { return m_alignMatrix.data() + m_sequenceLength*rnaId; }
		AlignmentScore* scanSequenceRowEnd(RnaIndex rnaId) { return m_alignMatrix.data() + m_sequenceLength*(rnaId+1); }

		AlignmentScore& at(RnaIndex rnaId, SequenceIndex seqId) { return m_alignMatrix[m_sequenceLength*rnaId+seqId]; }
		AlignmentScore at(RnaIndex rnaId, SequenceIndex seqId) const { return m_alignMatrix[m_sequenceLength*rnaId+seqId]; }

		void backTrack(AlignmentResult& result, RnaIndex rnaId, SequenceIndex seqId);

		void initMatrix() {
            for (AlignmentScore* begin = scanSequenceRow(0), *it = scanSequenceRow(0), *end = scanSequenceRowEnd(0); it != end; ++it)
				*it = AlignmentScore(it - begin, Operation::Deletion);
			for (RnaIndex i = 1u; i < m_miRnaLength; i++)
				at(i, 0) = AlignmentScore(i, Operation::Insertion);
		}

		void processScore(RnaIndex rnaId, SequenceIndex seqId, nt rnaNt, nt seqNt) {
            if (rnaNt == seqNt)
                at(rnaId, seqId) = AlignmentScore(at(rnaId-1, seqId-1).score, Operation::Match);
            else {
				int subs = at(rnaId-1, seqId-1).score+1;
				int insertion = at(rnaId-1, seqId).score+1;
				int deletion = at(rnaId, seqId-1).score+1;
				if (subs <= insertion and subs <= deletion)
					at(rnaId, seqId) = AlignmentScore(subs, Operation::Subsitution);
				else if (insertion <= subs and insertion <= deletion)
					at(rnaId, seqId) = AlignmentScore(insertion, Operation::Insertion);
				else
					at(rnaId, seqId) = AlignmentScore(deletion, Operation::Deletion);
			}
		}

		template <class SequenceConstIterator, class RnaConstIterator>
		AlignmentResult alignEdge(SequenceConstIterator seq, SequenceConstIterator seqEnd,
								  RnaConstIterator knownMiRna, RnaConstIterator knownMiRnaEnd) {
			AlignmentResult result;

			initMatrix();

			RnaIndex const rnaIdEnd = knownMiRnaEnd - knownMiRna;
			SequenceIndex const seqIdEnd = seqEnd - seq;

			RnaConstIterator miRnaSeq = knownMiRna;

			// We stop before the last row
			for (RnaIndex rnaId = 1u; rnaId < rnaIdEnd; ++rnaId, ++miRnaSeq) {
				SequenceConstIterator sequence = seq;
				for (SequenceIndex seqId = 1u; seqId <= seqIdEnd; ++seqId, ++sequence)
					processScore(rnaId, seqId, *miRnaSeq, *sequence);
			}
			// We compute the last row while looking for the minimum
			unsigned int errorMin = -1;
			SequenceIndex seqMinId = 0u;
			{
				RnaIndex const rnaId = rnaIdEnd;
				SequenceConstIterator sequence = seq;
				for (SequenceIndex seqId = 1u; seqId <= seqIdEnd; ++seqId, ++sequence) {
					processScore(rnaId, seqId, *miRnaSeq, *sequence);
					unsigned int score = at(rnaId, seqId).score;
					if (score == 0u) {
						errorMin = 0u;
						seqMinId = seqId;
						break;
					}
					else if (score < errorMin) {
						errorMin = score;
						seqMinId = seqId;
					}
				}
			}

			backTrack(result, rnaIdEnd, seqMinId);

			return result;
		}

	public:
		RnaAlignment() : RnaAlignment(0u, 0u) {}
		RnaAlignment(std::size_t sequenceLength, std::size_t maxRnaLength);
		RnaAlignment(RnaAlignment&&) = default;

		RnaAlignment& operator=(RnaAlignment&&) = default;

		AlignmentResult alignMiddle(nt const* seq, nt const* seqEnd, nt const* knownMiRna, nt const* knownMiRnaEnd);

		/// seq: The begining of the putative sequence (included).
		/// seqEnd: pointer to the starting position of the first detected seed/block. seqEnd is excluded from the alignment.
		/// knownMiRna: the begining of the miRna
		/// knownMiRnaEnd: knownMiRna + offset of the first detected seed/block.
		AlignmentResult alignFront(nt const* seq, nt const* seqEnd, nt const* knownMiRna, nt const* knownMiRnaEnd);

		/// seq: the current sequence position PLUS the past-end position of the rightmost detected seed/block.
		/// seqEnd: = seq +  knownMiRnaEnd - knownMiRna + number of allowed errors. seqEnd is excluded from the alignment.
		/// knownMiRna: the begining of the miRna (ast-end position of the rightmost detected seed/block).
		/// knownMiRnaEnd: the past-end position of the miRna
		AlignmentResult alignBack(nt const* seq, nt const* seqEnd, nt const* knownMiRna, nt const* knownMiRnaEnd);
};

#endif // RNAALIGNMENT_H
