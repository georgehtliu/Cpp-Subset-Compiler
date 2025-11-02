#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include "scanner.h"

/*
 * C++ Starter code for CS241 A3
 * All code requires C++14, so if you're getting compile errors make sure to
 * use -std=c++14.
 *
 * This file contains the main function of your program. By default, it just
 * prints the scanned list of tokens back to standard output.
 */

std::set<std::string> validOpcodes = {"add", "sub", "mult", "multu", "div", "divu", "mfhi", "mflo", "lis", "lw", "sw", "slt", "sltu", "beq", "bne", "jr", "jalr", ".word"};

void printInstruction(int64_t instr) {
    unsigned char c = instr >> 24;
    std::cout << c;
    c = instr >> 16;
    std::cout << c;
    c = instr >> 8;
    std::cout << c;
    c = instr;
    std::cout << c;
}

int getLabelOffset(std::string label, int pc, std::map<std::string, int> labels) {

  if (labels.count(label) == 0) {
    throw std::runtime_error("ERROR: label not found");
  }

  int labelAddress = labels.at(label);
  int offset = (labelAddress - pc)/4;
  return offset;
}

void singleTokenCheck(Token token, std::map<std::string, int> labels, bool secondPass) {
  std::string lexeme = token.getLexeme();
  if (token.getKind() == Token::REG) {
    int64_t regValue = token.toNumber();
    if (regValue > 31 || regValue < 0) {
      throw std::runtime_error("ERROR: register number to large or too small");
    }
    if (lexeme.at(0) != '$') {
      throw std::runtime_error("ERROR: dollar sign not found in front of register");
    }
  } else if (token.getKind() == Token::ID && secondPass) {
    // check if label
    if (labels.count(lexeme) == 0 && validOpcodes.count(lexeme) == 0) {
      throw std::runtime_error("ERROR: label or opcode not found");
    }
  }
}

void invalidArgsCheck(std::vector<Token> tokenLine, int i, int start, int end) { // start,end are indices

  for (int i = 0; i < tokenLine.size(); i++) {
    if (i < start || i > end) {
      if (tokenLine[i].getKind() != Token::LABEL) {
        throw std::runtime_error("ERROR: invalid number of arguments");
      }
    }
  }

}

void operandCheck1(Token token, std::vector<Token> tokenLine, int i) { // add, sub, slt, sltu


  // how to handle add $3, $3, $3, $3, $3

  std::string lexeme = token.getLexeme();
  int minLength = 1 + i + 5;
  if (tokenLine.size() < minLength) {
    throw std::runtime_error("ERROR: invalid number of arguments");
  }

  if (!(tokenLine[i+1].getKind() == Token::REG && tokenLine[i+2].getKind() == Token::COMMA && tokenLine[i+3].getKind() == Token::REG && tokenLine[i+4].getKind() == Token::COMMA && tokenLine[i+5].getKind() == Token::REG)) {
    throw std::runtime_error("ERROR: invalid argument types");
  }

  int start = i;
  int end = i + 5;

  invalidArgsCheck(tokenLine, i, start, end);

}

void operandCheck2(Token token, std::vector<Token> tokenLine, int i) { // mult, multu, div, divu


  std::string lexeme = token.getLexeme();
  int minLength = 1 + i + 3;
  if (tokenLine.size() < minLength) {
    throw std::runtime_error("ERROR: invalid number of arguments");
  }

  if (!(tokenLine[i+1].getKind() == Token::REG && tokenLine[i+2].getKind() == Token::COMMA && tokenLine[i+3].getKind() == Token::REG)) {
    throw std::runtime_error("ERROR: invalid argument types");
  }

  int start = i;
  int end = i + 3;

  invalidArgsCheck(tokenLine, i, start, end);

}

void operandCheck3(Token token, std::vector<Token> tokenLine, int i) { // mflo, mfhi, lis

  std::string lexeme = token.getLexeme();
  int minLength = 1 + i + 1;
  if (tokenLine.size() < minLength) {
    throw std::runtime_error("ERROR: invalid number of arguments");
  }

  if (!(tokenLine[i+1].getKind() == Token::REG)) {
    throw std::runtime_error("ERROR: invalid argument types");
  }

  int start = i;
  int end = i + 1;

  invalidArgsCheck(tokenLine, i, start, end);

}

void operandCheck4(Token token, std::vector<Token> tokenLine, int i) { // lw, sw

  std::string lexeme = token.getLexeme();
  int minLength = 1 + i + 6;
  if (tokenLine.size() < minLength) {
    throw std::runtime_error("ERROR: invalid number of arguments");
  }

  if (!(tokenLine[i+1].getKind() == Token::REG && tokenLine[i+2].getKind() == Token::COMMA && (tokenLine[i+3].getKind() == Token::INT || tokenLine[i+3].getKind() == Token::HEXINT) && tokenLine[i+4].getKind() == Token::LPAREN && tokenLine[i+5].getKind() == Token::REG && tokenLine[i+6].getKind() == Token::RPAREN)) {
    throw std::runtime_error("ERROR: invalid argument types");
  }

  int start = i;
  int end = i + 6;

  invalidArgsCheck(tokenLine, i, start, end);

}

void operandCheck5(Token token, std::map<std::string, int> labels, std::vector<Token> tokenLine, int i) { // bne, beq

  std::string lexeme = token.getLexeme();
  int minLength = 1 + i + 5;
  if (tokenLine.size() < minLength) {
    throw std::runtime_error("ERROR: invalid number of arguments");
  }

  if (!(tokenLine[i+1].getKind() == Token::REG) || !(tokenLine[i+2].getKind() == Token::COMMA) || !(tokenLine[i+3].getKind() == Token::REG) || !(tokenLine[i+4].getKind() == Token::COMMA)) {
    throw std::runtime_error("ERROR: invalid argument types");
  }
  if (!(tokenLine[i+5].getKind() == Token::INT) && !(tokenLine[i+5].getKind() == Token::HEXINT) && labels.count(tokenLine[i+5].getLexeme()) != 1) {
    throw std::runtime_error("ERROR: invalid argument types");
  }

  int start = i;
  int end = i + 5;

  invalidArgsCheck(tokenLine, i, start, end);
  
}

void wordCheck(Token token, std::vector<Token> tokenLine, int i, std::map<std::string, int> labels) {
  int minLength = 1 + i + 1;

  if (tokenLine.size() < minLength) {
    throw std::runtime_error("ERROR: invalid number of arguments");
  }

  if(!(tokenLine[i+1].getKind() == Token::INT) && !(tokenLine[i+1].getKind() == Token::HEXINT) && labels.count(tokenLine[i+1].getLexeme()) == 0) {
    throw std::runtime_error("ERROR: invalid second argument for .word");
  }

  invalidArgsCheck(tokenLine, i, i, i + 1);
}



int main() {
  std::string line;
  std::map<std::string, int> labels;
  int ramAddress = -4;
  std::vector<std::string> input;

  while (getline(std::cin, line)) { // store all input into input vector
    input.push_back(line);
  }

  // get labels first pass
  try {
    for (auto inputLine : input) {

      std::vector<Token> tokenLine = scan(inputLine);
      bool lineOnlyContainsLabelDefinitions = true;

      for (auto i=0; i<tokenLine.size(); i++) {

        // *************

        // std::cout << tokenLine[i] << ' '; // just for debugging *****COMMENT THIS LINE OUT WHEN COMPILING

        // *************

        singleTokenCheck(tokenLine[i], labels, false);

        const std::string lexeme = tokenLine[i].getLexeme();
        const Token::Kind kind = tokenLine[i].getKind();

        // if we see something other than a label definition, then it has to be an instruction so we increment the address
        if (kind != Token::LABEL && lineOnlyContainsLabelDefinitions) { 
          lineOnlyContainsLabelDefinitions = false;
          ramAddress += 4;
        }

        // new label initialize
        if (kind == Token::LABEL && lexeme.find(':') != std::string::npos) {
            std::string labelTitle = lexeme.substr(0, lexeme.size()-1);
            if (labels.count(labelTitle) > 0) {
              throw std::runtime_error("ERROR: label already exists");
            }
            labels.insert(std::pair<std::string, int>(labelTitle, ramAddress + 4));
        }

      }
    }

  } catch (std::runtime_error& error) {
    std::cerr << error.what() << "\n";
    return 1;
  }

  int programCounter = 0;

  try {
    for (auto inputLine : input) {
      // This example code just prints the scanned tokens on each line.
      std::vector<Token> tokenLine = scan(inputLine);
    
      programCounter += 4;
      bool lineOnlyContainsLabelDefinitions = true;

      for (int i=0; i<tokenLine.size(); i++) {

        singleTokenCheck(tokenLine[i], labels, true);
        
        const std::string lexeme = tokenLine[i].getLexeme();
        const Token::Kind kind = tokenLine[i].getKind();


        // if we see something other than a label definition
        if (kind != Token::LABEL) { 
          lineOnlyContainsLabelDefinitions = false;
        }

        if (kind == Token::WORD && (tokenLine[i+1].getKind() == Token::INT || tokenLine[i+1].getKind() == Token::HEXINT)) { // handles .word number

          wordCheck(tokenLine[i], tokenLine, i, labels);

          Token numberToken = tokenLine[i + 1];
          int64_t instr = numberToken.toNumber();
          printInstruction(instr);

        } else if (kind == Token::WORD) { // .word label

          wordCheck(tokenLine[i], tokenLine, i, labels);

          std::string nextLabel = tokenLine[i+1].getLexeme();
          std::string nextLabelTitle = nextLabel;
          if (labels.count(nextLabelTitle) == 0) {
            throw std::runtime_error("ERROR: label not found");
          }
          int64_t instr = (int64_t) labels.at(nextLabelTitle);
          printInstruction(instr);
        }
        
        if (kind == Token::ID && (lexeme == "add" || lexeme == "sub" || lexeme == "slt" || lexeme == "sltu")) { // handles add, sub, slt, sltu

          operandCheck1(tokenLine[i], tokenLine, i);

          int64_t resultRegister = (tokenLine[i + 1]).toNumber();
          int64_t addRegister1 = (tokenLine[i + 3]).toNumber();
          int64_t addRegister2 = (tokenLine[i + 5]).toNumber();

          int end = 0;

          if (lexeme == "add") end = 32;
          if (lexeme == "sub") end = 34;
          if (lexeme == "slt") end = 42;
          if (lexeme == "sltu") end = 43;

          int instr = (0<<26) | (addRegister1<<21) | (addRegister2<<16) | (resultRegister<<11) | end;
          printInstruction(instr);

        }
        
        if (kind == Token::ID && (lexeme == "beq" || lexeme == "bne")) { // handles beq and bne

          operandCheck5(tokenLine[i], labels, tokenLine, i);

          int64_t register1 = (tokenLine[i + 1]).toNumber();
          int64_t register2 = (tokenLine[i + 3]).toNumber();

          int start = (lexeme == "beq") ?  4 : 5;
          int end = 0;
          int64_t offset;

          if (tokenLine[i + 5].getKind() == Token::INT || tokenLine[i + 5].getKind() == Token::HEXINT) {
            offset = (tokenLine[i + 5]).toNumber();
          } else {
            offset = (int64_t) getLabelOffset(tokenLine[i + 5].getLexeme(), programCounter, labels);
          }

          if (offset < 0) { // check negative
            end = offset & 0xFFFF;
          } else {
            end = offset;
          }

          int instr = (start<<26) | (register1<<21) | (register2<<16) | end;
          printInstruction(instr);
        } 

        if (kind == Token::ID && (lexeme == "lw" || lexeme == "sw")) { // handles lw and sw

          operandCheck4(tokenLine[i], tokenLine, i);

          int64_t r1 = (tokenLine[i + 1]).toNumber();
          int64_t num = (tokenLine[i + 3]).toNumber();
          int64_t r2 = (tokenLine[i + 5]).toNumber();

          int start = (lexeme == "lw") ?  35 : 43;
          int end = 0;
          int64_t offset;

          offset = num;

          if (offset < 0) { // check negative
            end = offset & 0xFFFF;
          } else {
            end = offset;
          }

          int instr = (start<<26) | (r2<<21) | (r1<<16) | end;
          printInstruction(instr);
        } 

        if (kind == Token::ID && (lexeme == "mult" || lexeme == "multu" || lexeme == "div" || lexeme == "divu")) { // handles mult

          operandCheck2(tokenLine[i], tokenLine, i);

          int64_t r1 = (tokenLine[i + 1]).toNumber();
          int64_t r2 = (tokenLine[i + 3]).toNumber();

          int end = 0;

          if (lexeme == "mult") end = 24;
          if (lexeme == "multu") end = 25;
          if (lexeme == "div") end = 26;
          if (lexeme == "divu") end = 27;

          int instr = (0<<26) | (r1<<21) | (r2<<16) | end;
          printInstruction(instr);

        }

        if (kind == Token::ID && (lexeme == "mflo" || lexeme == "mfhi" || lexeme == "lis")) { // handles mflo..

          operandCheck3(tokenLine[i], tokenLine, i);

          int64_t r1 = (tokenLine[i + 1]).toNumber();

          int end = 0;

          if (lexeme == "mfhi") end = 16;
          if (lexeme == "mflo") end = 18;
          if (lexeme == "lis") end = 20;

          int instr = (0<<16) | (r1<<11) | end;
          printInstruction(instr);

        } 

        if (kind == Token::ID && (lexeme == "jr" || lexeme == "jalr")) { // handles jalr and jr

          operandCheck3(tokenLine[i], tokenLine, i);

          int64_t r1 = (tokenLine[i + 1]).toNumber();

          int end = 0;

          if (lexeme == "jr") end = 8;
          if (lexeme == "jalr") end = 9;

          int instr = (0<<26) | (r1<<21) | end;
          printInstruction(instr);

        }

      }

      if (lineOnlyContainsLabelDefinitions) {
        programCounter -= 4;
      }

    }
  } catch (std::runtime_error& error) {
    std::cerr << error.what() << "\n";
    return 1;
  }
  // You can add your own catch clause(s) for other kinds of errors.
  // Throwing exceptions and catching them is the recommended way to
  // handle errors and terminate the program cleanly in C++. Do not
  // use the std::exit function, which may leak memory.
  
  return 0;
}
