#include "main.h"

bool parseParameters(int argc, char const* argv[], Parameters& parameters) {
	enum { I_OPTION = 0, R_OPTION = 1, O_OPTION = 2 };
    std::array<char const*, 6> boolOptions({"-h", "--best", "--half", "--all", "-l", "-g"});
    std::array<bool*, 6> boolOptionsOutput({&parameters.humanReadable, &parameters.best, &parameters.half, &parameters.all, &parameters.local, &parameters.global});
	std::array<char const*, 3> textOptions({"-i", "-r", "-o"});
	std::array<char const**, 3> textOptionsOutput({&parameters.inputFile, &parameters.referenceFile, &parameters.outputFile});
	int currentOption = -1;
	for (int it = 1; it < argc; ++it) {
		bool optionFound = false;
		for (int i = 0; i < (int)textOptions.size(); ++i) {
			if (std::strcmp(argv[it], textOptions[i]) == 0) {
				if (currentOption != -1)
					return false;
				currentOption = i;
				optionFound = true;
				break;
			}
		}
		if (optionFound)
			continue;
		if (currentOption != -1) {
			*textOptionsOutput[currentOption] = argv[it];
			currentOption = -1;
			continue;
		}
		else {
			for (int i = 0; i < (int)boolOptions.size(); ++i) {
				if (std::strcmp(argv[it], boolOptions[i]) == 0) {
					*boolOptionsOutput[i] = true;
					optionFound = true;
					break;
				}
			}
			if (optionFound)
				continue;
		}
		return false;
	}
	if (parameters.inputFile == nullptr)
		parameters.inputFile = "-";
	if (parameters.outputFile == nullptr)
		parameters.outputFile = "-";
	if (!parameters.best && !parameters.all && !parameters.half)
		parameters.best = true;
	return currentOption == -1 && parameters.referenceFile != nullptr;
}

void run(RnaDataBase const& sequences, RnaIndex const& index, std::ostream& out, Parameters const& parameters) {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    RnaMatch matcher(index);

	if (parameters.half) {
		for (MiRnaEntry const& entry : sequences) {
			if (entry.second.size() > 10) {
				nt const* first_half_begin = entry.second.data();
				nt const* first_half_end = first_half_begin + (entry.second.size() / 2) + 5;
				nt const* second_half_begin = first_half_end - 10;
				nt const* second_half_end = first_half_begin + entry.second.size();
                matcher.match_small_in_large(first_half_begin, first_half_end, true);
				if (!matcher.displayResult(entry, out, parameters.humanReadable)) {
					std::cerr << "Unable to write output." << std::endl;
					return;
				}
                matcher.match_small_in_large(second_half_begin, second_half_end, true);
				if (!matcher.displayResult(entry, second_half_begin-first_half_begin, out, parameters.humanReadable)) {
                    std::cerr << "Unable to write output."
                              << std::endl;
					return;
				}
			}
			else {
                matcher.match(entry.second, false, parameters.global);
				if (!matcher.displayResult(entry, out, parameters.humanReadable)) {
					std::cerr << "Unable to write output." << std::endl;
					return;
				}
			}
		}
	}
	else {
		for (MiRnaEntry const& entry : sequences) {
            matcher.match(entry.second, parameters.best, parameters.global);
            if (!matcher.displayResult(entry, out, parameters.humanReadable)) {
                std::cerr << "Unable to write output." << std::endl;
                return;
            }
		}
	}
    end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;

    std::cout  << "elapsed time with construction of index: " << elapsed_seconds.count() << " s" << std::endl;
}

int main(int argc, char const* argv[]) {
#ifdef RNACOMP_COMPUTE_OPTIMAL_QUERY_SEQUENCE
	OptimalQuerySequenceBuilder::generateCppCodeForOptimalQuerySequence();
#else
	Parameters parameters;
	if (!parseParameters(argc, argv, parameters)) {
          std::cout << "piccolo v1.0.0\t\tDesigned by Sebastien BINI" << std::endl << std::endl;
		std::cout << "Invalid input parameters." << std::endl
                          << "Usage: " << argv[0] << " [OPTIONS] -r reference.fa" << std::endl;
		std::cout << "\t-h    : Human readable output." << std::endl;
		std::cout << "\t-i    : Input file path. Write '-' for stdin. '-' by default." << std::endl;
		std::cout << "\t-r    : Path to the reference file (of known miRNAs)." << std::endl;
		std::cout << "\t-o    : Output file path. Write '-' for stdout. '-' by default." << std::endl;
		std::cout << "\t--best: Prints only optimal alignments. This is the default option." << std::endl;
		std::cout << "\t--half: Prints the optimal alignments of the first half of the sequence and "
					 "the optimal alignments of the second half of the sequence. Same as running --best on the first half and then on the second half."
				  << std::endl;
		std::cout << "\t--all: Prints all acceptable alignments (which have no more than " << BLOCK_ERROR_THRESHOLD << " errors)." << std::endl;
        std::cout << "\t-l   : Will compute a local (small to large) alignment" << std::endl;
        std::cout << "\t-g   : Will compute a global alignment" << std::endl;
		return EXIT_SUCCESS;
	}

	RnaIndex index;
	if (!index.parse(parameters.referenceFile)) {
		std::cerr << "Unable to parse reference file." << std::endl;
		return EXIT_FAILURE;
	}

	// ================================
	//		Input
	// ================================
	std::streambuf* inputBuffer;
    std::ifstream inputFileStream;

	if (std::strcmp(parameters.inputFile, "-") != 0) {
		inputFileStream.open(parameters.inputFile);
		inputBuffer = inputFileStream.rdbuf();
	}
	else
		inputBuffer = std::cin.rdbuf();
	std::istream in(inputBuffer);

	RnaDataBase sequences;
	if (!sequences.parse(in) || sequences.size() == 0u) { // Accept a multi-fasta.
		std::cerr << "Unable to parse input file." << std::endl;
		return EXIT_FAILURE;
	}

	// ================================
	//		Output
	// ================================
	std::streambuf* outputBuffer;
	std::ofstream outputFileStream;

	if (std::strcmp(parameters.outputFile, "-") != 0) {
		outputFileStream.open(parameters.outputFile);
		outputBuffer = outputFileStream.rdbuf();
	}
	else
		outputBuffer = std::cout.rdbuf();
	std::ostream out(outputBuffer);

    run(sequences, index, out, parameters);

#endif
	return EXIT_SUCCESS;
}
