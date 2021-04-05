/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node = _node->_next; return (*this); }
      iterator operator ++ (int) { iterator temp = (*this); _node = _node->_next; return temp; }
      iterator& operator -- () { _node = _node->_prev; return *(this); }
      iterator operator -- (int) { iterator temp = (*this); _node = _node->_prev; return temp; }

      iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

      bool operator != (const iterator& i) const { return (_node != i._node); }
      bool operator == (const iterator& i) const { return (_node == i._node); }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head); }
   iterator end() const { return iterator(_head->_prev); }
   bool empty() const { return (_head == _head->_next); }
   size_t size() const {
       size_t sz = 0;
       for(iterator i = begin(); i != end(); i++){
           sz++;
       }
       return sz;
   }

   void push_back(const T& x) {
       if(empty()){
           DListNode<T> *t = new DListNode<T>(x, _head, _head); //t->_prev = t->_next = _head;
           _head->_next = _head->_prev = t; // _head is a dummy node first
           _head = t;
       }
       else{
           DListNode<T> *t = new DListNode<T>(x, _head->_prev->_prev, _head->_prev);
           (_head->_prev->_prev)->_next = t;
           (_head->_prev)->_prev = t;
       }
       _isSorted = false;
   }
   void pop_front() { //what if only head left??
       if(empty())
           return;
       DListNode<T>* t = _head->_next; //t = new head
       t->_prev = _head->_prev; //t's previous = dummy node
       (_head->_prev)->_next = t; //dummy node's next = t
       delete _head;
       _head = t;
       
   }
   void pop_back() {
       if(empty())
           return;
       DListNode<T>* t = _head->_prev->_prev; //t = last node to be poped
       (t->_prev)->_next = _head->_prev; //new tail's next = dummy node
       (_head->_prev)->_prev = t->_prev; //dummy node's previous = new tail
       delete t;
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
       if(empty())
           return false;
       
       if(pos == begin())
           _head = _head->_next;
       /*else if((pos == begin()) && (_head->_next->_next == _head)){
           _head = _head->_next;
           _head->_prev = _head->_next = _head; // _head is a dummy node
       } //only head left*/
       
       (pos._node)->_prev->_next = (pos._node)->_next;
       (pos._node)->_next->_prev = (pos._node)->_prev;
       delete (pos._node);
       
       return true;
   }
   bool erase(const T& x) {
       for(iterator i = begin(); i != end(); i++){
           if(*i == x)
               return erase(i);
       }
       return false;
   }

   iterator find(const T& x) {
       for(iterator i = begin(); i != end(); i++){
           if(*i == x)
               return i;
       }
       return end();
   }

   void clear() {
       if(!empty()){
           DListNode<T> *dummy = _head->_prev;
           for(iterator i = begin(); i != end(); i++){
               delete i._node;
           }
           _head = dummy;
           _head->_prev = _head->_next = dummy;
           _isSorted = false;
       }
       
   }  // delete all nodes except for the dummy node

   void sort() const {
       if(_isSorted)
           return;
        _quickSort(_head, _head->_prev->_prev);
        _isSorted = true;
   }
    
    void swap(T* a, T* b) const {
        T t = *a;
        *a = *b;
        *b = t;
    }
    
    DListNode<T>* partition(DListNode<T> *l, DListNode<T> *h) const
    {
        T x = h->_data;
        DListNode<T> *i = l->_prev;

        for (DListNode<T> *j = l; j != h; j = j->_next)
        {
            if (j->_data <= x)
            {
                if(i == _head->_prev)
                    i = l;
                else i = i->_next;
                
               
                swap(&(i->_data), &(j->_data));
            }
        }
        i = (i == (_head->_prev))? l : i->_next;
        swap(&(i->_data), &(h->_data));
        return i;
    }
      
    /* A recursive implementation
    of quicksort for linked list */
    void _quickSort(DListNode<T> *l, DListNode<T> *h) const
    {
        if (l != _head->_prev && l != h && l != h->_next)
        {
            DListNode<T> *p = partition(l, h);
            _quickSort(l, p->_prev);
            _quickSort(p->_next, h);
        }
    }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
