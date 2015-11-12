#ifndef RNAINDEX_H
#define RNAINDEX_H

#include <vector>
#include <string>
#include <array>

#include "BlockMeta.h"
#include <memory>
#include "QueryResult.h"

class Query;
//class DataStream;

#include "RnaDataBase.h"

#include "Util.h"

enum class LoadResult { FileError, IncompatibleIndex, Success };

class RnaIndex : public RnaDataBase {

		typedef std::size_t Key;

		typedef std::vector<Key> Store;

		class RnaBlockPairIndex {

				std::vector<Store> m_blockIndex;

				BlockId m_blockA;
				BlockId m_blockB;

				unsigned int m_blockASize;
				unsigned int m_blockBSize;

				unsigned int m_blockAIndexSize;
				unsigned int m_blockBIndexSize;

				unsigned int m_blockAOffset;
				unsigned int m_blockBOffset;
				unsigned int m_blockAOffsetEnd;
				unsigned int m_blockBOffsetEnd;

				void insertKey(BlockHash at, Key k);

			public:
				RnaBlockPairIndex() : RnaBlockPairIndex(0, 0) {}
				RnaBlockPairIndex(BlockId blockA, BlockId blockB);

				void index(Key key, nt const* sequence, std::size_t seq_length);

				QueryResult search(const Query &q) const;

				void clear();

//				void save(DataStream& out) const;
//				void load(DataStream& in);

		};

		class RnaTailIndex {

				Store m_correspondingMiRna;
				// 4 children, one for each nt (A, C, G, U)
				// The pointers may be null
				std::array<std::unique_ptr<RnaTailIndex>, 4> m_children;

			public:
				QueryResult search(nt const* seq, nt const* end) const;

				void index(Key key, nt const* sequence, nt const* seq_end);

				std::size_t childCount() const;

				void clear();

//				void save(DataStream& out) const;
//				void load(DataStream& in);

		};

		class RnaBlockPairTailIndex {

				std::vector<std::unique_ptr<RnaTailIndex>> m_tailIndex;

				BlockId m_blockA;
				static BlockId constexpr m_blockB = BLOCK_COUNT-1;

				unsigned int m_blockASize;
				static unsigned int constexpr m_blockBSize = BLOCK_SIZE_AT(BLOCK_COUNT-1);

				unsigned int m_blockAIndexSize;
				static unsigned int constexpr m_blockBIndexSize = 1 << (BLOCK_SIZE_AT(BLOCK_COUNT-1)*2);

				unsigned int m_blockAOffset;
				static unsigned int constexpr m_blockBOffset = BLOCK_OFFSET_AT(BLOCK_COUNT-1);
				unsigned int m_blockAOffsetEnd;
				static unsigned int constexpr m_blockBOffsetEnd = BLOCK_OFFSET_END_AT(BLOCK_COUNT-1);

			public:
				RnaBlockPairTailIndex() : RnaBlockPairTailIndex(0u) {}
				RnaBlockPairTailIndex(BlockId blockA);

				QueryResult search(Query const& q) const;

				void index(Key key, nt const* sequence, std::size_t seq_length);

				void clear();

//				void save(DataStream& out) const;
//				void load(DataStream& in);

		};

		std::array<std::array<RnaBlockPairIndex, BLOCK_COUNT-1>, BLOCK_COUNT-1> m_blockIndex;
		std::array<RnaBlockPairTailIndex, BLOCK_COUNT-1> m_tailIndex;

		RnaBlockPairIndex& accessPairIndex(BlockId blockA, BlockId blockB) {
			return m_blockIndex[blockA][blockB];
			// If no extra memory:
			// #define N BLOCK_COUNT-1
			// #define OFFSET 2*N+1
			// m_blockIndex[ ((OFFSET-q.blockA())*q.blockA())/2 + q.blockB()-q.blockA()-1 ]
		}
		RnaBlockPairIndex const& accessPairIndex(BlockId blockA, BlockId blockB) const {
			return m_blockIndex[blockA][blockB];
		}

//		void writeBlockInformation(DataStream& out) const;
//		bool checkBlockInformation(DataStream& in) const;

		void buildIndex();

	public:
		RnaIndex();

		void clear();

		using RnaDataBase::parse;
		bool parse(std::istream& file);

//		bool save(std::string const& path, std::string& error) const;
//		LoadResult load(std::string const& path, std::string& error);

		bool isEmpty() const;

		QueryResult search(Query const& q) const;

};

#endif // RNAINDEX_H
