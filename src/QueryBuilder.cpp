#include "QueryBuilder.h"
#include "BlockMeta.h"

QueryBuilder::QueryBuilder(BlockId blockA, BlockId blockB) : QueryBuilder(blockA, blockB, BLOCK_OFFSET_AT(blockB) - BLOCK_OFFSET_AT(blockA)) {}

QueryBuilder::QueryBuilder(BlockId blockA, BlockId blockB, unsigned int offset) : /*BlockA(blockA), BlockB(blockB),*/
	SizeA(BLOCK_SIZE_AT(blockA)), OffsetB(offset), OffsetEndB(OffsetB + BLOCK_SIZE_AT(blockB)),
    BlockMaskA(BLOCK_MASK_AT(blockA)), BlockMaskB(BLOCK_MASK_AT(blockB)), m_queryMeta(offset - SizeA), OffsetA(0) {
	m_lastQuery.setBlockIds(blockA, blockB);
}

QueryBuilder::QueryBuilder(BlockId blockA, BlockId blockB, unsigned int offsetA, unsigned int offsetB) : /*BlockA(blockA), BlockB(blockB),*/
    SizeA(BLOCK_SIZE_AT(blockA)), OffsetB(offsetB), OffsetEndB(OffsetB + BLOCK_SIZE_AT(blockB)),
    BlockMaskA(BLOCK_MASK_AT(blockA)), BlockMaskB(BLOCK_MASK_AT(blockB)), m_queryMeta(offsetB - SizeA), OffsetA(offsetA) {
    m_lastQuery.setBlockIds(blockA, blockB);
}

const QueryGap& QueryBuilder::initialize(const nt* seq, const nt* seq_end) {
    m_lastQuery.setBlockHash(util::hash(seq + OffsetA, seq + SizeA + OffsetA), util::hash(seq + OffsetB + OffsetA, seq + OffsetEndB + OffsetA));
//	m_lastQuery.setBlockHash(util::hash(seq, seq + SizeA), util::hash(seq + OffsetB, seq + OffsetEndB));
	m_lastQuery.setTail(seq + OffsetEndB, seq_end);
	return m_lastQuery;
}

const QueryGap& QueryBuilder::buildNextQuery(const nt* seq, const nt*end) {
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
