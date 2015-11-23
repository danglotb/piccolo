#ifndef QUERY_H
#define QUERY_H

#include <string>

#include "Util.h"

class Query {

		BlockId m_blockA = 0, m_blockB = 0; //m_blockA must be < m_blockB
		BlockHash m_hashA = 0, m_hashB = 0;
		nt const* m_tail = nullptr; // Tail is queried iff m_blockB == 4
		nt const* m_tailEnd = nullptr;

	public:
		Query() {}
		Query(Query const&) = default;
		Query(Query&&) = default;

		Query& operator=(Query const&) = default;
		Query& operator=(Query&&) = default;

		void setBlockIds(BlockId blockA, BlockId blockB) { m_blockA = blockA; m_blockB = blockB; }
		void setBlockHash(BlockHash blockA, BlockHash blockB) { m_hashA = blockA; m_hashB = blockB; }

		BlockId blockA() const { return m_blockA; }
		void setBlockA(BlockId blockA) { m_blockA = blockA; }

		BlockId blockB() const { return m_blockB; }
		void setBlockB(BlockId blockB) { m_blockB = blockB; }

		BlockHash hashA() const { return m_hashA; }
		BlockHash& rhashA() { return m_hashA; }
		void setHashA(BlockHash hashA) { m_hashA = hashA; }

		BlockHash hashB() const { return m_hashB; }
		BlockHash& rhashB() { return m_hashB; }
		void setHashB(BlockHash hashB) { m_hashB = hashB; }

		nt const* tail() const { return m_tail; }
		nt const* tailEnd() const { return m_tailEnd; }
		void advanceTail() { m_tail++; }
		void setTail(nt const* tail, nt const* tail_end) { m_tail = tail; m_tailEnd = tail_end; }



};

class QueryGap : public Query {

    const int gap_blockA;
    const int gap_between_block;

public:

    QueryGap(int gapBlockA, int gapBetweenBlock) : Query(), gap_blockA(gapBlockA), gap_between_block(gapBetweenBlock) {}

    int gapBlockA() const {return gap_blockA;}
    int gapBetweenBlock() const {return gap_between_block;}

};

#endif // QUERY_H
