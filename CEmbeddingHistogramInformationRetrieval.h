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
public:
	CEmbeddingHistogramInformationRetrieval(WordLibrary& dict, Corpus& train);
	virtual ~CEmbeddingHistogramInformationRetrieval();
	virtual void Transform(Document *querydoc);
	virtual real distance(Document* doc1, Document* doc2);
	virtual void rank(Document* queryDoc);
};

#endif /* TFPROJECT_CEMBEDDINGHISTOGRAMINFORMATIONRETRIEVAL_H_ */
