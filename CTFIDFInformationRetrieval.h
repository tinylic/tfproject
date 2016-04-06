/*
 * CTFIDFInformationRetrieval.h
 *
 *  Created on: 2016年3月9日
 *      Author: tinylic
 */

#ifndef TFPROJECT_CTFIDFINFORMATIONRETRIEVAL_H_
#define TFPROJECT_CTFIDFINFORMATIONRETRIEVAL_H_

#include <Document.h>
#include "cluster.h"
#include "CInformationRetrieval.h"
#include "WordLibrary.h"
#include "heads.h"

class CTFIDFInformationRetrieval : public CInformationRetrieval{
protected:
	vector<embed_word*> KeyWords;
public:
	CTFIDFInformationRetrieval(WordLibrary& dict, Corpus& train);
	virtual ~CTFIDFInformationRetrieval();
	virtual Real distance(Document* doc1, Document* doc2);
	virtual void Transform(Document *querydoc);
	virtual void rank(Document* queryDoc);
};

#endif /* TFPROJECT_CTFIDFINFORMATIONRETRIEVAL_H_ */
