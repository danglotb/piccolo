#include "RnaIndex.h"
#include <algorithm>
#include "BlockMeta.h"
#include "RnaDataBase.h"
//#include "IO/IODevice.h"
//#include "IO/DataStream.h"
#include "Query.h"

#define RNA_INDEX_HEADER "RNAComp_1-0-0"

#define RNA_INDEX_HEADER_SIZE (40-27)

// ===========================================================================
//		RnaIndex::RnaBlockPairIndex
// ===========================================================================

void RnaIndex::RnaBlockPairIndex::insertKey(BlockHash at, RnaIndex::Key k) {
	util::insert(m_blockIndex[at], k);
}

RnaIndex::RnaBlockPairIndex::RnaBlockPairIndex(BlockId blockA, BlockId blockB) : m_blockA(blockA), m_blockB(blockB),
	m_blockASize(BLOCK_SIZE_AT(blockA)), m_blockBSize(BLOCK_SIZE_AT(blockB)),
	m_blockAIndexSize(1 << (m_blockASize*2)), m_blockBIndexSize(1 << (m_blockBSize*2)),
	m_blockAOffset(BLOCK_OFFSET_AT(blockA)), m_blockBOffset(BLOCK_OFFSET_AT(blockB)),
	m_blockAOffsetEnd(m_blockAOffset+m_blockASize), m_blockBOffsetEnd(m_blockBOffset+m_blockBSize) {
	m_blockIndex.resize(m_blockAIndexSize*m_blockBIndexSize);
}

void RnaIndex::RnaBlockPairIndex::index(Key key, const nt* sequence, std::size_t seq_length) {
	if (seq_length < m_blockAOffsetEnd)
		return;

	BlockHash hA = util::hash(sequence + m_blockAOffset, sequence + m_blockAOffsetEnd) << (m_blockBSize*2);
	if (seq_length >= m_blockBOffsetEnd) {
		BlockHash hB = util::hash(sequence + m_blockBOffset, sequence + m_blockBOffsetEnd);
		insertKey(hA | hB, key);
	}
	else {
		BlockHash hB = util::hash(sequence + m_blockBOffset, sequence + seq_length) << ((m_blockBOffsetEnd-seq_length)*2);
		hA |= hB;
		for (uint i = 0, e = 1 << ((m_blockBOffsetEnd-seq_length)*2); i < e; i++)
			insertKey(hA | i, key);
	}
}

QueryResult RnaIndex::RnaBlockPairIndex::search(const Query& q) const {
	return QueryResult(m_blockIndex[(q.hashA() << (m_blockBSize*2)) | q.hashB()]);
}

void RnaIndex::RnaBlockPairIndex::clear() {
	std::for_each(m_blockIndex.begin(), m_blockIndex.end(), [](std::vector<Key>& v) { v.clear(); });
}

//void RnaIndex::RnaBlockPairIndex::save(DataStream& out) const {
//	auto non_empty = [](Store const& store) -> bool { return store.size(); };
//	out << (u32)std::count_if(m_blockIndex.begin(), m_blockIndex.end(), non_empty);

//	auto current = m_blockIndex.begin(), end = m_blockIndex.end();
//	while (current != end){
//		current = std::find_if(current, end, non_empty);
//		if (current == end)
//			break;
//		out << u32(current - m_blockIndex.begin());
//		out << *current;
//		++current;
//	}
//}

//void RnaIndex::RnaBlockPairIndex::load(DataStream& in) {
//	u32 non_empty;
//	in >> non_empty;
//	for (; non_empty > 0u; --non_empty) {
//		u32 index;
//		in >> index;
//		in >> m_blockIndex[index];
//	}
//}

// ===========================================================================
//		RnaIndex::RnaTailIndex
// ===========================================================================

QueryResult RnaIndex::RnaTailIndex::search(const nt* seq, const nt* end) const {
	QueryResult result;
	for (RnaTailIndex const* node = this; node; node = node->m_children[(unsigned char)*seq].get(), ++seq) {
		if (node->m_correspondingMiRna.size())
			result.mergedWith(node->m_correspondingMiRna.begin(), node->m_correspondingMiRna.end());
		if (seq == end)
			break;
	}
	return result;
}

void RnaIndex::RnaTailIndex::index(RnaIndex::Key key, const nt* sequence, const nt* seq_end) {
	if (sequence == seq_end)
		util::insert(m_correspondingMiRna, key);
	else {
		auto& child = m_children[(unsigned char)*sequence];
		if (!child)
			child.reset(new RnaTailIndex);
		child->index(key, sequence+1, seq_end);
	}
}

std::size_t RnaIndex::RnaTailIndex::childCount() const {
	std::size_t count = 0u;
	if (m_children[(u8)nt::A])
		count++;
	if (m_children[(u8)nt::C])
		count++;
	if (m_children[(u8)nt::G])
		count++;
	if (m_children[(u8)nt::U])
		count++;
	return count;
}

void RnaIndex::RnaTailIndex::clear() {
	m_children[(u8)nt::A] = nullptr;
	m_children[(u8)nt::C] = nullptr;
	m_children[(u8)nt::G] = nullptr;
	m_children[(u8)nt::U] = nullptr;
}

//void RnaIndex::RnaTailIndex::save(DataStream& out) const {
//	out << m_correspondingMiRna;
//	out << (u8)childCount();
//	for (u8 i = 0; i < m_children.size(); i++) {
//		if (m_children[i]) {
//			out << i;
//			m_children[i]->save(out);
//		}
//	}
//}

//void RnaIndex::RnaTailIndex::load(DataStream& in) {
//	in >> m_correspondingMiRna;
//	u8 childCount;
//	in >> childCount;
////	u8 lastId = 0u;
//	for (u8 i = 0; i < childCount; i++) {
//		u8 childNt;
//		in >> childNt;
////		while (lastId < childNt)
////			m_children[lastId++] = nullptr;
////		lastId++; // lastId = childNt+1;
//		m_children[childNt].reset(new RnaTailIndex);
//		m_children[childNt]->load(in);
//	}
//}

// ===========================================================================
//		RnaIndex::RnaBlockPairTailIndex
// ===========================================================================

RnaIndex::RnaBlockPairTailIndex::RnaBlockPairTailIndex(BlockId blockA) : m_blockA(blockA),
	m_blockASize(BLOCK_SIZE_AT(blockA)),
	m_blockAIndexSize(1 << (m_blockASize*2)),
	m_blockAOffset(BLOCK_OFFSET_AT(blockA)),
	m_blockAOffsetEnd(m_blockAOffset+m_blockASize) {
	m_tailIndex.resize(m_blockAIndexSize*m_blockBIndexSize);
}

QueryResult RnaIndex::RnaBlockPairTailIndex::search(const Query& q) const {
	auto& ptr = m_tailIndex[(q.hashA() << (m_blockBSize*2)) | q.hashB()];
	if (ptr)
		return ptr->search(q.tail(), q.tailEnd());
	return QueryResult();
}

void RnaIndex::RnaBlockPairTailIndex::index(RnaIndex::Key key, const nt* sequence, std::size_t seq_length) {
	if (seq_length < m_blockAOffsetEnd)
		return;

	auto index_seq = [this](BlockHash at, RnaIndex::Key key, const nt* sequence, const nt* sequence_end) {
		auto& ptr = m_tailIndex[at];
		if (!ptr)
			ptr.reset(new RnaTailIndex);
		ptr->index(key, sequence, sequence_end);
	};

	BlockHash hA = util::hash(sequence + m_blockAOffset, sequence + m_blockAOffsetEnd) << (m_blockBSize*2);
	if (seq_length >= m_blockBOffsetEnd) {
		BlockHash hB = util::hash(sequence + m_blockBOffset, sequence + m_blockBOffsetEnd);
		index_seq(hA | hB, key, sequence + m_blockBOffsetEnd, sequence + seq_length);
	}
	else {
		BlockHash hB = util::hash(sequence + m_blockBOffset, sequence + seq_length) << ((m_blockBOffsetEnd-seq_length)*2);
		hA |= hB;
		const nt* sequence_end = sequence + seq_length;
		for (uint i = 0, e = 1 << ((m_blockBOffsetEnd-seq_length)*2); i < e; i++)
			index_seq(hA | i, key, sequence_end, sequence_end);
	}
}

void RnaIndex::RnaBlockPairTailIndex::clear() {
	std::for_each(m_tailIndex.begin(), m_tailIndex.end(), [](std::unique_ptr<RnaTailIndex>& ptr) { ptr = nullptr; });
}

//void RnaIndex::RnaBlockPairTailIndex::save(DataStream& out) const {
//	auto not_null = [](std::unique_ptr<RnaTailIndex> const& ptr) -> bool { return (bool)ptr; };
//	out << (u32)std::count_if(m_tailIndex.begin(), m_tailIndex.end(), not_null);

//	auto current = m_tailIndex.begin(), end = m_tailIndex.end();
//	while (current != end){
//		current = std::find_if(current, end, not_null);
//		if (current == end)
//			break;
//		out << u32(current - m_tailIndex.begin());
//		(*current)->save(out);
//		++current;
//	}
//}

//void RnaIndex::RnaBlockPairTailIndex::load(DataStream& in) {
//	u32 size;
//	in >> size;
//	for (; size > 0u; --size) {
//		u32 index;
//		in >> index;
//		m_tailIndex[index].reset(new RnaTailIndex);
//		m_tailIndex[index]->load(in);
//	}
//}

// ===========================================================================
//		RnaIndex
// ===========================================================================

RnaIndex::RnaIndex() {
	for (BlockId i = 0; i < BLOCK_COUNT-1; i++) for (BlockId j = i+1; j < BLOCK_COUNT-1; j++)
		accessPairIndex(i, j) = RnaBlockPairIndex(i, j);
	for (BlockId i = 0; i < BLOCK_COUNT-1; i++)
		m_tailIndex[i] = RnaBlockPairTailIndex(i);
}

void RnaIndex::buildIndex() {
	Key key = 0u;
	for (auto const& entry : *this) {
		std::vector<nt> const& nt_seq = entry.second;
		for (BlockId i = 0; i < BLOCK_COUNT-1; i++) for (BlockId j = i+1; j < BLOCK_COUNT-1; j++)
			accessPairIndex(i, j).index(key, nt_seq.data(), nt_seq.size());
		for (BlockId i = 0; i < BLOCK_COUNT-1; i++)
			m_tailIndex[i].index(key, nt_seq.data(), nt_seq.size());
		key++;
	}
}

void RnaIndex::clear() {
	if (size()) {
		RnaDataBase::clear();
		for (BlockId i = 0; i < BLOCK_COUNT-1; i++) for (BlockId j = i+1; j < BLOCK_COUNT-1; j++)
			accessPairIndex(i, j).clear();
		for (BlockId i = 0; i < BLOCK_COUNT-1; i++)
			m_tailIndex[i].clear();
	}
}

bool RnaIndex::parse(std::istream& file) {
	if (RnaDataBase::parse(file)) {
		buildIndex();
		return true;
	}
	return false;
}

//void RnaIndex::writeBlockInformation(DataStream& out) const {
//	out << (u32)BLOCK_COUNT;
//	for (uint i = 0; i < BLOCK_COUNT; i++)
//		out << (u32)BLOCK_SIZE_AT(i);
//}

//bool RnaIndex::checkBlockInformation(DataStream& in) const {
//	u32 blockCount;
//	in >> blockCount;
//	if (blockCount != BLOCK_COUNT)
//		return false;
//	for (uint i = 0; i < BLOCK_COUNT; i++) {
//		u32 blockSize;
//		in >> blockSize;
//		if (blockSize != BLOCK_SIZE_AT(i))
//			return false;
//	}
//	return true;
//}


//bool RnaIndex::save(const std::string& path, std::string& error) const {
//	try {
//		FileDevice file(path);
//		file.open(IOMode::WriteOnly);
//		DataStream out(file);

//		out << RNA_INDEX_HEADER;
//		writeBlockInformation(out);

//		RnaDataBase::save(out);

//		for (BlockId i = 0; i < BLOCK_COUNT-1; i++) for (BlockId j = i+1; j < BLOCK_COUNT-1; j++)
//			accessPairIndex(i, j).save(out);
//		for (BlockId i = 0; i < m_tailIndex.size(); i++)
//			m_tailIndex[i].save(out);
//	}
//	catch (Exception const& e) {
//		error = e.what();
//		return false;
//	}
//	return true;
//}

//LoadResult RnaIndex::load(const std::string& path, std::string& error) {
//	clear();
//	try {
//		FileDevice file(path);
//		file.open(IOMode::ReadOnly);
//		DataStream in(file);

//		char header[RNA_INDEX_HEADER_SIZE+1];
//		in.read(header, sizeof(header));

//		if (!std::equal(header, header + sizeof(header), RNA_INDEX_HEADER)) {
//			error = "The file doesn't seem to be written by this software (incorrect header).";
//			return LoadResult::FileError;
//		}
//		if (!checkBlockInformation(in)) {
//			error = "The index was written with incorrect block sizes. Please re-index the file.";
//			return LoadResult::IncompatibleIndex;
//		}

//		RnaDataBase::load(in);
////		buildIndex();

//		for (BlockId i = 0; i < BLOCK_COUNT-1; i++) for (BlockId j = i+1; j < BLOCK_COUNT-1; j++)
//			accessPairIndex(i, j).load(in);
//		for (BlockId i = 0; i < m_tailIndex.size(); i++)
//			m_tailIndex[i].load(in);
//	}
//	catch (Exception const& e) {
//		error = e.what();
//		return LoadResult::FileError;
//	}
//	return LoadResult::Success;
//}

QueryResult RnaIndex::search(const Query& q) const {
	if (q.blockB() == BLOCK_COUNT-1)
		return m_tailIndex[q.blockA()].search(q);
	else
		return accessPairIndex(q.blockA(), q.blockB()).search(q);
}
