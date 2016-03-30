/*
 * CEmbeddingHIstogramInformationRetrieval.h
 *
 *  Created on: 2016年3月3日
 *      Author: xiezp
 */

#ifndef TFPROJECT_CEMBEDDINGHISTOGRAMINFORMATIONRETRIEVAL_H_
#define TFPROJECT_CEMBEDDINGHISTOGRAMINFORMATIONRETRIEVAL_H_
#include <Document.h>
#include "cluster.h"
#include "CInformationRetrieval.h"
#include "WordLibrary.h"
#include "emd.h"

class CEmbeddingHistogramInformationRetrieval: public CInformationRetrieval {
private:
	real **DistanceMatrix;
	real ImprovedDistance(real* vec1, real* vec2,	int size);
	real WMDDistance(Document* doc1, Document* doc2, int cluster_size, int embedding_size);
	real CenterDistance(Document* doc1, Document* doc2, int cluster_size, int embedding_size);
	cluster *pCluster;
public:
	CEmbeddingHistogramInformationRetrieval(WordLibrary& dict, Corpus& train);
	virtual ~CEmbeddingHistogramInformationRetrieval();
	virtual void Transform(Document *querydoc);
	virtual real distance(Document* doc1, Document* doc2);
	virtual void rank(Document* queryDoc);
};

#endif /* TFPROJECT_CEMBEDDINGHISTOGRAMINFORMATIONRETRIEVAL_H_ */
