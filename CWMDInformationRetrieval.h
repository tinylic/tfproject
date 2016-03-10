/*
 * CWMDInformationRetrieval.h
 *
 *  Created on: 2016年3月2日
 *      Author: xiezp
 */

#ifndef TFPROJECT_CWMDINFORMATIONRETRIEVAL_H_
#define TFPROJECT_CWMDINFORMATIONRETRIEVAL_H_

#include "CInformationRetrieval.h"
#include "emd.c"

class CWMDInformationRetrieval: public CInformationRetrieval {

protected:
	real Nearest(const Embeds &a, Document *doc);

	Embeds GetDocCenter(Document *a);

public:
	CWMDInformationRetrieval(WordLibrary& dict, Corpus& train);

	virtual ~CWMDInformationRetrieval();

	virtual real distance(Document* doc1, Document* doc2);

	virtual void Transform(Document *querydoc);

	real WCD(Document *a, Document *b);

	real RWMD(Document *a, Document *b);

	real WMD(Document *a, Document *b);




};

#endif /* TFPROJECT_CWMDINFORMATIONRETRIEVAL_H_ */
