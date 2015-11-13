#ifndef ISOMIRMATCH_H
#define ISOMIRMATCH_H

#include <sstream>

#include "RnaIndex.h"
#include "RnaAlignment.h"
#include "IsomirQuerySequence.h"
#include "Util.h"

namespace isomir {

class IsomirMatch {

private:

    RnaIndex const& _index;
    QuerySequence const& _query_seq;

public:

    IsomirMatch(RnaIndex const& index, QuerySequence const& query_seq);
    ~IsomirMatch();

    void match(RnaSequence seq);


};

}

#endif // ISOMIRMATCH_H
