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

Real CWMDInformationRetrieval::Nearest(const Embeds &a, Document *doc) {
	Real result = 1e9;

	for (auto P = doc->mWordCount.begin(); P != doc->mWordCount.end(); P++) {
		int id = P->first;
		Embeds vec = mDict.GetEmbedding(id);
		Real temp = SquaredEuclideanDistance(a, vec, layer1_size);
		if (temp < result)
			result = temp;
	}
	return result;
}

Embeds CWMDInformationRetrieval::GetDocCenter(Document *doc) {
	Embeds result;
	result = new Real[layer1_size];
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

Real CWMDInformationRetrieval::WCD(Document *a, Document *b) {
	Embeds CentA, CentB;
	CentA = GetDocCenter(a);
	CentB = GetDocCenter(b);
	return SquaredEuclideanDistance(CentA, CentB, layer1_size);
}

Real CWMDInformationRetrieval::RWMD(Document *a, Document *b) {
	Real result = 0;
	Real temp = 0;

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
		temp += b->GetTF(id) * Nearest(vec, a);
	}
	if (result < temp)
		result = temp;
	return result;
}

Real CWMDInformationRetrieval::WMD(Document *a, Document *b) {
	int lena = a->GetWordSize();
	int lenb = b->GetWordSize();

	//cerr << "lena = " << lena << " lenb = " << lenb << endl;
	Real *DA = new Real[lena];
	Real *DB = new Real[lenb];

	int cnta, cntb;
	cnta = cntb = 0;
	for (auto P = a->mWordCount.begin(); P != a->mWordCount.end(); P++)
		DA[cnta ++] = a->GetTF(P -> first);
	for (auto P = b->mWordCount.begin(); P != b->mWordCount.end(); P++)
		DB[cntb ++] = b->GetTF(P -> first);

	Real **cost = new Real *[lena];
	for (int i = 0; i < lena; i++)
		cost[i] = new Real[lenb];

	cnta = cntb = 0;
	for (auto P = a->mWordCount.begin(); P != a->mWordCount.end(); P++) {
		cntb = 0;
		for (auto Q = b->mWordCount.begin(); Q != b->mWordCount.end(); Q++) {
			Embeds veca, vecb;
 			veca = mDict.GetEmbedding(P -> first);
			vecb = mDict.GetEmbedding(Q -> first);
			cost[cnta][cntb] = SquaredEuclideanDistance(veca, vecb, layer1_size);
			cntb++;
		}
		cnta ++;
	}
	signature_t doca = signature_t { lena, DA };
	signature_t docb = signature_t { lenb, DB };
	emd_node mEmd_node;
	Real result = mEmd_node.emd(&doca, &docb, cost, 0, 0);
	if (!isfinite(result))
		result = 1e9;
	//cerr << result << endl;
	return result;
}

Real CWMDInformationRetrieval::distance(Document* doc1, Document* doc2) {
	return RWMD(doc1, doc2);
}

void CWMDInformationRetrieval::Transform(Document *querydoc) {
	return;
}
void CWMDInformationRetrieval::rank(Document* queryDoc) {
	std::thread mThreads[MAX_THREADS];
	Transform(queryDoc);
	/*for (int i = 0; i < trainCorpus.size(); i++) {
		Real dist = distance(trainCorpus.getDocument(i), queryDoc);
		//printf("%.6f\n", dist);
		DocCmp mDocCmp = DocCmp(trainCorpus.getDocument(i), dist);
		dis.push_back(mDocCmp);
	}*/
	for (int i = 0; i < MAX_THREADS; i++)
		mThreads[i] = std::thread(&CInformationRetrieval::ThreadFunction, *this, queryDoc, i);
	for (int i = 0; i < MAX_THREADS; i++)
		mThreads[i].join();
	sort(dis, dis + trainCorpus.size());
	return;
}
