#ifndef RNADATABASE_H
#define RNADATABASE_H

#include <vector>
#include <utility>
#include <string>

#include "Util.h"

//class DataStream;

class RnaDataBase {

		std::vector<MiRnaEntry> m_sequenceList;
		std::size_t m_maxRnaLength = 0u;

		static std::size_t safeGetline(std::istream& is, char* buffer, std::size_t maxlen);

	protected:
//		void save(DataStream& out) const;
//		void load(DataStream& in);

	public:
		RnaDataBase() {}
		virtual ~RnaDataBase() {}

		bool parse(char const* path);
		virtual bool parse(std::istream& file);

		virtual void clear() { m_sequenceList.clear(); m_maxRnaLength =  0u; }

		std::size_t size() const { return m_sequenceList.size(); }

		std::vector<MiRnaEntry>::const_iterator begin() const { return m_sequenceList.begin(); }
		std::vector<MiRnaEntry>::const_iterator end() const { return m_sequenceList.end(); }

		MiRnaEntry const& at(std::size_t id) const { return m_sequenceList[id]; }

		std::size_t maxRnaLength() const;
};

#endif // RNADATABASE_H

