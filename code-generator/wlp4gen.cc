#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include <set>
#include <memory>

const std::string INT = "int";
const std::string INT_PTR = "int*";
const int TRASHED_REGISTER = 9;
int ifCount = 0;
int loopCount = 0;
int deleteCount = 0;

bool isTerminalLexeme(std::string lexeme) {
    return isupper(lexeme[0]);
}

std::pair<std::string,std::string> splitByFirstSpace(std::string &line) {
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

std::string getBeforeColon(std::string line) {
    auto mySplit = split(line);
    std::string res = "";
    std::vector<std::string> words;
    for (auto word : mySplit) {
        if (word == ":") break;
        words.push_back(word);
    }
    for (auto word : words) {
        res += word;
        res += " ";
    }

    res.pop_back(); // delete last space

    return res;
}

struct ProcedureInfo {

    std::string name;
    std::vector<std::string> signature;
    std::map<std::string, std::string> symbolTable;
    std::map<std::string, int> offsetTable;
    int numParams = 0;

    ProcedureInfo(std::string name) {
        this->name = name;
    }

    void setSignature(std::vector<std::string> &sig) {
        for (auto type : sig) {
            signature.push_back(type);
        }
        numParams = sig.size();
    }

    void insertVariable(std::string name, std::string type, int offset) {
        symbolTable.insert({name, type});
        offsetTable.insert({name, offset});
    }

    std::string getType(std::string varName) {
        return symbolTable.at(varName);
    }

    int getOffset(std::string varName) {
        return offsetTable.at(varName);
    }

    ~ProcedureInfo() {}
};

// procedure -> procedureInfo
std::map<std::string, ProcedureInfo> tables;

void insertNewProcedure(std::string procedureName) {
    tables.insert({procedureName, ProcedureInfo(procedureName)});
}

struct Tree {

    std::string lexeme;
    std::vector<std::shared_ptr<Tree>> children;
    std::string name;
    bool isTerminal;
    bool isProcedure = false;
    bool hasType = false;
    std::string type;

    Tree() {}

    Tree(std::string lexeme) {
        this->lexeme = lexeme;
        name = splitByFirstSpace(lexeme).first;
        isTerminal = isTerminalLexeme(lexeme);
    }

    ~Tree() {}

    std::string getName() {
        return name;
    }

    void addChild(std::shared_ptr<Tree> child) {
        children.push_back(child);
    }

    std::string getVarName() {
        return split(lexeme)[1];
    }
};

void traverseTree(std::shared_ptr<Tree> node, std::string name, std::vector<std::shared_ptr<Tree>> &matchingNodes) {
    if (node->getName() == name) {
        matchingNodes.push_back(node);
    }
    for (auto child : node->children) {
        traverseTree(child, name, matchingNodes);
    }
}

std::vector<std::shared_ptr<Tree>> getChildNodes(std::shared_ptr<Tree> root, std::string name) {
    std::vector<std::shared_ptr<Tree>> res;
    traverseTree(root, name, res);
    return res;
}

std::shared_ptr<Tree> buildTree() {
    std::string line;
    getline(std::cin, line);
    auto derivationSplit = split(line);
    bool hasType = false;
    std::string type = "";
    if (line.find(':') != std::string::npos) {
        hasType = true;
        type = derivationSplit[derivationSplit.size() - 1];
    }
    derivationSplit.erase(derivationSplit.begin()); // delete first element
    std::string lexeme = line;
    if (hasType) lexeme = getBeforeColon(line);
    std::shared_ptr<Tree> node = std::make_shared<Tree>(lexeme);
    node->hasType = hasType;
    node->type = type;
    int numChildren = derivationSplit.size();
    if (hasType) {
        numChildren -= 2;
    }
    if ((numChildren == 1 && derivationSplit[0] == ".EMPTY") || isTerminalLexeme(line)) {
        return node;
    }
    for (int i = 0; i < numChildren; i++) {
        node->addChild(buildTree());
    }
    return node;
}

void insertNewVariable(std::string procedureName, std::string varName, std::string type, int offset) {
    tables.at(procedureName).insertVariable(varName, type, offset);
}

void setSignature(std::string procedureName, std::vector<std::string> signature) {
    tables.at(procedureName).setSignature(signature);
}

int getNumParams(std::string procedureName) {
    return tables.at(procedureName).numParams;
}

std::pair<std::string, std::string> extractDCLNode(std::shared_ptr<Tree> dclNode) {
    std::string id = "";
    std::string type = "";
    auto typeNode = dclNode->children[0];
    auto idNode = dclNode->children[1];
    type = typeNode->children.size() <= 1 ? INT : INT_PTR;
    id = idNode->getVarName();
    return {type, id};
}

void fillTables(std::shared_ptr<Tree> root) {

    int offset = 0;
    auto mainNodes = getChildNodes(root, "main");
    std::vector<std::string> mainSig;

    // traverse main
    auto mainNode = mainNodes[0];

    insertNewProcedure(mainNode->getName());
    
    for (auto mainChildNode : mainNode->children) {

        // get param variables and signature
        if (mainChildNode->getName() == "dcl") {
            auto dclNode = mainChildNode;
            auto pair = extractDCLNode(dclNode);
            auto id = pair.second;
            auto type = pair.first;
            insertNewVariable("main", id, type, offset);
            mainSig.push_back(type);
            offset -= 4;
        }

        setSignature("main", mainSig);

        // get body params
        if (mainChildNode->getName() == "dcls") {
            auto dclsNode = mainChildNode;
            auto dclNodes = getChildNodes(dclsNode, "dcl");
            for (auto dclNode : dclNodes) {
                auto pair = extractDCLNode(dclNode);
                auto id = pair.second;
                auto type = pair.first;
                insertNewVariable("main", id, type, offset);
                offset -= 4;
            }
        }
    }

    // traverse procedure
    auto procedureNodes = getChildNodes(root, "procedure");

    for (auto procedureNode : procedureNodes) {

        auto IDNode = procedureNode->children[1];
        std::string procedureName = IDNode->getVarName();
        insertNewProcedure(procedureName);

        auto paramsNode = procedureNode->children[3];
        auto sigDCLNodes = getChildNodes(paramsNode, "dcl");

        // traverse signature, add signature and add signature variables
        std::vector<std::string> signature;

        // we just push register 5

        int paramNum = 0;
        int totalParams = sigDCLNodes.size();

        for (auto sigDCLNode : sigDCLNodes) {
            paramNum++;
            auto pair = extractDCLNode(sigDCLNode);
            std::string type = pair.first;
            std::string id = pair.second;
            signature.push_back(type);
            int varOffset = 4*(totalParams - paramNum + 1);
            insertNewVariable(procedureName, id, type, varOffset);
        }

        setSignature(procedureName, signature);

        // traverse body
        int localVarCount = 0;
        auto dclsNode = procedureNode->children[6];
        auto bodyDCLNodes = getChildNodes(dclsNode, "dcl");
        int numRegistersPushed = 1;

        for (auto bodyDCLNode : bodyDCLNodes) {
            localVarCount++;
            auto pair = extractDCLNode(bodyDCLNode);
            std::string type = pair.first;
            std::string id = pair.second;
            int varOffset = -4*numRegistersPushed - 4*(localVarCount - 1);
            insertNewVariable(procedureName, id, type, varOffset);
        }
    }
}

std::string push(int regNum) {
    std::string line1 = "sw $" + std::to_string(regNum) + ", -4($30) ; push " + std::to_string(regNum);
    std::string line2 = "sub $30, $30, $4";
    return line1 + '\n' + line2;
}

std::string pop(int regNum) {
    std::string line1 = "add $30, $30, $4 ; pop " + std::to_string(regNum);
    std::string line2 = "lw $" + std::to_string(regNum) + ", -4($30)";
    return line1 + '\n' + line2;
}

std::string linesToString(std::vector<std::string> &lines) {
    std::string res = "";
    for (auto line : lines) {
        if (line == "") continue;
        res += line + '\n';
    }
    return res;
}

int getOffset(std::string procedureName, std::string varName) {
    return tables.at(procedureName).getOffset(varName);
}

std::string getType(std::string procedureName, std::string varName) {
    return tables.at(procedureName).getType(varName);
}

std::shared_ptr<Tree> getInnerlValueNode(std::shared_ptr<Tree> lValueNode) {

    if (lValueNode->children.size() == 3) {
        return getInnerlValueNode(lValueNode->children[1]);
    }

    return lValueNode;
}

std::string callInit() {

    std::vector<std::string> lines;

    bool arrayInput = false;

    auto mainSig = tables.at("main").signature;

    for (auto type : mainSig) {
        if (type == INT_PTR) {
            arrayInput = true;
            break;
        }
    }

    lines.push_back("lis $5");
    lines.push_back(".word init");
    lines.push_back(push(2));
    lines.push_back(push(31));
    if (!arrayInput) {
        lines.push_back("add $2, $0, $0");
    }
    lines.push_back("jalr $5");
    lines.push_back(pop(31));
    lines.push_back(pop(2));

    return linesToString(lines);
}

std::string generateCode(std::shared_ptr<Tree> node, std::string procName) {

    if (node->getName() == "start") {
        std::shared_ptr<Tree> proceduresNode = node->children[1];
        return generateCode(proceduresNode, "");
    }

    if (node->getName() == "procedures") {
        std::shared_ptr<Tree> mainNode = node->children[0];
        return generateCode(mainNode, "main");
    }

    if (node->getName() == "main") {

        std::vector<std::string> lines;
        std::shared_ptr<Tree> dclsNode = node->children[8];
        std::shared_ptr<Tree> statementsNode = node->children[9];
        std::shared_ptr<Tree> exprNode = node->children[11];

        lines.push_back("; begin prologue");
        lines.push_back("lis $4");
        lines.push_back(".word 4");
        lines.push_back("lis $11");
        lines.push_back(".word 1");
        lines.push_back("sub $29, $30, $4");
        lines.push_back(push(1));
        lines.push_back(push(2));
        lines.push_back(generateCode(dclsNode,procName));
        lines.push_back("; call init");
        lines.push_back(callInit());
        lines.push_back("; end prologue");
        lines.push_back("");

        lines.push_back(generateCode(statementsNode,procName));
        lines.push_back(generateCode(exprNode,procName));

        lines.push_back("");
        lines.push_back("; begin epilogue");
        lines.push_back("add $30, $30, $4");
        lines.push_back("add $30, $30, $4");
        lines.push_back("jr $31");

        return linesToString(lines);
    }

    if (node->getName() == "expr") { 
        if (node->children.size() == 1) return generateCode(node->children[0],procName);
        if (node->children.size() == 3) { // children: expr PLUS term or expr MINUS term
            auto exprNode = node->children[0];
            auto termNode = node->children[2];

            std::string operation = "add";
            if (node->children[1]->getName() == "MINUS") operation = "sub";

            if (exprNode->type == INT && termNode->type == INT) {
                std::vector<std::string> lines;
                lines.push_back(generateCode(node->children[0],procName));
                lines.push_back(push(3));
                lines.push_back(generateCode(node->children[2],procName));
                lines.push_back(pop(5));
                lines.push_back(operation + " $3, $5, $3");
                return linesToString(lines);
            } else if (exprNode->type == INT_PTR && termNode->type == INT && operation == "add") {
                std::vector<std::string> lines;
                lines.push_back(generateCode(exprNode,procName));
                lines.push_back(push(3));
                lines.push_back(generateCode(termNode,procName));
                lines.push_back("mult $3, $4");
                lines.push_back("mflo $3");
                lines.push_back(pop(5));
                lines.push_back("add $3, $5, $3");
                return linesToString(lines);
            } else if (exprNode->type == INT && termNode->type == INT_PTR && operation == "add") {
                std::vector<std::string> lines;
                lines.push_back(generateCode(termNode,procName));
                lines.push_back(push(3));
                lines.push_back(generateCode(exprNode,procName));
                lines.push_back("mult $3, $4");
                lines.push_back("mflo $3");
                lines.push_back(pop(5));
                lines.push_back("add $3, $5, $3");
                return linesToString(lines);
            } else if (exprNode->type == INT_PTR && termNode->type == INT && operation == "sub") {
                std::vector<std::string> lines;
                lines.push_back(generateCode(exprNode,procName));
                lines.push_back(push(3));
                lines.push_back(generateCode(termNode,procName));
                lines.push_back("mult $3, $4");
                lines.push_back("mflo $3");
                lines.push_back(pop(5));
                lines.push_back("sub $3, $5, $3");
                return linesToString(lines);
            } else if (exprNode->type == INT_PTR && termNode->type == INT_PTR && operation == "sub") {
                std::vector<std::string> lines;
                lines.push_back(generateCode(exprNode,procName));
                lines.push_back(push(3));
                lines.push_back(generateCode(termNode,procName));
                lines.push_back(pop(5));
                lines.push_back("sub $3, $5, $3");
                lines.push_back("div $3, $4");
                lines.push_back("mflo $3");
                return linesToString(lines);
            }

        }
    }

    if (node->getName() == "term") {
        if (node->children.size() == 1) return generateCode(node->children[0],procName);
        if (node->children.size() == 3) { // children: term STAR factor or term SLASH factor or term PCT factor
            std::vector<std::string> lines;
            std::string operation = "mult";
            if (node->children[1]->getName() != "STAR") operation = "div";
            lines.push_back(generateCode(node->children[0],procName));
            lines.push_back(push(3));
            lines.push_back(generateCode(node->children[2],procName));
            lines.push_back(pop(5));
            
            lines.push_back(operation + " $5, $3");
            if (node->children[1]->getName() == "STAR" || node->children[1]->getName() == "SLASH") {
                lines.push_back("mflo $3");
            } else {
                lines.push_back("mfhi $3");
            }
            return linesToString(lines);
        }
    }

    if (node->getName() == "factor") {

        int numChildren = node->children.size();

        if (node->children[0]->getName() == "NEW") {
            std::vector<std::string> lines;
            lines.push_back(generateCode(node->children[3], procName));
            lines.push_back("add $1, $3, $0");
            lines.push_back(push(31));
            lines.push_back("lis $5");
            lines.push_back(".word new");
            lines.push_back("jalr $5");
            lines.push_back(pop(31));
            lines.push_back("bne $3, $0, 1");
            lines.push_back("add $3, $11, $0");
            return linesToString(lines);
        }

        if (numChildren == 3 && node->children[0]->getName() == "LPAREN") {
            return generateCode(node->children[1], procName);
        }

        if (numChildren >= 3 && node->children[0]->getName() == "ID") {
            
            std::vector<std::string> lines;

            std::string procedureName = node->children[0]->getVarName();
            std::string procedureLabel = "FUNCTION" + procedureName;

            int numParams = getNumParams(procedureName);
            lines.push_back("");
            lines.push_back("; call " + procedureName);
            lines.push_back(push(29));
            lines.push_back(push(31));
            int argCount = 0;

            if (numChildren > 3) { // has paramaters
                auto argsListNodes = getChildNodes(node, "arglist");

                // traverse arguments
                std::vector<std::shared_ptr<Tree>> arguments;

                auto argsListNode = node->children[2];
                while (argsListNode->children.size() == 3) {
                    arguments.push_back(argsListNode->children[0]);
                    argsListNode = argsListNode->children[2];
                }
                arguments.push_back(argsListNode->children[0]);

        
                for (auto exprNode : arguments) {
                    argCount++;
                    lines.push_back("; argument for " + procedureName);
                    lines.push_back(generateCode(exprNode, procName));
                    lines.push_back(push(3));
                }
        
            }

            if (argCount != numParams) {
                throw std::runtime_error("ERROR: argCount was " + std::to_string(argCount) + " but numParams was" + std::to_string(numParams));
            }

            lines.push_back("lis $5");
            lines.push_back(".word " + procedureLabel);
            lines.push_back("jalr $5");

            for (int i = 0; i < numParams; i++) lines.push_back(pop(TRASHED_REGISTER));

            lines.push_back(pop(31));
            lines.push_back(pop(29));

            return linesToString(lines);
        }

        if (numChildren == 1 && (node->children[0]->getName() == "ID" || node->children[0]->getName() == "NUM")) return generateCode(node->children[0], procName); // either NUM or ID
        if (numChildren == 1 && node->children[0]->getName() == "NULL") {
            std::vector<std::string> lines;
            lines.push_back("add $3, $0, $11 ; NULL");
            return linesToString(lines);
        }
        if (numChildren == 2 && node->children[0]->getName() == "STAR") {
            std::vector<std::string> lines;
            lines.push_back(generateCode(node->children[1], procName));
            lines.push_back("lw $3, 0($3) ; dereference");
            return linesToString(lines);
        }

        if (numChildren == 2 && node->children[0]->getName() == "AMP") {
            auto innerlValueNode = getInnerlValueNode(node->children[1]);
            // case 1: ID
            if (innerlValueNode->children.size() == 1) {
                std::string varName = innerlValueNode->children[0]->getVarName();
                std::vector<std::string> lines;
                int offset = getOffset(procName, varName);

                lines.push_back("; amp id case");
                
                lines.push_back("lis $3");
                lines.push_back(".word " + std::to_string(getOffset(procName, varName)) + " ; get offset ");
                
                lines.push_back("add $3, $3, $29");
                return linesToString(lines);
            }

            // case 2: dereference then take address 
            if (innerlValueNode->children.size() == 2) {
                std::vector<std::string> lines;
                lines.push_back("; amp dereference case");
                lines.push_back(generateCode(innerlValueNode->children[1], procName));
                return linesToString(lines);
            }
        }
    }

    if (node->getName() == "ID") {
        std::string varName = node->getVarName();
        int offset = getOffset(procName, varName);
        std::string res = "lw $3, " + std::to_string(offset) + "($29)";
        return res; 
    }

    if (node->getName() == "NUM") {
        std::string line1 = "lis $3";
        std::string line2 = ".word " + node->getVarName();
        return line1 + '\n' + line2;
    }

    if (node->getName() == "NULL") {
        std::string line1 = "add $3, $0, $11";
        return line1;
    }

    if (node->getName() == "dcls") {

        int numChildren = node->children.size();
        
        if (numChildren == 0) { // empty
            return "";
        }

        // children: dcls dcl BECOMES NULL SEMI
        if (numChildren == 5 && node->children[3]->getVarName() == "NULL") {
            std::vector<std::string> lines;
            lines.push_back(generateCode(node->children[0], procName)); // dcls
            lines.push_back("; null dcl");
            lines.push_back(generateCode(node->children[3], procName));
            lines.push_back(push(3));
            return linesToString(lines);
        }

        std::vector<std::string> lines;

        // children: dcls dcl BECOMES NUM SEMI

        std::string num = node->children[3]->getVarName();
        lines.push_back(generateCode(node->children[0], procName));
        lines.push_back("lis $5");
        lines.push_back(".word " + num);
        lines.push_back(push(5));

        return linesToString(lines);
    }

    if (node->getName() == "statements") {

        int numChildren = node->children.size();
        
        if (numChildren == 0) { // empty
            return "";
        } else if (numChildren == 2) { // children: statements statement
            std::string line1 = generateCode(node->children[0], procName);
            std::string line2 = generateCode(node->children[1], procName);
            return line1 + '\n' + line2;
        }

    }

    if (node->getName() == "statement") {

        if (node->children[0]->getName() == "DELETE") {
            deleteCount++;
            std::string deleteLabel = "LABELskipDelete" + std::to_string(deleteCount);

            std::vector<std::string> lines;
            lines.push_back(generateCode(node->children[3], procName));
            lines.push_back("beq $3, $11, " + deleteLabel);
            lines.push_back("add $1, $3, $0");
            lines.push_back(push(31));
            lines.push_back("lis $5");
            lines.push_back(".word delete");
            lines.push_back("jalr $5");
            lines.push_back(pop(31));
            lines.push_back(deleteLabel + ":");
            return linesToString(lines);
        }

        if (node->children[0]->getName() == "lvalue") { // children: lvalue BECOMES expr SEMI
            auto innerlValueNode = getInnerlValueNode(node->children[0]);
            if (innerlValueNode->children[0]->getName() == "ID") {
                std::string varName = innerlValueNode->children[0]->getVarName();
                int offset = getOffset(procName, varName);
                auto exprNode = node->children[2];
                std::string line1 = generateCode(exprNode, procName);
                std::string line2 = "sw $3, " + std::to_string(offset) + "($29)";
                return line1 + '\n' + line2;
            } else {
                std::vector<std::string> lines;
                lines.push_back("; *x = expr");
                lines.push_back(generateCode(node->children[2], procName));  
                lines.push_back(push(3));  
                lines.push_back(generateCode(innerlValueNode->children[1], procName));  
                lines.push_back(pop(5));  
                
                lines.push_back("sw $5, 0($3)"); 
                return linesToString(lines);
            }
        }
        if (node->children[0]->getName() == "IF") { // children: IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE
            ifCount++;
            int currIfCount = ifCount;
            std::vector<std::string> lines;
            lines.push_back(generateCode(node->children[2], procName));
            lines.push_back("beq $3, $0, labelelse" + std::to_string(currIfCount));
            lines.push_back(generateCode(node->children[5], procName));
            lines.push_back("beq $0, $0, labelendif"+ std::to_string(currIfCount));
            lines.push_back("labelelse" + std::to_string(currIfCount) + ":");
            lines.push_back(generateCode(node->children[9], procName));
            lines.push_back("labelendif" + std::to_string(currIfCount) + ":");    
            return linesToString(lines);
        }

        if (node->children[0]->getName() == "WHILE") { // children: WHILE LPAREN test RPAREN LBRACE statements RBRACE
            loopCount++;
            int currLoopCount = loopCount;
            std::vector<std::string> lines;
            lines.push_back("labelloop" + std::to_string(currLoopCount) + ":");
            lines.push_back(generateCode(node->children[2], procName));
            lines.push_back("beq $3, $0, labelendWhile" + std::to_string(currLoopCount));
            lines.push_back(generateCode(node->children[5], procName));
            lines.push_back("beq $0, $0, labelloop" + std::to_string(currLoopCount));
            lines.push_back("labelendWhile" + std::to_string(currLoopCount) + ":");
            return linesToString(lines);
        }
        if (node->children[0]->getName() == "PRINTLN") { // children: PRINTLN LPAREN expr RPAREN SEMI
            std::vector<std::string> lines;
            lines.push_back(push(1));
            lines.push_back(generateCode(node->children[2], procName));
            lines.push_back("add $1, $3, $0");
            lines.push_back(push(31));
            lines.push_back("lis $5");
            lines.push_back(".word print");
            lines.push_back("jalr $5");
            lines.push_back(pop(31));
            lines.push_back(pop(1));
            return linesToString(lines);
        }
    }

    if (node->getName() == "test") {
        std::string operation = node->children[1]->getName();
        std::string type = node->children[0]->type;

        std::string slt = (type == INT) ? "slt" : "sltu";

        if (operation == "LT") {
            std::vector<std::string> lines;
            lines.push_back(generateCode(node->children[0], procName));
            lines.push_back(push(3));
            lines.push_back(generateCode(node->children[2], procName));
            lines.push_back(pop(5));
            
            lines.push_back(slt + " $3, $5, $3");
            return linesToString(lines);
        } else if (operation == "GT") {
            std::vector<std::string> lines;
            lines.push_back(generateCode(node->children[0], procName));
            lines.push_back(push(3));
            lines.push_back(generateCode(node->children[2], procName));
            lines.push_back(pop(5));
            
            lines.push_back(slt + " $3, $3, $5");
            return linesToString(lines);
        } else if (operation == "NE") {
            std::vector<std::string> lines;
            lines.push_back(generateCode(node->children[0], procName));
            lines.push_back(push(3));
            lines.push_back(generateCode(node->children[2], procName));
            lines.push_back(pop(5));
            
            lines.push_back(slt + " $6, $3, $5 ; $6 = $3 < $5");
            lines.push_back(slt + " $7, $5, $3 ; $7 = $5 < $3");
            lines.push_back("add $3, $6, $7");
            return linesToString(lines);
        } else if (operation == "EQ") {
            std::vector<std::string> lines;
            lines.push_back(generateCode(node->children[0], procName));
            lines.push_back(push(3));
            lines.push_back(generateCode(node->children[2], procName));
            lines.push_back(pop(5));
            
            lines.push_back(slt + " $6, $3, $5 ; $6 = $3 < $5");
            lines.push_back(slt + " $7, $5, $3 ; $7 = $5 < $3");
            lines.push_back("add $3, $6, $7");
            lines.push_back("lis $11");
            lines.push_back(".word 1");
            lines.push_back("sub $3, $11, $3");
            return linesToString(lines);
        } else if (operation == "GE") {
            std::vector<std::string> lines;
            lines.push_back(generateCode(node->children[0], procName));
            lines.push_back(push(3));
            lines.push_back(generateCode(node->children[2], procName));
            lines.push_back(pop(5));
            
            lines.push_back(slt + " $3, $5, $3");
            lines.push_back("lis $11");
            lines.push_back(".word 1");
            lines.push_back("sub $3, $11, $3");
            return linesToString(lines);
        } else if (operation == "LE") {
            std::vector<std::string> lines;
            lines.push_back(generateCode(node->children[0], procName));
            lines.push_back(push(3));
            lines.push_back(generateCode(node->children[2], procName));
            lines.push_back(pop(5));
            
            lines.push_back(slt + " $3, $3, $5");
            lines.push_back("lis $11");
            lines.push_back(".word 1");
            lines.push_back("sub $3, $11, $3");
            return linesToString(lines);
        }
    }

    if (node->getName() == "procedure") {
        std::string procedureLabel = "FUNCTION" + procName;
        int totalVars = tables.at(procName).symbolTable.size();
        int totalParams = getNumParams(procName);
        int localVars = totalVars - totalParams;
        std::vector<std::string> lines;
        lines.push_back(procedureLabel + ":");
        lines.push_back("sub $29, $30, $4");
        lines.push_back(push(5));
        lines.push_back(generateCode(node->children[6],procName));
        lines.push_back(generateCode(node->children[7],procName));
        lines.push_back(generateCode(node->children[9],procName));
        for (int i = 0; i < localVars; i++) lines.push_back(pop(5));
        lines.push_back(pop(5));
        lines.push_back("jr $31");
        return linesToString(lines);
    }

    // Shouldn't reach here - unrecognized node type
    throw std::runtime_error("ERROR: didn't get name for node : " + node->lexeme);
}

void generateInit(std::shared_ptr<Tree> root) {

    std::cout << ".import print" << std::endl;
    std::cout << ".import init" << std::endl;
    std::cout << ".import new" << std::endl;
    std::cout << ".import delete" << std::endl;

    auto mainNode = getChildNodes(root, "main")[0];
    std::cout << generateCode(mainNode, "main") << std::endl;

    auto procedureNodes = getChildNodes(root, "procedure");
    for (auto procedureNode : procedureNodes) {
        std::string procedureName = procedureNode->children[1]->getVarName();
        std::cout << generateCode(procedureNode, procedureName) << std::endl;
    }
}

int main() {
    std::shared_ptr<Tree> root = buildTree();
    fillTables(root);
    generateInit(root);
    return 0;
}