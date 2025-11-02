#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <vector>
#include <map>
const std::string ALPHABET    = ".ALPHABET";
const std::string STATES      = ".STATES";
const std::string TRANSITIONS = ".TRANSITIONS";
const std::string INPUT       = ".INPUT";
const std::string EMPTY       = ".EMPTY";

bool isChar(std::string s) {
  return s.length() == 1;
}
bool isRange(std::string s) {
  return s.length() == 3 && s[1] == '-';
}

std::set<char> alphabet;

struct State {

  std::string name;
  std::map<char, std::string> neighbors;
  bool initial = false;
  bool accepting = false;

  State(std::string name) {
    this->name = name;
  }

  ~State() {}

  void addNeighborState(std::string neighbor, char c) {
    neighbors.insert({c,neighbor});
  }

  std::pair<std::string, bool> getNextState(char input) {

    if (neighbors.count(input) == 0) return std::make_pair("", false);

    return std::make_pair(neighbors.at(input),true);
  }
 
};

std::map<std::string, State> states;

char peek(std::string w) {
  return w.at(0);
}

char consume(std::string &w) {
  char res = w.at(0);
  w = w.substr(1, w.size() - 1);
  return res;
}

bool simplifiedMaximalMunch(std::string w, std::map<std::string, State> states, std::string initialStateName, std::vector<std::string> &tokens) {

  std::string stateName = initialStateName;
  State currState = states.at(stateName);
  std::string currToken = "";

  while (!w.empty()) {

    char peekedChar = peek(w);
    std::pair<std::string, bool> res = currState.getNextState(peekedChar);
    bool error = !res.second;

    if (error) { // error
      if (currState.accepting) {
        tokens.push_back(currToken);
        currToken = "";
        currState = states.at(initialStateName);
      } else {
        return false;
      }
    } else {
      std::string &ref = w;
      char nextChar = peek(w);
      stateName = currState.getNextState(consume(ref)).first;
      currState = states.at(stateName);
      currToken += nextChar;
    }
  }

  // reached empty string
  if (currState.accepting) {
    tokens.push_back(currToken);
    currToken = "";
    return true;
  } else {
    return false;
  }

}

std::string initialState = " ";

// Locations in the program that you should modify to store the
// DFA information have been marked with four-slash comments:
//// (Four-slash comment)
int main() {
  std::istream& in = std::cin;
  std::string s;

  std::getline(in, s); // Alphabet section (skip header)
  // Read characters or ranges separated by whitespace
  while(in >> s) {
    if (s == STATES) { 
      break; 
    } else {
      if (isChar(s)) {
        //// Variable 's[0]' is an alphabet symbol
        alphabet.insert(s[0]);
      } else if (isRange(s)) {
        for(char c = s[0]; c <= s[2]; ++c) {
          //// Variable 'c' is an alphabet symbol
          alphabet.insert(c);
        }
      } 
    }
  }

  std::getline(in, s); // States section (skip header)
  // Read states separated by whitespace
  while(in >> s) {
    if (s == TRANSITIONS) { 
      break; 
    } else {
      static bool initial = true;
      bool accepting = false;
      if (s.back() == '!' && !isChar(s)) {
        accepting = true;
        s.pop_back();
      }
      //// Variable 's' contains the name of a state
      State state{s};

      if (initial) {
        //// The state is initial
        state.initial = true;
        initial = false;
        initialState = s;
      }
      if (accepting) {
        //// The state is accepting
        state.accepting = true;
      }

      states.insert({s, state});
    }
  }

  std::getline(in, s); // Transitions section (skip header)
  // Read transitions line-by-line
  while(std::getline(in, s)) {
    if (s == INPUT) { 
      // Note: Since we're reading line by line, once we encounter the
      // input header, we will already be on the line after the header
      break; 
    } else {
      std::string fromState, symbols, toState;
      std::istringstream line(s);
      line >> fromState;
      bool last;
      while(line >> s) {
        if(line.peek() == EOF) { // If we reached the last item on the line
          // Then it's the to-state
          toState = s;
        } else { // Otherwise, there is more stuff on the line
          // We expect a character or range
          if (isChar(s)) {
            symbols += s;
          } else if (isRange(s)) {
            for(char c = s[0]; c <= s[2]; ++c) {
              symbols += c;
            }
          }
        }
      }
      for ( char c : symbols ) {
        //// There is a transition from 'fromState' to 'toState' on 'c'
        State &from = states.at(fromState);

        from.addNeighborState(toState, c);
      }
    }
  }

  // Input section (already skipped header)
  while(in >> s) {
    //// Variable 's' contains an input string for the DFA
    std::vector<std::string> tokens;
    if (simplifiedMaximalMunch(s, states, initialState, tokens)) {
      for (auto token : tokens) {
        std::cout << token << std::endl;
      }
    } else {
      std::cerr << "ERROR" << std::endl;
    }
  }
}