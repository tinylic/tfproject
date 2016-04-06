/*
 * CInformationRetrieval.cpp
 *
 *  Created on: 2016年3月2日
 *      Author: xiezp
 */

#include "CInformationRetrieval.h"

Real CInformationRetrieval::SquaredEuclideanDistance(Real* vec1, Real* vec2,
		int size) {
	Real result = 0;
	if (vec1 == NULL || vec2 == NULL)
		return DIS_INF;
	for (int i = 0; i < size; i++) {
		result += (vec1[i] - vec2[i]) * (vec1[i] - vec2[i]);
	}
	return result;
}

CInformationRetrieval::CInformationRetrieval(WordLibrary& dict, Corpus& train) :
		mDict(dict), trainCorpus(train){
		dis = new DocCmp[trainCorpus.size()];
}

CInformationRetrieval::~CInformationRetrieval() {
	// TODO Auto-generated destructor stub
}

void CInformationRetrieval::ThreadFunction(Document* queryDoc, int thread_id) {

	int num = trainCorpus.size() / MAX_THREADS;
	int start = thread_id * num;
	int end = (thread_id + 1) * num;
	if (thread_id == MAX_THREADS - 1)
		end = trainCorpus.size();

	for (int j = start; j < end; j++) {
		Real dist = distance(trainCorpus.getDocument(j), queryDoc);
		DocCmp mDocCmp = DocCmp(trainCorpus.getDocument(j), dist);
		dis[j] = mDocCmp;
	}
}

Real CInformationRetrieval::MAP(Document* queryDoc) {
	int queryTag = queryDoc->Getmtag();
	Real result = 0;
	for (unsigned i = 0; i < trainCorpus.size(); i++) {
		Document* mDoc = dis[i].doc;
		if (mDoc->Getmtag() == queryTag)
			result += 1 / (Real) (i + 1);
	}
	return result;
}


Real CInformationRetrieval::GetMAPScore(Document* queryDoc) {
	rank(queryDoc);
	Real curMAP = MAP(queryDoc);
	return curMAP;
}
