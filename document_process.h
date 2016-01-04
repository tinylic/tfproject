
#include "dictionary.h"

using namespace std;

class Document {
public:
	Dictionary* mDict;
	Umap mWordCount;
	vector<Upair> AllWord;
private:
	void ReadWord(Char *word, FILE *f) {
		int a = 0;
		while (1) {
			word[a] = fgetc(f);
			if (feof(f) || (word[a] == ' ')) break;
						if ((a < max_w) && (word[a] != '\n')) a++;
					}
		word[a] = 0;
	}

public:

	Document(){
		// Initialization
		mDict = (Dictionary *)calloc(1, sizeof(Dictionary));
		mWordCount.clear();
	}
	~Document(){
		free(mDict);
		mWordCount.clear();
	}


	void ReadFile(const char *fn){
		//read from a file that represents a document
		mDict = (Dictionary *)calloc(1, sizeof(Dictionary));
		(*mDict) = Dictionary();
		(*mDict).Init();
		mWordCount.clear();
		AllWord.clear();
		Char word[MAX_STRING];
		FILE *fin = fopen(fn, "r");
		while (1) {
				if (feof(fin)) break;
				ReadWord(word, fin);
				unsigned index = mDict -> SearchVocab(word);
				if (index == -1) {
					unsigned a = mDict -> AddWordToVocab(word);
					mWordCount[a] = 1;
				} else mWordCount[index] ++;
			}
	}

void ReadFromCorpus(Char *document) {
		//Read a document in the form of <w1, c1>
		int a = 0, ch;
		int len = strlen(document);
		Char word[MAX_STRING];
		for (int i = 0; ; ) {
			//read the word
			while (i < len && document[i] != '<') i++;
			if (i == len) break;
			memset(word, 0, sizeof word);
			int n = 0; i ++;
			for (; document[i] != ','; i++) {
				word[n++] = document[i];
			}
			word[n] = 0;

			//read the count
			while (!isdigit(document[i])) i++;
			int cnt = 0;
			for (; isdigit(document[i]); i++)
				cnt = cnt * 10 + document[i] - '0';

			//add to dictionary
			int index = mDict -> SearchVocab(word);
			if (index == -1) {
				a = mDict -> AddWordToVocab(word);
				mWordCount[a] = cnt;
			} else mWordCount[index] ++;
		}
	}

	void ReadDocument(const char *fn) {

		int cnt;
		Char word[MAX_STRING];
		FILE *fin = fopen(fn, "r");
		if (fin == NULL) {
			printf("ERROR: training data file not found!\n");
			exit(1);
		}

		while (1) {
			if (feof(fin)) break;
			ReadWord(word, fin);
			fscanf(fin, "%d", &cnt);
			unsigned index = mDict -> SearchVocab(word);
			if (index == -1) {
				unsigned a = mDict -> AddWordToVocab(word);
				mWordCount[a] = cnt;
			} else mWordCount[index] += cnt;
		}
	}
/*	Document(const Umap& wc) {
		for (Umap :: iterator Uit = mWordCount.begin(); Uit != mWordCount.end(); Uit ++)
			wc[Uit -> first] = Uit -> second;
	};
*/
	vector<Upair> GetAllWord() {
		vector<Upair> result;
		result.clear();
		for (Umap :: iterator Uit = mWordCount.begin(); Uit != mWordCount.end(); Uit++) {
			result.push_back(make_pair(Uit -> first, Uit -> second));
		}
		return result;
	}
};

class Corpus {
private:
	vector<Document*> mCorpus;
public:
	
	Corpus() {
		mCorpus.clear();
	}
	
	Corpus(const char *fn, int mode) {
		//read a corpus from a file
		//each line represents a document in the format "<w1, c1>, ...."
		if (mode != 0) return;
		mCorpus.clear();
		FILE *fin = fopen(fn, "rb");
		if (fin == NULL) {
			printf("ERROR: training data file not found!\n");
			exit(1);
		}

		Char Doc[MAX_STRING];
		while (1) {
			if (feof(fin)) break;
			Document* NewDocument = (Document *)calloc(1, sizeof(Document));
			fgets(Doc, vocab_hash_size, fin);
			NewDocument -> ReadFromCorpus(Doc);
			mCorpus.push_back(NewDocument);
		}
	}

	inline const Document* getDocument(unsigned i) {
		return mCorpus.at(i);
	}

};
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
