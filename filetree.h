#ifndef FILETREE_H
#define FILETREE_H
#include <QString>

struct TreeNode{
    QString element;//该节点的元素
    TreeNode *firstChild;//指向该节点的第一个孩子
    TreeNode *nextSibling;//指向该节点的兄弟节点

};

class FileTree
{
public:
    FileTree(TreeNode * r = NULL):root(r){}
    FileTree(int node_num);
    ~FileTree();

    void addNode(int i, int j);
    void preOrder();//前序遍历
    void print();//打印
private:
    void print(TreeNode* node, int num);
    void addBrotherNode(TreeNode* bro, TreeNode* node);
    void preOrder(TreeNode* parent);//前序遍历
private:
    TreeNode * root;//该树的根
};
#endif // FILETREE_H
