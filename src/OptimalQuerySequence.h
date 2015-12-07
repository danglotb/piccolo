#ifndef OPTIMALQUERYSEQUENCE_H
#define OPTIMALQUERYSEQUENCE_H

#include "QueryBuilder.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>

template <bool DisplayQuerySequence = false>
struct Assigner {

        std::vector<QueryBuilder>& m_queries;
        std::vector<std::vector<QueryBuilder>::iterator>& m_queryEnds;

        Assigner(std::vector<QueryBuilder>& queries, std::vector<std::vector<QueryBuilder>::iterator>& queryEnds) : m_queries(queries),
        m_queryEnds(queryEnds) {}

        void reserveQueryMemory(unsigned int amount) {
            m_queries.reserve(amount);
        }

        void addQuery(unsigned int blockIdA, unsigned int blockIdB, unsigned int blockOffset) {
            m_queries.emplace_back(blockIdA, blockIdB, blockOffset);
        }

        void addQuery(unsigned int blockIdA, unsigned int blockIdB, unsigned int blockOffsetA, unsigned int blockOffsetB) {
            m_queries.emplace_back(blockIdA, blockIdB, blockOffsetA, blockOffsetB);
        }

        void addErrorCutoff(unsigned int queryPastEndIndex) {
            m_queryEnds.push_back(m_queries.begin()+queryPastEndIndex);
        }

};

template <>
struct Assigner<true> : Assigner<false> {

        typedef Assigner<false> MainClass;

        using MainClass::MainClass;

        void reserveQueryMemory(unsigned int amount) {
            MainClass::reserveQueryMemory(amount);
            std::cout << "Allocating space for " << amount << " queries." << std::endl;
        }

        void addQuery(unsigned int blockIdA, unsigned int blockIdB, unsigned int blockOffset) {
            MainClass::addQuery(blockIdA, blockIdB, blockOffset);
            std::cout << "Adding query for block: (" << blockIdA << ", " << blockIdB << ") offset: " << blockOffset << std::endl;
        }

        void addErrorCutoff(unsigned int queryPastEndIndex) {
            MainClass::addErrorCutoff(queryPastEndIndex);
            std::cout << "Adding a cut-off at " << queryPastEndIndex << std::endl;
        }

};

struct BlockQuery {

        unsigned int blockA;
        unsigned int blockB;
        unsigned int allowedInbetweenErrors;

        BlockQuery() {}
        BlockQuery(unsigned int blockA, unsigned int blockB) : blockA(blockA), blockB(blockB), allowedInbetweenErrors(blockB - blockA - 1) {}

        BlockQuery(BlockQuery const&) = default;

        bool operator<(BlockQuery const& other) const {
            return blockA < other.blockA || (blockA == other.blockA && blockB < other.blockB);
        }

};

struct BlockQueryGap : public BlockQuery {

        int gap_blockA;
        int gap_between_block;

        BlockQueryGap() {}
        BlockQueryGap(unsigned int blockA, unsigned int blockB,int gapBlockA, int gapBetweenBlock) : BlockQuery(blockA,blockB), gap_blockA(gapBlockA), gap_between_block(gapBetweenBlock) {}

        BlockQueryGap(BlockQueryGap const&) = default;

        bool operator<(BlockQueryGap const& other) const {
            return blockA < other.blockA || (blockA == other.blockA && blockB < other.blockB);
        }

};

typedef unsigned int Error;

typedef std::vector<Error> ErrorDistribution;
typedef std::vector<ErrorDistribution> ErrorDistributionList;

typedef std::vector<BlockQuery> QuerySequence;

typedef std::vector<QuerySequence> SetOfQuerySequence;

// The first query sequence is for 0 errors, the second for 1 error, the 3rd for 2 errors...
typedef std::vector<QuerySequence> OptimalQuerySequence;

class OptimalQuerySequenceBuilder {

public:

        OptimalQuerySequenceBuilder(OptimalQuerySequenceBuilder&&) = default;
        OptimalQuerySequenceBuilder();
        OptimalQuerySequenceBuilder(bool isomir_mode);

        std::vector<QueryBuilder> m_queries;
        std::vector<std::vector<QueryBuilder>::iterator> m_queryEnds;

        std::vector<QueryBuilder>::iterator begin() { return m_queries.begin(); }
        std::vector<QueryBuilder>::iterator end() { return m_queries.end(); }

//        std::vector<BlockQuery>::iterator begin() {return _sequence.begin();}
//        std::vector<BlockQuery>::iterator end() {return _sequence.end();}

        std::vector<QueryBuilder>::iterator endForErrorThreshold(int threshold) {
            return m_queryEnds[threshold];
        }

        static OptimalQuerySequence computeOptimalQuerySequence();

        static void generateCppCodeForOptimalQuerySequence();

    private:

        void computeSequenceForIsomir();

        typedef std::unordered_map<std::string, SetOfQuerySequence> dynmatrix;
        typedef std::unordered_set<std::string> dynset;

        static ErrorDistributionList errorSet(uint errorThreshold);

        static void generate_errors_loop_init(ErrorDistributionList& list, unsigned int NestedLoopCount, bool LoopContinues);
        static void generate_errors_loop_main(ErrorDistributionList& list, unsigned int NestedLoopCount, bool LoopContinues, std::vector<uint> ids);

        static QuerySequence longuestQuerySequence();

        static bool evaluateQuerySequence(QuerySequence const& q, ErrorDistribution const& errors);
        static bool evaluateQuerySequence(QuerySequence const& q, ErrorDistributionList const& errors);

        static std::string querySequenceToString(QuerySequence const& querySeq);

        static unsigned int scoreQuerySequence(QuerySequence const& q, uint errorThreshold);
        static unsigned int scoreOptimalQuerySequence(OptimalQuerySequence const& q);
        static unsigned int scoreOptimalQuerySequence(OptimalQuerySequence const& q, uint errorCutoff);

        static SetOfQuerySequence findOptimalSetOfQuerySequence(uint errorThreshold);
        static SetOfQuerySequence findOptimalSetOfQuerySequence_rec(QuerySequence const& querySeq, std::string const& querySeqString,
                                                                    ErrorDistributionList const& errors, dynmatrix& cache);

        static void computeOptimalQuerySequence(const std::vector<SetOfQuerySequence>& optimalQueries, uint errorThreshold,
                                                OptimalQuerySequence const& optimalBeingBuilt, unsigned int currentScore,
                                                QuerySequence const& concatenatedOptimalBeingBuilt,
                                                OptimalQuerySequence& found,
                                                unsigned int& foundScore);
        static void finalizeComputeOptimalQuerySequence(const std::vector<SetOfQuerySequence>& optimalQueries,
                                                        OptimalQuerySequence const& optimalBeingBuilt, unsigned int currentScore,
                                                        QuerySequence const& concatenatedOptimalBeingBuilt,
                                                        OptimalQuerySequence& found,
                                                        unsigned int& foundScore);
};

#endif // OPTIMALQUERYSEQUENCE_H
