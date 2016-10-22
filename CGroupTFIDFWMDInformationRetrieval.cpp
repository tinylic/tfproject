/*
 * CGroupTFIDFInformationRetrieval.cpp
 *
 *  Created on: 2016年8月13日
 *      Author: tinylic
 */

#include <CGroupTFIDFWMDInformationRetrieval.h>

CGroupTFIDFWMDInformationRetrieval::CGroupTFIDFWMDInformationRetrieval(WordLibrary& dict, Corpus& train) :
CInformationRetrieval(dict, train) {
	cost = NULL;
	cerr << mDict.size() << endl;
	for (int i = 0; i < mDict.size(); i++) {
		//if (mDict.IsInCorpus(i) == false) continue;
		Real idf = train.size();
		int ContainCount = 0;
		int tWordCount = 0;
		for (int j = 0; j < train.size(); j++) {
			Document* doc = train.getDocument(j);
			if (doc->HasWord(i)) {
				ContainCount++;
				tWordCount += doc ->GetWordCount(i);
			}
		}
		if (ContainCount == 0 || tWordCount < THRESHOLD_FREQUENCY)
			idf = 1e-9;
		else
			idf /= ContainCount;
		if (i % 50000 == 0)
			cerr << i << endl;
		mDict.ChangeIDF(i, idf);
	}
}

CGroupTFIDFWMDInformationRetrieval::~CGroupTFIDFWMDInformationRetrieval() {
	// TODO Auto-generated destructor stub
}


Real CGroupTFIDFWMDInformationRetrieval::WMD(Document *doc1, Document *doc2) {
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
	//assert(DA[0] == 1.0);

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
	//assert(DB[0] == 1.0);

	//calculate the cost matrix
	Real **cost = new Real *[Anclusters];
	for (int i = 0; i < Anclusters; i++) {
		cost[i] = new Real[Bnclusters];
		for (int j = 0; j < Bnclusters; j++)
			cost[i][j] = SquaredEuclideanDistance(aCluster->GetCentroid(i), bCluster->GetCentroid(j), layer1_size);
	}

	Real *Weight = new Real[Bnclusters];
	for (int i = 0; i < Bnclusters; i++)
		Weight[i] = 0;
	for (int i = 0; i < bvecEmbeddings.size(); i++) {
		int index = bvecIDs[i];
		Real mIDF = mDict.GetIDF(index);
		Weight[bcl[i]] += mIDF;
	}
	for (int i = 0; i < Bnclusters; i++)
		Weight[i] = log(Weight[i]);
	signature_t doca = signature_t{ Anclusters, DA };
	signature_t docb = signature_t{ Bnclusters, DB };
	signature_t QueryWeight = signature_t { Bnclusters, Weight};
	weighted_emd_node mEmd_node;
	Real result = mEmd_node.weighted_emd(&doca, &docb, &QueryWeight, cost, 0, 0);
	if (!isfinite(result))
		result = 1e9;

	delete DA;
	delete DB;
	delete[] cost;
	delete aCluster;
	delete bCluster;

	return result;
}


Real CGroupTFIDFWMDInformationRetrieval::distance(Document* TrainDoc, Document* QueryDoc) {
	return WMD(TrainDoc, QueryDoc);
}

void CGroupTFIDFWMDInformationRetrieval::Transform(Document *querydoc) {
	return;
}
void CGroupTFIDFWMDInformationRetrieval::rank(Document* queryDoc) {
	std::thread mThreads[MAX_THREADS];
	Transform(queryDoc);
	for (int i = 0; i < MAX_THREADS; i++)
		mThreads[i] = std::thread(&CInformationRetrieval::ThreadFunction, *this, queryDoc, i);
	for (int i = 0; i < MAX_THREADS; i++)
		mThreads[i].std::thread::join();
	sort(dis, dis + trainCorpus.size());
	return;
}
