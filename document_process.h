#include <cstdio>
#include <vector>
#include <cstring>
#include <iterator>
#include "dictionary.h"

using namespace std;

typedef map<unsigned, unsigned> Umap;
class Document {
	Dictionary* mDict;
	Umap mWordCount;
public:

	Document(){
		mWordCount.clear();
	}

	Document(const string & fn){
		//read from a file that represents a document

		Char word[MAX_STRING];
		FILE *fin = fopen(fn, "rb");
		if (fin == NULL) {
			printf("ERROR: training data file not found!\n");
			exit(1);
		}

		while (1) {
			(*mDict).ReadWord(word, fin);
			if (feof(fin)) break;
			i = SearchVocab(word);
			if (i == -1) {
				a = (*mDict).AddWordToVocab(word);
				mWordCount[a] = 1;
			} else mWordCount[i] ++;
		}
		//mDict has to be modified
	}
	
	Document(const Umap& wc) {
		for (Umap :: iterator Uit = mWordCount.begin(); Uit != mWordCount.end(); Uit ++)
			wc[Uit -> first] = Uit -> second;
	};
	

}

class Corpus {
private:
	vector<Document*> mCorpus;
public:
	Corpus(const string & fn, int mode) {
		//read a corpus from a file
		//each line represents a document in the format "<w1, c1>, ...."
		
	}
	
	inline const Document* getDocument(unsigned i) {
		return mCorpus.at(i);
	}
	
}
/*
vector<word_count> document_processing(vector<u32string> &doc) {
	int cnt[vocab_hash_size];
	vector<word_count> result;
	int i, j;
	int hash;
	int doc_len = (int)doc.size();
	memset(cnt, 0, sizeof cnt);
	result.clear();
	for (i = 0; i < doc_len; i++) {
		int cur_id = SearchVocab(doc[i]);
		if (cur_id != -1) cnt[cur_id] ++;
	}
	for (i = 0; i < vocab_hash_size; i++)
		if (cnt[i] > 0)
			result.push_back(make_pair(GetWord(i), cnt[i]));
	return result;
}*/