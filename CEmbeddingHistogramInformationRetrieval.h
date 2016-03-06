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

class CEmbeddingHistogramInformationRetrieval: public CInformationRetrieval {
protected:
	real SquaredEuclideanDistance(real* vec1, real* vec2, int size) {
		real result = 0;
		if (vec1 == NULL || vec2 == NULL)
			return DIS_INF;
		for (int i = 0; i < size; i++) {
			result += (vec1[i] - vec2[i]) * (vec1[i] - vec2[i]);
		}
		return result;
	}
public:
	CEmbeddingHistogramInformationRetrieval(WordLibrary& dict, Corpus& train);
	virtual ~CEmbeddingHistogramInformationRetrieval();

	virtual real distance(const Document* doc1, const Document* doc2);
};

#endif /* TFPROJECT_CEMBEDDINGHISTOGRAMINFORMATIONRETRIEVAL_H_ */
