#include "IsomirMatch.h"

using namespace isomir;

IsomirMatch::IsomirMatch(RnaIndex const& index,  QuerySequence const& query_seq) : RnaMatch(index), _query_seq(query_seq){}
IsomirMatch::~IsomirMatch() {}

void IsomirMatch::match(const std::vector<nt>& seq) {
    match(seq.data(), seq.data()+seq.size());
}

void IsomirMatch::match(nt const* seq_beg, nt const* seq_end) {
    unsigned int size = 4;
    reset(seq_end-seq_beg, m_index);
    for (BlockQuery b : _query_seq) {
        for (int j = -b._variation ; j <= b._variation ; j++) {
            for (int e = -b._err ; e <= b._err ; e++) {
                Query q;
                q.setBlockIds(b._blockA, b._blockB);
                q.setBlockHash(util::hash(seq_beg+(b._blockA*size+e), seq_beg+((b._blockA+1)*size+e)), util::hash(seq_beg+(b._blockB*size+j), seq_beg+((b._blockB+1)*size+j)));
                processQueryResult(seq_beg,seq_end, m_index.search(q));
            }
        }
    }
}

void IsomirMatch::processQueryResult(nt const* seq_beg, nt const* seq_end,const QueryResult& queryResult) {
    for (auto rnaId : queryResult) {
        RnaResult& rnaResult = m_results[rnaId];
        MiRnaEntry const& miRna = m_index.at(rnaId);
        MiRnaAlignmentResult r;
        r.alignment = m_aligner.alignMiddle(seq_beg, seq_end, miRna.second.data(), miRna.second.data() + miRna.second.size());
        rnaResult.push_back(std::move(r));
    }
}
