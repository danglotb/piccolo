#ifndef QUERYBUILDER_H
#define QUERYBUILDER_H

#include "BlockMeta.h"
#include "Query.h"
#include "QueryMeta.h"

#include <ostream>

class QueryBuilder {

        BlockId const BlockA;
        BlockId const BlockB;

		const unsigned int SizeA; //= BLOCK_SIZE_AT(N);
		const unsigned int OffsetB; //= BLOCK_OFFSET_AT(P);
//		const unsigned int OffsetEndA; //= BLOCK_OFFSET_AT(N);
		const unsigned int OffsetEndB; //= BLOCK_OFFSET_AT(P);

		const BlockHash BlockMaskA; //= BLOCK_MASK_AT(N);
		const BlockHash BlockMaskB; //= BLOCK_MASK_AT(P);

        Query m_lastQuery;
		QueryMeta m_queryMeta;

	public:
		QueryBuilder(BlockId blockA, BlockId blockB);
        QueryBuilder(BlockId blockA, BlockId blockB, unsigned int offset);

		// seq points to the begining of the sequence
        Query const& initialize(nt const* seq, nt const* seq_end);

		// seq points to the current position of the sequence
        Query const& buildNextQuery(nt const* seq, nt const* seq_end);

        //iso_mod?
        const Query& nextQuery(const nt* seq, const nt* seq_end);

		/// seq points to the current position of the sequence
		/// seq_end - seq may be less than OffsetEndB
		/// returns (true, q) if the query falls within the sequence
		/// returns (false, q) otherwise. In this case, the query shall not be issued.
        std::pair<bool, Query const&> buildNextQueryFromTruncatedSeq(nt const* seq, nt const* seq_end);

		QueryMeta const& meta() const { return m_queryMeta; }

        friend std::ostream& operator<<(std::ostream& os, QueryBuilder q);

};

#endif // QUERYBUILDER_H
