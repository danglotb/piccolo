#include "IsomirMatch.h"

using namespace isomir;

IsomirMatch::IsomirMatch(RnaIndex const& index,  QuerySequence const& query_seq) : _index(index), _query_seq(query_seq){}
IsomirMatch::~IsomirMatch() {}

void IsomirMatch::match(RnaSequence seq) {
    for (BlockQuery b : _query_seq) {
        for (int j = -b._variation ; j <= b._variation ; j++) {

        }
    }
}

