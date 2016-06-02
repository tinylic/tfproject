/*
 * CHistogramWMDInformationRetrieval.h
 *
 *  Created on: 2016年5月28日
 *      Author: tinylic
 */

#ifndef TFPROJECT_CHISTOGRAMWMDINFORMATIONRETRIEVAL_H_
#define TFPROJECT_CHISTOGRAMWMDINFORMATIONRETRIEVAL_H_

#include <Document.h>
#include "cluster.h"
#include "CInformationRetrieval.h"
#include "WordLibrary.h"
#include "emd.h"

class CHistogramWMDInformationRetrieval : public CInformationRetrieval{
private:
	cluster *aCluster, *bCluster;
	int aLen, bLen;
	emd_node mEmd_node;
public:
	CHistogramWMDInformationRetrieval(WordLibrary& dict, Corpus& train);
	virtual ~CHistogramWMDInformationRetrieval();
	virtual Real distance(Document* doc1, Document* doc2);
	virtual void Transform(Document *querydoc);
	virtual void rank(Document* queryDoc);
};

#endif /* TFPROJECT_CHISTOGRAMWMDINFORMATIONRETRIEVAL_H_ */
