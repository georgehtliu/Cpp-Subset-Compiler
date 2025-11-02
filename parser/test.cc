#include <iostream>
#include <algorithm>
#include <vector>

std::pair<std::string,std::string> splitByFirstSpace(std::string line) {
    int spaceIdx = line.find(' ');
    std::string first = line.substr(0, spaceIdx);
    std::string second = "";
    if (spaceIdx < line.size()) {
        second = line.substr(spaceIdx + 1);
    }
    return std::make_pair(first,second);
}

std::vector<std::string> split(std::string &line) {

    int pos_start = 0;
    int pos_end = 1;

    std::string token;
    std::vector<std::string> arr;

    while ((pos_end = line.find_first_of(" \n\t", pos_start)) != std::string::npos) {
        int dist = pos_end - pos_start;
        token = line.substr(pos_start, dist);
        pos_start = pos_end + 1;
        arr.emplace_back(token);
    }

    arr.emplace_back(line.substr(pos_start));
    return arr;
}

int main() {
    std::string line = "hello there hi hi hi hi 123";
    std::pair<std::string,std::string> pair = splitByFirstSpace("hello  there\nhi hi hi\thi 123");
    auto vec = split(line);
    for (auto myToken : vec) {
        std::cout << myToken << " ";
    }
    return 0;
}