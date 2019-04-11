#include <iostream>
#include <regex>
#include <list>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

bool compare_nocase (const std::string& first, const std::string& second);

class Transaction{
public:
	void addstring(string s){
		str = s;
	}
	void additem(string s){
		items.push_back(s);
	}
	void sortitems(){
		items.sort(compare_nocase);
	}
	void printitems(){
		for(auto& i : items){
			cout << i << endl;
		}
	}
	list<string> getitemlist(){
		return items;
	}
private:
	list<string> items;
	string str;
};

class Tree{
public:
	void addstring(string s){
		str = s;
	}
	int getfrequency(){
		return num;
	}
	string getstring(){
		return str;
	}
	void setparent(Tree* p){
		parent = p;
		return;
	}
	Tree* ifchildinlsit(string s){
		auto iter = children.find(s);
		if(iter!=children.end()){
			cout << "old = " << s << endl;
			iter->second->num++;
			return iter->second;
		}else{
			cout << "new = " << s << endl;
			Tree* leaf = new Tree();
			leaf->str = s;
			leaf->num = 1;
			children[s] = leaf;
			return leaf;
		}
	}
	map<string, Tree*> getchildren(){
		return children;
	}

private:
	string str;
	int num=0;
	map<string, Tree*> children;
	Tree* parent=nullptr;
};

class HeaderNode{
public:
	void addstring(string s){
		str = s;
	}
	void addfrequency(int frequency){
		fq = frequency;
	}
	int getfrequency(){
		return fq;
	}
	string getstring(){
		return str;
	}
	bool cmpstring(string s){
		if(s == str){
			return true;
		}else{
			return false;
		}
	}
	void addstring(Tree* child){
		node.push_back(child);
	}

private:
	string str;
	int fq;
	vector<Tree*> node;
};

vector<Transaction> DB;
vector<HeaderNode> HT;
set<string> stopword;
map<string, int> freTable;
map<string, int> minfreTable;
Tree *root;

bool compare_nocase (const std::string& first, const std::string& second){

	if(minfreTable[first] < minfreTable[second]){
		return true;
	}else{
		return false;
	}
}

Transaction splittoword(string str, string pattern){
    string::size_type pos;
    Transaction tran;
    str+=pattern;
    int size=str.size();
    for(int i=0; i<size; i++){
        pos=str.find(pattern,i);
        if(pos<size){
            string s=str.substr(i,pos-i);
            if(s.size()!=0){
				if(!stopword.count(s)){
					tran.additem(s);
					if(freTable.find(s) != freTable.end()){
						freTable[s]++;
					}else{
						freTable[s] = 1;
					}
				}
            }
            i=pos+pattern.size()-1;
        }
    }
    return tran;
}

void split(string str, string pattern){
    string::size_type pos;
    Transaction tran;
    str+=pattern;
    int size=str.size();
    for(int i=0; i<size; i++){
        pos=str.find(pattern,i);
        if(pos<size){
            string s=str.substr(i,pos-i);
			tran = splittoword(s, " ");
			tran.addstring(s);
			DB.push_back(tran);
            i=pos+pattern.size()-1;
        }
    }
    return;
}

void insertHTnode(HeaderNode hd){
	int len=HT.size();
	if(len==0){
		HT.push_back(hd);
	}else{
		vector<HeaderNode>::iterator iter;
		for(int i=0; i<len; i++){
            if(HT[i].getfrequency()<hd.getfrequency()){
				HT.insert(HT.begin()+i, hd);
				return;
            }
		}
	}
}

void createHT(){
	map<string, int>::iterator iter;
	for(iter = minfreTable.begin(); iter != minfreTable.end(); iter++){
		HeaderNode Hd;
		Hd.addfrequency(iter->second);
		Hd.addstring(iter->first);
		insertHTnode(Hd);
	}
	/*vector<HeaderNode>::iterator hiter;
	for(hiter = HT.begin(); hiter != HT.end(); hiter++){
		cout << hiter->getstring() << "------" << hiter->getfrequency() << endl;
	}*/
}

void deleteminfreq(int min_sup){
	map<string, int>::iterator iter;
	for(iter = freTable.begin(); iter != freTable.end(); iter++){
		if(iter->second>min_sup){
			minfreTable[iter->first] = iter->second;
		}
	}
	/*for(iter = minfreTable.begin(); iter != minfreTable.end(); iter++){
		cout << iter->first << "------" << iter->second << endl;
	}*/
}

void updateDB(){
	vector<Transaction>::iterator iter;
	for(iter=DB.begin(); iter!=DB.end(); iter++){
		iter->sortitems();
	}
}

void createmainTree(Tree *node){
	Tree *n;
	for(auto& i:DB){
		n = node;
		for(auto& j:i.getitemlist()){
			n = n->ifchildinlsit(j);
			//cout << n->getstring() << "==========" << n->getfrequency() <<endl;
			for(auto& k:HT){
				if(k.cmpstring(j)){
					k.addstring(n);
				}
			}
		}
		for(auto& j:n->getchildren()){
			//cout << n->getstring() << "=========" << n->getfrequency() << endl;
			cout << j.first << "    " << j.second << endl;
		}
	}
	return;
}

void printnode(Tree* root){
	if(root->getfrequency()!=0){
		cout << root->getstring() << "=========" << root->getfrequency() << endl;
		for(auto& i: root->getchildren()){
			printnode(i.second);
		}
	}
}

int main(){
	ifstream filein("sample_in.txt");
	ifstream filestop("stop_words.txt");
	ofstream fileout("test.txt");
	string Str;
	regex signs("[!@#\\$%\\^&\\*\(\\)\\:;\\[\\]]");
	regex chanege("[,?]");

	int min_sup = 10;
	//cout << "min_support:" << endl;
	//cin >> min_sup;

	if(!filestop){
		cout << "can not find stop words file" << endl;
	}else{
		while(getline(filestop, Str)){
			stopword.insert(Str);
		}
		filestop.close();
	}

	if(!filein){
		cout << "can not find input file" << endl;
	}else if(!fileout){
		cout << "can not find output file" << endl;
	}else{
		while(getline(filein, Str)){
			Str=regex_replace(Str, signs, " ");
			Str=regex_replace(Str, chanege, ".");
			transform(Str.begin(),Str.end(),Str.begin(),::tolower);
			split(Str, ".");
		}
		filein.close();
	}

	for(auto& i:freTable){
		if(i.first=="yet"){
			cout << i.second << endl;
		}
	}
	// delete min support
	deleteminfreq(min_sup);

	// update data base
	updateDB();

	// create header table
	createHT();

	//create main tree and record item header
	if(root==nullptr){
		root = new Tree();
		root->addstring("");
	}

	if(root!=nullptr){
		cout << "new tree success" << endl;
		createmainTree(root);
	}


	//printnode(root);
	//
	return 0;
}
