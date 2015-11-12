#include "IsomirQuerySequence.h"

using namespace isomir;

BlockQuery::BlockQuery() : _blockA(0), _blockB(0), _err(0), _variation(0) {}
BlockQuery::BlockQuery(const BlockQuery& that) : _blockA(that._blockA), _blockB(that._blockB), _err(that._err), _variation(that._variation) {}
BlockQuery::BlockQuery(unsigned int blockA, unsigned int blockB, unsigned int err, int variation) : _blockA(blockA), _blockB(blockB), _err(err), _variation(variation) {}
BlockQuery::~BlockQuery() {}

BlockQuery& BlockQuery::operator=(const BlockQuery&& that) {
    *const_cast<unsigned int*>(&_blockA) = that._blockA;
    *const_cast<unsigned int*>(&_blockB) = that._blockB;
    *const_cast<unsigned int*>(&_err) = that._err;
    *const_cast<int*>(&_variation) = that._variation;
    return *this;
}

std::ostream& isomir::operator<<(std::ostream& os, const BlockQuery& b) {
    os << b._err << "[" << b._blockA << ";" << b._blockB << ":" << b._variation << "]" << std::endl;
    return os;
}

QuerySequenceBuilder::QuerySequenceBuilder(unsigned int k) {
    for (unsigned int x = 1 ; x <= k + 1 ; x++) {
        for (unsigned int i = x + 1 ; i <= k + 2 ; i++) {
            unsigned int e = x - 1;
            unsigned int j = i - x - 1;
            BlockQuery b;
            if (e + j == k) {
                /*if (x == i - 1)
                    b  = BlockQuery(x,i,e,-1);
                else                Not Necessary*/
                    b = BlockQuery(x,i,e,STAR);
            } else
                b = BlockQuery(x,i,e,j);
            _sequence.push_back(b);
        }
    }
}

QuerySequenceBuilder::~QuerySequenceBuilder() {}

std::ostream& isomir::operator<<(std::ostream& os, const QuerySequenceBuilder& q) {

    for (auto it : q._sequence)
        os << it << std::endl;

    return os;
}

