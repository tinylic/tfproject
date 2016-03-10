/*
 * CInformationRetrieval.h
 *
 *  Created on: 2016年3月2日
 *      Author: xiezp
 */

#ifndef TFPROJECT_CINFORMATIONRETRIEVAL_H_
#define TFPROJECT_CINFORMATIONRETRIEVAL_H_

#include <Document.h>
#include <vector>
#include "heads.h"
#include "WordLibrary.h"

using namespace std;

struct DocCmp {
	Document* doc;
	real distance;

	DocCmp() {
		doc = NULL;
		distance = 0;
	}

	DocCmp(const DocCmp & dc) {
		doc = dc.doc;
		distance = dc.distance;
	}

	DocCmp(Document* _doc, real _distance) {
		doc = _doc;
		distance = _distance;
	}
	friend bool operator <(const DocCmp &a, const DocCmp &b) {
		return a.distance < b.distance;
	}
};

class CInformationRetrieval {
protected:
	WordLibrary& mDict;
	Corpus& trainCorpus;
	//Corpus& queryCorpus;
	vector<DocCmp> dis;
	pthread_t *pt;

	void* RunMethod(void *arg);

	real MAP(Document* queryDoc);

	real SquaredEuclideanDistance(real* vec1, real* vec2, int size);
public:
	CInformationRetrieval(WordLibrary& dict, Corpus& train);
	virtual ~CInformationRetrieval();

	//real distance(int trainID, int queryID) {
	//	return distance(trainCorpus.getDocument(trainID), queryCorpus.getDocument(queryID));
	//}

	virtual real distance(const Document* doc1, const Document* doc2) = 0;

	virtual void Transform(Document *querydoc) = 0;

	void rank(Document* queryDoc);

	real GetMAPScore(Document* queryDoc);
};

#endif /* TFPROJECT_CINFORMATIONRETRIEVAL_H_ */
