#ifndef QUERY_H
#define QUERY_H

#include <string>

#include <sstream>

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


        friend std::ostream& operator<<(std::ostream &os, Query q);

};

class QueryGap : public Query {

    int _gap_blockA;
    int _gap_blockB;

public:

    QueryGap(): Query() {}
    QueryGap(int gapBlockA, int gapBetweenBlock) : Query(), _gap_blockA(gapBlockA), _gap_blockB(gapBetweenBlock) {}

    void setGaps(int gap_blockA, int gapBlockB) {_gap_blockA = gap_blockA ; _gap_blockB = gapBlockB;}

    int gapBlockA() const {return _gap_blockA;}
    int gapBlockB() const {return _gap_blockB;}

};

#endif // QUERY_H
