#pragma once

#include <list>
#include <set>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <utility>

class Steps {
    long randomSeed = 0;
    std::list<std::pair<size_t, std::set<char>>> steps; // pair: iteration, set of keys
    int modeChoice;
    int colorMode = 0;
public:
    static Steps loadSteps(const std::string& filename);
    void saveSteps(const std::string& filename) const;

    long getRandomSeed() const {
        return randomSeed;
    }

    void setRandomSeed(long seed) {
        randomSeed = seed;
    }

    void addStep(size_t iteration, const std::set<char>& keys) {
        steps.emplace_back(iteration, keys);
    }

    bool isNextStepOnIteration(size_t iteration) const {
        return !steps.empty() && steps.front().first == iteration;
    }

    std::set<char> popStep() {
        auto keys = steps.front().second;
        steps.pop_front();
        return keys;
    }

    void setModeChoice(int mode) { modeChoice = mode; }
    int getModeChoice() const { return modeChoice; }

    int getColorMode() const { return colorMode; }
    void setColorMode(int mode) { colorMode = mode; }

   std:: vector<std::pair<size_t, std::set<char>>> getAllSteps() const {
        return std::vector<std::pair<size_t, std::set<char>>>(steps.begin(), steps.end());
    }

    std::set<char> peekNextStep() const {
        return steps.empty() ? std::set<char>() : steps.front().second;
    }
    
};
