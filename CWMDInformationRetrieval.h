/*
 * CWMDInformationRetrieval.h
 *
 *  Created on: 2016年3月2日
 *      Author: xiezp
 */

#ifndef TFPROJECT_CWMDINFORMATIONRETRIEVAL_H_
#define TFPROJECT_CWMDINFORMATIONRETRIEVAL_H_

#include "CInformationRetrieval.h"
#include "emd.h"
#include "cluster.h"
class CWMDInformationRetrieval: public CInformationRetrieval {

protected:
	Real Nearest(const Embeds &a, Document *doc);

	Embeds GetDocCenter(Document *a);

	Real **cost;
public:
	CWMDInformationRetrieval(WordLibrary& dict, Corpus& train);

	virtual ~CWMDInformationRetrieval();

	virtual Real distance(Document* doc1, Document* doc2);

	virtual void Transform(Document *querydoc);

	virtual void rank(Document* queryDoc);

	Real WCD(Document *a, Document *b);

	Real RWMD(Document *a, Document *b);

	Real WMD(Document *a, Document *b);




};

#endif /* TFPROJECT_CWMDINFORMATIONRETRIEVAL_H_ */
