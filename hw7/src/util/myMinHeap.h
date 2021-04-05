/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

#define num_p(x) ((x-1)/2)
#define num_l(x) ((x*2) +1)
#define num_r(x) ((x*2) +2)

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const {
       if(_data.size()!=0)
           return _data[0];
   }
   void insert(const Data& d) {
       size_t i = size();
       _data.push_back(d);
       
       while (i > 0 && _data[i] < _data[num_p(i)])
       {
          std::swap(_data[i], _data[num_p(i)]);
          i = num_p(i);
       }
   }
   void delMin() {
       delData(0);
       /*size_t p = 1, t = 2 * p;
       size_t n = _data.size();
       while (t <= n) {
         if (t < n) // has right child
           if (_data[t] < _data[t-1])
             ++t; // to the smaller child
         if (_data[n-1] < _data[t-1])
           break;
         //_data[p-1] = _data[t-1];
         swap(_data[p - 1], _data[t - 1]);
         p = t;
         t = 2 * p;
       }
       _data[p-1] = _data.back();
       _data.pop_back();*/

   }
   void delData(size_t i) {
       /*Data back = _data.back();
       _data.pop_back();
       
       int a = i, b = a * 2 + 1;
       if(a == _data.size()) return;
       while(b + 1 < _data.size()) {
          if(b + 1 < _data.size())
             if(_data[b + 1] < _data[b]) ++b;
          if(back < _data[b]) break;
          _data[a] = _data[b];
          a = b;
          b = a * 2 + 1;
       }
       
       b = (a - 1) / 2;
       while(b > 0) {
          if(!(_data[b] < back)) {
             _data[a] = _data[b];
             a = b;
             b = (a - 1) / 2;
          }
          else break;
       }
       
       _data[a] = back;*/
       
       swap(_data[i], _data[size()-1]);
       _data.pop_back();
       size_t s = size();


        while (num_r(i) < s)
        {
          if(_data[num_r(i)] < _data[i] && _data[num_r(i)] < _data[num_l(i)])
          {
             swap(_data[i], _data[num_r(i)]);
             i = num_r(i);
          }
          else if (_data[num_l(i)] < _data[i])
          {
             swap(_data[i], _data[num_l(i)]);
             i = num_l(i);
          }
          else
             break;
       }
       while (i > 0 && _data[i] < _data[num_p(i)])
       {
          std::swap(_data[i], _data[num_p(i)]);
          i = num_p(i);
       }

       // special case: two items
       // only left child
       if(num_r(i) == s && _data[num_l(i)] < _data[i])
          swap(_data[i], _data[num_l(i)]);
       
       
    }
    

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
