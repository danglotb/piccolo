#ifndef QUERY_H
#define QUERY_H

#include <string>

#include <sstream>
#include <tuple>
#include "QueryMeta.h"

#include "Util.h"

class Query {

    protected:

		BlockId m_blockA = 0, m_blockB = 0; //m_blockA must be < m_blockB
		BlockHash m_hashA = 0, m_hashB = 0;

		nt const* m_tail = nullptr; // Tail is queried iff m_blockB == 4
		nt const* m_tailEnd = nullptr;

	public:
		Query() {}
		Query(Query const&) = default;
		Query(Query&&) = default;
        Query(BlockId blockA, BlockId blockB) : m_blockA(blockA) , m_blockB(blockB) {}

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

        std::tuple<bool,bool,bool> unqueriedBlock() const {return std::make_tuple(blockA() != 0u, (blockB() - blockA()) > 1, blockB() != BLOCK_COUNT-1);}

        static const bool global = false;

        friend std::ostream& operator<<(std::ostream &os, Query q);

};


class QueryGlobal : public Query {

   public:
    QueryGlobal() : Query(), m_offsetA(0), m_offsetB(0) {}
    QueryGlobal(QueryGlobal const&) = default;
    QueryGlobal(QueryGlobal&&) = default;
    QueryGlobal(BlockId blockA, BlockId blockB, int OffsetA, int OffsetB) : Query(blockA, blockB), m_offsetA(OffsetA), m_offsetB(OffsetB) {}
    int m_offsetA;
    int m_offsetB;

    static const bool global = true;

    std::tuple<bool,bool,bool> unqueriedBlock() const {
        return std::make_tuple(blockA() != 0u || m_offsetA > 0, (blockB() - blockA()) > 1, blockB() != BLOCK_COUNT-1 || m_offsetB > 0);
    }

    friend std::ostream& operator<<(std::ostream& os, const QueryGlobal& b);

};

std::ostream& operator<<(std::ostream& os, const QueryGlobal& b);
#endif // QUERY_H
