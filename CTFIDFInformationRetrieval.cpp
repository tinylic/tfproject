/*
 * CTFIDFInformationRetrieval.cpp
 *
 *  Created on: 2016年3月9日
 *      Author: tinylic
 */

#include <CTFIDFInformationRetrieval.h>


CTFIDFInformationRetrieval::CTFIDFInformationRetrieval(WordLibrary& dict,
		Corpus& train) :
		CInformationRetrieval(dict, train) {
	// TODO Auto-generated constructor stub

	cerr << mDict.size() << endl;
	for (int i = 0; i < mDict.size(); i++) {
		if (mDict.IsInCorpus(i) == false) continue;
		real result = train.size();
		int ContainCount = 0;
		for (int j = 0; j < train.size(); j++) {
			Document* doc = train.getDocument(j);
			if (doc->HasWord(i))
				ContainCount++;
		}
		if (ContainCount == 0)
			result = 1e-9;
		else
			result /= ContainCount;
		if (i % 100000 == 0)
			cerr << i << endl;
		mDict.ChangeIDF(i, log(result));
	}

	for (int i = 0; i < trainCorpus.size(); i++) {
		Document* doc = trainCorpus.getDocument(i);
		for (auto mit = doc->mWordCount.begin(); mit != doc->mWordCount.end();
				mit++) {
			int index = mit->first;
			real mTF_IDF = doc->GetTF(index) * mDict.GetIDF(index);
			mDict.UpdateTFIDF(index, mTF_IDF);
		}
	}
	KeyWords.clear();
	for (int i = 0; i < mDict.size(); i++) {
		KeyWords.push_back(mDict.GetEmbedWord(i));
	}
	sort(KeyWords.begin(), KeyWords.end(), cmp);

	for (int i = 0; i < MAX_KEY_WORDS; i++)
		debug("%s %.6f\n", KeyWords[i]->word, KeyWords[i]->TF_IDF);
	for (int i = 0; i < trainCorpus.size(); i++)
		trainCorpus.getDocument(i) ->TFIDFTransform(KeyWords, MAX_KEY_WORDS);
}

CTFIDFInformationRetrieval::~CTFIDFInformationRetrieval() {
	// TODO Auto-generated destructor stub
}

void CTFIDFInformationRetrieval::Transform(Document *querydoc) {
	querydoc->TFIDFTransform(KeyWords, MAX_KEY_WORDS);
}

void CTFIDFInformationRetrieval::rank(Document* queryDoc) {
	std::thread mThreads[MAX_THREADS];
	Transform(queryDoc);
	/*for (int i = 0; i < trainCorpus.size(); i++) {
		real dist = distance(trainCorpus.getDocument(i), queryDoc);
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

real CTFIDFInformationRetrieval::distance(Document* doc1,
		Document* doc2) {
	real* vec1 = doc1->GetTransformed();
	real* vec2 = doc2->GetTransformed();
	if (vec1 == NULL || vec2 == NULL) cerr << "fuck" << endl;
	return SquaredEuclideanDistance(vec1, vec2, MAX_KEY_WORDS);
}

