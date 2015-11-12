#include "RnaDataBase.h"

#include <fstream>
#include <algorithm>

//#include "IO/DataStream.h"


std::size_t RnaDataBase::maxRnaLength() const
{
	return m_maxRnaLength;
}

std::size_t RnaDataBase::safeGetline(std::istream& is, char* buffer, std::size_t maxlen) {

	// The characters in the stream are read one-by-one using a std::streambuf.
	// That is faster than reading them one-by-one using the std::istream.
	// Code that uses streambuf this way must be guarded by a sentry object.
	// The sentry object performs various tasks,
	// such as thread synchronization and updating the stream state.

	std::istream::sentry se(is, true);
	std::streambuf* sb = is.rdbuf();

	for (char* begin = buffer, *end = buffer + maxlen; buffer < end; buffer++) {
		int c = sb->sbumpc();
		switch (c) {
			case '\n':
				return buffer - begin;
			case '\r':
				if(sb->sgetc() == '\n')
					sb->sbumpc();
				return buffer - begin;
			case EOF:
				// Also handle the case when the last line has no line ending
				if (buffer == begin)
					is.setstate(std::ios::eofbit);
				return buffer - begin;
			default:
				*buffer = (char)c;
		}
	}
	return maxlen;
}

//void RnaDataBase::save(DataStream& out) const {
//	out << m_sequenceList;
//}

//void RnaDataBase::load(DataStream& in) {
//	in >> m_sequenceList;
//}

bool RnaDataBase::parse(const char* path) {
	std::ifstream file(path, std::ios_base::in);
	return parse(file);
}

bool RnaDataBase::parse(std::istream& file) {
//	std::ifstream file(multifasta, std::ios_base::in);
	if (file.fail())
		return false;

	clear();

	char lineBuffer[1024];
	bool entryFound = false;

	MiRnaEntry currentSequence;

	while (file.good()) {
		std::size_t lineSize = safeGetline(file, lineBuffer, sizeof(lineBuffer));
		if (lineSize == 0u)
			continue;
		if (lineBuffer[0u] == '>') {
			if (entryFound) {
				if (m_maxRnaLength < currentSequence.second.size())
					m_maxRnaLength = currentSequence.second.size();
				m_sequenceList.push_back(currentSequence);
				currentSequence.second.clear();
			}
			currentSequence.first.assign(lineBuffer+1, lineBuffer+lineSize);
			while (lineSize == sizeof(lineBuffer)) {
				lineSize = safeGetline(file, lineBuffer, sizeof(lineBuffer));
				currentSequence.first.insert(currentSequence.first.end(), lineBuffer, lineBuffer+lineSize);
			}
			entryFound = true;
			continue;
		}
		std::transform(lineBuffer, lineBuffer+lineSize, std::back_inserter(currentSequence.second), util::asciiToNt);
//		currentSequence.second.append();
	}

	if (currentSequence.first.size() && currentSequence.second.size()) {
		if (m_maxRnaLength < currentSequence.second.size())
			m_maxRnaLength = currentSequence.second.size();
		m_sequenceList.push_back(currentSequence);
	}

	return true;
}
