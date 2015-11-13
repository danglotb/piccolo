#ifndef ISOMIRQUERYSEQUENCE_H
#define ISOMIRQUERYSEQUENCE_H

#define STAR -1

#include <vector>
#include <sstream>

namespace isomir {

class BlockQuery {


public:

    const unsigned int _blockA;
    const unsigned int _blockB;
    const unsigned int _err;
    const int _variation;

    BlockQuery();
    BlockQuery(const BlockQuery& that);
    BlockQuery(unsigned int blockA, unsigned int blockB, unsigned int err, int variation);
    ~BlockQuery();

    BlockQuery& operator=(const BlockQuery&& that);

    friend std::ostream& operator<<(std::ostream& os, const BlockQuery& b);

};

std::ostream& operator<<(std::ostream& os, const BlockQuery& b);

typedef std::vector<BlockQuery> SequenceQuery;

class QuerySequence {

private:

    SequenceQuery _sequence;

public:

    const unsigned int _k;

    QuerySequence(unsigned int k);
    ~QuerySequence();

    std::vector<BlockQuery>::iterator begin() { return _sequence.begin(); }
    std::vector<BlockQuery>::iterator end() { return _sequence.end(); }

    std::vector<BlockQuery>::const_iterator begin() const { return _sequence.begin(); }
    std::vector<BlockQuery>::const_iterator end() const { return _sequence.end(); }

    friend std::ostream& operator<<(std::ostream& os, const QuerySequence& q);

};

std::ostream& operator<<(std::ostream& os, const QuerySequence& q);

}

#endif // ISOMIRQUERYSEQUENCE_H
