
#include "dictionary.h"

using namespace std;

typedef map<unsigned, unsigned> Umap;
class Document {
	Dictionary* mDict;
	Umap mWordCount;
private:
	void ReadWord(Char *word, FILE *fin) {
		int a = 0, ch;
		while (!feof(fin)) {
			ch = fgetc(fin);
			if (ch == 13) continue;
			if ((ch == ' ') || (ch == '\t') || (ch == '\n')) {
				if (a > 0) {
					if (ch == '\n') ungetc(ch, fin);
					break;
				}
				if (ch == '\n') {
					strcpy(word, (char *)"</s>");
					return;
				} else continue;
			}
			word[a] = ch;
			a++;
		}
		word[a] = 0;
	}

public:

	Document(){
		mDict = (Dictionary *)calloc(1, sizeof(Dictionary));
		mWordCount.clear();
	}
	~Document(){
		free(mDict);
		mWordCount.clear();
	}


	Document(const char *fn){
		//read from a file that represents a document

		Char word[MAX_STRING];
		FILE *fin = fopen(fn, "rb");
		if (fin == NULL) {
			printf("ERROR: training data file not found!\n");
			exit(1);
		}

		while (1) {
			mDict -> ReadWord(word, fin);
			if (feof(fin)) break;
			int index = mDict -> SearchVocab(word);
			if (index == -1) {
				int a = mDict -> AddWordToVocab(word);
				mWordCount[a] = 1;
			} else mWordCount[index] ++;
		}
		//mDict has to be modified
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
			int index = (*mDict).SearchVocab(word);
			if (index == -1) {
				a = mDict -> AddWordToVocab(word);
				mWordCount[a] = cnt;
			} else mWordCount[index] ++;
		}
	}

	void ReadDocument(const char *fn) {

		int cnt;
		Char word[MAX_STRING];
		FILE *fin = fopen(fn, "rb");
		if (fin == NULL) {
			printf("ERROR: training data file not found!\n");
			exit(1);
		}

		while (1) {
			if (feof(fin)) break;
			mDict -> ReadWord(word, fin);
			fscanf(fin, "%d", &cnt);
			int index = mDict -> SearchVocab(word);
			if (index == -1) {
				int a = mDict -> AddWordToVocab(word);
				mWordCount[a] = cnt;
			} else mWordCount[index] += cnt;
		}
	}
/*	Document(const Umap& wc) {
		for (Umap :: iterator Uit = mWordCount.begin(); Uit != mWordCount.end(); Uit ++)
			wc[Uit -> first] = Uit -> second;
	};
*/

};

class Corpus {
private:
	vector<Document*> mCorpus;
public:
	Corpus(const char *fn, int mode) {
		//read a corpus from a file
		//each line represents a document in the format "<w1, c1>, ...."
		if (mode != 0) return;
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
			(*NewDocument).ReadFromCorpus(Doc);
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
