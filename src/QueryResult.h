#ifndef QUERYRESULT_H
#define QUERYRESULT_H

#include <vector>
#include <string>

#include "Util.h"

//typedef std::vector<std::pair<std::string, std::string> const*> QueryResult;

class QueryResult {

	public:
		typedef std::size_t Key;

	private:
		std::vector<Key> m_result;

	public:
		QueryResult() {}

		explicit QueryResult(std::vector<Key> const& other) : m_result(other) {}
		explicit QueryResult(std::vector<Key>&& other) : m_result(std::move(other)) {}

		QueryResult(QueryResult const&) = default;
		QueryResult(QueryResult&&) = default;

		QueryResult& operator=(QueryResult const&) = default;
		QueryResult& operator=(QueryResult&&) = default;

		bool empty() { return m_result.empty(); }

		std::size_t size() const { return m_result.size(); }

		std::vector<Key>::const_iterator begin() const { return m_result.begin(); }
		std::vector<Key>::const_iterator end() const { return m_result.end(); }

		void mergedWith(Key k);
		void mergedWith(QueryResult const& qr) { m_result.insert(m_result.end(), qr.begin(), qr.end()); }
		void mergedWith(QueryResult&& qr) {
			m_result.insert(m_result.end(), std::make_move_iterator(qr.m_result.begin()), std::make_move_iterator(qr.m_result.end()));
		}

		template <class InputIterator>
		void mergedWith(InputIterator begin, InputIterator end) { m_result.insert(m_result.end(), begin, end); }


};

#endif // QUERYRESULT_H
