#include <cstdio>
#include <algorithm>

#include "document_process.h"

using namespace std;
struct embed_word {
		Char *word;
		double *embedding;
		int cn;
		int word_id;
	};
class WordEmbedding {
private:
	unsigned nDim;  //the dimensionality of word embedding
	embed_word* mWordEmbeds;

	int * word_hash;
	int word_size;

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

	int GetWordHash(Char *word) {
		unsigned long long a, hash = 0;
		int len = strlen(word);
		for (a = 0; a < len; a++) hash = hash * 257 + word[a];
		hash = hash % vocab_hash_size;
		return hash;
	}

// Returns position of a word in the vocabulary; if the word is not found, returns -1

	int SearchVocab(Char *word) {
		unsigned int hash = GetWordHash(word);
		while (1) {
			if (word_hash[hash] == -1) return -1;
			if (!strcmp(word, mWordEmbeds[word_hash[hash]].word)) return word_hash[hash];
			hash = (hash + 1) % vocab_hash_size;
		}
		return -1;
	}

	Char* GetWord(unsigned int hash) {
		if (word_hash[hash] == -1) return NULL;
		return mWordEmbeds[word_hash[hash]].word;
	}

	double* GetEmbedding(unsigned int hash) {
		if (word_hash[hash] == -1) return NULL;
		return mWordEmbeds[word_hash[hash]].embedding;
	}

	int AddWordToVocab(Char *word) {
		unsigned int hash, length = strlen(word) + 1;
		if (length > MAX_STRING) length = MAX_STRING;
		mWordEmbeds[word_size].word = (Char *)calloc(length, sizeof(Char));
		mWordEmbeds[word_size].cn = 0;
		// mWordEmbeds[word_size].embedding = (double *)calloc(layer1size, sizeof(double));
		strcpy(mWordEmbeds[word_size].word, word);
		// strcpy(mWordEmbeds[word_size].embedding, embedding);
		word_size++;
		hash = GetWordHash(word);
		while (word_hash[hash] != -1) hash = (hash + 1) % vocab_hash_size;
		mWordEmbeds[word_size].word_id = word_hash[hash] = word_size - 1;
		return vocab_size - 1;
	}


	WordEmbedding() {
		mWordEmbeds = (struct embed_word *)calloc(vocab_max_size, sizeof(struct embed_word));
		word_hash = (int *)calloc(vocab_hash_size, sizeof(int));
		word_size = 0;
	}

	~WordEmbedding() {
		free(mWordEmbeds);
		free(word_hash);
		word_size = 0;
	}


	WordEmbedding(const char *fn, bool isBinary){
		// read in from a word embedding file
		// in the form of word array[embedding]
		Char word[MAX_STRING];
		unsigned index, i;
		double embedding[layer1_size + 10];
		FILE *fin = fopen(fn, "rb");
		if (fin == NULL) {
			printf("ERROR: training data file not found!\n");
			exit(1);
		}

		while (1) {
			ReadWord(word, fin);
			if (feof(fin)) break;
			for (i = 0; i < layer1_size; i++) {
				fscanf(fin, "%lf", &embedding[i]);
			}
			index = getEmbedding(word, embedding);
			mWordEmbeds[index].cn++;
		}

	}

	int getEmbedding(Char* word, double* embedding){
		int index = SearchVocab(word);
		if (index == -1)
			index = AddWordToVocab(word);
		mWordEmbeds[index].embedding = (double *)calloc(layer1_size, sizeof(double));
		memcpy(mWordEmbeds[index].embedding, embedding, sizeof(double) * layer1_size);
		return index;
	}


	vector<double *> getAllEmbedding() {
		vector<double *> result;
		result.clear();
		for (int i = 0; i < vocab_hash_size; i++) {
			 // get all embeddings
			if (word_hash[i] == -1) continue;
			int index = word_hash[i];
			if (mWordEmbeds[index].embedding != NULL) {
				result.push_back(mWordEmbeds[index].embedding);
			}
		}
		return result;
	}

};
