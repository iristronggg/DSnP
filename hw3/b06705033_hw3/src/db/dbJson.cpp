/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include <vector>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
string get_key(string s)
{
    size_t current = 0; //最初由 0 的位置開始找
    size_t next = 0;
    string key;
    
    while (1)
    {
       //current = next + 1;
        next = s.find_first_of("\"", current);
        if (next == string::npos) break;
        current = next;
        next = s.find_first_of("\"", current+1);
        if ((next != current) && (next >= 0))
        {
            string tmp = s.substr(current+1, next - current-1 );
            if (tmp.size() != 0) {//忽略空字串
                key = tmp;
            }
        }

        current = next + 1; //下次由 next + 1 的位置開始找起。
        
    }
    return key;
}

int get_val(string s){
    char chr[100];
    int val = 1;
    int x = 0;
    bool flag;
    bool negative;
    strcpy(chr,s.c_str());
    
    flag = false;
    negative = false;
    for(size_t i = 0; i < s.length(); i++){
        if((int)chr[i]==45){
            flag = true;
            negative = true;
            val = val * (-1);
        }
        if((47<(int)chr[i])  && ((int)chr[i] < 58)){
            flag = true;
            int n = (int)chr[i]-48;
            if(negative == false){
                if (x==0){
                    val = val*n;
                }
                else val = val*10 + n;
            }
            else if(negative == true){
                if (x==0){
                    val = val*n;
                }
                else val = val*10 - n;
            }
            x++;
        }
    }
    if(flag == true)
        return val;
}

ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
}
    


istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
    assert(j._obj.empty());
   string jsonFile;
   int linecnt=0;
   string word;
    
   while(getline(is,word)){
       linecnt++;
       if(linecnt == 1){
           continue;
       }
       size_t n = 0;
       n = word.find_first_of("}", 0);
       if((n != string::npos) || (word == ""))
           break;
       else{
           DBJsonElem jsel(get_key(word), get_val(word));
           j.add(jsel);
       }
   }
   
   j.read = true;
   return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
    os << "{" << "\n" ;
    for(size_t i = 0; i < j.size(); i++){
        if(i == j.size()-1){
            os << "  " << j[i] << "\n";
        }
        else{
            os << "  " << j[i] << "," << "\n";
        }
    }
    os << "}" << "\n";
    return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
    
void
DBJson::reset()
{
   // TODO
    _obj.clear();
    read = false;
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   // TODO
    for(size_t i = 0; i < this->size(); i++){
        if(elm.key() == _obj[i].key())
            return false;
    }
   _obj.push_back(elm);
   return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
    if(this->empty())
        return NAN;
    else{
        float jave;
        jave = this->sum()/(_obj.size());
        return jave;
    }
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
    int maxN = INT_MIN;
    if(this->empty())
    {
        idx = this->size();
        return INT_MIN;
    }
    else{
        for(size_t i = 0; i < this->size(); i++){
            if(_obj[i].value() >= maxN){
                maxN = _obj[i].value();
                idx = i;
            }
        }
        return maxN;
    }
}

// If DBJson is empty, set idx to size() and return INT_MAX
int
DBJson::min(size_t& idx) const
{
   // TODO
    int minN = INT_MAX;
    if(this->empty())
    {
        idx = this->size();
        return INT_MAX;
    }
    else{
        for(size_t i = 0; i < this->size(); i++){
            if(_obj[i].value() <= minN){
                minN = _obj[i].value();
                idx = i;
            }
        }
        return minN;
    }
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
    int s = 0;
    if(this->size() != 0){
        for(size_t i = 0; i < this->size(); i++){
            s += _obj[i].value();
        }
    }
    return s;
}
    
bool
DBJson::find_key_idx(const string &key, size_t &idx) const
{
    for(size_t i = 0; i < this->size(); i++){
        if(key == _obj[i].key()){
            idx = i;
            return true;
        }
    }
    return false;
}
