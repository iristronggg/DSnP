/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
    friend class BSTree<T>;
    friend class BSTree<T>::iterator;
    
     BSTreeNode(const T& d, BSTreeNode<T>* p = 0, BSTreeNode<T>* r = 0, BSTreeNode<T>* l = 0):
       _data(d), _parent(p), _right(r), _left(l) {}
     ~BSTreeNode() {}
    
    
   // TODO: design your own class!!
    T                    _data;
    BSTreeNode<T>*   _parent;
    BSTreeNode<T>*   _left;
    BSTreeNode<T>*   _right;
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
public:
    BSTree(): _size(0){
        _dummy = new BSTreeNode<T>(T());
        _root = _dummy;
    }
    class iterator {
       friend class BSTree;

       public:
          iterator(BSTreeNode<T>* n= 0): _node(n) {}
          iterator(const iterator& i) : _node(i._node) {}
          ~iterator() {} // Should NOT delete _node

          // TODO: implement these overloaded operators
          const T& operator * () const { return _node->_data; }
          T& operator * () { return _node->_data; }
          iterator& operator ++ () {
              if(_node->_right) {
                  _node = _node->_right;
                  while(_node->_left)
                      _node = _node->_left;
               }
               else {
                  while(1) {
                     if(_node->_parent == 0) break;
                     if(_node ==_node->_parent->_left) break;
                     _node = _node->_parent;
                  }
                  _node = _node->_parent;
               }
              return *(this);
          }
          iterator operator ++ (int) { iterator temp = (*this); ++(*this); return temp; }
          iterator& operator -- () {
              if(_node->_left) {
                 _node = _node->_left;
                 while(_node->_right)
                     _node = _node->_right;
              }
              else {
                 while(1) {
                    if(_node->_parent == 0) break;
                    if(_node == _node->_parent->_right) break;
                    _node = _node->_parent;
                 }
                 _node = _node->_parent;
              }
              return *(this);
          }
          iterator operator -- (int) { iterator temp = (*this); --(*this); return temp; }

          iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

          bool operator != (const iterator& i) const { return (_node != i._node); }
          bool operator == (const iterator& i) const { return (_node == i._node); }

       private:
        BSTreeNode<T>* _node;

    };
    
    iterator begin() const {
        BSTreeNode<T>* temp = _root;
        return iterator(Leftmost(temp));
    }
    
    iterator end() const { return iterator(Rightmost(_root))++; }
    
    bool empty() const { return (_size == 0); }

    size_t size() const { return _size; }
    
    void pop_front() { erase(begin()); }
    void pop_back() { erase(iterator(Rightmost(_root))); }
    
    void insert(const T& x){
        BSTreeNode<T>* temp = _root;
        while(1) {
            if(temp == _dummy) {
                BSTreeNode<T>* n = new BSTreeNode<T>(x, _dummy->_parent, _dummy);
                if(temp == _root) _root = n;
                else _dummy->_parent->_right = n;
                _dummy->_parent = n;
                break;
            }
            else if(x < temp->_data) {
                if(temp->_left) temp = temp->_left;
                else {
                    BSTreeNode<T>* n = new BSTreeNode<T>(x, temp);
                    temp->_left = n;
                    break;
                }
            }
            else {
                if(temp->_right) temp = temp->_right;
                else {
                    BSTreeNode<T>* n = new BSTreeNode<T>(x, temp);
                    temp->_right = n;
                    break;
                }
            }
        }
        ++_size;
    }
    
    bool erase(iterator pos){
        BSTreeNode<T> *delete_node = pos._node;
        
        if(empty())
            return false;
        
        BSTreeNode<T> *y = 0;      // 真正要刪
        BSTreeNode<T> *x = 0;      // 要被刪除的node的"child"

        if (delete_node->_left == NULL || delete_node->_right == NULL){
            y = delete_node;
        }
        else{
            y = Successor(delete_node);        // 有兩個child，將y設成delete_node的Successor
        }
                                               // 全部調整成case1或case2來處理
        if (y->_left != NULL){
            x = y->_left;                  // x = y's child
        }
        else{
            x = y->_right;
        }
        
        if (x != NULL){                        // 在y被刪除之前把x接回BST
            x->_parent = y->_parent;
        }
                                            
        if (y->_parent == NULL){
            this->_root = x;
        }
        else if (y == y->_parent->_left){    // x replaces y
            y->_parent->_left = x;
        }
        else{
            y->_parent->_right = x;
        }
                                                
        if (y != delete_node) {                 //case3:若y是delete_node的替身, 最後要再將y的資料
            delete_node->_data = y->_data;      // 放回delete_node的記憶體位置, 並將y的記憶體位置釋放
        }

        delete y;
        y = 0;
        _size--;
        return true;
    }
    
    bool erase(const T& x) {
       for (iterator i = begin(); i != end(); i++) {
          if(i._node->_data == x)
             return erase(i);
       }
       return false;
    }
    
    void clear() {
        iterator h = begin(), t = end(), i = h;
        while(i != t){
           ++i;
           erase(h);
           h = i;
        }
    }
    
    iterator find(const T &x){

        BSTreeNode<T> *current = _root;
        while (current != NULL && x != current->_data) {
            if (x < current->_data){
                current = current->_left;   //turn left
            }
            else {
                current = current->_right;  //turn right
            }
        }
        return iterator(current);
    }
    
    void sort() const {}
    
    
    /*void preorderPrint(BSTreeNode<T> *root) const
    {
       cout << "  ";
       if (root != _dummy)
       {
           cout << "  ";
           cout << root->_data << endl; // L
           preorderPrint(root->_left);  // R
           preorderPrint(root->_right); // V
        }
        else
        {
            cout << "[0]" << endl;
        }
    }*/
    
    void print() const{
    }
    
    
private:
    BSTreeNode<T>*  _root;
    BSTreeNode<T>* _dummy;
    
    BSTreeNode<T>* Rightmost(BSTreeNode<T> *current) const{
        while (current->_right != NULL){
            current = current->_right;
        }
        return current;
    }
    BSTreeNode<T>* Leftmost(BSTreeNode<T> *current) const{
        while (current->_left != NULL){
            current = current->_left;
        }
        return current;
    }
    
    BSTreeNode<T>* Predecessor(BSTreeNode<T> *current){
        if (current->_left != NULL){
            return Rightmost(current->_left);
        }

        BSTreeNode<T> *new_node = current->_parent;

        while (new_node != NULL && current == new_node->_left) {
            current = new_node;
            new_node = new_node->_parent;
        }
        return new_node;
    }
    
    BSTreeNode<T>* Successor(BSTreeNode<T> *current) const{
        if (current->_right != NULL){
            return Leftmost(current->_right);
        }
        
        BSTreeNode<T> *new_node = current->_parent;
        while (new_node != NULL && current == new_node->_right) {
            current = new_node;
            new_node = new_node->_parent;
        }
        return new_node;
    }
    
    size_t _size;
};

#endif // BST_H
