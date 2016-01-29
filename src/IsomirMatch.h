#ifndef ISOMIRMATCH_H
#define ISOMIRMATCH_H

#include <sstream>
#include <iostream>
#include <algorithm>

#include "Query.h"
#include "RnaIndex.h"
#include "RnaAlignment.h"
#include "IsomirQuerySequence.h"
#include "QueryResult.h"
#include "Util.h"
#include "RnaMatch.h"

/**
 *
 *  NOT USED ANYMORE
 *
 */

namespace isomir {

class IsomirMatch : public RnaMatch {

private:

    QuerySequence const& _query_seq;

public:

    IsomirMatch(RnaIndex const& index, QuerySequence const& query_seq);
    ~IsomirMatch();

    void match(const std::vector<nt>& seq);

    void match(nt const* sequence_begin, nt const* sequence_end);

    void processQueryResult(nt const* seq_beg, nt const* seq_end,const QueryResult& queryResult);

};

}

#endif // ISOMIRMATCH_H
