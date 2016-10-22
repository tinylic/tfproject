/*
 * CGroupTFIDFInformationRetrieval.h
 *
 *  Created on: 2016年8月13日
 *      Author: tinylic
 */

#ifndef TFPROJECT_CGROUPTFIDFWMDINFORMATIONRETRIEVAL_H_
#define TFPROJECT_CGROUPTFIDFWMDINFORMATIONRETRIEVAL_H_

#include "CInformationRetrieval.h"
#include "WeightedEmd.h"
#include "cluster.h"

class CGroupTFIDFWMDInformationRetrieval : public CInformationRetrieval {
protected:
	Real **cost;
private:
	cluster *aCluster, *bCluster;
	int aLen, bLen;
public:
	virtual Real distance(Document* doc1, Document* doc2);

	virtual void Transform(Document *querydoc);

	virtual void rank(Document* queryDoc);

	Real WMD(Document *a, Document *b);

	CGroupTFIDFWMDInformationRetrieval(WordLibrary& dict, Corpus& train);
	virtual ~CGroupTFIDFWMDInformationRetrieval();
};

#endif /* TFPROJECT_CGROUPTFIDFWMDINFORMATIONRETRIEVAL_H_ */
