#include "IsomirMatch.h"

using namespace isomir;

IsomirMatch::IsomirMatch(RnaIndex const& index,  QuerySequence const& query_seq) : RnaMatch(index), _query_seq(query_seq){ m_findBest = false; }
IsomirMatch::~IsomirMatch() {}

void IsomirMatch::match(const std::vector<nt>& seq) {
    match(seq.data(), seq.data()+seq.size());
}

void IsomirMatch::match(nt const* seq_beg, nt const* seq_end) {

    unsigned int size_block = (seq_end - seq_beg) / (_query_seq._k + 2);

    reset(seq_end - seq_beg, m_index);

    for (BlockQuery b : _query_seq) {

        Query q;
        q.setBlockIds(b._blockA, b._blockB);

        for (int j = -b._variation ; j <= b._variation ; j++) {
            for (int e = -b._err ; e <= b._err ; e++) {

                unsigned int offset_block_B = 0;

                if (b._blockB == _query_seq._k + 1) //last block
                    offset_block_B = (seq_end - seq_beg) % (_query_seq._k + 2);

                q.setBlockHash(util::hash(seq_beg+(b._blockA*size_block+e), seq_beg+((b._blockA+1)*size_block+e)),
                               util::hash(seq_beg+(b._blockB*size_block+j+e), seq_beg+((b._blockB+1)*size_block+offset_block_B+j+e)));

                processQueryResult(seq_beg, seq_end, m_index.search(q));
            }
        }
    }
}

void IsomirMatch::processQueryResult(nt const* seq_beg, nt const* seq_end,const QueryResult& queryResult) {
    for (auto rnaId : queryResult) {
        if (std::find(_indices.begin(), _indices.end(), rnaId) == _indices.end()) {//check if we already find this one.
            _indices.push_back(rnaId);
            RnaResult& rnaResult = m_results[rnaId];
            MiRnaEntry const& miRna = m_index.at(rnaId);
            std::cout << "#" << rnaId << " " << miRna.second << std::endl;
            MiRnaAlignmentResult r;
            r.alignment = m_aligner.alignMiddle(seq_beg, seq_end, miRna.second.data(), miRna.second.data() + miRna.second.size());
            rnaResult.push_back(std::move(r));
        }
    }
}
