/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdio.h>
#include "p2Json.h"
#include <sstream>
#include <iomanip>
//#include <json.h>

using namespace std;
void readFileJson(string s);
string get_key(string s);
int get_val(string s);
int linecnt1;

int main()
{
   Json json;

   // Read in the csv file. Do NOT change this part of code.
   string jsonFile;
    string command;
    string word;
    ifstream file;
    
   cout << "Please enter the file name: ";
   cin >> jsonFile;
    if (json.read(jsonFile)){
        cout << "File \"" << jsonFile << "\" was read in successfully." << endl;
        file.open(jsonFile, ifstream::in);

        while(getline(file,word)){
            linecnt1++;
            if(linecnt1 == 1){
                continue;
            }
            size_t n = 0;
            n = word.find_first_of("}", 0);
            if((n != string::npos) || (word == ""))
                break;
            else{
                JsonElem jsel(get_key(word), get_val(word));
                json.add(jsel);
            }
        }
        
        cout << "Enter command: ";
    }

   else {
      cerr << "Failed to read in file \"" << jsonFile << "\"!" << endl;
      exit(-1); // jsonFile does not exist.
   }

   // TODO read and execute commands
   // cout << "Enter command: ";
   //while (true) {
      // cout << "Enter command: ";
   //}
    while(true){
        cin >> command;
        if(command == "PRINT"){
            json.print();
        }
        else if (command == "SUM"){
            if(!json.exist());
            /*{
                cout << "Error: No element found!!" << "\n";
            }*/
            else{
                cout << "The summation of the values is: " << json.sum() << "." << "\n";
            }
        }
        else if (command == "AVE"){
            if(!json.exist());
            /*{
                cout << "Error: No element found!!" << "\n";
            }*/
            else{
                cout << "The average of the values is: " << fixed << setprecision(1) << json.ave() << "." << "\n";
            }
        }
        else if (command == "MAX"){
            if(!json.exist());
            /*{
                cout << "Error: No element found!!" << "\n";
            }*/
            else
                json.max();
        }
        else if (command == "MIN"){
            if(!json.exist());
            /*{
                cout << "Error: No element found!!" << "\n";
            }*/
            else
                json.min();
        }
        else if (command == "ADD"){
            string key;
            int val;
            cin >> key >> val;
            JsonElem jsel("\""+key+"\"", val);
            json.add(jsel);
        }
        else if (command == "EXIT"){
            return 0;
        }
        cout << "Enter command: ";
    }
}

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
            string tmp = s.substr(current, next - current + 1);
            if (tmp.size() != 0) {//忽略空字串
                key = tmp;
            }
        }

        current = next + 1; //下次由 next + 1 的位置開始找起。
        
    }
    return key;
};

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

/*void split(string s){
    char * pch;
    pch = strtok (s ,"{},\"");
    while (pch != NULL)
    {
        printf ("%s\n",pch);
        pch = strtok (NULL, "{},\"");
    }
}*/

/*void readFileJson(string s)
{
    Json::Reader reader;
    Json::Value root;
 
    ifstream in(s, ios::binary);
    
    if (reader.parse(in, root))
    {
        cout << root;
    }
    
    system("pause");
}*/
