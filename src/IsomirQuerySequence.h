#ifndef ISOMIRQUERYSEQUENCE_H
#define ISOMIRQUERYSEQUENCE_H

#define STAR -1

#include <vector>
#include <sstream>

namespace isomir {

class BlockQuery {

private:

    const unsigned int _blockA;
    const unsigned int _blockB;
    const unsigned int _err;
    const int _variation;

public:

    BlockQuery();
    BlockQuery(const BlockQuery& that);
    BlockQuery(unsigned int blockA, unsigned int blockB, unsigned int err, int variation);
    ~BlockQuery();

    BlockQuery& operator=(const BlockQuery&& that);

    friend std::ostream& operator<<(std::ostream& os, const BlockQuery& b);

};

std::ostream& operator<<(std::ostream& os, const BlockQuery& b);

typedef std::vector<BlockQuery> SequenceQuery;

class QuerySequenceBuilder {

private:

    SequenceQuery _sequence;

public:

    QuerySequenceBuilder(unsigned int k);
    ~QuerySequenceBuilder();

    std::vector<BlockQuery>::iterator begin() { return _sequence.begin(); }
    std::vector<BlockQuery>::iterator end() { return _sequence.end(); }

    friend std::ostream& operator<<(std::ostream& os, const QuerySequenceBuilder& q);

};

std::ostream& operator<<(std::ostream& os, const QuerySequenceBuilder& q);

}

#endif // ISOMIRQUERYSEQUENCE_H
