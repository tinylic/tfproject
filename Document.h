#ifndef TFPROJECT_DOCUMENT_H_
#define TFPROJECT_DOCUMENT_H_
#include <set>
#include "WordLibrary.h"
using namespace std;

class Document {
private:
	WordLibrary& mDict;
	int mTag;   //tag of document
	int TotalWord;

	Real* mTransformed;
	bool hasTransformed;

	void ReadWord(Char *word, FILE *f) {
		int a = 0;
		while (1) {
			word[a] = fgetc(f);
			if (!isalpha(word[a]))
				break;
			if (isupper(word[a]))
				word[a] = word[a] - 'A' + 'a';
			if ((a < max_w) && (word[a] != '\n'))
				a++;
		}
		word[a] = 0;
	}
	Document(const Document &other);
public:
	Umap mWordCount;

	Document(WordLibrary& dict, const int Tag, const char *fDoc) :
			mDict(dict), mTag(-1), TotalWord(0), hasTransformed(false) {
		//read from a file that represents a document
		//Read 20_news
		Char word[MAX_STRING];
		FILE *fin = fopen(fDoc, "r");

		//useless headers
		/*
		while (1) {
			if (fgetc(fin) == '\n') {
				char ch = fgetc(fin);
				if (ch == '<' || ch == '\n')
					break;
			}
		}*/
		mTag = Tag;

		while (1) {
			if (feof(fin))
				break;
			ReadWord(word, fin);
			//cerr << word << endl;
			if (strlen(word) < MIN_WORDS)
				continue;
			AddTotalWord(1);
			if (GetTotalWord() > MAX_DOC_WORDS) break;
			int id = mDict.AddWordToVocab(word);
			if (mDict.hasEmbedding(id) == false) continue;
			mDict.SetInCorpus(id, true);
			map<unsigned, unsigned>::iterator P = mWordCount.find(id);
			if (P != mWordCount.end()) {
				P->second += 1;
			} else {
				mWordCount[id] = 1;
			}
		}
		//GetAllWord();
	}

	Document(WordLibrary& dict, Char *document) :
			mDict(dict), mTag(-1), TotalWord(0), hasTransformed(false){
		/*		int cnt;
		 //Char word[MAX_STRING];
		 //FILE *fin = fopen(fn, "r");
		 //if (fin == NULL) {
		 //	printf("ERROR: training data file not found!\n");
		 //	exit(1);
		 //}

		 while (1) {
		 if (feof(fin)) break;
		 ReadWord(word, fin);
		 fscanf(fin, "%d", &cnt);

		 if (strlen(word) < MIN_WORDS) continue;
		 AddTotalWord(cnt);
		 int id = mDict.AddWordToVocab(word);
		 map<unsigned, unsigned>::iterator P = mWordCount.find(id);
		 if (P!=mWordCount.end()){
		 P->second += cnt;
		 }
		 else{
		 mWordCount[id] = cnt;
		 }
		 }*/
	}

	~Document() {
		//delete mDict;
	}

	inline int Getmtag() {
		return mTag;
	}

	inline int GetTotalWord() {
		return TotalWord;
	}

	inline void AddTotalWord(int delta) {
		TotalWord += delta;
	}

	inline int GetWordCount(int id) {
		return mWordCount[id];
	}

	int GetWordSize() {
		return mWordCount.size();		//AllWord.size();
	}
	inline Real GetTF(int id) {
		Real result = mWordCount[id];
		return result / TotalWord;
	}

	inline bool HasWord(int id) {
		map<unsigned, unsigned>::iterator P = mWordCount.find(id);
		return (P != mWordCount.end());
	}
	void ClusterTransform(unsigned clcn) {
		// Documents are represented in word_id
		hasTransformed = true;
		unsigned i;
		int total = 0;
		Real *ans = new Real[clcn];
		mTransformed = new Real[clcn];
		for (i = 0; i < clcn; i++)
			ans[i] = 0;

		for (auto P = mWordCount.begin(); P != mWordCount.end(); P++) {
			int id = mDict.GetEmbedWordCl(P->first);
			if (id == -1) continue;
			//Real* mDistributions = mDict.GetDistributions(id);
			//if (mDistributions == NULL) continue;
			//for (int i = 0; i < clcn; i++)
			//	ans[i] += mDistributions[i] * P->second;
				ans[id] += P -> second;
			total += P->second;
		}
		if (total == 0)
			total++;
		for (i = 0; i < clcn; i++)
			mTransformed[i] = ans[i] / total;
		//return result;
	}
	void TFIDFTransform(const vector<embed_word*> &KeyWords, int KeyNum) {
		hasTransformed = true;
		mTransformed = new Real[KeyNum];
		int total = 0;
		for (int i = 0; i < KeyNum; i++) {
			mTransformed[i] = 0;
			if (HasWord(KeyWords[i] -> word_id)) {
				int mCnt = GetWordCount(KeyWords[i] -> word_id);
				mTransformed[i] = mCnt;;
				total += mCnt;
			}
		}
		for (int i = 0; i < KeyNum; i++)
			mTransformed[i] /= total;
	}

	inline Real* GetTransformed() const {
		if (hasTransformed)
			return mTransformed;
		else
			return NULL;
	}

//	vector<Upair> GetAllWord() {
//		AllWord.clear();
//		for (Umap :: iterator Uit = mWordCount.begin(); Uit != mWordCount.end(); Uit++) {
//			if (AllEmbed[Uit -> first] == NULL) continue;
//			AllWord.push_back(make_pair(Uit -> first, Uit -> second));
//		}
//		return AllWord;
//	}

	/*
	 void ReadCorpus() {
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
	 if (strlen(word) < MIN_WORDS) continue;

	 //add to dictionary
	 AddTotalWord(cnt);
	 int index = mDict -> SearchVocab(word);
	 if (index == -1) {
	 a = mDict -> AddWordToVocab(word);
	 mWordCount[a] = cnt;
	 } else mWordCount[index] += cnt;
	 }
	 }*/
};

class Corpus {
private:
	vector<Document*> mCorpus;
	WordLibrary& mDict;

public:

	Corpus(WordLibrary& dict) :
			mDict(dict) {
		mCorpus.clear();
	}


	inline void addDocument(Document* doc) {
		mCorpus.push_back(doc);
	}

	inline int size() {
		return mCorpus.size();
	}

	inline Document* getDocument(unsigned i) {
		return mCorpus.at(i);
	}

	void GetAllEmbeddings(vector<Real*>& vecEmbeddings,
			vector<unsigned>& vecIDs) {
		vecEmbeddings.clear();
		vecIDs.clear();

		set<unsigned> IDs;
		for (unsigned i = 0; i < mCorpus.size(); i++) {
			Document* doc = getDocument(i);
			int cnt = 0, cntvalid = 0;
			for (Umap::iterator Uit = doc->mWordCount.begin();
					Uit != doc->mWordCount.end(); Uit++) {
				cnt++;
				int id = Uit->first;
				if (mDict.hasEmbedding(id)) {
					IDs.insert(id);
					cntvalid++;
				}
				//printf("%d valid\n%d in total\n word contained ratio: %.6lf\n",
						//cntvalid, cnt, (double) cntvalid / cnt);
			}
		}
		for (auto P = IDs.begin(); P != IDs.end(); P++) {
			vecIDs.push_back(*P);
			vecEmbeddings.push_back(mDict.GetEmbedding(*P));
		}
	}
};
#endif
