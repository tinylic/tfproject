/*
 * CInterHistogramWMDInformationRetrieval.h
 *
 *  Created on: 2016年9月17日
 *      Author: tinylic
 */

#ifndef TFPROJECT_CINTERHISTOGRAMWMDINFORMATIONRETRIEVAL_H_
#define TFPROJECT_CINTERHISTOGRAMWMDINFORMATIONRETRIEVAL_H_

#include <Document.h>
#include "cluster.h"
#include "CInformationRetrieval.h"
#include "WordLibrary.h"
#include "emd.h"
#include "CWMDInformationRetrieval.h"
class CInterHistogramWMDInformationRetrieval : public CInformationRetrieval{
private:
	cluster *pCluster;
	int pLen;
	emd_node mEmd_node;
public:
	CInterHistogramWMDInformationRetrieval(WordLibrary& dict, Corpus& train);
	virtual ~CInterHistogramWMDInformationRetrieval();
	virtual Real distance(Document* doc1, Document* doc2);
	virtual void Transform(Document *querydoc);
	virtual void rank(Document* queryDoc);
};

#endif /* TFPROJECT_CINTERHISTOGRAMWMDINFORMATIONRETRIEVAL_H_ */
