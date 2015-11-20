#include "RnaAlignment.h"

#include <algorithm>
#include <iostream>

void RnaAlignment::backTrack(AlignmentResult& result, RnaAlignment::RnaIndex rnaId, RnaAlignment::SequenceIndex seqId) {
	result.sequenceLocus.begin = 0u;
	result.sequenceLocus.end = seqId;
	result.errorCount = at(rnaId, seqId).score;
	while (rnaId > 0u || seqId > 0u) {
		Operation op = at(rnaId, seqId).operation;
		result.operations.push_front(op);
		switch (op) {
			case Operation::Deletion:
				seqId--;
				break;
			case Operation::Insertion:
				rnaId--;
				break;
			default:
				seqId--;
				rnaId--;
				break;
		}
	}
}

RnaAlignment::RnaAlignment(std::size_t sequenceLength, std::size_t maxRnaLength) : m_alignMatrix((sequenceLength+1)*(maxRnaLength+1)),
	m_sequenceLength(sequenceLength+1), m_miRnaLength(maxRnaLength+1) {}

AlignmentResult RnaAlignment::alignMiddle(const nt* seq, nt const* seqEnd, const nt* knownMiRna, const nt* knownMiRnaEnd) {
	AlignmentResult result;

	initMatrix();

	RnaIndex const rnaIdEnd = knownMiRnaEnd - knownMiRna;
	SequenceIndex const seqIdEnd = seqEnd - seq;

	nt const* miRnaSeq = knownMiRna;

	for (RnaIndex rnaId = 1u; rnaId <= rnaIdEnd; rnaId++, miRnaSeq++) {
		nt const* sequence = seq;
        for (SequenceIndex seqId = 1u; seqId <= seqIdEnd; seqId++, sequence++) {
            processScore(rnaId, seqId, *miRnaSeq, *sequence);
        }
	}

	backTrack(result, rnaIdEnd, seqIdEnd);

	return result;
}

AlignmentResult RnaAlignment::alignFront(const nt* seq, const nt* seqEnd, const nt* knownMiRna, const nt* knownMiRnaEnd) {
	AlignmentResult result = alignEdge(std::reverse_iterator<nt const*>(seqEnd), std::reverse_iterator<nt const*>(seq),
					 std::reverse_iterator<nt const*>(knownMiRnaEnd), std::reverse_iterator<nt const*>(knownMiRna));
	auto locus = result.sequenceLocus;
	std::reverse(result.operations.begin(), result.operations.end());
	result.sequenceLocus = Locus(seqEnd - seq - locus.end, seqEnd - seq - locus.begin);
	return result;
}

AlignmentResult RnaAlignment::alignBack(const nt* seq, const nt* seqEnd, const nt* knownMiRna, const nt* knownMiRnaEnd) {
	return alignEdge(seq, seqEnd, knownMiRna, knownMiRnaEnd);
}
