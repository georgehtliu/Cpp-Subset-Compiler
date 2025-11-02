
#include <iostream>
#include <algorithm>

std::string removeComments(std::string code) {
    int commentIdx = -1;
    char prev = ' ';
    for (int i = 0; i < code.size(); i++) {
        if (code.at(i) == '/' && prev == '/') {
            commentIdx = i - 1;
            break;
        }
        prev = code.at(i);
    }

    if (commentIdx < 0) return code;

    return code.substr(0, commentIdx);
}

std::string removeStartSpace(std::string code) {

    code.erase(code.begin(), std::find_if(code.begin(), code.end(), [](unsigned char c) {
        return !std::isspace(c);
    }));

    return code;
}

std::string removeEndSpace(std::string code) {
    code.erase(std::find_if(code.rbegin(), code.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), code.end());

    return code;
}


int main() {
    std::string test = "    hello there123             ";
    std::cout << test << std::endl;
    std::cout << removeStartSpace(removeComments(test)) << std::endl;
    return 0;
}