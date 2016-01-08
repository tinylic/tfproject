
#include "dictionary.h"

using namespace std;

class Document {
public:
	int mtag;
	Dictionary* mDict;
	Umap mWordCount;
	vector<Upair> AllWord;


	void Init() {
		// Initialization
		mDict = new Dictionary;
		mDict -> Init();
		mWordCount.clear();
		AllWord.clear();
		mtag = -1;
	}
	Document(){
		Init();
	}
	~Document(){
		//delete mDict;
	}


	void ReadFile(const int Tag, const char *fn){
		//read from a file that represents a document
		//Read 20_news
		Init();
		Char word[MAX_STRING];
		FILE *fin = fopen(fn, "r");
		while (1) {
			if (fgetc(fin) == '\n'){
				char ch = fgetc(fin);
				if (ch == '<' || ch == '\n') break;
			}
		}
		mtag = Tag;
		int wcnt = 0;
		while (1) {
				if (feof(fin)) break;
				ReadWord(word, fin);
				if (strlen(word) < MIN_WORDS) continue;
				wcnt ++;
				int index = mDict -> SearchVocab(word);
				if (index == -1) {
					int a = mDict -> AddWordToVocab(word);
					mWordCount[a] = 1;
				} else mWordCount[index] ++;

			}
		//cout << wcnt << endl;
	}

	void ReadCorpus(Char *document) {
		//Read a document in the form of <w1, c1>
		int a = 0;
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
			if (strlen(word) < 3) continue;

			//add to dictionary
			int index = mDict -> SearchVocab(word);
			if (index == -1) {
				a = mDict -> AddWordToVocab(word);
				mWordCount[a] = cnt;
			} else mWordCount[index] += cnt;
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
			if (strlen(word) < MIN_WORDS) continue;
			int index = mDict -> SearchVocab(word);
			if (index == -1) {
				int a = mDict -> AddWordToVocab(word);
				mWordCount[a] = cnt;
			} else mWordCount[index] += cnt;
		}
	}
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
			Document *NewDocument = new Document;
			fgets(Doc, vocab_hash_size, fin);
			NewDocument -> ReadCorpus(Doc);
			mCorpus.push_back(NewDocument);
		}
	}

	inline const Document* getDocument(unsigned i) {
		return mCorpus.at(i);
	}

};
