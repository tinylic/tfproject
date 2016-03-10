/*
 * CBrownInformationRetrieval.h
 *
 *  Created on: 2016年3月10日
 *      Author: tinylic
 */

#ifndef TFPROJECT_CBROWNINFORMATIONRETRIEVAL_H_
#define TFPROJECT_CBROWNINFORMATIONRETRIEVAL_H_

#include <Document.h>
#include "cluster.h"
#include "CInformationRetrieval.h"
#include "WordLibrary.h"

class CBrownInformationRetrieval : public CInformationRetrieval{
public:
	CBrownInformationRetrieval(WordLibrary& dict, Corpus& train, const string &BrownClusterResultFile);
	virtual ~CBrownInformationRetrieval();
	virtual void Transform(Document *querydoc);
	virtual real distance(Document* doc1, Document* doc2);
};

#endif /* TFPROJECT_CBROWNINFORMATIONRETRIEVAL_H_ */
