#ifndef ISOMIR_QUERY_SEQUENCE_TEMPLATE_H
#define ISOMIR_QUERY_SEQUENCE_TEMPLATE_H

namespace isomir {

template<unsigned int BlockA, unsigned int BlockB, unsigned int Err, unsigned int SizeVariation>
struct BlockQuery {

    static const unsigned int blockA = BlockA;
    static const unsigned int blockB = BlockB;
    static const unsigned int err = Err;
    static const unsigned int size_variation = SizeVariation;

};


}


#endif // ISOMIR_QUERY_SEQUENCE_TEMPLATE_H
