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
#include <Eigen/Dense>
#include "emd.h"
#include "distributions.h"
#include "libcluster.h"
#include "probutils.h"
using namespace Eigen;
using namespace libcluster;
using namespace distributions;


class CEmbeddingHistogramInformationRetrieval: public CInformationRetrieval {
private:
	Real **DistanceMatrix;
	Real ImprovedDistance(Real* vec1, Real* vec2,	int size);
	Real WMDDistance(Document* doc1, Document* doc2, int cluster_size, int embedding_size);
	Real CenterDistance(Document* doc1, Document* doc2, int cluster_size, int embedding_size);
	cluster *pCluster;
public:
	CEmbeddingHistogramInformationRetrieval(WordLibrary& dict, Corpus& train);
	virtual ~CEmbeddingHistogramInformationRetrieval();
	virtual void Transform(Document *querydoc);
	virtual Real distance(Document* doc1, Document* doc2);
	virtual void rank(Document* queryDoc);
};

#endif /* TFPROJECT_CEMBEDDINGHISTOGRAMINFORMATIONRETRIEVAL_H_ */
