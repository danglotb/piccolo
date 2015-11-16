#ifndef ISOMIRMATCH_H
#define ISOMIRMATCH_H

#include <sstream>
#include <iostream>

#include "Query.h"
#include "RnaIndex.h"
#include "RnaAlignment.h"
#include "IsomirQuerySequence.h"
#include "QueryResult.h"
#include "Util.h"
#include "RnaMatch.h"

namespace isomir {

class IsomirMatch : public RnaMatch {

private:

    //RnaIndex const& _index;
    QuerySequence const& _query_seq;

public:

    IsomirMatch(RnaIndex const& index, QuerySequence const& query_seq);
    ~IsomirMatch();

    void match(const std::vector<nt>& seq);
    void match(const nt* seq_beg);
    void processQueryResult(const QueryResult& queryResult);


};

}

#endif // ISOMIRMATCH_H
