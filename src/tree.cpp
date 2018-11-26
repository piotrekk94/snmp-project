#include <tree.hpp>
#include <iostream>
#include <queue>

TreeNode::TreeNode(Object *object){
    this->object = object;
}

void TreeNode::addNode(TreeNode *node){
    this->children.push_back(node);
}

void TreeNode::addNode(TreeNode *node, int n){
    if(this->children.size() < n)
        this->children.resize(n, nullptr);

    if(this->children[n - 1] == nullptr)
        this->children[n - 1] = node;
}

TreeNode *TreeNode::getNode(int n){
    if(n >= this->children.size())
        return nullptr;
    return this->children.at(n);
}

TreeNode *TreeNode::getNode(std::string name){
    return nullptr;
}

Object *TreeNode::getValue(){
    return this->object;
}

int TreeNode::getSize(){
    return this->children.size();
}

void TreeNode::printStructure(std::vector<std::string> &res, int level){
    std::string node;
    for(int i = 0; i < level; i++)
        node.append("\t");

    node.append(this->getValue()->getName());
    res.push_back(node);

    if(this->getValue()->isLeaf())
        printLeaf(res, level + 1);

    for(auto& n : this->children)
        if(n != nullptr)
            n->printStructure(res, level + 1);
        else
            printNull(res, level + 1);
}

void TreeNode::printLeaf(std::vector<std::string> &res, int level){
    std::string node;
    for(int i = 0; i < level; i++)
        node.append("\t");

    res.push_back(node + "SYNTAX: " + this->getValue()->getTypeName());
    res.push_back(node + "ACCESS: " + this->getValue()->getAccess());
    res.push_back(node + "STATUS: " + this->getValue()->getStatus());
    res.push_back(node + "DESC: " + this->getValue()->getDesc());
}

void TreeNode::printNull(std::vector<std::string> &res, int level){
    std::string node;
    for(int i = 0; i < level; i++)
        node.append("\t");

    node.append("---");
    res.push_back(node);
}

ObjectTree *tree;

ObjectTree::ObjectTree(Object *object){
    this->root = new TreeNode(object);
}

TreeNode *ObjectTree::getNode(std::string name){
    std::queue<TreeNode*> queue;

    queue.push(this->root);

    while(!queue.empty()){
        auto node = queue.front();
        if(!name.compare(node->getValue()->getName()))
            return node;
        else
            for(int i = 0; i < node->getSize(); i++){
                auto newNode = node->getNode(i);
                if(newNode != nullptr)
                    queue.push(newNode);
            }
        queue.pop();
    }

    return nullptr;
}

Object *ObjectTree::getObject(std::string name){
    return this->getNode(name)->getValue();
}

TreeNode *ObjectTree::getNode(ObjectPath& path, bool addMissing){
    auto node = this->getNode(std::get<0>(path.path[0]));
    if(node == nullptr){
        std::cout<<"Could not find node: "<<std::get<0>(path.path[0])<<std::endl;
        return nullptr;
    }
    if(path.path.size() > 1){
        for(int i = 1; i < path.path.size(); i++){
            auto nextNode = node->getNode(std::get<1>(path.path[i]) - 1);
            if(nextNode == nullptr){
                if(addMissing){
                    Object *newObject = new Object(std::get<0>(path.path[i]));
                    nextNode = new TreeNode(newObject);
                    node->addNode(nextNode, std::get<1>(path.path[i]));
                } else {
                    std::cout<<"Could not find"<<i<<":"<<std::get<1>(path.path[i])<<std::endl;
                    return nullptr;
                }
            }
            node = nextNode;
        }
    }

    return node;
}

Object *ObjectTree::getObject(ObjectPath& path){
    auto node = this->getNode(path, false);
    return node != nullptr ? node->getValue() : nullptr;
}

std::vector<std::string> ObjectTree::printTree(){
    std::vector<std::string> res;
    this->root->printStructure(res, 0);
    return res;
}

void ObjectTree::addObject(Object *object, ObjectPath& path){
    auto node = this->getNode(std::get<0>(path.path[0]));
    if(node == nullptr){
        std::cout<<"Could not find node: "<<std::get<0>(path.path[0])<<std::endl;
        return;
    }
    if(path.path.size() > 2){
        for(int i = 1; i < path.path.size() - 1; i++){
            auto nextNode = node->getNode(std::get<1>(path.path[i]) - 1);
            if(nextNode == nullptr){
                Object *newObject = new Object(std::get<0>(path.path[i]));
                nextNode = new TreeNode(newObject);
                node->addNode(nextNode, std::get<1>(path.path[i]));
            }
            node = nextNode;
        }
    }

    TreeNode *newNode = new TreeNode(object);

    node->addNode(newNode, std::get<1>(path.path[path.path.size() - 1]));
}

std::ostream &operator<<(std::ostream &os, ObjectTree &tree){
    auto out = tree.printTree();
    for(auto& val : out)
        os<<val<<std::endl;
    return os;
}