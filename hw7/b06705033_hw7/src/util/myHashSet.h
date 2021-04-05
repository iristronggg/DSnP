/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
       //iterator(const iterator&i):
       //iterator(size_t n, vector<Data>* b): _numBuckets(n), bktIndex(0), index(0), _buckets(b) {}
       iterator(const size_t idx, const size_t bktidx, const size_t n, vector<Data>* b):  _node(&b[bktidx][idx]), index(idx), bktIndex(bktidx), _numBuckets(n), _buckets(b) {}
       ~iterator() {}
      const Data& operator * () const { return *(this->_node); }
      Data& operator * () { return *(this->_node); }
      iterator& operator ++ () {
          if(bktIndex == _numBuckets) return (*this);
          else if(index < _buckets[bktIndex].size() - 1) ++index;
          else{
             while(++bktIndex < _numBuckets) {
                if(!_buckets[bktIndex].empty()) {
                   break;
                }
             }
             index = 0;
          }
          if(bktIndex != _numBuckets) _node = &_buckets[bktIndex][index];
          return (*this);
      }
       iterator operator ++ (int) { iterator it = (*this); ++(*this); return it; }
      iterator& operator -- () {
          if(index > 0){
              index--;
              _node = &_buckets[bktIndex][index];
          }
          else{
              while(bktIndex-- >= 0){
                  if(!_buckets[bktIndex].empty())
                      break;
              }
              index = _buckets[bktIndex].size()-1;
              _node = &_buckets[bktIndex][index];
          }
          return (*this);
      }
      iterator operator -- (int) { iterator it = (*this); --(*this); return it; }
      iterator& operator = (const iterator& i) {
          this->_node = i._node;
          this->_numBuckets = i._numBuckets;
          this->bktIndex = i.bktIndex;
          this->index = i.index;
          return *(this);
      }
       bool operator == (const iterator& i) const { return (i.index == index && i.bktIndex == bktIndex); }
      bool operator != (const iterator& i) const { return !(i == *(this)); }
      
   private:
       Data* _node;
       size_t            index, bktIndex, _numBuckets;
       vector<Data>*     _buckets;
       
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
       for(size_t i = 0; i < _numBuckets; i++){
           if(!_buckets[i].empty())
               return iterator(0, i, _numBuckets, _buckets);
       }
       return end();
   }
   // Pass the end
   iterator end() const { return iterator(0, _numBuckets, _numBuckets, _buckets); }
   // return true if no valid data
   bool empty() const { return (begin() == end()); }
   // number of valid data
   size_t size() const {
       size_t s = 0;
       for(size_t i = 0; i < _numBuckets; i++){
           if(!_buckets[i].empty())
               s += _buckets[i].size();
       }
       return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
       size_t num = bucketNum(d);
       if(_buckets[num].empty()){
           return false;
       }
       else{
           for(size_t i = 0; i < _buckets[num].size(); i++){
               if(_buckets[num][i] == d)
                   return true;
           }
       }
       return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
       size_t num = bucketNum(d);
       if(_buckets[num].empty()){
           return false;
       }
       else{
           for(size_t i = 0; i < _buckets[num].size(); i++){
               if(_buckets[num][i] == d){ //same name
                   d = _buckets[num][i]; //replace
                   return true;
               }
           }
       }
       return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
       size_t num = bucketNum(d);
       for(size_t i = 0; i < _buckets[num].size(); i++){
           if(_buckets[num][i] == d){ //same name
               _buckets[num][i] = d; //replace
               return true;
           }
       }
       _buckets[num].push_back(d);
       return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
       size_t num = bucketNum(d);
       if(check(d))
           return false;
       _buckets[num].push_back(d);
       return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
       size_t num = bucketNum(d);
       if(_buckets[num].empty()){
           return false;
       }
       else{
           for(size_t i = 0; i < _buckets[num].size(); i++){
               if(_buckets[num][i] == d){ //same name
                   //_buckets[num].erase(_buckets[num].begin() + i);
                   _buckets[num][i] = _buckets[num][_buckets[num].size()-1];
                   _buckets[num].pop_back();
                   return true;
               }
           }
       }
       return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
