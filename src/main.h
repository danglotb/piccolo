#ifndef MAIN_H
#define MAIN_H

#include "RnaIndex.h"
#include "RnaMatch.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <atomic>

#include "IsomirQuerySequence.h"
#include "IsomirMatch.h"

struct Parameters {
		char const* inputFile, *referenceFile, *outputFile;
		bool humanReadable, best, half, all;

		Parameters() : inputFile(nullptr), referenceFile(nullptr), outputFile(nullptr), humanReadable(false), best(false),
			half(false), all(false) {}
};

bool parseParameters(int argc, char const* argv[], Parameters& parameters);

void run(RnaDataBase const& sequences, RnaIndex const& index, std::ostream& out, Parameters const& parameters);

#endif // MAIN_H
