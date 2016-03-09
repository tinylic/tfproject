/*
 * CEmbeddingHIstogramInformationRetrieval.cpp
 *
 *  Created on: 2016年3月3日
 *      Author: xiezp
 */

#include "CEmbeddingHistogramInformationRetrieval.h"

real CEmbeddingHistogramInformationRetrieval::SquaredEuclideanDistance(real* vec1, real* vec2, int size) {
	real result = 0;
	if (vec1 == NULL || vec2 == NULL)
		return DIS_INF;
	for (int i = 0; i < size; i++) {
		result += (vec1[i] - vec2[i]) * (vec1[i] - vec2[i]);
	}
	return result;
}

CEmbeddingHistogramInformationRetrieval::CEmbeddingHistogramInformationRetrieval(
		WordLibrary& dict, Corpus& train) :
		CInformationRetrieval(dict, train) {
	// TODO Auto-generated constructor stub
	vector<unsigned> vecIDs;
	vector<real*> vecEmbeddings;

	trainCorpus.GetAllEmbeddings(vecEmbeddings, vecIDs);

	cluster* pCluster = new cluster(max_w, 10, vecEmbeddings);

	mDict.ClearAllLabels(); //clear all the cluster assignment of the words in the library

	int* cl = pCluster -> GetLabels();
	for (unsigned a = 0; a < vecEmbeddings.size(); a++)
		mDict.ChangeEmbedWordCl(vecIDs[a], cl[a]);
	cout << "End Labels" << endl;
	cout << trainCorpus.size() << endl;
	for (unsigned i = 0; i < trainCorpus.size(); i++) {
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
real CEmbeddingHistogramInformationRetrieval::distance(const Document* doc1,
		const Document* doc2) {
	real* vec1 = doc1->GetTransformed();
	real* vec2 = doc2->GetTransformed();
	return SquaredEuclideanDistance(vec1, vec2, max_w);
}
