#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <unordered_map>

// Token types
enum class TokenType {
    IDENTIFIER,
    KEYWORD,
    OPERATOR,
    LITERAL,
    COMMENT,
    WHITESPACE,
    UNKNOWN
};

// Token structure
struct Token {
    TokenType type;
    std::string value;
};

// Lexer
class Lexer {
public:
    Lexer(const std::string& code) : code(code), pos(0) {}

    Token getNextToken() {
        skipWhitespace();

        if (pos >= code.size()) {
            return {TokenType::UNKNOWN, ""};
        }

        char currentChar = code[pos];
        if (isalpha(currentChar) || currentChar == '_') {
            return lexIdentifier();
        } else if (isdigit(currentChar)) {
            return lexLiteral();
        } else {
            return {TokenType::UNKNOWN, std::string(1, currentChar)};
        }
    }

private:
    const std::string code;
    size_t pos;

    void skipWhitespace() {
        while (pos < code.size() && isspace(code[pos])) {
            ++pos;
        }
    }

    Token lexIdentifier() {
        std::string identifier;
        while (pos < code.size() && (isalnum(code[pos]) || code[pos] == '_')) {
            identifier += code[pos++];
        }
        return {TokenType::IDENTIFIER, identifier};
    }

    Token lexLiteral() {
        std::string literal;
        while (pos < code.size() && isdigit(code[pos])) {
            literal += code[pos++];
        }
        return {TokenType::LITERAL, literal};
    }
};

// Parser
class Parser {
public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens), index(0) {}

    std::shared_ptr<ASTNode> parse() {
        return parseProgram();
    }

private:
    const std::vector<Token>& tokens;
    size_t index;

    std::shared_ptr<ASTNode> parseProgram() {
        auto programNode = std::make_shared<ASTNode>("Program", "");
        while (index < tokens.size()) {
            auto statementNode = parseStatement();
            if (statementNode) {
                programNode->children.push_back(statementNode);
            }
        }
        return programNode;
    }

    std::shared_ptr<ASTNode> parseStatement() {
        auto token = getCurrentToken();
        if (token.type == TokenType::IDENTIFIER) {
            // Example: Identifier followed by a semicolon
            auto statementNode = std::make_shared<ASTNode>("Statement", token.value);
            consumeToken();
            if (getCurrentToken().value == ";") {
                consumeToken(); // Consume semicolon
                return statementNode;
            } else {
                std::cerr << "Expected semicolon after identifier." << std::endl;
            }
        } else {
            std::cerr << "Unexpected token: " << token.value << std::endl;
        }
        return nullptr;
    }

    Token getCurrentToken() const {
        if (index < tokens.size()) {
            return tokens[index];
        } else {
            return {TokenType::UNKNOWN, ""};
        }
    }

    void consumeToken() {
        ++index;
    }
};

// AST Node
class ASTNode {
public:
    std::string type;
    std::string value;
    std::vector<std::shared_ptr<ASTNode>> children;

    ASTNode(const std::string& type, const std::string& value) : type(type), value(value) {}
};

// AST Visitor
class ASTVisitor {
public:
    void visit(const std::shared_ptr<ASTNode>& node) {
        std::cout << "Visited node of type " << node->type << " with value " << node->value << std::endl;
        for (const auto& child : node->children) {
            visit(child);
        }
    }
};

// AST Serialization
class ASTSerializer {
public:
    std::string serialize(const std::shared_ptr<ASTNode>& node) {
        std::ostringstream oss;
        serializeNode(node, oss);
        return oss.str();
    }

private:
    void serializeNode(const std::shared_ptr<ASTNode>& node, std::ostringstream& oss) {
        oss << "{ \"type\": \"" << node->type << "\", \"value\": \"" << node->value << "\", \"children\": [";
        for (size_t i = 0; i < node->children.size(); ++i) {
            serializeNode(node->children[i], oss);
            if (i != node->children.size() - 1) {
                oss << ", ";
            }
        }
        oss << "] }";
    }
};

// Main function
int main() {
    // Read input C source code
    std::ifstream file("input.c");
    if (!file.is_open()) {
        std::cerr << "Failed to open input file." << std::endl;
        return 1;
    }
    std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Lexing
    Lexer lexer(code);
    std::vector<Token> tokens;
    Token token;
    do {
        token = lexer.getNextToken();
        tokens.push_back(token);
    } while (token.type != TokenType::UNKNOWN);

    // Parsing
    Parser parser(tokens);
    auto ast = parser.parse();

    // AST Traversal
    ASTVisitor visitor;
    visitor.visit(ast);

    // AST Serialization
    ASTSerializer serializer;
    std::string serializedAst = serializer.serialize(ast);
    std::cout << "Serialized AST: " << serializedAst << std::endl;

    return 0;
}
