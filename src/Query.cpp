#include "Query.h"


std::ostream& operator<<(std::ostream& os, Query q) {
    os << q.m_blockA << " ; " << q.m_blockB << " : " << q.m_hashA << " ; " << q.m_hashB;
    return os;
}

std::ostream& operator<<(std::ostream& os, const QueryGlobal& b) {
    os << b.m_offsetA << "[" << b.m_blockA << ";" << b.m_blockB << ":" << b.m_offsetB << "]" << std::endl;
    return os;
}
