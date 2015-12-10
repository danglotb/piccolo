#include "QueryBuilder.h"
#include "BlockMeta.h"

#include <iostream>

QueryBuilder::QueryBuilder(BlockId blockA, BlockId blockB) : QueryBuilder(blockA, blockB, BLOCK_OFFSET_AT(blockB) - BLOCK_OFFSET_AT(blockA)) {}

QueryBuilder::QueryBuilder(BlockId blockA, BlockId blockB, unsigned int offset) : BlockA(blockA), BlockB(blockB),
	SizeA(BLOCK_SIZE_AT(blockA)), OffsetB(offset), OffsetEndB(OffsetB + BLOCK_SIZE_AT(blockB)),
    BlockMaskA(BLOCK_MASK_AT(blockA)), BlockMaskB(BLOCK_MASK_AT(blockB)), m_queryMeta(offset - SizeA), E(0), J(0) {
	m_lastQuery.setBlockIds(blockA, blockB);
}

QueryBuilder::QueryBuilder(BlockId blockA, BlockId blockB, unsigned int offset, int e, int j) : BlockA(blockA), BlockB(blockB),
    SizeA(BLOCK_SIZE_AT(blockA)), OffsetB(offset), OffsetEndB(OffsetB + BLOCK_SIZE_AT(blockB)),
    BlockMaskA(BLOCK_MASK_AT(blockA)), BlockMaskB(BLOCK_MASK_AT(blockB)), m_queryMeta(offset - SizeA), E(e), J(j), it_e(-E), it_j(-J) {
    m_lastQuery.setBlockIds(blockA, blockB);
}

const Query& QueryBuilder::nextQuery(const nt* seq, const nt* seq_end) {
    unsigned int offset_block_B = 0;
    if (BlockA == 4) //last block
        offset_block_B = (seq_end - seq) % 5 ;
    m_lastQuery.setBlockIds(BlockA,BlockB);
    m_lastQuery.setBlockHash(util::hash(seq+(BlockA*SizeA+E), seq+((BlockA+1)*SizeA+E)),
                   util::hash(seq+(BlockB*SizeA+J+E), seq+((BlockB+1)*SizeA+offset_block_B+J+E)));
    update_cursor();
    return m_lastQuery;
}

void QueryBuilder::update_cursor() {
    it_e++;
    if (it_e == E+1) {
        it_e = -E;
        it_j++;
        if (it_j == J+1)
            it_j = -J;
    }
}

const Query& QueryBuilder::initialize(const nt* seq, const nt* seq_end) {
    m_lastQuery.setBlockHash(util::hash(seq, seq + SizeA), util::hash(seq + OffsetB, seq + OffsetEndB));
	m_lastQuery.setTail(seq + OffsetEndB, seq_end);
	return m_lastQuery;
}

const Query& QueryBuilder::buildNextQuery(const nt* seq, const nt*end) {
	m_lastQuery.rhashA() <<= 2;
	m_lastQuery.rhashA() |= static_cast<u8>(*(seq + SizeA-1));
	m_lastQuery.rhashA() &= BlockMaskA;

        if (end > seq+OffsetEndB - 1) {
          end = seq + OffsetEndB - 1;
        } else {
          end--;
        }

	m_lastQuery.rhashB() <<= 2;
	m_lastQuery.rhashB() |= static_cast<u8>(*(end));
	m_lastQuery.rhashB() &= BlockMaskB;

	m_lastQuery.advanceTail();

    return m_lastQuery;
}

std::pair<bool, const Query&> QueryBuilder::buildNextQueryFromTruncatedSeq(const nt* seq, const nt* seq_end) {
	if (seq_end - seq < OffsetEndB)
		return std::pair<bool, const Query&>(false, m_lastQuery);
	else
		return std::pair<bool, const Query&>(true, buildNextQuery(seq, seq_end));
}
