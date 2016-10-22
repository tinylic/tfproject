/*
 * CTFIDFWMDInformationRetrieval.cpp
 *
 *  Created on: 2016年7月13日
 *      Author: tinylic
 */

#include <CTFIDFWMDInformationRetrieval.h>

CTFIDFWMDInformationRetrieval::CTFIDFWMDInformationRetrieval(WordLibrary& dict, Corpus& train) :
CInformationRetrieval(dict, train) {
	cost = NULL;
	cerr << mDict.size() << endl;
	for (int i = 0; i < mDict.size(); i++) {
		//if (mDict.IsInCorpus(i) == false) continue;
		Real idf = train.size();
		int ContainCount = 0;
		int tWordCount = 0;
		for (int j = 0; j < train.size(); j++) {
			Document* doc = train.getDocument(j);
			if (doc->HasWord(i)) {
				ContainCount++;
				tWordCount += doc ->GetWordCount(i);
			}
		}
		if (ContainCount == 0 || tWordCount < THRESHOLD_FREQUENCY)
			idf = 1e-9;
		else
			idf /= ContainCount;
		if (i % 50000 == 0)
			cerr << i << endl;
		mDict.ChangeIDF(i, log(idf));
	}
/*
	for (int i = 0; i < trainCorpus.size(); i++) {
		Document* doc = trainCorpus.getDocument(i);
		for (auto mit = doc->mWordCount.begin(); mit != doc->mWordCount.end();
				mit++) {
			int index = mit->first;
			Real mTF_IDF = doc->GetTF(index) * mDict.GetIDF(index);
			mDict.UpdateTFIDF(index, mTF_IDF);
		}
	}

	FILE* KeyWordsLog = fopen("KeyWordsLog.txt", "w");
	vector<embed_word*> KeyWords;
	for (int i = 0; i < mDict.size(); i++) {
		KeyWords.push_back(mDict.GetEmbedWord(i));
	}
	sort(KeyWords.begin(), KeyWords.end(), cmp);

	for (int i = 0; i < KeyWords.size(); i++)
		fprintf(KeyWordsLog, "%s %.6f %.6f\n", KeyWords[i]->word, KeyWords[i]->TF_IDF, KeyWords[i] -> IDF);
	fclose(KeyWordsLog);*/
}

CTFIDFWMDInformationRetrieval::~CTFIDFWMDInformationRetrieval() {
	// TODO Auto-generated destructor stub
}


Real CTFIDFWMDInformationRetrieval::WMD(Document *TrainDoc, Document *QueryDoc) {
	int LenTrain = TrainDoc->GetWordSize();
	int LenQuery = QueryDoc->GetWordSize();

	Real *DTrain = new Real[LenTrain];
	Real *DQuery = new Real[LenQuery];

	int cnta, cntb;
	cnta = cntb = 0;
	for (auto P = TrainDoc->mWordCount.begin(); P != TrainDoc->mWordCount.end(); P++)
		DTrain[cnta ++] = TrainDoc -> GetTF(P -> first);
	for (auto P = QueryDoc->mWordCount.begin(); P != QueryDoc -> mWordCount.end(); P++)
		DQuery[cntb ++] = QueryDoc -> GetTF(P -> first);

	cntb = 0;
	Real *TrainTFIDFWeights = new Real[LenTrain];
	Real *QueryTFIDFWeights = new Real[LenQuery];
	for (auto Q = QueryDoc -> mWordCount.begin(); Q != QueryDoc -> mWordCount.end(); Q++) {
		Real QueryTFIDF = QueryDoc -> GetTF(Q -> first) * mDict.GetIDF(Q -> first);
		//cerr << mDict.GetWord(Q -> first) << " " << QueryTFIDF << endl;
		QueryTFIDFWeights[cntb++] = QueryTFIDF;
	}

	Real **cost = new Real *[LenTrain];
	for (int i = 0; i < LenTrain; i++)
		cost[i] = new Real[LenQuery];
	cnta = cntb = 0;
	for (auto P = TrainDoc-> mWordCount.begin(); P != TrainDoc -> mWordCount.end(); P++) {
		cntb = 0;
		//Real TrainTFIDF = TrainDoc -> GetTF(P -> first) * mDict.GetIDF(P -> first);
		//TrainTFIDFWeights[cnta] = TrainTFIDF;
		for (auto Q = QueryDoc -> mWordCount.begin(); Q != QueryDoc -> mWordCount.end(); Q++) {
			Embeds veca, vecb;
 			veca = mDict.GetEmbedding(P -> first);
			vecb = mDict.GetEmbedding(Q -> first);
			cost[cnta][cntb] = SquaredEuclideanDistance(veca, vecb, layer1_size) ;
			cntb++;
		}
		cnta ++;
	}


	signature_t DocTrain = signature_t { LenTrain, DTrain};
	signature_t DocQuery = signature_t { LenQuery, DQuery};
	signature_t TrainWeight = signature_t { LenTrain, TrainTFIDFWeights};
	signature_t QueryWeight = signature_t { LenQuery, QueryTFIDFWeights};
	weighted_emd_node mEmd_node;
	Real result = mEmd_node.weighted_emd(&DocTrain, &DocQuery, &QueryWeight, cost, 0, 0);
	if (!isfinite(result))
		result = 1e9;
	return result;
}


Real CTFIDFWMDInformationRetrieval::distance(Document* TrainDoc, Document* QueryDoc) {
	return WMD(TrainDoc, QueryDoc);
}

void CTFIDFWMDInformationRetrieval::Transform(Document *querydoc) {
	return;
}
void CTFIDFWMDInformationRetrieval::rank(Document* queryDoc) {
	std::thread mThreads[MAX_THREADS];
	Transform(queryDoc);
	for (int i = 0; i < MAX_THREADS; i++)
		mThreads[i] = std::thread(&CInformationRetrieval::ThreadFunction, *this, queryDoc, i);
	for (int i = 0; i < MAX_THREADS; i++)
		mThreads[i].std::thread::join();
	sort(dis, dis + trainCorpus.size());
	return;
}
