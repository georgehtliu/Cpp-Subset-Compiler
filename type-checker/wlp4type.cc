#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include <set>

int numV = 0;

// map( functionName -> pair( vector( paramTypes ), map( varName -> varType ) ) )
std::map<std::string, std::pair<std::vector<std::string>, std::map<std::string, std::string>>> tables;
std::vector<std::string> procedureOrder;

int findIndex(std::vector<std::string> &arr, std::string str) {
    auto res = std::find(arr.begin(), arr.end(), str);

    if (res != arr.end())
        return res - arr.begin();
    return -1;
}

bool isTerminalLexeme(std::string lexeme) {
    return isupper(lexeme[0]);
}

bool declaredBefore(std::string procedure1, std::string procedure2) {
    int idx1 = findIndex(procedureOrder, procedure1);
    int idx2 = findIndex(procedureOrder, procedure2);
    return idx1 <= idx2;
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

struct Tree {

    std::string lexeme;
    std::vector<Tree> children;
    std::string name;
    bool isTerminal;
    bool isProcedure = false;

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

    void addChild(Tree child) {
        children.push_back(child);
    }

    std::string getVarName() {
        return split(lexeme)[1];
    }
};

std::pair<std::string, std::string> extractDCLNode(Tree &dclNode) {

    std::string type = "";
    std::string id = "";
    auto typeNode = dclNode.children[0];
    int typeNodeChildLen = typeNode.children.size();
    if (typeNodeChildLen == 1) {
        type = "int";
    } else if (typeNodeChildLen == 2) {
        type = "int*";
    }
    auto idNode = dclNode.children[1];
    id = idNode.getVarName();
    return std::make_pair(id, type);
}

Tree buildTree() {

    std::string line;
    getline(std::cin, line);

    auto derivationSplit = split(line);
    std::string lexeme = derivationSplit[0];

    derivationSplit.erase(derivationSplit.begin()); // delete first element

    int numChildren = derivationSplit.size();

    Tree node = Tree(line);

    if (numChildren == 1 && derivationSplit[0] == ".EMPTY") {
        return node;
    }

    if (isTerminalLexeme(line)) {
        numV++;
        return node;
    }

    for (int i = 0; i < numChildren; i++) {
        node.addChild(buildTree());
        numV++;
    }

    return node;

}

void insertNewProcedure(std::string procedureName) {
    std::pair<std::vector<std::string>, std::map<std::string, std::string>> attributes;
    if (tables.count(procedureName) > 0) {
        throw std::runtime_error("ERROR");
    }
    tables.insert({procedureName, attributes});
}

void insertNewVariable(std::string procedureName, std::pair<std::string, std::string> var) {
    if (tables.at(procedureName).second.count(var.first) > 0) {
        throw std::runtime_error("ERROR");
    }
    tables.at(procedureName).second.insert(var);
}

void insertProcedureSignature(std::string procedureName, std::string type) {
    tables.at(procedureName).first.push_back(type);
}

void traverseTree(Tree &node, std::string name, std::vector<Tree> &matchingNodes) {
    if (node.getName() == name) {
        matchingNodes.push_back(node);
    }
    for (auto &child : node.children) {
        traverseTree(child, name, matchingNodes);
    }
}

std::vector<Tree> getChildNodes(Tree &root, std::string name) {
    std::vector<Tree> res;
    traverseTree(root, name, res);
    return res;
}

std::string getProcedureName(Tree &procedureNode) {

    for (auto procedureNodeChild : procedureNode.children) {
        if (procedureNodeChild.getName() == "ID") {
            return procedureNodeChild.getVarName();
        }
    }

    // shouldn't reach here
    throw std::runtime_error("no procedure name");
    return "fail";
    
}

void fillTable(Tree &root) {

    auto procedureNodes = getChildNodes(root, "procedure");
    auto mainNodes = getChildNodes(root, "main");

    if (mainNodes.size() != 1) throw std::runtime_error("ERROR");

    // traverse main
    Tree &mainNode = mainNodes[0];
    insertNewProcedure(mainNode.getName());
    mainNode.isProcedure = true;
    for (auto mainChildNode : mainNode.children) {

        // get param variables and signature
        if (mainChildNode.getName() == "dcl") {
            auto &dclNode = mainChildNode;
            auto newVar = extractDCLNode(dclNode);
            insertNewVariable("main", newVar);
            insertProcedureSignature("main", newVar.second);
        }

        // get body params
        if (mainChildNode.getName() == "dcls") {
            auto &dclsNode = mainChildNode;
            auto dclNodes = getChildNodes(dclsNode, "dcl");
            for (auto dclNode : dclNodes) {
                auto newVar = extractDCLNode(dclNode);
                insertNewVariable("main", newVar);
            }
        }
    }

    // traverse procedures
    for (auto &procedureNode : procedureNodes) {

        procedureNode.isProcedure = true;
        std::string procedureName = getProcedureName(procedureNode);
        procedureOrder.push_back(procedureName);
        insertNewProcedure(procedureName);
        for (auto &procedureNodeChild : procedureNode.children) {

            // flag id
            if (procedureNodeChild.getName() == "ID") {
                procedureNodeChild.isProcedure = true;
            }

            // get param variables and signature
            if (procedureNodeChild.getName() == "params") {
                Tree &paramsNode = procedureNodeChild;
                auto dclNodes = getChildNodes(paramsNode, "dcl");
                for (auto dclNode : dclNodes) {
                    auto newVar = extractDCLNode(dclNode);
                    insertNewVariable(procedureName, newVar);
                    insertProcedureSignature(procedureName, newVar.second);
                }
            }

            // get body variables
            if (procedureNodeChild.getName() == "dcls") {
                Tree &dclsNode = procedureNodeChild;
                auto dclNodes = getChildNodes(dclsNode, "dcl");
                for (auto dclNode : dclNodes) {
                    auto newVar = extractDCLNode(dclNode);
                    insertNewVariable(procedureName, newVar);
                } 
            }
        }
    }

    procedureOrder.push_back("main");
}

std::string getType(Tree &node, std::string procedureName)  {

    if (node.getName() == "ID") {
        if (tables.at(procedureName).second.count(node.getVarName()) == 0) throw std::runtime_error("ERROR");
        std::string type = tables.at(procedureName).second.at(node.getVarName());
        return type;
    }

    if (node.getName() == "NUM") {
        return "int";
    }

    if (node.getName() == "NULL") {
        return "int*";
    }

    if (node.getName() == "expr") {

        if (node.children.size() == 1) {
            return getType(node.children[0], procedureName);
        } else if (node.children.size() == 3) {

            auto firstNode = node.children[0];
            auto secNode = node.children[2];
            auto opNode = node.children[1];

            std::string typeFirst = getType(firstNode, procedureName);
            std::string typeSecond = getType(secNode, procedureName);

            if (opNode.getName() == "PLUS" && typeFirst == "int*" && typeSecond == "int*") {
                throw std::runtime_error("ERROR");
            }

            if (typeFirst == typeSecond) return "int";
            return "int*";

        }

    }

    if (node.getName() == "term") {

        if (node.children.size() == 1) {
            return getType(node.children[0], procedureName);
        } else if (node.children.size() == 3) {

            auto firstNode = node.children[0];
            auto secNode = node.children[2];

            std::string typeFirst = getType(firstNode, procedureName);
            std::string typeSecond = getType(secNode, procedureName);

            if (typeFirst == typeSecond && typeFirst == "int") return "int"; // must be both int

            throw std::runtime_error("ERROR");

            return "fail";

        }
    }

    if (node.getName() == "factor") {

        if (node.children.size() == 1) {
            return getType(node.children[0], procedureName);
        } else if (node.children.size() == 2) {

            auto firstNode = node.children[0];
            auto secNode = node.children[1];

            std::string typeSecond = getType(secNode, procedureName);

            if (firstNode.getName() == "AMP") {

                if (typeSecond != "int") throw std::runtime_error("ERROR");

                return "int*";

            } else if (firstNode.getName() == "STAR") {

                if (typeSecond != "int*") throw std::runtime_error("ERROR");

                return "int";

            }

        } else if (node.children.size() == 3) {

            auto firstNode = node.children[0];
            auto middleNode = node.children[1]; 

            if (firstNode.getName() == "LPAREN") {
                return getType(middleNode, procedureName);
            } else {
                // procedure must return int
                return "int";
            }

        } else if (node.children.size() == 4) {

            return "int";

        } else if (node.children.size() == 5) {

            auto exprNode = node.children[3];
            if (getType(exprNode, procedureName) != "int") throw std::runtime_error("ERROR");

            return "int*";

        }
    }

    if (node.getName() == "lvalue") {

        if (node.children.size() == 1) {
            return getType(node.children[0], procedureName);
        } else if (node.children.size() == 2) {
            if (getType(node.children[1], procedureName) != "int*") throw std::runtime_error("ERROR");
            return "int";
        } else {
            auto middleNode = node.children[1]; 
            return getType(middleNode, procedureName);
        }

    }

    throw std::runtime_error("ERROR");

    return "fail";
}

std::set<std::string> wlp4tiNodes = {"expr", "term", "factor", "lvalue", "ID", "NUM", "NULL"};

void printTree(Tree &root, std::string procedureName, std::string parentName, int parentNumChildren) {

    std::string name = root.getName();

    std::cout << root.lexeme;

    bool isProcedure = name == "ID" && ((parentName == "procedure") || (parentName == "factor" && parentNumChildren >= 3));

    if (root.getName() == "procedure") {
        procedureName = getProcedureName(root);
    } else if (root.getName() == "main") {
        procedureName = "main";
    } else if (wlp4tiNodes.count(root.getName()) > 0 && !isProcedure) {
        std::cout << " : " << getType(root, procedureName);
    }

    std::cout << std::endl;

    int numChildren = root.children.size();

    for (Tree node : root.children) {
        if (node.lexeme.size() > 0) printTree(node, procedureName, name, numChildren);
    }
}

void checkDCLs(Tree &procedureNode, std::string procedureName) {

    auto dclsSubtrees = getChildNodes(procedureNode, "dcls");
    for (auto dclsNode : dclsSubtrees) {

        if (dclsNode.getVarName() == ".EMPTY") {
            continue;
        }

        Tree &dclNode = dclsNode.children[1];
        Tree &rhsNode = dclsNode.children[3];

        auto lhsVar = extractDCLNode(dclNode);
        std::string typeLhs = lhsVar.second;
        std::string typeRhs = getType(rhsNode, procedureName);

        if (typeLhs != typeRhs) throw std::runtime_error("ERROR");

    }
}

void traverseArgslists(Tree &argsListNode, std::vector<std::string> &candidateArgs, std::string procedureName) {

    candidateArgs.push_back(getType(argsListNode.children[0], procedureName));

    if (argsListNode.children.size() == 1) {
        return;
    } else {
        traverseArgslists(argsListNode.children[2], candidateArgs, procedureName);
    }
}

void checkProcedureCalls(Tree &procedureNode, std::string procedureName) {

    auto factorNodes = getChildNodes(procedureNode, "factor");
    for (auto factorNode : factorNodes) {

        if (factorNode.children.size() < 3 || factorNode.children[0].getName() != "ID") {
            continue;
        }

        std::string calledProcedureName = factorNode.children[0].getVarName();

        // calledProcedureName must be before procedureName
        if (!declaredBefore(calledProcedureName, procedureName)) throw std::runtime_error("ERROR");

        // procedure doesn't exist
        if (tables.count(calledProcedureName) == 0) throw std::runtime_error("ERROR");
        std::vector<std::string> calledProcedureArgs = tables.at(calledProcedureName).first;

        // provided no arguments
        if (factorNode.children.size() == 3) {

            if (calledProcedureArgs.size() > 0) throw std::runtime_error("ERROR");

        } else if (factorNode.children.size() == 4) {

            // has args list so we want to obtain them
            std::vector<std::string> candidateArgs;
            auto argsListNode = factorNode.children[2];
            traverseArgslists(argsListNode, candidateArgs, procedureName);

            if (candidateArgs.size() != calledProcedureArgs.size()) throw std::runtime_error("ERROR");

            for (int i = 0; i < calledProcedureArgs.size(); i++) {
                if (candidateArgs[i] != calledProcedureArgs[i]) {
                    throw std::runtime_error("ERROR");
                }
            }

        }
    }

}

void checkIDs(Tree &procedureNode, std::string procedureName) {

    auto factorNodes = getChildNodes(procedureNode, "factor");
    auto lValueNodes = getChildNodes(procedureNode, "lvalue");

    for (auto factorNode : factorNodes) {
        if (factorNode.children.size() == 1 && factorNode.children[0].getName() == "ID") {
            // variable
            std::string varName = factorNode.children[0].getVarName();
            if (tables.at(procedureName).second.count(varName) == 0) {

                // std::cout << "var " << varName << std::endl;

                throw std::runtime_error("ERROR");
            }
        } else if (factorNode.children.size() >= 3 && factorNode.children[0].getName() == "ID") {
            // procedureName
            auto &procedureIdNode = factorNode.children[0];
            procedureIdNode.isProcedure = true;
            std::string funcName = procedureIdNode.getVarName();
            if (tables.count(funcName) == 0 || !declaredBefore(funcName, procedureName)) {

                // std::cout << "PROCEDURE " << procedureName << std::endl;
                // std::cout << "func " << funcName << std::endl;

                throw std::runtime_error("ERROR");
            }

        }

    }

    for (auto lValueNode : lValueNodes) {
        if (lValueNode.children.size() == 1 && lValueNode.children[0].getName() == "ID") {
            std::string varName = lValueNode.children[0].getVarName();
            if (tables.at(procedureName).second.count(varName) == 0) {
                throw std::runtime_error("ERROR");
            }
        }
    }
}

void wellTyped(Tree &statementNode, std::string procedureName) {

    std::string name = statementNode.getName();
    int numChildren = statementNode.children.size();

    if (name == "statement") {
        if (numChildren == 4) {
            // lvalue
            auto lvalueNode = statementNode.children[0];
            auto exprNode = statementNode.children[2];

            if (getType(lvalueNode, procedureName) != getType(exprNode, procedureName)) {
                throw std::runtime_error("ERROR");
            }
        }

        if (numChildren == 11) {
            // if
            auto testNode = statementNode.children[2];
            auto statements1Node = statementNode.children[5];
            auto statements2Node = statementNode.children[5];

            wellTyped(testNode, procedureName);
            wellTyped(statements1Node, procedureName);
            wellTyped(statements2Node, procedureName);
        }

        if (numChildren == 7) {
            // while

            auto testNode = statementNode.children[2];
            auto statementsNode = statementNode.children[5];

            wellTyped(testNode, procedureName);
            wellTyped(statementsNode, procedureName);
        }

        if (numChildren == 5 && statementNode.children[0].getName() == "PRINTLN") {
            // println

            auto exprNode = statementNode.children[2];
            if (getType(exprNode, procedureName) != "int") throw std::runtime_error("ERROR");

        } else if (numChildren == 5) {
            // delete

            auto exprNode = statementNode.children[3];
            if (getType(exprNode, procedureName) != "int*") throw std::runtime_error("ERROR");
            
        }
    } else if (name == "test") {

        auto expr1Node = statementNode.children[0];
        auto expr2Node = statementNode.children[2];

        if (getType(expr1Node, procedureName) != getType(expr2Node, procedureName)) {
            throw std::runtime_error("ERROR");
        }

    }
}

void mainCheck(Tree &root) {

    // if main doesn't exist
    if (tables.count("main") == 0) {
        throw std::runtime_error("ERROR");
    }

    auto mainNodes = getChildNodes(root, "main");
    auto mainNode = mainNodes[0];

    // check parameters cannot have same name already handled

    // check if 2nd parameter is not int
    if (tables.at("main").first.size() != 2) {
        throw std::runtime_error("ERROR");
    }

    if (tables.at("main").first[1] != "int") {
        throw std::runtime_error("ERROR");
    }
    
    for (auto mainChildNode : mainNode.children) {
        // return is type int
        if (mainChildNode.getName() == "expr" && getType(mainChildNode, "main") != "int") {
            throw std::runtime_error("ERROR");
        }
    }

    checkIDs(mainNode, "main");

    // check DCLs
    checkDCLs(mainNode, "main");

    // check procedure calls
    checkProcedureCalls(mainNode, "main");

    auto statements = getChildNodes(mainNode, "statement");
    for (auto statementNode : statements) {
        wellTyped(statementNode, "main");
    }
}

void genericProcedureCheck(Tree &procedureNode) {

    std::string procedureName = getProcedureName(procedureNode);

    for (auto procedureChild : procedureNode.children) {

        // return is type int
        if (procedureChild.getName() == "expr" && getType(procedureChild, procedureName) != "int") {
            throw std::runtime_error("ERROR");
        }

    }

    //std::cout << "hi 1" << std::endl;
    checkIDs(procedureNode, procedureName);

    //std::cout << "hi 2" << std::endl;
    // check DCLs
    checkDCLs(procedureNode, procedureName);

    //std::cout << "hi 3" << std::endl;
    // check procedure calls
    checkProcedureCalls(procedureNode, procedureName);

    auto statements = getChildNodes(procedureNode, "statement");
    for (auto statementNode : statements) {
        wellTyped(statementNode, procedureName);
    }
}

void checkAllProcedures(Tree &root) {
    auto allProcedures = getChildNodes(root, "procedure");
    for (auto &procedureNode : allProcedures) {
        genericProcedureCheck(procedureNode);
    }
}

int main() {

    Tree root = buildTree();
    std::vector<bool> flag(numV, true);
    
    try {

        fillTable(root);
        mainCheck(root);
        checkAllProcedures(root);
        printTree(root, "", "", -1);
        
    } catch (std::runtime_error& error) {
        std::cerr << error.what() << "\n";
        return 1;
    }
}