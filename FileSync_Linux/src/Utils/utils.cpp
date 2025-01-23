#include "utils.h"


void addNode(TreeNode* root, const std::string& path, const std::string hash = "")
{
    size_t pos = 0;
    TreeNode* current = root;

    // Split the path into parts and traverse the tree
    std::string remaining_path = path;
    while((pos = remaining_path.find('/')) != std::string::npos)
    {
        std::string dir = remaining_path.substr(0,pos);
        remaining_path.erase(0,pos + 1);

        // If the directory does not exist, create it
        if(current->children.find(dir) == current->children.end())
        {
            current->children[dir] = new TreeNode{dir, true, "", {}};
        }

        // Move to the next level
        current = current->children[dir];
    }

    // Add the final file or directory
    std::string final_name = remaining_path;
    if(!hash.empty())
    {
        current->children[final_name] = new TreeNode{final_name, false, hash, {}};
    }
    else
    {
        current->children[final_name] = new TreeNode{final_name, true, "", {}};
    }

}

void printTree(TreeNode* node, const std::string& path="")
{
    std::string full_path = path + "/" + node->name;

    if(!node->is_directory)
    {
        // Print file details
        printf("File: %s, Hash: %s\n", full_path.c_str(), node->hash.c_str());
    }
    else
    {
        // Recurse for child nodes
        for (const auto& child : node->children)
        {
            printTree(child.second, full_path);
        }
    }
}

void compareTree(TreeNode* client, TreeNode* server, const std::string& path = "")
{
    std::string full_path = path + "/" + client->name;

    // Check if the node is a file
    if(!client->is_directory)
    {
        if(!server || client->hash != server->hash)
        {
            printf("Mismatch or missing file: %s\n", full_path.c_str());
        }
        return;
    }

    // Compare directories
    for(const auto& child : client->children)
    {
        if(server->children.find(child.first) == server->children.end())
        {
            printf("Missing on server: %s/%s\n", full_path.c_str(), child.first);
        }
        else
        {
            compareTree(child.second, server->children.at(child.first), full_path);
        }
    }
}

void deleteTree(TreeNode* node)
{
    for (auto& child : node->children)
    {
        deleteTree(child.second);   // Recursively delete child node
    }
    delete node;
}