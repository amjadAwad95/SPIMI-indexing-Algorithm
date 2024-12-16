#pragma once
#include<vector>
#include<string>
#include<list>
#include<algorithm>

using namespace std;

class TermInformation {
private:
	int numberOfFrequency;
	list<int> documentID;

	inline int getDocumentNameIndex(vector<string>::const_iterator it, vector<string>::const_iterator begin) {
		return it - begin;
	}
public:
	TermInformation() {this->numberOfFrequency = 0;}

	inline int getNumberOfFrequency() const{return this->numberOfFrequency;}

	list<int> getDocumentIDList() const{return this->documentID;}

	//The function take numberOfFrequency and add it to old one
	void addNumberOfFrequency(int numberOfFrequency) {
		this->numberOfFrequency += numberOfFrequency;
	}

	//The function add a document name  with out inc number of frequency
	void addInformation(const string& documentName, const vector<string>& documentListName);

	//The function add a document name with inc number of frequency
	void addDocument(const string& documentName, const vector<string>& documentListName);

	//The function sort the dpcument id
	void sortDocumentID();
};

void TermInformation:: addInformation(const string& documentName, const vector<string>& documentListName) {
	auto begin = documentListName.begin();
	auto end = documentListName.end();

	auto it = find(begin, end, documentName);

	if (it == end) {
		return;
	}
	int documentNameIndex = this->getDocumentNameIndex(it, begin);

	auto itList = find(this->documentID.begin(), this->documentID.end(), documentNameIndex);

	if (itList == this->documentID.end()) {
		this->documentID.push_back(documentNameIndex);
	}
}

void TermInformation:: addDocument(const string& documentName, const vector<string>& documentListName) {
	this->numberOfFrequency++;
	this->addInformation(documentName, documentListName);
}

void TermInformation:: sortDocumentID() {
	this->documentID.sort([](const auto& a, const auto& b) {
		return a < b;
		});
}