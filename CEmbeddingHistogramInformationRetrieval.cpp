/*
 * CEmbeddingHIstogramInformationRetrieval.cpp
 *
 *  Created on: 2016年3月3日
 *      Author: xiezp
 */

#include "CEmbeddingHistogramInformationRetrieval.h"


CEmbeddingHistogramInformationRetrieval::CEmbeddingHistogramInformationRetrieval(
		WordLibrary& dict, Corpus& train) :
		CInformationRetrieval(dict, train) {
	// TODO Auto-generated constructor stub
	vector<unsigned> vecIDs;
	vector<real*> vecEmbeddings;

	trainCorpus.GetAllEmbeddings(vecEmbeddings, vecIDs);

	cluster* pCluster = new cluster(max_w, 10, vecEmbeddings);

	vector<pair<real, int>> RelativeDistance;//<distance to cur node, index>
	for (int i = 0; i < max_w; i++)
		RelativeDistance.push_back(make_pair(0.0, i));
	DistanceMatrix = new real*[max_w];
	for (int i = 0; i < max_w; i++) {
		DistanceMatrix[i] = new real[max_w];
		for (int j = 0; j < max_w; j++) {
			real tDistance = pCluster->CalcDistance(i, j, layer1_size);
			RelativeDistance[j] = make_pair(tDistance, j);
		}
		sort(RelativeDistance.begin(), RelativeDistance.end());
		debug("\n i = %d\n", i);
		for (int j = 0; j < 5; j++)
			debug("%d : %.6f\n", RelativeDistance[j].second, RelativeDistance[j].first);
		real DistCoef = max_w * DIST_STEP;//1;
		for (int j = 0; j < max_w; j++) {
			DistanceMatrix[i][RelativeDistance[j].second] = DistCoef;// / (j + 1);
			//DistCoef -= DIST_STEP;
			DistCoef /= 2;
			//DistanceMatrix[i][j] = i == j;
		}
	}
	mDict.ClearAllLabels(); //clear all the cluster assignment of the words in the library

	int* cl = pCluster -> GetLabels();
	for (unsigned a = 0; a < vecEmbeddings.size(); a++)
		mDict.ChangeEmbedWordCl(vecIDs[a], cl[a]);
	cout << "End Labels" << endl;
	cout << trainCorpus.size() << endl;
	for (int i = 0; i < trainCorpus.size(); i++) {
		trainCorpus.getDocument(i) -> ClusterTransform(max_w);
	}
	cerr << "Transform Finished" << endl;
}

CEmbeddingHistogramInformationRetrieval::~CEmbeddingHistogramInformationRetrieval() {
	// TODO Auto-generated destructor stub
}

void CEmbeddingHistogramInformationRetrieval::Transform(Document *querydoc) {
	querydoc->ClusterTransform(max_w);
}
void CEmbeddingHistogramInformationRetrieval::rank(Document* queryDoc) {
	std::thread mThreads[MAX_THREADS];
	Transform(queryDoc);
	/*for (int i = 0; i < trainCorpus.size(); i++) {
		real dist = distance(trainCorpus.getDocument(i), queryDoc);
		//printf("%.6f\n", dist);
		DocCmp mDocCmp = DocCmp(trainCorpus.getDocument(i), dist);
		dis.push_back(mDocCmp);
	}*/
	for (int i = 0; i < MAX_THREADS; i++)
		mThreads[i] = std::thread(&CInformationRetrieval::ThreadFunction, *this, queryDoc, i);
	for (int i = 0; i < MAX_THREADS; i++)
		mThreads[i].join();
	sort(dis, dis + trainCorpus.size());
	return;
}
real CEmbeddingHistogramInformationRetrieval::distance(Document* doc1,
		Document* doc2) {
	real* vec1 = doc1->GetTransformed();
	real* vec2 = doc2->GetTransformed();
	return ImprovedDistance(vec1, vec2, max_w);
}

real CEmbeddingHistogramInformationRetrieval::ImprovedDistance(real* vec1, real* vec2,	int size) {
	if (vec1 == NULL || vec2 == NULL)
		return DIS_INF;

	real* vec = new real[size];
	for (int i = 0; i < size; i++)
		vec[i] = vec1[i] - vec2[i];

	real* left_hand = new real[size];
	for (int i = 0; i < size; i++) {
		left_hand[i] = 0;
		for (int j = 0; j < size; j++)
			left_hand[i] += vec[j] * DistanceMatrix[i][j];
	}

	real result = 0;
	for (int i = 0; i < size; i++)
		result += left_hand[i] * vec[i];

	return result;
}
