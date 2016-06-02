/*
 * CHistogramWMDInformationRetrieval.cpp
 *
 *  Created on: 2016年5月28日
 *      Author: tinylic
 */

#include <CHistogramWMDInformationRetrieval.h>

CHistogramWMDInformationRetrieval::CHistogramWMDInformationRetrieval(WordLibrary& dict, Corpus& train) :
CInformationRetrieval(dict, train) {
	// TODO Auto-generated constructor stub

}

CHistogramWMDInformationRetrieval::~CHistogramWMDInformationRetrieval() {
	// TODO Auto-generated destructor stub
}
void CHistogramWMDInformationRetrieval::Transform(Document *querydoc) {
	return;
}
void CHistogramWMDInformationRetrieval::rank(Document* queryDoc) {
	std::thread mThreads[MAX_THREADS];
	for (int i = 0; i < MAX_THREADS; i++)
		mThreads[i] = std::thread(&CInformationRetrieval::ThreadFunction, *this, queryDoc, i);
	for (int i = 0; i < MAX_THREADS; i++)
		mThreads[i].join();
	sort(dis, dis + trainCorpus.size());
	return;
}

Real CHistogramWMDInformationRetrieval::distance(Document* doc1, Document* doc2) {
	Real *DA = new Real[max_c + 10];
	Real *DB = new Real[max_c + 10];
		vector<unsigned> avecIDs, bvecIDs;
	vector<Real*> avecEmbeddings, bvecEmbeddings;
	//process on document 1
	doc1->GetAllEmbeddings(avecEmbeddings, avecIDs);
	aCluster = new cluster(max_c, 10, avecEmbeddings, true);
	int* acl = aCluster -> GetLabels();
	int aSum = 0;
	for (unsigned i = 0; i < max_c; i++)
		DA[i] = 0;
	for (unsigned i = 0; i < avecEmbeddings.size(); i++) {
		int index = avecIDs[i];
		int WordCnt = 1;//doc1->GetWordCount(index);
		DA[acl[i]] += WordCnt;
		aSum += WordCnt;
	}
	aSum = max(aSum, 1);
	for (unsigned i = 0; i < max_c; i++)
		DA[i] /= aSum;

	// process on document 2
	doc2->GetAllEmbeddings(bvecEmbeddings, bvecIDs);
	bCluster = new cluster(max_c, 10, bvecEmbeddings, true);
	int* bcl = bCluster -> GetLabels();
	int bSum = 0;
	for (unsigned i = 0; i < max_c; i++)
		DB[i] = 0;
	for (unsigned i = 0; i < bvecEmbeddings.size(); i++) {
		int index = bvecIDs[i];
		int WordCnt = 1;//doc2->GetWordCount(index);
		DB[bcl[i]] += WordCnt;
		bSum += WordCnt;
	}
	bSum = max(bSum, 1);
	for (unsigned i = 0; i < max_c; i++)
		DB[i] /= bSum;
	//calculate the cost matrix
	Real **cost = new Real *[max_c];
	for (int i = 0; i < max_c; i++) {
		cost[i] = new Real[max_c];
		for (int j = 0; j < max_c; j++)
			cost[i][j] = SquaredEuclideanDistance(aCluster->GetCentroid(i), bCluster->GetCentroid(j), layer1_size);
	}

	signature_t doca = signature_t{ max_c, DA };
	signature_t docb = signature_t{ max_c, DB };

	Real result = mEmd_node.emd(&doca, &docb, cost, 0, 0);
	if (!isfinite(result))
		result = 1e9;
	//cerr << result << endl;
	return result;
}
