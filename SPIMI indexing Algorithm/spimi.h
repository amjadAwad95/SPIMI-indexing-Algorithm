#pragma once
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include<iostream>
#include <experimental/filesystem>
#include<string>
#include<vector>
#include<list>
#include<map>
#include<sstream>
#include<fstream>
#include<regex>
#include<algorithm>
#include<queue>
#include"term_information.h"


using namespace std;
using namespace std::experimental::filesystem;

class Term {
private:
	string token;
	string documentName;
public:
	Term(string token, string documentName) {
		this->token = token;
		this->documentName = documentName;
	}

	void setToken(string toekn) { this->token = token; }
	void setDocumentName(string documentName) { this->documentName = documentName; }

	inline string getToken() const { return this->token; }
	inline string getDocumentName()const { return this->documentName; }
};

class SPIMI {
private:
	int mxTermsInSubInd;
	int filesNumber;
	string directoryName;
	vector<string> documentListName;
	map<string, TermInformation> invertedIndex;

	//Function generates a unique and descriptive name for a block file where a sub-inverted index will be stored
	string createBlockName();

	//Function convert term to lower
	string toLower(string word);

	//The function takes a path and extracts the document name
	string getDocumentName(string path);

	//The function generates a vector array for document name IDs
	void createDocumentListName();

	//The function take a pair (token,token info) and return a string
	string informationToString(const pair<string, TermInformation>& term);

	//The Function takes a outputFile and write the information in disk
	void writeBlockToDisk(ofstream& outputFile);

	//The Function take tokenStream and run SPIMI algorithm 
	void SPIMINvert(const vector<Term>& tokenStream);

	//The function take file and read the next term
	string readNext(ifstream& file);

	//The function take information like csv format and return a splited data
	void splitInformation(const string& information, vector<string>& splitedList);

public:
	SPIMI(string directoryName, int mxTermsInSubInd = 100);

	//The function create tokenStream for pass it form SPIMINvert
	void SPIMInvertedIndex();

	//The function takes a blocks and merge it for big inverted index
	void multiWayMerge();

	void printDocumentListName();
};

string SPIMI::createBlockName() {
	string blockName;
	stringstream ss;
	ss << "block" << this->filesNumber++ << ".csv";
	blockName = ss.str();
	return blockName;
}

string SPIMI::toLower(string word) {
	transform(word.begin(), word.end(), word.begin(), ::tolower);
	return word;
}

string SPIMI::getDocumentName(string path) {
	stringstream ss(path);
	string documentName;
	while (getline(ss, documentName, '/'));
	return documentName;
}

void SPIMI::createDocumentListName() {
	auto directorysName = directory_iterator(this->directoryName);
	for (auto directory : directorysName) {
		string documentName = this->getDocumentName(directory.path().generic_string());
		this->documentListName.push_back(documentName);
	}
}

string SPIMI::informationToString(const pair<string, TermInformation>& term) {
	stringstream ss;
	ss << term.first << "," << term.second.getNumberOfFrequency();
	for (auto documentID : term.second.getDocumentIDList()) {
		ss << "," << this->documentListName[documentID];
	}
	return ss.str();
}

void SPIMI::writeBlockToDisk(ofstream& outputFile) {
	for (auto& term : this->invertedIndex) {
		outputFile << this->informationToString(term) << endl;
	}
}

void SPIMI::SPIMINvert(const vector<Term>& tokenStream) {
	ofstream outputFile("blocks/" + this->createBlockName());
	this->invertedIndex.clear();

	for (Term term : tokenStream) {
		this->invertedIndex[term.getToken()].addDocument(term.getDocumentName(), this->documentListName);
	}
	this->writeBlockToDisk(outputFile);
}

string SPIMI::readNext(ifstream& file) {
	string information;
	file >> information;
	return information;
}

void SPIMI::splitInformation(const string& information, vector<string>& splitedList) {
	stringstream ss(information);
	string token;
	while (getline(ss, token, ',')) {
		splitedList.push_back(token);
	}
}

SPIMI::SPIMI(string directoryName, int mxTermsInSubInd) {
	this->mxTermsInSubInd = mxTermsInSubInd;
	this->filesNumber = 0;
	this->directoryName = directoryName;
	this->createDocumentListName();
}

void SPIMI::SPIMInvertedIndex() {
	auto directorysName = directory_iterator(this->directoryName);
	vector<Term> tokenStream;

	regex wordRegex("\\b\\w+\\b");

	for (auto entry : directorysName) {
		ifstream file(entry.path());
		string entryPath = this->getDocumentName(entry.path().generic_string());

		string line;
		while (getline(file, line)) {
			stringstream lineStream(line);
			string word;
			while (lineStream >> word) {
				sregex_iterator it(word.begin(), word.end(), wordRegex);
				sregex_iterator end;
				while (it != end) {
					string match = this->toLower(it->str());
					Term term(match, entryPath);
					tokenStream.push_back(term);
					++it;
				}
				if (tokenStream.size() >= this->mxTermsInSubInd) {
					this->SPIMINvert(tokenStream);
					tokenStream.clear();
				}
			}
		}
	}
	if (tokenStream.size()) {
		this->SPIMINvert(tokenStream);
	}
}

void SPIMI::multiWayMerge() {
	this->invertedIndex.clear();//clear inverted index
	ofstream outputFile("invIndex.csv");//create invIndex.csv
	auto directorysName = directory_iterator("blocks");
	vector<ifstream*> filePointers;
	priority_queue<string> priorityQueue;

	int idx = 0;

	for (auto& directory : directorysName) {
		string blockFileName = directory.path().generic_string();
		ifstream* file = new ifstream(blockFileName);
		filePointers.push_back(file);
		priorityQueue.push(this->readNext(*filePointers.back()));
		idx++;
	}

	int counter = 0;

	while (!priorityQueue.empty()) {
		vector<string> splitedList;
		string information = priorityQueue.top();
		if (information.empty()) {
			priorityQueue.pop();
			continue;
		}
		priorityQueue.pop();
		this->splitInformation(information, splitedList);
		string token = splitedList[0];
		string frequency = splitedList[1];
		this->invertedIndex[token].addNumberOfFrequency(stoi(frequency));
		for (int i = 2; i < splitedList.size(); i++) {
			this->invertedIndex[token].addInformation(splitedList[i], this->documentListName);
		}

		for (ifstream*& filePointer : filePointers) {
			priorityQueue.push(this->readNext(*filePointer));
		}
	}
	if (!this->invertedIndex.empty()) {
		for (auto& index : this->invertedIndex) {
			index.second.sortDocumentID();
			outputFile << this->informationToString(index) << endl;
		}
	}
	for (auto file : filePointers) {
		if (file) {
			file->close();
			delete file;
		}
	}
}

void SPIMI::printDocumentListName() {
	for (auto documentName : this->documentListName) {
		cout << documentName << endl;
	}
}