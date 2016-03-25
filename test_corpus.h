#ifndef TFPROJECT_TEST_CORPUS_H_
#define TFPROJECT_TEST_CORPUS_H_
#include "cluster.h"
#include "WordLibrary.h"
#include "Document.h"
#include "CEmbeddingHistogramInformationRetrieval.h"
#include "CInformationRetrieval.h"
#include "CTFIDFInformationRetrieval.h"
#include "CBrownInformationRetrieval.h"
#include "CWMDInformationRetrieval.h"

class test_corpus {
private:
	WordLibrary& mDict;
	const string& rootaddr; //const char rootaddr[] = "20_newsgroups";
	vector<Document*> mCorpus;
	vector<vector<Document*>*> mTagCorpus;

	Corpus* trainCorpus;
	Corpus* queryCorpus;

	int tag_count;
	int doc_count;
	int tot_doc;

	void ReadCorpus() {
		DIR *dir, *curdir;
		struct dirent *ptr;
		struct dirent *curptr;
		struct stat info;
		tot_doc = 0;
		tag_count = 0;
		dir = opendir(rootaddr.c_str());
		while ((ptr = readdir(dir)) != NULL) {
			if (ptr->d_name[0] == '.')
				continue;  //skip the subdirectories ( dot and dotdot )
			char *addr = (char *) calloc(255, sizeof(char));
			sprintf(addr, "%s/%s", rootaddr.c_str(), ptr->d_name);
			stat(addr, &info);
			cerr << addr << endl;
			cerr << info.st_nlink << endl;
			if (S_ISDIR(info.st_mode)) {   //if the entry is an subdirectory
				curdir = opendir(addr);
				doc_count = 0;
				while ((curptr = readdir(curdir)) != NULL) {
					if (curptr->d_name[0] == '.')
						continue;
					char *curaddr = (char *) calloc(255, sizeof(char));
					sprintf(curaddr, "%s/%s", addr, curptr->d_name);
					cerr << curaddr << endl;
					Document* doc = new Document(mDict, tag_count, curaddr);
					mCorpus.push_back(doc);
					tot_doc++;
					doc_count++;
					if (doc_count >= MAX_DOC_PER_TAG)
						break;
				}
				tag_count++;
			}
		}
		mTagCorpus.clear();
		for (int i = 0; i < tag_count; i++) {
			mTagCorpus.push_back(new vector<Document*>);
		}
		for (int i = 0; i < tot_doc; i++) {
			Document* doc = mCorpus.at(i);
			mTagCorpus[doc->Getmtag()]->push_back(doc);
		}
	}
	test_corpus(test_corpus &other);
public:

	cluster *Cluster;

	test_corpus(WordLibrary& dict, const string& dir) :
			mDict(dict), rootaddr(dir), trainCorpus(NULL), queryCorpus(NULL), Cluster(
					NULL) {
		ReadCorpus();
	}

	virtual ~test_corpus() {
		//delete Cluster;
		//delete trainCorpus;
		//delete queryCorpus;
	}

	void HeldOut(real ratio) {
		if (queryCorpus != NULL)
			delete queryCorpus;
		if (trainCorpus != NULL)
			delete trainCorpus;
		queryCorpus = new Corpus(mDict);
		trainCorpus = new Corpus(mDict);

		for (int i = 0; i < tag_count; i++) {
			int count = mTagCorpus[i]->size();
			int numQuery = floor(count * ratio);
			random_shuffle(mTagCorpus[i]->begin(), mTagCorpus[i]->end());
			for (int j = 0; j < count; j++) {
				if (j <= numQuery) {
					queryCorpus->addDocument(mTagCorpus[i]->at(j));
				} else {
					trainCorpus->addDocument(mTagCorpus[i]->at(j));
				}

			}
		}
	}

	void doExperiment() {
		CEmbeddingHistogramInformationRetrieval * pIR =
				new CEmbeddingHistogramInformationRetrieval(mDict,
						*trainCorpus);
		//CTFIDFInformationRetrieval * pIR =
		//				new CTFIDFInformationRetrieval(mDict,
		//						*trainCorpus);
		//string BrownInput = "BrownOutput.txt";
		//CBrownInformationRetrieval * pIR =
		//				new CBrownInformationRetrieval(mDict,
		//						*trainCorpus, BrownInput);
		//CWMDInformationRetrieval * pIR =
		//				new CWMDInformationRetrieval(mDict,
		//						*trainCorpus);
		real TotalMAP = 0;
		for (int i = 0; i < queryCorpus->size(); i++) {
			Document* queryDoc = queryCorpus->getDocument(i);
			real curMAP = (pIR->GetMAPScore(queryDoc));
			TotalMAP += curMAP;
			cout << i << " MAP = " <<  curMAP << endl;
			cerr << i << " MAP = " <<  curMAP << endl;
			cout << "Average MAP = " << TotalMAP / (i + 1) << endl;
			cerr << "Average MAP = " << TotalMAP / (i + 1) << endl;
		}
	}
	inline Corpus* getTrainCorpus() {
		return trainCorpus;
	}
	inline Corpus* getQueryCorpus() {
		return queryCorpus;
	}

};

#endif

