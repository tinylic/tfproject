/*
 * CTFIDFWMDInformationRetrieval.h
 *
 *  Created on: 2016年7月13日
 *      Author: tinylic
 */

#ifndef TFPROJECT_CTFIDFWMDINFORMATIONRETRIEVAL_H_
#define TFPROJECT_CTFIDFWMDINFORMATIONRETRIEVAL_H_

#include "CInformationRetrieval.h"
#include "WeightedEmd.h"
#include "cluster.h"

class CTFIDFWMDInformationRetrieval: public CInformationRetrieval {



protected:
	Real **cost;
public:

	virtual Real distance(Document* doc1, Document* doc2);

	virtual void Transform(Document *querydoc);

	virtual void rank(Document* queryDoc);

	Real WMD(Document *a, Document *b);

	CTFIDFWMDInformationRetrieval(WordLibrary& dict, Corpus& train);
	virtual ~CTFIDFWMDInformationRetrieval();
};

#endif /* TFPROJECT_CTFIDFWMDINFORMATIONRETRIEVAL_H_ */
