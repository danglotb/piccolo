#include "IsomirMatch.h"

using namespace isomir;

IsomirMatch::IsomirMatch(RnaIndex const& index,  QuerySequence const& query_seq) : RnaMatch(index), _query_seq(query_seq){}
IsomirMatch::~IsomirMatch() {}

void IsomirMatch::match(const std::vector<nt>& seq) {
    match(seq.data());
}

void IsomirMatch::match(const nt* seq_beg) {
    unsigned int size = 4;
    unsigned int cpt = 0;
    for (BlockQuery b : _query_seq) {
        //std::cout << b;
        for (int j = -b._variation ; j <= b._variation ; j++) {
            //std::cout << "J " << j << " b ERR " << b._err << std::endl;
            for (int e = -b._err ; e <= b._err ; e++) {
                cpt++;
                //std::cout << "QUERY " << e << "[" << b._blockA << ";" << b._blockB << ":" << j << "]" << std::endl;
                //std::cout << b._blockA*size+e << "_" << (b._blockA+1)*size+e << ";" << b._blockB*size+j << "_" <<  (b._blockB+1)*size+j  << std::endl;
                Query q;
                //std::cout << b._blockA*size+e << " ; " << (b._blockA+1)*size+e << "\t" << b._blockB*size+j << ";" << (b._blockB+1)*size+j << std::endl;
                //q.setBlockHash(util::hash(&seq_beg[b._blockA*size+e], &seq_beg[(b._blockA+1)*size+e]), util::hash(&seq_beg[b._blockB*size+j], &seq_beg[(b._blockB+1)*size+j]));
                q.setBlockIds(b._blockA, b._blockB);
                q.setBlockHash(util::hash(seq_beg+(b._blockA*size+e), seq_beg+((b._blockA+1)*size+e)), util::hash(seq_beg+(b._blockB*size+j), seq_beg+((b._blockB+1)*size+j)));
                processQueryResult(m_index.search(q));
            }
        }
    }
    //std::cout << "CPT : " << cpt << std::endl;
}

void IsomirMatch::processQueryResult(const QueryResult& queryResult) {
    for (auto rnaId : queryResult) {
        RnaResult& rnaResult = m_results[rnaId];
        MiRnaEntry const& miRna = m_index.at(rnaId);

        std::cout << miRna.second << std::endl;

        //std::cout << rnaResult << std::endl;
    }
}

std::ostream& operator<<(std::ostream& os, const QueryResult& q) {

    for (auto it : q) {
        os << it;
    }

    //os << std::endl;

    return os;
}


