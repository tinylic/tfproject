/*
 * CBrownInformationRetrieval.cpp
 *
 *  Created on: 2016年3月10日
 *      Author: tinylic
 */

#include <CBrownInformationRetrieval.h>

CBrownInformationRetrieval::CBrownInformationRetrieval(WordLibrary& dict, Corpus& train, const string &BrownClusterResultFile) :
			CInformationRetrieval(dict, train) {
		// TODO Auto-generated constructor stub

		mDict.ClearAllLabels(); //clear all the cluster assignment of the words in the library

		FILE *infile = fopen(BrownClusterResultFile.c_str(), "r");
		int labels;
		Char word[255];

		while (1) {
			fscanf(infile, "%d%s", &labels, word);
			if (feof(infile))
				break;
			int index = mDict.GetID(word);
			cerr << "index = " << index << endl;
			if (index != -1)
				mDict.ChangeEmbedWordCl(index, labels);
		}

		cerr << "End Labels" << endl;
		cerr << trainCorpus.size() << endl;
		for (int i = 0; i < trainCorpus.size(); i++) {
			trainCorpus.getDocument(i) -> ClusterTransform(max_w);
			cerr << i << " Transform Finished" << endl;
		}
		cerr << "Transform Finished" << endl;
	}


void CBrownInformationRetrieval::Transform(Document *querydoc) {
	querydoc->ClusterTransform(max_w);
}

CBrownInformationRetrieval::~CBrownInformationRetrieval() {
	// TODO Auto-generated destructor stub
}
void CBrownInformationRetrieval::rank(Document* queryDoc) {
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
Real CBrownInformationRetrieval::distance(Document* doc1,
		Document* doc2) {
	Real* vec1 = doc1->GetTransformed();
	Real* vec2 = doc2->GetTransformed();
	return SquaredEuclideanDistance(vec1, vec2, max_w);
}
