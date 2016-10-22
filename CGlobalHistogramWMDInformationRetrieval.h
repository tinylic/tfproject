/*
 * CGlobalHistogramWMDInformationRetrieval.h
 *
 *  Created on: 2016年9月8日
 *      Author: tinylic
 */

#ifndef TFPROJECT_CGLOBALHISTOGRAMWMDINFORMATIONRETRIEVAL_H_
#define TFPROJECT_CGLOBALHISTOGRAMWMDINFORMATIONRETRIEVAL_H_

#include <Document.h>
#include "cluster.h"
#include "CInformationRetrieval.h"
#include "WordLibrary.h"
#include "emd.h"
#include "CWMDInformationRetrieval.h"
class CGlobalHistogramWMDInformationRetrieval : public CInformationRetrieval{
private:
	cluster *pCluster;
	int aLen, bLen;
	emd_node mEmd_node;
	Real **cost;
public:
	CGlobalHistogramWMDInformationRetrieval(WordLibrary& dict, Corpus& train);
	virtual ~CGlobalHistogramWMDInformationRetrieval();
	virtual Real distance(Document* doc1, Document* doc2);
	virtual void Transform(Document *querydoc);
	virtual void rank(Document* queryDoc);
};

#endif /* TFPROJECT_CGLOBALHISTOGRAMWMDINFORMATIONRETRIEVAL_H_ */
