#include "result.h"
#include <fstream>

Result::Result(const std::string& baseName) {
    filename_ = baseName + ".result";
}

void Result::pushResult(const std::string& entry) {
    entries_.push_back(entry);
}

void Result::writeToFile(int randomseed) {
    std::ofstream ofs(filename_, std::ios::out);
    if (!ofs.is_open()) {
        // handle error as you prefer
        return;
    }
    ofs << randomseed << "\n";
    // Write in the same order entries were pushed
    while (!entries_.empty()) {
        ofs << entries_.front() << "\n";
        entries_.pop_front();
    }
    ofs.close();
}