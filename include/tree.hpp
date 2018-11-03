#pragma once
#include <vector>
#include <object.hpp>

class TreeNode {
private:
    Object *object;
    std::vector<TreeNode*> children;
public:
    TreeNode(Object *object);
    TreeNode* getNode(int n);
    TreeNode* getNode(std::string name);
    void addNode(TreeNode *node);
    void addNode(TreeNode *node, int n);
    Object* getValue();
    int getSize();
    void printStructure(std::vector<std::string> &res, int level);
};

class ObjectTree {
private:
    TreeNode *root;
    TreeNode *getNode(std::string name);
public:
    ObjectTree(Object *object);
    void addObject(Object *object, ObjectPath& path);
    Object *getObject(ObjectPath& path);
    Object *getObject(std::string name);
    std::vector<std::string> printTree();
};

extern ObjectTree *tree;

std::ostream &operator<<(std::ostream &os, ObjectTree &tree);