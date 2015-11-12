#ifndef QUERYMETA_H
#define QUERYMETA_H

#include "BlockMeta.h"

#include "Query.h"

class QueryMeta {

		unsigned int const m_blockOffset;

	public:
		QueryMeta(QueryMeta const&) = default;
		QueryMeta(unsigned int offset) : m_blockOffset(offset) {}

		// The number of nt in the middle of the two blocks (the two block excluded)
		unsigned int blockOffset() const { return m_blockOffset; }

		static bool hasFrontUnqueriedBlock(Query const& q) {
			return q.blockA() != 0u;
		}
		static bool hasMiddleUnqueriedBlock(Query const& q) {
			return q.blockB() - q.blockA() > 1;
		}
		static bool hasTrailingUnqueriedBlock(Query const& q) {
			return q.blockB() != BLOCK_COUNT-1;
		}

//		unsigned int allowedFrontErrors(Query const& q) const {
//			return abs(static_cast<int>(BLOCK_OFFSET_AT(q.blockA())) - static_cast<int>(m_blockAOffset));
//		}
//		unsigned int allowedMiddleErrors(Query const& q) const {
//			return abs(static_cast<int>(BLOCK_OFFSET_AT(q.blockA())) - static_cast<int>(m_blockAOffset));
//		}
//		unsigned int allowedTrailingErrors(Query const& q) const {
//			return abs(static_cast<int>(BLOCK_OFFSET_AT(q.blockA())) - static_cast<int>(m_blockAOffset));
//		}
};

#endif // QUERYMETA_H
