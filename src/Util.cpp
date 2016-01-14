#include "Util.h"

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

void util::tabular2fasta(const char* in_path, const char* out_path) {
    std::ifstream in_file(in_path, std::ios_base::in);
    std::ofstream out_file(out_path);

    std::string line;
    if (in_file.is_open() && out_file.is_open()) {

        //RemoveHeader, we do not care about it (3 lines)
        getline(in_file,line);
        getline(in_file,line);
        getline(in_file,line);

        while ( getline(in_file,line) ) {
            std::vector<std::string> elems;
            std::stringstream ss(line);
            std::string item;
            while (std::getline(ss, item, '\t'))
                elems.push_back(item);
            out_file << ">" << elems.at(0) << " | " << elems.at(1) << std::endl;
            out_file << elems.at(0) << std::endl;
        }
        in_file.close();
        out_file.close();
    } else
        std::cout << "Unable to open file";

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
