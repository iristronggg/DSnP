/****************************************************************************
  FileName     [ p2Json.h]
  PackageName  [ p2 ]
  Synopsis     [ Header file for class Json JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef P2_JSON_H
#define P2_JSON_H

#include <vector>
#include <string>
#include <unordered_set>
#include <cmath>

using namespace std;

class JsonElem
{
public:
   // TODO: define constructor & member functions on your own
   JsonElem() {}
   JsonElem(const string& k, int v): _key(k), _value(v) {}
    string key(){
        return _key;
    };
    int val(){
        return _value;
    }

   friend ostream& operator << (ostream&, const JsonElem&);

private:
   string  _key;   // DO NOT change this definition. Use it to store key.
   int     _value; // DO NOT change this definition. Use it to store value.
};

class Json
{
public:
   // TODO: define constructor & member functions on your own
    bool read(const string&);
    bool exist(){
        if(_obj.size()==0)
            return false;
        else return true;
    }
    void print(){
        cout << "{" << "\n" ;
        for(size_t i = 0; i < _obj.size(); i++){
            
            if(i == _obj.size()-1){
                cout << "  " << _obj[i].key() << " : " << _obj[i].val() << "\n";
            }
            else{
                cout << "  " << _obj[i].key() << " : " << _obj[i].val() << "," << "\n";
            }
        }
        cout << "}" << "\n";
    }
    void add(JsonElem je){
        _obj.push_back(je);
    }
    double sum(){
        double jsum = 0;
        for(size_t i = 0; i < _obj.size(); i++){
            jsum += _obj[i].val();
        }
        return jsum;
    }
    double ave(){
        double jave;
        jave = this->sum()/(_obj.size());
        return jave;
    }
    void max(){
        int jmax = _obj[0].val();
        string jmax_key;
        for(size_t i = 0; i < _obj.size(); i++){
            if(_obj[i].val() >= jmax){
                jmax = _obj[i].val();
                jmax_key = _obj[i].key();
            }
        }
        cout << "The maximum element is: { " << jmax_key << " : " << jmax << " }." << "\n";
    }
    void min(){
        int jmin = _obj[0].val();
        string jmin_key;
        for(size_t i = 0; i < _obj.size(); i++){
            if(_obj[i].val() <= jmin){
                jmin = _obj[i].val();
                jmin_key = _obj[i].key();
            }
        }
        cout << "The minimum element is: { " << jmin_key << " : " << jmin << " }." << "\n";
    }

private:
   vector<JsonElem>       _obj;  // DO NOT change this definition.
                                 // Use it to store JSON elements.
};

#endif // P2_TABLE_H
