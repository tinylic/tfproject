/*
 * CInterHistogramWMDInformationRetrieval.cpp
 *
 *  Created on: 2016年9月17日
 *      Author: tinylic
 */

#include <CInterHistogramWMDInformationRetrieval.h>

CInterHistogramWMDInformationRetrieval::CInterHistogramWMDInformationRetrieval(WordLibrary& dict, Corpus& train) :
CInformationRetrieval(dict, train) {
	// TODO Auto-generated constructor stub

}

CInterHistogramWMDInformationRetrieval::~CInterHistogramWMDInformationRetrieval() {
	// TODO Auto-generated destructor stub
	//delete pCluster;
}
void CInterHistogramWMDInformationRetrieval::Transform(Document *querydoc) {
	return;
}
void CInterHistogramWMDInformationRetrieval::rank(Document* queryDoc) {
	std::thread mThreads[MAX_THREADS];
	for (int i = 0; i < MAX_THREADS; i++)
		mThreads[i] = std::thread(&CInformationRetrieval::ThreadFunction, *this, queryDoc, i);
	for (int i = 0; i < MAX_THREADS; i++)
		mThreads[i].join();
	sort(dis, dis + trainCorpus.size());
	return;
}

Real CInterHistogramWMDInformationRetrieval::distance(Document* doc1, Document* doc2) {
	vector<unsigned> vecIDs;
	vector<Real*> vecEmbeddings;
	int aEmbeddingSize, bEmbeddingSize;

	//Get the embeddings and ids for both document
	doc1 -> GetAllEmbeddings(vecEmbeddings, vecIDs);
	aEmbeddingSize = vecEmbeddings.size();
	doc2 -> GetAllEmbeddings(vecEmbeddings, vecIDs);
	bEmbeddingSize = vecEmbeddings.size() - aEmbeddingSize;
	//cerr << "AEmbeddingSize = " << aEmbeddingSize << endl;
	//cerr << "BEmbeddingSize = " << bEmbeddingSize << endl;

	int Pnclusters = max(1, (int)(vecEmbeddings.size() * compress_ratio));
	//assert(aEmbeddingSize + bEmbeddingSize == Pnclusters);
	//Do K-Means on embeddings
	pCluster = new cluster(Pnclusters, 10, vecEmbeddings, true);

	int* pcl = pCluster -> GetLabels();

	//Process on document one
	int aSum = 0;
	int aLen = 0;
	Real *DA = new Real[Pnclusters + 10];
	int *IDA = new int[Pnclusters + 10];
	int *CntA = new int[Pnclusters + 10];
	for (int i = 0; i < Pnclusters; i++)
		DA[i] = IDA[i] = CntA[i] = 0;
	for (int i = 0; i < aEmbeddingSize; i++) {
		int index = vecIDs[i];
		int WordCnt = 1;//doc1->GetWordCount(index);
		CntA[pcl[i]] += WordCnt;
		aSum += WordCnt;
	}
	for (int i = 0; i < Pnclusters; i++)
		if (CntA[i] > 0) {
			IDA[aLen] = i;
			DA[aLen] = CntA[i];
			aLen ++;
		}
	aSum = max(aSum, 1);
	for (int i = 0; i < aLen; i++)
		DA[i] /= aSum;


	//Process on document two
	int bSum = 0;
	int bLen = 0;
	Real *DB = new Real[Pnclusters + 10];
	int *IDB = new int[Pnclusters + 10];
	int *CntB = new int[Pnclusters + 10];
	for (int i = 0; i < Pnclusters; i++)
		DB[i] = IDB[i] = CntB[i] = 0;
	for (int i = aEmbeddingSize; i < vecEmbeddings.size(); i++) {
		int index = vecIDs[i];
		int WordCnt = 1;//doc1->GetWordCount(index);
		CntB[pcl[i]] += WordCnt;
		bSum += WordCnt;
	}
	for (int i = 0; i < Pnclusters; i++)
		if (CntB[i] > 0) {
			IDB[bLen] = i;
			DB[bLen] = CntB[i];
			bLen ++;
		}
	bSum = max(bSum, 1);
	for (int i = 0; i < bLen; i++)
		DB[i] /= bSum;

	//calculate the cost matrix
	Real **cost = new Real *[aLen];
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

	delete DA;
	delete DB;
	delete IDA;
	delete IDB;
	delete CntA;
	delete CntB;
	delete[] cost;
	delete pCluster;
	return result;
}
