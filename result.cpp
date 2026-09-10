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

std::vector<std::string> Result::toLines(int randomseed) const {
    std::vector<std::string> lines;
    lines.push_back(std::to_string(randomseed));
    lines.insert(lines.end(), entries_.begin(), entries_.end());
    return lines;
}
