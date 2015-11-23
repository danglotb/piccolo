#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>

//typedef unsigned char nt;
typedef unsigned int BlockId;
typedef unsigned int BlockHash;

enum class nt : unsigned char {
	A = 0,
	C = 1,
	G = 2,
	U = 3
};

//#include "IO/DataStream.h"

//DataStream& operator<<(DataStream& out, nt n);
//DataStream& operator>>(DataStream& in, nt n);

typedef std::vector<nt> RnaSequence;
typedef std::pair<std::string, RnaSequence> MiRnaEntry;

std::ostream& operator<<(std::ostream& os, const nt* s);

std::ostream& operator<<(std::ostream& os, const RnaSequence& s);

namespace util {
	nt asciiToNt(char ascii);
	char ntToAscii(nt n);

	std::vector<nt> stringToNt(std::string const& seq);
	std::string ntToString(const std::vector<nt>& seq);

	BlockHash hash(nt const* begin, nt const* end);

	template <class T>
	void insert(std::vector<T>& vec, T val) {
		vec.push_back(val);
	}
}

#endif // UTILIL_H
