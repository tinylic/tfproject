/*
 * CGlobalHistogramWMDInformationRetrieval.cpp
 *
 *  Created on: 2016年9月8日
 *      Author: tinylic
 */

#include <CGlobalHistogramWMDInformationRetrieval.h>


CGlobalHistogramWMDInformationRetrieval::CGlobalHistogramWMDInformationRetrieval(WordLibrary& dict, Corpus& train) :
CInformationRetrieval(dict, train) {
	// TODO Auto-generated constructor stub
	vector<unsigned> vecIDs;
	vector<Real*> vecEmbeddings;

	trainCorpus.GetAllEmbeddings(vecEmbeddings, vecIDs);
	int vecsize = vecEmbeddings.size();
	cerr << vecsize << endl;
	pCluster = new cluster(max_c, 10, vecEmbeddings, true);
	mDict.ClearAllLabels(); //clear all the cluster assignment of the words in the library

	int* cl = pCluster -> GetLabels();
	for (unsigned a = 0; a < vecEmbeddings.size(); a++)
		mDict.ChangeEmbedWordCl(vecIDs[a], cl[a]);

}

CGlobalHistogramWMDInformationRetrieval::~CGlobalHistogramWMDInformationRetrieval() {
	// TODO Auto-generated destructor stub
	//delete pCluster;
}
void CGlobalHistogramWMDInformationRetrieval::Transform(Document *querydoc) {
	return;
}
void CGlobalHistogramWMDInformationRetrieval::rank(Document* queryDoc) {
	std::thread mThreads[MAX_THREADS];
	for (int i = 0; i < MAX_THREADS; i++)
		mThreads[i] = std::thread(&CInformationRetrieval::ThreadFunction, *this, queryDoc, i);
	for (int i = 0; i < MAX_THREADS; i++)
		mThreads[i].join();
	sort(dis, dis + trainCorpus.size());
	return;
}

Real CGlobalHistogramWMDInformationRetrieval::distance(Document* doc1, Document* doc2) {

	aLen = 0;
	int aSum = 0;
	Real *CountA = new Real[max_c + 10];
	Real *DA = new Real[max_c + 10];
	int *IDA = new int[max_c + 10];
	for (unsigned i = 0; i < max_c; i++)
		DA[i] = CountA[i] = IDA[i] = 0;
	for (auto P = doc1->mWordCount.begin(); P != doc1->mWordCount.end(); P++) {
		int index = P -> first;
		int WordCnt = doc1->GetWordCount(index);
		CountA[mDict.GetEmbedWordCl(index)] += WordCnt;
		aSum += WordCnt;
	}
	aSum = max(aSum, 1);
	for (int i = 0; i < max_c; i++) {
		if (CountA[i] != 0) {
			IDA[aLen] = i;
			DA[aLen++] = CountA[i];
		}
	}
	for (unsigned i = 0; i < aLen; i++)
		DA[i] /= aSum;

	// process on document 2
	bLen = 0;
	int bSum = 0;
	Real *CountB = new Real[max_c + 10];
	Real *DB = new Real[max_c + 10];
	int *IDB = new int[max_c + 10];
	for (unsigned i = 0; i < max_c; i++)
		DB[i] = CountB[i] = IDB[i]= 0;
	for (auto P = doc2->mWordCount.begin(); P != doc2->mWordCount.end(); P++) {
		int index = P -> first;
		int WordCnt = doc2->GetWordCount(index);
		CountB[mDict.GetEmbedWordCl(index)] += WordCnt;
		bSum += WordCnt;
	}
	bSum = max(bSum, 1);
	for (int i = 0; i < max_c; i++) {
		if (CountB[i] != 0) {
			IDB[bLen] = i;
			DB[bLen++] = CountB[i];
		}
	}
	for (unsigned i = 0; i < bLen; i++)
		DB[i] /= bSum;

	//calculate the cost matrix
	cost = new Real *[aLen];
	for (int i = 0; i < aLen; i++) {
		cost[i] = new Real[bLen];
		for (int j = 0; j < bLen; j++)
			cost[i][j] = SquaredEuclideanDistance(pCluster->GetCentroid(IDA[i]), pCluster->GetCentroid(IDB[j]), layer1_size);
	}

	signature_t doca = signature_t{ aLen, DA };
	signature_t docb = signature_t{ bLen, DB };

	Real result = mEmd_node.emd(&doca, &docb, cost, 0, 0);
	if (!isfinite(result))
		result = 1e9;

	return result;
}
