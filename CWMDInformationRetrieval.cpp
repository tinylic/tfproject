/*
 * CWMDInformationRetrieval.cpp
 *
 *  Created on: 2016年3月2日
 *      Author: xiezp
 */

#include "CWMDInformationRetrieval.h"

CWMDInformationRetrieval::CWMDInformationRetrieval(WordLibrary& dict, Corpus& train) :
		CInformationRetrieval(dict, train) {
	// TODO Auto-generated constructor stub

}

CWMDInformationRetrieval::~CWMDInformationRetrieval() {
	// TODO Auto-generated destructor stub
}

real CWMDInformationRetrieval::Nearest(const Embeds &a, Document *doc) {
	real result = 1e9;

	for (auto P = doc->mWordCount.begin(); P != doc->mWordCount.end(); P++) {
		int id = P->first;
		Embeds vec = mDict.GetEmbedding(id);
		real temp = SquaredEuclideanDistance(a, vec, layer1_size);
		if (temp < result)
			result = temp;
	}
	return result;
}

Embeds CWMDInformationRetrieval::GetDocCenter(Document *doc) {
	Embeds result;
	result = new real[layer1_size];
	int *cnt = new int[layer1_size];
	for (int i = 0; i < layer1_size; i++)
		result[i] = cnt[i] = 0;
	for (auto P = doc->mWordCount.begin(); P != doc->mWordCount.end(); P++) {
		int id = P->first;
		int wordcnt = P->second;
		Embeds vec = mDict.GetEmbedding(id);
		if (vec == NULL)
			continue;
		for (int j = 0; j < layer1_size; j++) {
			result[j] += vec[j] * wordcnt;
			cnt[j] += wordcnt;
		}
	}
	for (int i = 0; i < layer1_size; i++)
		result[i] /= max(cnt[i], 1);
	return result;
}

real CWMDInformationRetrieval::WCD(Document *a, Document *b) {
	Embeds CentA, CentB;
	CentA = GetDocCenter(a);
	CentB = GetDocCenter(b);
	return SquaredEuclideanDistance(CentA, CentB, layer1_size);
}

real CWMDInformationRetrieval::RWMD(Document *a, Document *b) {
	real result = 0;
	real temp = 0;

	for (auto P = a->mWordCount.begin(); P != a->mWordCount.end(); P++) {
		int id = P -> first;
		Embeds vec = mDict.GetEmbedding(id);
		if (vec == NULL)	continue;
		temp += a->GetTF(id) * Nearest(vec, b);
	}
	if (result < temp)
		result = temp;

	temp = 0;
	for (auto P = b->mWordCount.begin(); P != b->mWordCount.end(); P++) {
		int id = P -> first;
		Embeds vec = mDict.GetEmbedding(id);
		if (vec == NULL)	continue;
		temp += a->GetTF(id) * Nearest(vec, a);
	}
	if (result < temp)
		result = temp;
	return result;
}

real CWMDInformationRetrieval::WMD(Document *a, Document *b) {
	int lena = a->GetWordSize();
	int lenb = b->GetWordSize();

	cout << "lena = " << lena << " lenb = " << lenb << endl;
	real *DA = new real[lena];
	real *DB = new real[lenb];

	int cnta, cntb;
	cnta = cntb = 0;
	for (auto P = a->mWordCount.begin(); P != a->mWordCount.end(); P++)
		DA[cnta ++] = a->GetTF(P -> first);
	for (auto P = b->mWordCount.begin(); P != b->mWordCount.end(); P++)
		DB[cntb ++] = b->GetTF(P -> first);

	real **cost = new real *[lena];
	for (int i = 0; i < lena; i++)
		cost[i] = new real[lenb];

	cnta = cntb = 0;
	for (auto P = a->mWordCount.begin(); P != a->mWordCount.end(); P++) {
		cntb = 0;
		for (auto Q = b->mWordCount.begin(); Q != b->mWordCount.end(); Q++) {
			Embeds veca, vecb;
			//cerr << "P = " << P -> first << " Q = " << Q -> first << endl;
 			veca = mDict.GetEmbedding(P -> first);
			vecb = mDict.GetEmbedding(Q -> first);
			cost[cnta][cntb] = SquaredEuclideanDistance(veca, vecb, layer1_size);
			//cerr << "cost = " << cost[cnta][cntb] << endl;
			//cerr << "cntb = " << cntb << endl;
			cntb++;
		}
		//cerr << "cnta = " << cnta << endl;
		cnta ++;
	}
	cerr << "fuck" << endl;
	signature_t doca = signature_t { lena, DA };
	signature_t docb = signature_t { lenb, DB };
	check;
	real result = emd(&doca, &docb, cost, 0, 0);
	if (!isfinite(result))
		result = 1e9;
	cerr << result << endl;
	return result;
}

real CWMDInformationRetrieval::distance(Document* doc1, Document* doc2) {
	return WMD(doc1, doc2);
}

void CWMDInformationRetrieval::Transform(Document *querydoc) {

}
