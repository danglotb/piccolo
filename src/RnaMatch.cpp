#include "RnaMatch.h"
#include <algorithm>
#include <iostream>
#include <iterator>

std::vector<nt> RnaMatch::stringToNt(const std::string& seq) {
    return util::stringToNt(seq);
}

void RnaMatch::processQuery(int seq_pos, Query const& q, QueryMeta const& qMeta) {
    processQueryResult(seq_pos, q, qMeta, m_index.search(q));
}

template<typename Query_t>
void RnaMatch::processQueryResult(int seq_pos, const Query_t& query, QueryMeta const& qMeta, const QueryResult& queryResult) {
    if (!queryResult.size())
        return;

    //In order to get right boolean, which depend of the type Query_t (Query or QueryGlobal)
    std::tuple<bool,bool,bool> unqueriedBlock = (query.unqueriedBlock());

    bool const hasFront = std::get<0>(unqueriedBlock);
    bool const hasMiddle = std::get<1>(unqueriedBlock);
    bool const hasBack = std::get<2>(unqueriedBlock);

    int const offset = BLOCK_OFFSET_AT(query.blockA());
    int const maxIndels = std::min((int)BLOCK_ERROR_THRESHOLD, offset);
    /// BAD == depends on the block (first block = 0 errors)
    int min_begin_pos = Query_t::global?0:seq_pos - offset - maxIndels;
    int const max_begin_pos = seq_pos - offset + maxIndels;

    if (max_begin_pos < 0)
        return;
    else if (min_begin_pos < 0)
        min_begin_pos = 0;

    AlignmentResult frontResult;
    AlignmentResult middleResult;
    AlignmentResult backResult;

    for (auto& rnaId : queryResult) {
        RnaResult& rnaResult = m_results[rnaId];
        // We already aligned this miRNA on this pos, and the alignment was perfect (i.e. we cannot improve it)
        if (!Query_t::global && rnaResult.size() && (rnaResult.back().begin() >= min_begin_pos && rnaResult.back().begin() <= max_begin_pos) && m_minErrorFound == 0u)
            continue;

        MiRnaEntry const& miRna = m_index.at(rnaId);

        nt const* const miRnaSeq = miRna.second.data();
        std::size_t miRnaLastBlockSize = miRna.second.size() - BLOCK_OFFSET_AT(BLOCK_COUNT - 1);

        int starting_pos = min_begin_pos;
        int end_pos = starting_pos + miRna.second.size();
        if (hasMiddle) {
            nt const* const middle_begin = m_seq_begin + seq_pos + BLOCK_SIZE_AT(query.blockA()); // End of blockA
            nt const* const middle_end = middle_begin + qMeta.blockOffset(); // Start of block B

            middleResult = m_aligner.alignMiddle(middle_begin, middle_end, miRnaSeq+BLOCK_OFFSET_END_AT(query.blockA()),
                                                 miRnaSeq+BLOCK_OFFSET_AT(query.blockB()));
            if (middleResult.errorCount > m_minErrorFound ||  m_aligner.getnbIndel() > m_nbIndel)
                continue;

            // We update the putative seq positions
            uint const length = middle_begin - m_seq_begin;
            middleResult.sequenceLocus.begin += length;
            middleResult.sequenceLocus.end += length;
            starting_pos = seq_pos;
            if (!hasBack)
                end_pos = middleResult.sequenceLocus.end + miRnaLastBlockSize;
        }
        else
            middleResult.clear();

        if (hasFront) {

            nt const* const front_begin = m_seq_begin + min_begin_pos; // Start
            nt const* const front_end = m_seq_begin + seq_pos; // Start of blockA

            frontResult = Query_t::global?m_aligner.alignMiddle(front_begin, front_end, miRnaSeq, miRnaSeq+BLOCK_OFFSET_AT(query.blockA())):
                                          m_aligner.alignFront(front_begin, front_end, miRnaSeq, miRnaSeq+BLOCK_OFFSET_AT(query.blockA()));

            if (frontResult.errorCount + middleResult.errorCount > m_minErrorFound ||  m_aligner.getnbIndel() > m_nbIndel)
                continue;

            frontResult.sequenceLocus.begin += min_begin_pos;
            frontResult.sequenceLocus.end += min_begin_pos;
            starting_pos = frontResult.sequenceLocus.begin;
            if (!hasBack && !hasMiddle)
                end_pos = frontResult.sequenceLocus.end + BLOCK_SIZE_AT(query.blockA()) + miRnaLastBlockSize;
            //				end_pos = frontResult.sequenceLocus.end + BLOCK_OFFSET_END_AT(query.blockB()) - BLOCK_OFFSET_AT(query.blockA());
        }
        else
            frontResult.clear();

        if (hasBack) {
            nt const* miRnaSeq_back_begin = miRnaSeq + BLOCK_OFFSET_END_AT(query.blockB());
            nt const* miRnaSeq_back_end = miRnaSeq + miRna.second.size();

            nt const* const back_begin = m_seq_begin +(seq_pos + BLOCK_SIZE_AT(query.blockA()) +
                                                       qMeta.blockOffset() + BLOCK_SIZE_AT(query.blockB())); // End of blockB

            nt const* back_end = back_begin + (miRnaSeq_back_end-miRnaSeq_back_begin) + BLOCK_ERROR_THRESHOLD - (frontResult.errorCount + middleResult.errorCount);
            if (back_end > m_seq_end)
                back_end = m_seq_end;

            backResult = Query_t::global?m_aligner.alignMiddle(std::min(m_seq_end,back_begin), m_seq_end, miRnaSeq_back_begin, miRnaSeq_back_end):
                                         m_aligner.alignBack(back_begin, back_end, miRnaSeq_back_begin, miRnaSeq_back_end);

            if (frontResult.errorCount + middleResult.errorCount + backResult.errorCount > m_minErrorFound || m_aligner.getnbIndel() > m_nbIndel)
                continue;

            uint const length = back_begin - m_seq_begin;
            backResult.sequenceLocus.begin += length;
            backResult.sequenceLocus.end += length;
            end_pos = backResult.sequenceLocus.end;
            if (!hasMiddle && !hasFront) {
                starting_pos = seq_pos;
            }
        }
        else
            backResult.clear();

        // If we already aligned this
        if (!Query_t::global && rnaResult.size() &&
                rnaResult.back().begin() == starting_pos &&
                rnaResult.back().end() == end_pos &&
                rnaResult.back().errorCount() <= frontResult.errorCount + middleResult.errorCount + backResult.errorCount)
            continue;

        MiRnaAlignmentResult alignResult;
        alignResult.mergeAlignmentResults(std::move(frontResult), std::move(middleResult), std::move(backResult),
                                          BLOCK_SIZE_AT(query.blockA()), query.blockB() == BLOCK_COUNT-1 ? miRnaLastBlockSize : BLOCK_SIZE_AT(query.blockB()),
                                          starting_pos, end_pos);

        // We clear the previous suboptimal results
        if (m_findBest && m_minErrorFound > alignResult.errorCount()) {
            m_minErrorFound = alignResult.errorCount();
            std::for_each(m_results.begin(), m_results.end(), [](RnaResult& r) { r.clear(); });
        }

        nbAligned++;
        isAligned=true;

        rnaResult.push_back(std::move(alignResult));
    }
}

void RnaMatch::displayHumanReadableResult(const MiRnaEntry& sequence, uint from, std::ostream& out) const {
    std::ostream_iterator<char> out_it (out);
    for (uint i = 0; i < m_results.size(); ++i) {
        std::for_each(m_results[i].begin(), m_results[i].end(), [&sequence, &out, this, i, from, &out_it](MiRnaAlignmentResult const& result) {
            out << sequence.first << ":" << result.begin()+1+from << "-" << result.end()+from << std::endl;
            auto find_first_space = std::find(m_index.at(i).first.begin(), m_index.at(i).first.end(), ' ');
            std::copy(m_index.at(i).first.begin(), find_first_space, out_it);
            out << ":1-" << m_index.at(i).second.size() << std::endl;
            out << "errors: " << result.errorCount() << std::endl;
            result.display(out, m_index.at(i).second, sequence.second.data()+from);
            out << std::endl;
        });
    }
}

void RnaMatch::displayExonerateResult(const MiRnaEntry& sequence, uint from, std::ostream& out) const {
    std::ostream_iterator<char> out_it (out);
    for (uint i = 0; i < m_results.size(); ++i) {
        std::for_each(m_results[i].begin(), m_results[i].end(), [&sequence, &out, &out_it, this, i, from](MiRnaAlignmentResult const& result) {
            auto find_first_space = std::find(m_index.at(i).first.begin(), m_index.at(i).first.end(), ' ');
            out << "- name : ";
            std::copy(m_index.at(i).first.begin(), find_first_space, out_it);
            out << std::endl
                << "  begin_target  : " << result.begin()+from << std::endl
                << "  end_target    : " << result.end()+from << std::endl
                << "  strand_target : +" << std::endl
                << "  begin_query   : 0" << std::endl
                << "  end_query     : " << m_index.at(i).second.size() << std::endl
                << "  def_query     : ";
            if (find_first_space != m_index.at(i).first.end())
                std::copy(find_first_space+1, m_index.at(i).first.end(), out_it);
            out << std::endl
                << "  seq_query     : ";
            std::transform(m_index.at(i).second.begin(), m_index.at(i).second.end(), out_it, &util::ntToAscii);
            out << std::endl << std::endl
                << "  score: " << -(int)result.errorCount() << std::endl
                << "  alignment: |" << std::endl;
            result.displayExonerate(out, m_index.at(i).second, sequence.second.data()+from);
        });
    }
}

RnaMatch::RnaMatch(const RnaIndex& index) : m_index(index), m_nbIndel(BLOCK_ERROR_THRESHOLD) {}

RnaMatch::RnaMatch(const RnaIndex& index, unsigned int nbIndel) : m_index(index), m_nbIndel(nbIndel) {}

void RnaMatch::match(const std::vector<nt>& sequence, bool best, bool global) {
    if (global)
        match_global(sequence.data(), sequence.data() + sequence.size());
    else
        match_small_in_large(sequence.data(), sequence.data() + sequence.size(), best);
}

void RnaMatch::match_global(const nt* sequence_begin, const nt* sequence_end) {

    reset(sequence_end - sequence_begin, m_index);

    m_seq_begin = sequence_begin;
    m_seq_end = sequence_end;
    m_findBest = false;

    unsigned int size_seq = m_seq_end - m_seq_begin;

    for (QueryGlobal b : m_querySequence.m_queries_global) {

        QueryGlobal q;
        q.setBlockIds(b.blockA(), b.blockB());

        for (int j = -b.m_offsetB ; j <= b.m_offsetB ; j++) {
            for (int e = -b.m_offsetA ; e <= b.m_offsetA; e++) {

                q.m_offsetA = e;
                q.m_offsetB = j;

                unsigned int blockA_beg = b.blockA()*BLOCK_SIZE_AT(b.blockA())+e;
                unsigned int blockA_end = (b.blockA()+1)*BLOCK_SIZE_AT(b.blockA())+e;
                unsigned int blockB_beg = std::min(b.blockB()*BLOCK_SIZE_AT(b.blockA())+j+e, size_seq);
                unsigned int blockB_end = std::min((b.blockB()+1)*BLOCK_SIZE_AT(b.blockA())+j+e, size_seq);

                q.setBlockHash(util::hash(sequence_begin + blockA_beg, sequence_begin + blockA_end),
                               util::hash(sequence_begin + blockB_beg, sequence_begin + blockB_end));

                unsigned int offsetMeta =  (b.blockB()*BLOCK_SIZE_AT(b.blockA())+j) - (b.blockA()+1)*BLOCK_SIZE_AT(b.blockA());

                processQueryResult((b.blockA()*BLOCK_SIZE_AT(b.blockA())+e) , q, QueryMeta(offsetMeta), m_index.search(q));
            }
        }
    }
}

void RnaMatch::match_small_in_large(const nt* sequence_begin, const nt* sequence_end, bool best) {
    if (sequence_begin == sequence_end)
        return;

    reset(sequence_end-sequence_begin, m_index);
    m_seq_begin = sequence_begin;
    m_seq_end = sequence_end;
    m_findBest = best;

    // Process the first nt
    for (QueryBuilder& q : m_querySequence) {
        auto query = q.initialize(sequence_begin, sequence_end);
        processQuery(0, query, q.meta());
    }
    sequence_begin++;

    // Process until close to the end of the sequence
    nt const* nt_seq_premature_end = sequence_end - BLOCK_TOTAL_SIZE;
    for (; sequence_begin < nt_seq_premature_end; sequence_begin++) {
        for (auto query_it = m_querySequence.begin(), e = m_querySequence.endForErrorThreshold(m_minErrorFound); query_it != e; ++query_it) {
            auto query = query_it->buildNextQuery(sequence_begin, sequence_end);
            processQuery(sequence_begin - m_seq_begin, query, query_it->meta());
        }
    }

    // Process until the end of the sequence
    // Extra care must be taken as the blocks queried may fall outside the sequence
    for (; sequence_begin < sequence_end; sequence_begin++) {
        for (auto query_it = m_querySequence.begin(), e = m_querySequence.endForErrorThreshold(m_minErrorFound); query_it != e; ++query_it) {
            // auto <=> std::pair<bool, Query const&>
            auto query = query_it->buildNextQueryFromTruncatedSeq(sequence_begin, sequence_end);
            if (query.first)
                processQuery(sequence_begin - m_seq_begin, query.second, query_it->meta());
        }
    }
}

bool RnaMatch::displayResult(const MiRnaEntry& sequence, std::ostream& out, bool humanReadable) const {
    return displayResult(sequence, 0u, out, humanReadable);
}

bool RnaMatch::displayResult(const MiRnaEntry& sequence, uint from, std::ostream& out, bool humanReadable) const {
    if (out.fail())
        return false;

    if (humanReadable)
        displayHumanReadableResult(sequence, from, out);
    else
        displayExonerateResult(sequence, from, out);
    return true;
}

void RnaMatch::reset(std::size_t sequence_size, const RnaIndex& new_index) {
    m_minErrorFound = BLOCK_ERROR_THRESHOLD;

    std::fill(m_results.begin(), m_results.begin() + std::min(m_results.size(), new_index.size()), RnaResult());
    m_results.resize(new_index.size(), RnaResult());

    m_aligner = RnaAlignment(sequence_size, new_index.maxRnaLength());
}

void RnaMatch::MiRnaAlignmentResult::mergeAlignmentResults(AlignmentResult&& front, AlignmentResult&& middle, AlignmentResult&& back,
                                                           std::size_t blockASize, std::size_t blockBSize, int begin, int end) {
    alignment.errorCount = front.errorCount + middle.errorCount + back.errorCount;
    alignment.operations = std::move(front.operations);
    auto inserter = std::back_inserter(alignment.operations);

    inserter = std::fill_n(inserter, blockASize, Operation::Match);
    inserter = std::copy(middle.operations.begin(), middle.operations.end(), std::back_inserter(alignment.operations));
    inserter = std::fill_n(inserter, blockBSize, Operation::Match);
    inserter = std::copy(back.operations.begin(), back.operations.end(), std::back_inserter(alignment.operations));

    alignment.sequenceLocus.begin = begin;
    alignment.sequenceLocus.end = end;
}

void RnaMatch::MiRnaAlignmentResult::display(std::ostream& out, std::vector<nt> const& miRna, const nt* seq) const {
    std::string seq_output, mirna_output, align_output;

    seq_output.reserve(alignment.operations.size());
    mirna_output.reserve(alignment.operations.size());
    align_output.reserve(alignment.operations.size());

    auto seq_it = seq + begin();
    auto miRna_it = miRna.begin();

    for (Operation op : alignment.operations) {
        switch (op) {
        case Operation::Match:
        case Operation::Subsitution:
            seq_output.push_back(util::ntToAscii(*seq_it));
            mirna_output.push_back(util::ntToAscii(*miRna_it));
            ++seq_it;
            ++miRna_it;
            align_output.push_back(op == Operation::Match ? '|' : ' ');
            break;
        case Operation::Deletion:
            seq_output.push_back(util::ntToAscii(*seq_it));
            ++seq_it;
            mirna_output.push_back('-');
            align_output.push_back(' ');
            break;
        case Operation::Insertion:
            mirna_output.push_back(util::ntToAscii(*miRna_it));
            ++miRna_it;
            seq_output.push_back('-');
            align_output.push_back(' ');
            break;
        default:
            break;
        }
    }

    out << seq_output << std::endl;
    out << align_output << std::endl;
    out << mirna_output << std::endl;
}

void RnaMatch::MiRnaAlignmentResult::displayExonerate(std::ostream& out, std::vector<nt> const& miRna, const nt* seq) const {
    auto seq_it = seq + begin();
    auto miRna_it = miRna.begin();

    for (Operation op : alignment.operations) {
        switch (op) {
        case Operation::Match:
        case Operation::Subsitution:
            out << "    " << util::ntToAscii(*miRna_it) << " " << util::ntToAscii(*seq_it) << " match" << std::endl;
            ++seq_it;
            ++miRna_it;
            break;
        case Operation::Deletion:
            out << "    " << "-" << " " << util::ntToAscii(*seq_it) << " gap" << std::endl;
            ++seq_it;
            break;
        case Operation::Insertion:
            out << "    " << util::ntToAscii(*miRna_it) << " " << "-" << " gap" << std::endl;
            ++miRna_it;
            break;
        default:
            break;
        }
    }
}
