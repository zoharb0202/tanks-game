#ifndef RESULT_H
#define RESULT_H

#include <string>
#include <deque>

class Result {
private:
    std::string filename_;
    std::deque<std::string> entries_;  // FIFO container
public:
    // Initialize with the base name (e.g. "tanks-game_01")
    explicit Result(const std::string& baseName);

    // Record a result-line (e.g. "23 HIT TANK at (5,7)")
    void pushResult(const std::string& entry);

    // Flush all recorded entries into the .result file, then clear them
    void writeToFile(int randomseed);


};

#endif // RESULT_H