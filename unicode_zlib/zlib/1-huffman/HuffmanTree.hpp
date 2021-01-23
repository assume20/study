/*
 * @Author:darren
 * @Date: 2020-06-04 16:21:01
 * @LastEditTime: 2020-06-06 08:48:16
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \1-huffman\HuffmanTree.hpp
 */ 
#ifndef _HUFFMAN_TREE_H_ 
#define _HUFFMAN_TREE_H_
#include <iostream>
#include <queue>
#include<vector>
using namespace std;

template<class W>
struct HuffmanTreeNode
{
    HuffmanTreeNode(const W &weight)
    : _pLeft(NULL)
    , _pRight(NULL)
    , _pParent(NULL)
    , _weight(weight)
    {}
    HuffmanTreeNode<W>*_pLeft;
    HuffmanTreeNode<W>*_pRight;
    HuffmanTreeNode<W>*_pParent;
    W _weight;
};

template<class W>
class HuffmanTree
{
    typedef HuffmanTreeNode<W>*PNode;
public:
        HuffmanTree()
        : _pRoot(NULL)
    {}
    HuffmanTree(W*array, size_t size, const W&invalid)
    {
        _CreateHuffmantree(array,  size, invalid);

    }
    void _Destroy(PNode&pRoot)
    {
        //后序
        if (pRoot)
        {
            _Destroy(pRoot->_pLeft);
            _Destroy(pRoot->_pRight);
            delete pRoot;
            pRoot = NULL;
        }
    }
    ~HuffmanTree()
    {
        _Destroy(_pRoot);
    }
    PNode GetRoot()
    {
        return  _pRoot;
    }
private:    
    //构建哈夫曼树
    void _CreateHuffmantree(W*array, size_t size, const W&invalid)
    {

        struct PtrNodeCompare
        {
            bool operator()(PNode n1, PNode n2)//重载“（）”
            {
                return n1->_weight <  n2->_weight;  // 实际比较的的是 CharInfo的 < 重载， char info比较的是
            }
        };
        priority_queue<PNode, vector<PNode>, PtrNodeCompare> hp;     // 实质构造的是最小堆来的

        for (size_t i = 0; i < size; ++i)
        {
            if (array[i] != invalid)    // 实际比较的是CharInfo的!=
            {
                hp.push(new HuffmanTreeNode<W>(array[i]));      // 数据入堆
            }
        }
        //空堆
        if (hp.empty())
            _pRoot = NULL;
        while (hp.size()>1)
        {
            PNode pLeft = hp.top(); // 取出来一个元素当做左边节点
            hp.pop();
            PNode pRight = hp.top();    // 取出来一个元素当做右边节点
            hp.pop();
            PNode pParent = new HuffmanTreeNode<W>(pLeft->_weight + pRight->_weight);//左加右的权值，作为新节点
            pParent->_pLeft = pLeft;
            pLeft->_pParent = pParent;

            pParent->_pRight = pRight;
            pRight->_pParent = pParent;
            hp.push(pParent);
        }
        _pRoot = hp.top();          // 节点在堆
    }

public:
    PNode _pRoot;
};
#endif