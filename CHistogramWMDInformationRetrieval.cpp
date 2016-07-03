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
	vector<unsigned> avecIDs, bvecIDs;
	vector<Real*> avecEmbeddings, bvecEmbeddings;
	//process on document 1
	doc1->GetAllEmbeddings(avecEmbeddings, avecIDs);
	int Anclusters = max(1, (int)(avecEmbeddings.size() * compress_ratio));
	//assert(Anclusters == 1);
	aCluster = new cluster(Anclusters, 10, avecEmbeddings, true);
	//cerr << "aCluster = " << Anclusters << endl;
	int* acl = aCluster -> GetLabels();
	int aSum = 0;
	Real *DA = new Real[Anclusters + 10];

	for (unsigned i = 0; i < Anclusters; i++)
		DA[i] = 0;
	for (unsigned i = 0; i < avecEmbeddings.size(); i++) {
		int index = avecIDs[i];
		int WordCnt = 1;//doc1->GetWordCount(index);
		DA[acl[i]] += WordCnt;
		aSum += WordCnt;
	}
	aSum = max(aSum, 1);
	for (unsigned i = 0; i < Anclusters; i++)
		DA[i] /= aSum;
	assert(DA[0] == 1.0);

	// process on document 2
	doc2->GetAllEmbeddings(bvecEmbeddings, bvecIDs);
	int Bnclusters = max(1, (int)(bvecEmbeddings.size() * compress_ratio));
	//assert(Bnclusters == 1);
	bCluster = new cluster(Bnclusters, 10, bvecEmbeddings, true);
	int* bcl = bCluster -> GetLabels();
	int bSum = 0;
	Real *DB = new Real[Bnclusters + 10];
	for (unsigned i = 0; i < Bnclusters; i++)
		DB[i] = 0;
	for (unsigned i = 0; i < bvecEmbeddings.size(); i++) {
		int index = bvecIDs[i];
		int WordCnt = 1;//doc2->GetWordCount(index);
		DB[bcl[i]] += WordCnt;
		bSum += WordCnt;
	}
	bSum = max(bSum, 1);
	for (unsigned i = 0; i < Bnclusters; i++)
		DB[i] /= bSum;
	assert(DB[0] == 1.0);

	//calculate the cost matrix
	Real **cost = new Real *[Anclusters];
	for (int i = 0; i < Anclusters; i++) {
		cost[i] = new Real[Bnclusters];
		for (int j = 0; j < Bnclusters; j++)
			cost[i][j] = SquaredEuclideanDistance(aCluster->GetCentroid(i), bCluster->GetCentroid(j), layer1_size);
	}

	signature_t doca = signature_t{ Anclusters, DA };
	signature_t docb = signature_t{ Bnclusters, DB };

	Real result = mEmd_node.emd(&doca, &docb, cost, 0, 0);
	if (!isfinite(result))
		result = 1e9;
	CWMDInformationRetrieval * WCDpIR =
							new CWMDInformationRetrieval(mDict,
									trainCorpus);
	Real WCDResult = WCDpIR ->distance(doc1, doc2);
	cerr <<"Histo = " << result << " WCD = " << WCDResult <<  endl;
	assert(abs(WCDResult - result) <= 1e-6);
	return result;
}
