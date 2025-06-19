#include "steps.h"
#include <fstream>
#include <sstream>
#include <iostream>

Steps Steps::loadSteps(const std::string& filename) {
    Steps steps;
    std::ifstream steps_file(filename);
    if (!steps_file.is_open()) {
        std::cerr << "Failed to open steps file: " << filename << '\n';
        return steps;
    }

    std::string line;
    bool isFirstLine = true;
    while (std::getline(steps_file, line)) {
        std::istringstream iss(line);
        if (isFirstLine) {
            // Read both randomSeed and modeChoice
            iss >> steps.randomSeed >> steps.modeChoice >> steps.colorMode;
            isFirstLine = false;
        }
        else {
            size_t iteration;
            iss >> iteration;
            std::set<char> keys;
            char ch;
            while (iss >> ch) {
                keys.insert(ch);
            }
            steps.addStep(iteration, keys);
        }
    }

    return steps;
}


void Steps::saveSteps(const std::string& filename) const {
    std::ofstream steps_file(filename);
    if (!steps_file.is_open()) {
        std::cerr << "Failed to open file for saving: " << filename << '\n';
        return;
    }

    // First line: seed, mode choice, and color flag
    steps_file << randomSeed << ' ' << modeChoice << ' ' << colorMode << '\n';

    // Then each recorded iteration + keys
    for (const auto& [iteration, keys] : steps) {
        steps_file << iteration;
        for (char ch : keys) {
            steps_file << ' ' << ch;
        }
        steps_file << '\n';
    }
}


