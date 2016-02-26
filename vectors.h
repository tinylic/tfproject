#include <cstdio>
#include <algorithm>

#include "document_process.h"

using namespace std;
struct embed_word {
		Char *word;
		real *embedding;
		int cl;
		int word_id;
	};
class WordEmbedding {
private:
	unsigned nDim;  //the dimensionality of word embedding
	int *word_hash;
	int word_size;
	embed_word *mWordEmbeds;
	AllEmbeds All;
public:

	int GetWordHash(Char *word) {
		unsigned long long hash = 0;
		int a, len = strlen(word);
		for (a = 0; a < len; a++) hash = hash * 257 + word[a];
		hash = hash % vocab_hash_size;
		return hash;
	}

// Returns position of a word in the vocabulary; if the word is not found, returns -1

	int SearchVocab(Char *word) {
		int hash = GetWordHash(word);
		while (1) {
			if (word_hash[hash] == -1) return -1;
			if (!strcmp(word, mWordEmbeds[word_hash[hash]].word)) return word_hash[hash];
			hash = (hash + 1) % vocab_hash_size;
		}
		// check;
		return -1;
	}

	Char* GetWord(int hash) {
		// Return the word in position hash
		if (hash == -1) return NULL;
		return mWordEmbeds[hash].word;
	}

	real* GetEmbedding(int hash) {
		// Return the embedding in position hash
		if (hash == -1) return NULL;
		return mWordEmbeds[hash].embedding;
	}

	embed_word GetEmbedWord(int index) {
		return mWordEmbeds[index];
	}

	void ChangeEmbedWordCl(int index, int cl) {
		mWordEmbeds[index].cl = cl;
	}

	int AddWordToVocab(Char *word) {
		int hash, length = strlen(word) + 1;
		//if (length > MAX_STRING) length = MAX_STRING;
		mWordEmbeds[word_size].word = new Char[length];
		strcpy(mWordEmbeds[word_size].word, word);
		word_size++;
		hash = GetWordHash(word);
		while (word_hash[hash] != -1) hash = (hash + 1) % vocab_hash_size;
		word_hash[hash] = word_size - 1;
		return word_size - 1;
	}

	void Init() {
		mWordEmbeds = new embed_word[vocab_hash_size];
		word_size = 0;
		word_hash = new int[vocab_hash_size];
		for (int i = 0; i < vocab_hash_size; i++)
            word_hash[i] = -1;
	}

	WordEmbedding() {
		Init();
	}

	~WordEmbedding() {
		// free(mWordEmbeds);
		//cout << "Destructing" << endl;
		//free(word_hash);
		//word_size = 0;
	}

	WordEmbedding(const char *fn, bool IsBinary) {
		long long words, size, a, b;
		real len;
		real *M;
		char *vocab;
		FILE *f;
		int wordcnt = 0;
		if (IsBinary) {
			f = fopen(fn, "rb");
		}
		else {
			f = fopen(fn, "r");
		}
		if (f == NULL) {
			printf("Input file not found\n");
			return ;
		}
		Init();
		fscanf(f, "%lld", &words);
		fscanf(f, "%lld", &size);
		layer1_size = size;
		cout << words << " " << size << endl;
		if (words > vocab_hash_size) words = vocab_hash_size;
		for (b = 0; b < words; b++) {
			vocab = new char[max_w];
			M = new real[size];
			a = 0;
			while (1) {
				vocab[a] = fgetc(f);
				if (feof(f) || (vocab[a] == ' ')) break;
				if ((a < max_w) && (vocab[a] != '\n')) a++;
			}
			vocab[a] = 0;
			if (IsBinary) {
				for (a = 0; a < size; a++) fread(&M[a], sizeof(float), 1, f);
				len = 0;
				for (a = 0; a < size; a++) len += M[a] * M[a];
				len = sqrt(len);
				for (a = 0; a < size; a++) M[a] /= len;
			}
			else {
				for (a = 0; a < size; a++)
					fscanf(f, "%f", &M[a]);
			}
			bool valid = true;
			for (a = 0; a < size; a++)
				if (!isfinite(M[a])) valid = false;
			if (strlen(vocab) < MIN_WORDS || valid == false) continue;
			wordcnt ++;
			if (wordcnt % 100000 == 0)  {
				cout << wordcnt << endl;
			}

			AddEmbedding(vocab, M);
		}
	}

	int AddEmbedding(Char* word, real embedding[]){
		// Insert the embedding to the dict and return the index
		int index = SearchVocab(word);
		if (index == -1) {
			index = AddWordToVocab(word);
			All.push_back(embedding);
		}
		mWordEmbeds[index].embedding = new real[layer1_size];
		for (int i = 0; i < layer1_size; i++)
			mWordEmbeds[index].embedding[i] = embedding[i];
		//cout << word << " " << index << endl;
		return index;
	}

	int AddCluster(Char* word, int cluster_id){
		// Insert the embedding to the dict and return the index

		int index = SearchVocab(word);
		if (index == -1)
			index = AddWordToVocab(word);
		mWordEmbeds[index].cl = cluster_id;
		return index;
	}

	vector<real *> getAllEmbedding() {
		return All;
	}

};
