#ifndef BLOCKMETA_H
#define BLOCKMETA_H

typedef unsigned char u8;

template <unsigned int... BlockSizes>
struct BlockMeta;

template <unsigned int BlockSizeA>
struct BlockMeta<BlockSizeA> {

		static_assert(BlockSizeA > 0, "Invalid block size");

		static unsigned int constexpr total_size = BlockSizeA;

		static unsigned int constexpr block_count = 1u;

		static unsigned int constexpr sizeAt(unsigned int) {
			return BlockSizeA;
		}

		static unsigned int constexpr offsetAt(unsigned int) {
			return 0u;
		}

};

template <unsigned int BlockSizeA, unsigned int... BlockSizes>
struct BlockMeta<BlockSizeA, BlockSizes...> {

		static_assert(BlockSizeA > 0, "Invalid block size");

		static unsigned int constexpr total_size = BlockSizeA + BlockMeta<BlockSizes...>::total_size;

		static unsigned int constexpr block_count = 1u + sizeof...(BlockSizes);

		static unsigned int constexpr sizeAt(unsigned int at) {
			return at == 0u ? BlockSizeA : BlockMeta<BlockSizes...>::sizeAt(at-1);
		}

		static unsigned int constexpr offsetAt(unsigned int at) {
			return at == 0u ? 0u : BlockSizeA + BlockMeta<BlockSizes...>::offsetAt(at-1);
		}

};

// TO MODIFY THE BLOCKS, SIMPLY MODIFY THE LINE BELOW
#define BLOCK_SIZES BlockMeta<4, 4, 4, 4, 6>

#define BLOCK_SIZE_AT(i) BLOCK_SIZES ::sizeAt(i)

#define BLOCK_OFFSET_AT(i) BLOCK_SIZES ::offsetAt(i)

#define BLOCK_OFFSET_END_AT(i) (BLOCK_SIZES ::sizeAt(i) + BLOCK_SIZES ::offsetAt(i))

#define BLOCK_TOTAL_SIZE BLOCK_SIZES ::total_size

#define BLOCK_COUNT BLOCK_SIZES ::block_count

static_assert(BLOCK_COUNT > 2, "There must be at least 3 blocks");

#define BLOCK_ERROR_THRESHOLD (BLOCK_COUNT-2)

#define BLOCK_MASK_AT(i) ((1 << (2*BLOCK_SIZE_AT(i)))-1)

#endif // BLOCKMETA_H
