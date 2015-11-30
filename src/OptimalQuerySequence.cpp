#include "OptimalQuerySequence.h"
#include "BlockMeta.h"

// TEMPLATE CODE

#include "OptimalQuerySequenceTemplate.h"

//OptimalQuerySequenceBuilder::OptimalQuerySequenceBuilder() {
//    Assigner<false /* replace by 'true' to print the query sequence to the standard output */> assigner(m_queries, m_queryEnds);
//    compute_and_assign_optimal_query_sequence<BLOCK_SIZES, BLOCK_ERROR_THRESHOLD>::assign(assigner);
//}

//OptimalQuerySequenceBuilder::OptimalQuerySequenceBuilder(unsigned int k, bool with_gap) {
//    if (with_gap)
//        init_sequence_with_gap(k);
//    else
//        init_sequence_without_gap(k);
//}

//void OptimalQuerySequenceBuilder::init_sequence_without_gap(unsigned int k) {
//    for (unsigned int x = 0 ; x < k + 1 ; x++) {
//        for (unsigned int i = x + 1 ; i < k + 2 ; i++) {
//            _sequence.push_back(BlockQuery(x,i));
//        }
//    }
//}

//void OptimalQuerySequenceBuilder::init_sequence_with_gap(unsigned int k) {
//    const int STAR = 0;
//    for (unsigned int x = 0 ; x < k + 1 ; x++) {
//        for (unsigned int i = x + 1 ; i < k + 2 ; i++) {
//            unsigned int e = x>0?x:0;
//            unsigned int j = i - x - 1;
//            BlockQueryGap b;
//            if (e + j == k) {
//                if (x == i - 1)
//                    b  = BlockQueryGap(x,i,STAR,STAR);
//                else
//                    b = BlockQueryGap(x,i,e,STAR);
//            } else
//                b = BlockQueryGap(x,i,e,j);
//            _sequence.push_back(b);
//        }
//    }
//}

#include <algorithm>

OptimalQuerySequenceBuilder::OptimalQuerySequenceBuilder() {
    Assigner<false /* replace by 'true' to print the query sequence to the standard output */> assigner(m_queries, m_queryEnds);

    // TO USE THE META PROGRAMMING VERSION, USE THE FOLLOWING LINE
    // THIS WILL COMPUTE THE OPTIMAL SEQUENCE VIA A TEMPLATE PROGRAM. BUT IT USES TOO MUCH MEMORY FOR LARGE BLOCK COUNT
        //	typedef meta_prog::compute_optimal_query_sequence<BLOCK_ERROR_THRESHOLD, BLOCK_COUNT>::type PrecomputedOptimalQuerySequence;
    // OTHERWISE, JUST PASTE THE OUTPUT OF OptimalQuerySequenceBuilder::generateCppCodeForOptimalQuerySequence() here:
    // ====================================================================================
    //		PASTE BEGIN
    // ====================================================================================
    //Paste the code below in the constructor of OptimalQuerySequenceBuilder:
    #ifndef RNACOMP_COMPUTE_OPTIMAL_QUERY_SEQUENCE
    static_assert(BLOCK_COUNT == 5, "The optimal query sequence and the block configuration don't match. Please recompile with RNACOMP_COMPUTE_OPTIMAL_QUERY_SEQUENCE (#define). Then run the program and paste the output in the constructor of OptimalQuerySequenceBuilder.");
    #endif
    typedef meta_prog::OptimalQuerySequence<
        meta_prog::QuerySequence<
            meta_prog::BlockQuery<0, 1>
        >,
        meta_prog::QuerySequence<
            meta_prog::BlockQuery<3, 4>
        >,
        meta_prog::QuerySequence<
            meta_prog::BlockQuery<2, 3>,
            meta_prog::BlockQuery<2, 4>
        >,
        meta_prog::QuerySequence<
            meta_prog::BlockQuery<0, 2>,
            meta_prog::BlockQuery<0, 3>,
            meta_prog::BlockQuery<0, 4>,
            meta_prog::BlockQuery<1, 2>,
            meta_prog::BlockQuery<1, 3>,
            meta_prog::BlockQuery<1, 4>
        >
    > PrecomputedOptimalQuerySequence;
    // ====================================================================================
    //		PASTE END
    // ====================================================================================
    assign_optimal_query_sequence<BLOCK_SIZES, PrecomputedOptimalQuerySequence>::assign(assigner);
}

ErrorDistributionList OptimalQuerySequenceBuilder::errorSet(uint errorThreshold) {
    ErrorDistributionList result;
    generate_errors_loop_init(result, errorThreshold, BLOCK_COUNT > 0);

//	for (ErrorDistribution const& d : result) {
//		std::cout << "Error distribution [" << errorThreshold << "]: ";
//		for (unsigned int e : d)
//			std::cout << e << " ";
//		std::cout << std::endl;
//	}
    return result;
}

void OptimalQuerySequenceBuilder::generate_errors_loop_init(ErrorDistributionList& list, unsigned int NestedLoopCount, bool LoopContinues) {
//	std::cout << "INIT: (" << NestedLoopCount << ", " << LoopContinues << ")" << std::endl;
    if (NestedLoopCount == 0u || !LoopContinues)
        return;
    std::vector<uint> ids;
    ids.push_back(0u);
    generate_errors_loop_main(list, NestedLoopCount-1, true, ids);
}

void OptimalQuerySequenceBuilder::generate_errors_loop_main(ErrorDistributionList& list, unsigned int NestedLoopCount, bool LoopContinues, std::vector<uint> ids) {
//	std::cout << "MAIN: (" << NestedLoopCount << ", " << LoopContinues << "); ids={";
//	for (uint i : ids)
//		std::cout << i << " ";
//	std::cout << "}" << std::endl;
    if (!LoopContinues)
        return;
    if (NestedLoopCount == 0u) {
        list.push_back(ids);
        ids.back()++;
        generate_errors_loop_main(list, 0u, ids.back() < BLOCK_COUNT, ids);
        return;
    }
    else {
        uint blockCount = BLOCK_COUNT;
        ids.push_back(std::min(ids.back()+1, blockCount));
        generate_errors_loop_main(list, NestedLoopCount-1, ids.back() < BLOCK_COUNT, ids);
        ids.pop_back();
        ids.back()++;
        generate_errors_loop_main(list, NestedLoopCount, ids.back() < BLOCK_COUNT, ids);
    }
}

QuerySequence OptimalQuerySequenceBuilder::longuestQuerySequence() {
    QuerySequence sequence;
    for (uint i = 0; i < BLOCK_COUNT; ++i) for (uint j = i+1; j < BLOCK_COUNT; ++j) {
        sequence.push_back(BlockQuery(i, j));
    }
    return sequence;
}

bool OptimalQuerySequenceBuilder::evaluateQuerySequence(const QuerySequence& q, const ErrorDistribution& errors) {
    return std::any_of(q.begin(), q.end(), [&errors](BlockQuery b) {
        return std::find(errors.begin(), errors.end(), b.blockA) == errors.end() && std::find(errors.begin(), errors.end(), b.blockB) == errors.end();
    });
}

bool OptimalQuerySequenceBuilder::evaluateQuerySequence(const QuerySequence& q, const ErrorDistributionList& errors) {
    return std::all_of(errors.begin(), errors.end(), [&q](ErrorDistribution const& e) { return evaluateQuerySequence(q, e); });
}

std::string OptimalQuerySequenceBuilder::querySequenceToString(const QuerySequence& querySeq) {
    std::string str;
    str.reserve(querySeq.size()*4);
    std::for_each(querySeq.begin(), querySeq.end(), [&str](BlockQuery b) {
        str += std::to_string(b.blockA);
        str.push_back('-');
        str += std::to_string(b.blockB);
        str.push_back(';');
    });
    return str;
}

unsigned int OptimalQuerySequenceBuilder::scoreQuerySequence(const QuerySequence& q, uint errorThreshold) {
    unsigned int score = 0u;
    std::for_each(q.begin(), q.end(), [&score, errorThreshold](BlockQuery b) {
        score += 1u + 2*std::min(b.allowedInbetweenErrors, errorThreshold);
    });
    return score;
}

unsigned int OptimalQuerySequenceBuilder::scoreOptimalQuerySequence(const OptimalQuerySequence& q) {
    unsigned int score = 0u;
//	int i = 0;
    std::for_each(q.begin(), q.end(), [&score](QuerySequence const& seq) { score += scoreQuerySequence(seq, BLOCK_ERROR_THRESHOLD); });
    return score;
}

unsigned int OptimalQuerySequenceBuilder::scoreOptimalQuerySequence(const OptimalQuerySequence& q, uint errorCutoff) {
    unsigned int score = 0u;
    auto it = q.begin();
    for (uint i = 0, e = std::min((uint)q.size(), errorCutoff+1); i < e; ++i, ++it)
        score += scoreQuerySequence(*it, errorCutoff);
    return score;
}

SetOfQuerySequence OptimalQuerySequenceBuilder::findOptimalSetOfQuerySequence(uint errorThreshold) {
    if (errorThreshold == 0) {
        SetOfQuerySequence result;
        for (uint i = 0; i < BLOCK_COUNT; ++i) for (uint j = i+1; j < BLOCK_COUNT; ++j) {
            QuerySequence seq;
            seq.push_back(BlockQuery(i, j));
            result.push_back(seq);
        }
        return result;
    }
    else {
        dynmatrix m;
        QuerySequence longuest = longuestQuerySequence();
        return findOptimalSetOfQuerySequence_rec(longuest, querySequenceToString(longuest), errorSet(errorThreshold), m);
    }
}

SetOfQuerySequence OptimalQuerySequenceBuilder::findOptimalSetOfQuerySequence_rec(const QuerySequence& querySeq, const std::string& querySeqString,
                                                                           ErrorDistributionList const& errors,
                                                                           OptimalQuerySequenceBuilder::dynmatrix& cache) {
    if (!evaluateQuerySequence(querySeq, errors)) {
        return cache[querySeqString] = SetOfQuerySequence();
    }
    SetOfQuerySequence result;
    result.push_back(querySeq);
    dynset result_set;
    result_set.insert(querySeqString);

    for (uint it = 0, e = querySeq.size(); it != e; ++it) {
        QuerySequence subSequence = querySeq;
        subSequence.erase(subSequence.begin() + it);
        std::string subSequenceStr = querySequenceToString(subSequence);
        auto cache_it = cache.find(subSequenceStr);
        SetOfQuerySequence subResult;
        if (cache_it == cache.end()) {
            subResult = findOptimalSetOfQuerySequence_rec(subSequence, subSequenceStr, errors, cache);
            cache[subSequenceStr] = subResult;
        }
        else
            subResult = cache_it->second;

        for (QuerySequence const& seq : subResult) {
            std::string key = querySequenceToString(seq);
            if (result_set.find(key) != result_set.end())
                continue;
            if (result.front().size() == seq.size()) {
                result.push_back(seq);
                result_set.insert(key);
            }
            else if (result.front().size() > seq.size()) {
                result.clear();
                result_set.clear();
                result.push_back(seq);
                result_set.insert(key);
            }
        }
    }
    return result;
}

void OptimalQuerySequenceBuilder::computeOptimalQuerySequence(const std::vector<SetOfQuerySequence>& optimalQueries, uint errorThreshold,
                                                                              OptimalQuerySequence const& optimalBeingBuilt, unsigned int currentScore,
                                                                              QuerySequence const& concatenatedOptimalBeingBuilt,
                                                                              OptimalQuerySequence& found,
                                                                              unsigned int& foundScore) {
    if (errorThreshold == BLOCK_ERROR_THRESHOLD) {
        finalizeComputeOptimalQuerySequence(optimalQueries, optimalBeingBuilt, currentScore, concatenatedOptimalBeingBuilt, found, foundScore);
        return;
    }
    else {
//		std::cout << errorThreshold << std::endl;
        SetOfQuerySequence const& q = optimalQueries[errorThreshold];
//		unsigned int score = scoreOptimalQuerySequence(optimalBeingBuilt);
        for (QuerySequence const& seq : q) {
//			if (std::includes(seq.begin(), seq.end(), concatenatedOptimalBeingBuilt.begin(), concatenatedOptimalBeingBuilt.end())) {
                QuerySequence newSeq;
                std::set_difference(seq.begin(), seq.end(), concatenatedOptimalBeingBuilt.begin(), concatenatedOptimalBeingBuilt.end(), std::back_inserter(newSeq));
                unsigned int finalScore = currentScore + scoreQuerySequence(newSeq, BLOCK_ERROR_THRESHOLD);
                if (foundScore == 0u || finalScore < foundScore) {
                    QuerySequence newConcatenatedOptimalBeingBuilt;
                    std::set_union(newSeq.begin(), newSeq.end(), concatenatedOptimalBeingBuilt.begin(), concatenatedOptimalBeingBuilt.end(),
                                   std::back_inserter(newConcatenatedOptimalBeingBuilt));
                    OptimalQuerySequence newOptimalBeingBuilt = optimalBeingBuilt;
                    newOptimalBeingBuilt.push_back(newSeq);

                    computeOptimalQuerySequence(optimalQueries, errorThreshold+1, newOptimalBeingBuilt, finalScore,
                                                newConcatenatedOptimalBeingBuilt, found, foundScore);
                }
//			}
        }
    }
}

void OptimalQuerySequenceBuilder::finalizeComputeOptimalQuerySequence(const std::vector<SetOfQuerySequence>& optimalQueries,
                                                                      OptimalQuerySequence const& optimalBeingBuilt, unsigned int currentScore,
                                                                      QuerySequence const& concatenatedOptimalBeingBuilt,
                                                                      OptimalQuerySequence& found,
                                                                      unsigned int& foundScore) {
    SetOfQuerySequence const& q = optimalQueries.back();
//	unsigned int score = scoreOptimalQuerySequence(optimalBeingBuilt);
    for (QuerySequence const& seq : q) {
//		if (std::includes(seq.begin(), seq.end(), concatenatedOptimalBeingBuilt.begin(), concatenatedOptimalBeingBuilt.end())) {
            QuerySequence newSeq;
            std::set_difference(seq.begin(), seq.end(), concatenatedOptimalBeingBuilt.begin(), concatenatedOptimalBeingBuilt.end(), std::back_inserter(newSeq));
            unsigned int finalScore = currentScore + scoreQuerySequence(newSeq, BLOCK_ERROR_THRESHOLD);
            if (foundScore == 0u || finalScore < foundScore) {
                foundScore = finalScore;
                found = optimalBeingBuilt;
                found.push_back(newSeq);
            }
            else if (finalScore == foundScore) {
                bool update = false;
                OptimalQuerySequence newCandidate = optimalBeingBuilt;
                newCandidate.push_back(newSeq);
                for (int error = int(BLOCK_ERROR_THRESHOLD)-1; error >= 0; error--) {
                    uint newCandidateScore = scoreOptimalQuerySequence(newCandidate, error);
                    uint candidateScore = scoreOptimalQuerySequence(found, error);
                    if (newCandidateScore < candidateScore) {
                        update = true;
                        break;
                    }
                }
                if (update)
                    found = std::move(newCandidate);
            }
//		}
    }
}

OptimalQuerySequence OptimalQuerySequenceBuilder::computeOptimalQuerySequence() {
    std::vector<SetOfQuerySequence> optimals;
    for (uint errors = 0; errors <= BLOCK_ERROR_THRESHOLD; errors++)
        optimals.push_back(findOptimalSetOfQuerySequence(errors));

//	int e = 0;
//	for (SetOfQuerySequence const& set : optimals) {
//		std::cout << "For errors " << e++ << std::endl;
//		for (QuerySequence const& q : set) {
//			std::cout << "Query Sequence:";
//			for (BlockQuery b : q)
//				std::cout << "(" << b.blockA << ", " << b.blockB << ") ";
//			std::cout << std::endl;
//		}
//	}

    OptimalQuerySequence opt;
    unsigned int score = 0u;
    computeOptimalQuerySequence(optimals, 0, OptimalQuerySequence(), 0, QuerySequence(), opt, score);
    return opt;
}

void OptimalQuerySequenceBuilder::generateCppCodeForOptimalQuerySequence() {
    OptimalQuerySequence optimal = computeOptimalQuerySequence();
    std::cout << "//Paste the code below in the constructor of OptimalQuerySequenceBuilder:" << std::endl;
    std::cout << "#ifndef RNACOMP_COMPUTE_OPTIMAL_QUERY_SEQUENCE" << std::endl;
    std::cout << "static_assert(BLOCK_COUNT == " << BLOCK_COUNT << ", \"The optimal query sequence and the block configuration don't match. "
                 "Please recompile with RNACOMP_COMPUTE_OPTIMAL_QUERY_SEQUENCE (#define). "
                 "Then run the program and paste the output in the constructor of OptimalQuerySequenceBuilder.\");" << std::endl;
    std::cout << "#endif" << std::endl;

    std::cout << "typedef meta_prog::OptimalQuerySequence<" << std::endl;
    uint i = 0;
    std::for_each(optimal.begin(), optimal.end(), [&i, &optimal](QuerySequence const& seq) {
        std::cout << "\t" << "meta_prog::QuerySequence<" << std::endl;
        uint j = 0;
        std::for_each(seq.begin(), seq.end(), [&j, &seq](BlockQuery b) {
            std::cout << "\t\t" << "meta_prog::BlockQuery<" << b.blockA << ", " << b.blockB << ">";
            if (++j != seq.size())
                std::cout << ", ";
            std::cout << std::endl;
        });
        if (++i != optimal.size())
            std::cout << "\t>, ";
        else
            std::cout << "\t>";
        std::cout << std::endl;
    });
    std::cout << "> PrecomputedOptimalQuerySequence;" << std::endl;
}
