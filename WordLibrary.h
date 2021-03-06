#ifndef TFPROJECT_WORDLIBRARY_H_
#define TFPROJECT_WORDLIBRARY_H_
#include <cstdio>
#include "cstring"
#include <algorithm>
#include "heads.h"
using namespace std;
struct embed_word {
	Char *word;   //the word form
	bool hasEmbedding;
	Real *embedding; //the embedding
	int cl;  //which cluster?
	int word_id;  //identifier
	bool InCorpus;
	double *distributions;
	Real IDF;
	Real TF_IDF;
	embed_word() {
		word = NULL;
		hasEmbedding = false;
		InCorpus = false;
		embedding = NULL;
		distributions = NULL;
		cl = word_id = -1;
		IDF = -1e9;
		TF_IDF = -1e9;
	}

	embed_word(const embed_word &other) {
		hasEmbedding = other.hasEmbedding;
		InCorpus = other.InCorpus;
		cl = other.cl;
		word_id = other.word_id;
		IDF = other.IDF;
		TF_IDF = other.TF_IDF;
		word = new Char[strlen(other.word)];
		strcpy(word, other.word);
		embedding = new Real[sizeof(other.embedding) / sizeof(Real)];
		memcpy(embedding, other.embedding, sizeof(other.embedding) / sizeof(Real));
	}

};

static bool cmp(const embed_word *a, const embed_word *b) {
	return a->TF_IDF > b->TF_IDF;
}
////////////////////////////////////////
/// This class serves as a dictionary
////////////////////////////////////////

class WordLibrary {
private:
	unsigned nDim;  //the dimensionality of word embedding

	int *word_hash;
	int word_size;

	vector<embed_word*> mWordEmbeds;
	//AllEmbeds All;
	WordLibrary(const WordLibrary &other);
public:

	int GetWordHash(Char *word) {
		unsigned long long hash = 0;
		int a, len = strlen(word);
		for (a = 0; a < len; a++)
			hash = hash * 257 + word[a];
		hash = hash % vocab_hash_size;
		return hash;
	}

// Returns position of a word in the vocabulary; if the word is not found, returns -1

	int SearchVocab(Char *word) {
		int hash = GetWordHash(word);
		while (1) {
			if (word_hash[hash] == -1)
				return -1;
			if (!strcmp(word, mWordEmbeds[word_hash[hash]]->word))
				return word_hash[hash];
			hash = (hash + 1) % vocab_hash_size;
		}
		// check;
		return -1;
	}

	inline Char* GetWord(int id) {
		// Return the word in position hash
		if (id == -1)
			return NULL;
		return mWordEmbeds[id]->word;
	}

	inline int size() {
		return word_size;
	}
	inline Real* GetEmbedding(int id) {
		// Return the embedding in position hash
		if (id == -1)
			return NULL;
		if (!mWordEmbeds[id]->hasEmbedding)
			return NULL;
		return mWordEmbeds[id]->embedding;
	}

	inline int GetID(Char *word) {
		return SearchVocab(word);
	}

	embed_word* GetEmbedWord(int index) {
		return mWordEmbeds[index];
	}

	inline bool hasEmbedding(int id) {
		return mWordEmbeds[id]->hasEmbedding;
	}

	inline void ClearAllLabels() {
		for (unsigned i = 0; i < mWordEmbeds.size(); i++)
			mWordEmbeds[i]->cl = -1;
	}

	inline Real GetIDF(int id) {
		return mWordEmbeds[id] -> IDF;
	}

	int GetEmbedWordCl(int index) {
		return mWordEmbeds[index]->cl;
	}
	void ChangeEmbedWordCl(int index, int cl) {
		mWordEmbeds[index]->cl = cl;
	}
	void ChangeEmbedWordGMM(int index, double *distribution) {
		mWordEmbeds[index] -> distributions = new double[max_w];
		for (int i = 0; i < max_w; i++)
			mWordEmbeds[index] -> distributions[i] = distribution[i];

	}

	void ChangeIDF(int index, Real mIDF) {
		mWordEmbeds[index] -> IDF = mIDF;
	}
	void UpdateTFIDF(int index, Real mTFIDF) {
		//if (mWordEmbeds[index] -> TF_IDF < mTFIDF)
		if (mTFIDF > 0) {
			if (mWordEmbeds[index] -> TF_IDF < 0)
				mWordEmbeds[index] -> TF_IDF = 0;
			mWordEmbeds[index] -> TF_IDF += mTFIDF;
		}
	}
	Real GetTFIDF(int index) {
		return mWordEmbeds[index] -> TF_IDF;
	}

	void SetInCorpus(int index, bool flag) {
		mWordEmbeds[index]->InCorpus = flag;
	}

	bool IsInCorpus(int index) {
		return mWordEmbeds[index]->InCorpus;
	}
	void SetDistributions(int index, double *distri, int size) {
		mWordEmbeds[index] -> distributions = new double[size];
		for (int i = 0; i < size; i++)
			mWordEmbeds[index] -> distributions[i] = distri[i];
	}
	double* GetDistributions(int index) {
		return mWordEmbeds[index] -> distributions;
	}

	WordLibrary() :
		nDim(50) {
		//Init();
		//mWordEmbeds = new embed_word[vocab_hash_size];  //to be modified
		word_size = 0;
		word_hash = new int[vocab_hash_size];
		for (int i = 0; i < vocab_hash_size; i++) {
			word_hash[i] = -1;
		}
	}

	~WordLibrary() {
		// free(mWordEmbeds);
		//cout << "Destructing" << endl;
		//free(word_hash);
		//word_size = 0;
	}

	//append a set of word embeddings -- from a word embedding file generated by Word2Vec
	void AppendEmbeddingsFromFile(const char *fn, bool IsBinary) {
		long long words, size, a, b;
		Real len;
		Real *M;
		char *vocab;
		FILE *f;
		if (IsBinary) {
			f = fopen(fn, "rb");
		} else {
			f = fopen(fn, "r");
		}
		if (f == NULL) {
			printf("Input file not found\n");
			return;
		}
		//Init();
		fscanf(f, "%lld", &words);  //the number of words
		fscanf(f, "%lld", &size);   //the dimensionality
		//layer1_size = size;
		cout << words << " " << size << endl;
		if (words > vocab_hash_size)
			words = vocab_hash_size;
		for (b = 0; b < words; b++) {
			vocab = new char[max_w];
			M = new Real[size];
			a = 0;
			//read in the word
			while (1) {
				vocab[a] = fgetc(f);
				if (feof(f) || (vocab[a] == ' '))
					break;
				if ((a < max_w) && (vocab[a] != '\n'))
					a++;
			}
			vocab[a] = 0;

			//read in the embedding
			if (IsBinary) {
				for (a = 0; a < size; a++)
					fread(&M[a], sizeof(float), 1, f);
				len = 0;
				for (a = 0; a < size; a++)
					len += M[a] * M[a];
				len = sqrt(len);
				for (a = 0; a < size; a++)
					M[a] /= len;
			} else {
				for (a = 0; a < size; a++)
					fscanf(f, "%f", &M[a]);
			}

			bool valid = true;
			for (a = 0; a < size; a++)
				if (!isfinite(M[a]))
					valid = false;
			if (strlen(vocab) < MIN_WORDS || valid == false) {
				delete vocab;
				delete M;
				continue;
			}

			//wordcnt ++;
			if (word_size % 100000 == 0) {
				cerr << word_size << endl;
			}

			AddWordEmbedding(vocab, M);
		}
	}

	int AddWordEmbedding(Char* word, Real embedding[]) {
		// Insert the embedding to the dict and return the index
		int index = AddWordToVocab(word); //SearchVocab(word);   //whether word exists in vocabulary?
		//if (index == -1) {  //if not found
			//mWordEmbeds.push_back(new embed_word);
			//index = AddWordToVocab(word);
			//All.push_back(embedding);
		//}

		//mWordEmbeds[index].embedding = new Real[layer1_size];
		if (mWordEmbeds[index]->hasEmbedding == false) {
			mWordEmbeds[index]->hasEmbedding = true;
		} else {
			delete mWordEmbeds[index]->embedding;
		}
		mWordEmbeds[index]->embedding = embedding;

		//mWordEmbeds[index].hasEmbedding = true;
		//for (int i = 0; i < layer1_size; i++)
		//	mWordEmbeds[index].embedding[i] = embedding[i];
		//cout << word << " " << index << endl;
		return index;
	}

	int AddWordToVocab(Char *word) {
		int index = SearchVocab(word);

		if (index == -1) {
			embed_word * p_ew = new embed_word;
			p_ew -> word = word;
			p_ew -> word_id = word_size;
			mWordEmbeds.push_back(p_ew);
			word_size++;

			int hash;
			hash = GetWordHash(word);
			while (word_hash[hash] != -1)
				hash = (hash + 1) % vocab_hash_size;
			word_hash[hash] = word_size - 1;
			return word_size - 1;
		}
		return index;
	}

	int AddCluster(Char* word, int cluster_id) {
		// Insert the embedding to the dict and return the index

		int index = SearchVocab(word);
		if (index == -1)
			index = AddWordToVocab(word);
		mWordEmbeds[index]->cl = cluster_id;
		return index;
	}

	vector<Real *> getAllEmbedding() {
		vector<Real *> embeds;
		embeds.clear();
		for (int i = 0; i < word_size; i++) {
			embeds.push_back(mWordEmbeds[i] -> embedding);
		}
		return embeds;
		//return All;//to be rewritten
	}



};
#endif
