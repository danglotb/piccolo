#include "Util.h"

#include <iostream>
#include <algorithm>

nt util::asciiToNt(char ascii) {
	switch (ascii) {
		case 'A':
		case 'a':
			return nt::A;
		case 'C':
		case 'c':
			return nt::C;
		case 'G':
		case 'g':
			return nt::G;
		case 'U':
		case 'u':
		case 'T':
		case 't':
			return nt::U;
		default:
			std::cerr << "Unexpected character in sequence: '" << ascii << "'. Replacing it by A." << std::endl;
			return nt::A;
	}
}

char util::ntToAscii(nt n) {
	switch (n) {
		case nt::A:
			return 'A';
		case nt::C:
			return 'C';
		case nt::G:
			return 'G';
		default:
			return 'U';
	}
}

BlockHash util::hash(nt const* begin, nt const* end) {
	BlockHash h = 0u;
	while (begin != end) {
		h <<= 2;
		h |= (unsigned char)*begin;
		++begin;
	}
	return h;
}

std::vector<nt> util::stringToNt(const std::string& seq) {
	std::vector<nt> nt_seq(seq.size());
	std::transform(seq.begin(), seq.end(), nt_seq.begin(), util::asciiToNt);
	return nt_seq;
}

std::string util::ntToString(const std::vector<nt>& seq) {
	std::string ascii_seq;
	ascii_seq.resize(seq.size());
	std::transform(seq.begin(), seq.end(), ascii_seq.begin(), util::ntToAscii);
	return ascii_seq;
}

std::ostream& operator<<(std::ostream& os, const nt n) {
    os << util::ntToAscii(n);
    return os;
}

std::ostream& operator<<(std::ostream& os, const RnaSequence& s) {
    for (nt n : s)
        os << n;
    return os;
}

//std::ostream& operator<<(std::ostream& os, const nt s) {

//    os << s ;

//    return os;
//}

//DataStream& operator<<(DataStream& out, nt n) {
//	return out << (u8)n;
//}


//DataStream& operator>>(DataStream& in, nt n) {
//	in.read(reinterpret_cast<char*>(&n), 1);
//	return in;
//}
