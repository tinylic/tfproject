/*
 * CInformationRetrieval.cpp
 *
 *  Created on: 2016年3月2日
 *      Author: xiezp
 */

#include "CInformationRetrieval.h"

CInformationRetrieval::CInformationRetrieval(WordLibrary& dict, Corpus& train) :
		mDict(dict), trainCorpus(train) {
	pt = (pthread_t *) malloc(MAX_THREADS * sizeof(pthread_t));

}

CInformationRetrieval::~CInformationRetrieval() {
	// TODO Auto-generated destructor stub
}
/*
 void* CInformationRetrieval::RunMethod(void *arg) {
 //int i = query_id;
 //int id = (intptr_t)arg;
 pair<Document*, int>* temp = (pair<Document*, int>*) arg;
 Document* doc = temp->first;
 int id = temp->second;

 //int num = (Groups.size() - QUERY_DOC) / MAX_THREADS;
 int num = trainCorpus.size() / MAX_THREADS;
 int start = id * num;
 int end = (id + 1) * num - 1;
 if (id == MAX_THREADS - 1)
 end = trainCorpus.size();

 for (int j = start; j <= end; j++) {
 //int k = QUERY_DOC + id * num + j;
 //real sum = RWMD(&Groups[i], &Groups[j]);
 real dist = distance(trainCorpus.getDocument(j), doc);
 //real sum = DistTFIDF(i, k);
 //dis[k - QUERY_DOC] = DocCmp(k, sum);
 dis[j] = DocCmp(j, dist);
 }
 pthread_exit(NULL);
 }
 */
real CInformationRetrieval::MAP(Document* queryDoc) {
	int queryTag = queryDoc -> Getmtag();
	real result = 0;
	for (unsigned i = 0; i < dis.size(); i++) {
		Document* mDoc = dis[i].doc;
		if (mDoc -> Getmtag() == queryTag)
			result += 1 / (real) (i + 1);
	}
	return result;
}
void CInformationRetrieval::rank(Document* queryDoc) {
	/*vector<pair<Document*, int>*> vecArgs;
	 for (int j = 0; j < MAX_THREADS; j++) {
	 pair<Document*, int>* args = new pair<Document*, int>;
	 args->first = queryDoc;
	 args->second = j;
	 vecArgs.push_back(args);
	 }
	 for (int j = 0; j < MAX_THREADS; j++) {
	 pthread_create(&pt[j], NULL, &RunMethod, (void *) vecArgs[j]);
	 }
	 for (int j = 0; j < MAX_THREADS; j++)
	 pthread_join(pt[j], NULL);*/

	//sort(dis, dis + Groups.size() - QUERY_DOC);
	dis.clear();
	Transform(queryDoc);
	//queryDoc->ClusterTransform(max_w);
	for (unsigned i = 0; i < trainCorpus.size(); i++) {
		real dist = distance(trainCorpus.getDocument(i), queryDoc);
		//printf("%.6f\n", dist);
		DocCmp mDocCmp = DocCmp(trainCorpus.getDocument(i), dist);
		dis.push_back(mDocCmp);
	}
	sort(dis.begin(), dis.end());
	return;
}

real CInformationRetrieval::GetMAPScore(Document* queryDoc) {
	rank(queryDoc);
	real curMAP = MAP(queryDoc);
	return curMAP;
}
