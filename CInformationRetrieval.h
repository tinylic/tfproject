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
#include <iostream>
#include <thread>
#include <chrono>
#include <pthread.h>
#include "heads.h"
#include "WordLibrary.h"

using namespace std;

struct DocCmp {
	Document* doc;
	Real distance;

	DocCmp() {
		doc = NULL;
		distance = 0;
	}

	DocCmp(const DocCmp & dc) {
		doc = dc.doc;
		distance = dc.distance;
	}

	DocCmp(Document* _doc, Real _distance) {
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
	DocCmp* dis;

	Real MAP(Document* queryDoc);

	Real SquaredEuclideanDistance(Real* vec1, Real* vec2, int size);


public:
	CInformationRetrieval(WordLibrary& dict, Corpus& train);

	virtual ~CInformationRetrieval();

	virtual Real distance(Document* doc1, Document* doc2) = 0;

	virtual void Transform(Document *querydoc) = 0;

	void ThreadFunction(Document* QueryDoc, int thread_id);

	virtual void rank(Document* queryDoc) = 0;

	Real GetMAPScore(Document* queryDoc);
};

#endif /* TFPROJECT_CINFORMATIONRETRIEVAL_H_ */
