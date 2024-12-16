#include<iostream>
#include"spimi.h"

using namespace std;

int main() {
	SPIMI spimi("../data",1000);//Create an instance from SPIMI class
	spimi.printDocumentListName();
	spimi.SPIMInvertedIndex();//Call SPIMInvertedIndex function 
	spimi.multiWayMerge();//Merge generated blocks
}