#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <unordered_map>

#define PORT 25500

typedef struct TreeNode{
    std::string name;          // File or directory name
    bool is_directory;
    std::string hash;       // MD5 is 16 bytes
    std::unordered_map<std::string, TreeNode*> children;
} TreeNode;

// Hashmap for clientID
std::unordered_map<int, TreeNode* > client_map;


void addNode(TreeNode* root, const std::string& path, const std::string hash = "");
void printTree(TreeNode* node, const std::string& path="");
void compareTree(TreeNode* cleint, TreeNode* server, const std::string& path = "");
void deleteTree(TreeNode* node);