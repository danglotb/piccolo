#include "Query.h"


std::ostream& operator<<(std::ostream& os, Query q) {

    os << q.m_blockA << " ; " << q.m_blockB << " : " << q.m_hashA << " ; " << q.m_hashB;

    return os;

}
