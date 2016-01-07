#include <cstdio>
#include <algorithm>

#include "document_process.h"

using namespace std;
struct embed_word {
		Char *word;
		real *embedding;
		int cn;
		int word_id;
	};
class WordEmbedding {
private:
	unsigned nDim;  //the dimensionality of word embedding
	embed_word* mWordEmbeds;

	int *word_hash;
	int word_size;

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

	int AddWordToVocab(Char *word) {
		int hash, length = strlen(word) + 1;
		if (length > MAX_STRING) length = MAX_STRING;
		mWordEmbeds[word_size].word = (Char *)calloc(length, sizeof(Char));
		mWordEmbeds[word_size].cn = 0;
		strcpy(mWordEmbeds[word_size].word, word);
		word_size++;
		hash = GetWordHash(word);
		while (word_hash[hash] != -1) hash = (hash + 1) % vocab_hash_size;
		mWordEmbeds[word_size].word_id = word_hash[hash] = word_size - 1;
		return word_size - 1;
	}

	void Init() {
		mWordEmbeds = (struct embed_word *)calloc(vocab_hash_size, sizeof(struct embed_word));
		word_size = 0;
		word_hash = (int *)calloc(vocab_hash_size, sizeof(int));
		for (int i = 0; i < vocab_hash_size; i++)
            word_hash[i] = -1;
	}

	WordEmbedding() {
		Init();
		// check;
	}

	~WordEmbedding() {
		// free(mWordEmbeds);
		// free(word_hash);
		// word_size = 0;
	}

	WordEmbedding(const char *fn, bool IsBinary) {
		long long words, size, a, b;
		real len;
		real *M;
		char *vocab;
		FILE *f;
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
		if (words > vocab_hash_size) words = vocab_hash_size;
		for (b = 0; b < words; b++) {
			vocab = (char *)malloc(max_w * sizeof(char));
			M = (real *)malloc((long long) size * sizeof(real));
			a = 0;
			while (1) {
				vocab[a] = fgetc(f);
				if (!isalpha(vocab[a])) break;
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
			if (strlen(vocab) < MIN_WORDS) continue;
			AddEmbedding(vocab, M);
		}
	}
	int AddEmbedding(Char* word, real* embedding){
		// Insert the embedding to the dict and return the index
		All.push_back(embedding);
		int index = SearchVocab(word);
		if (index == -1)
			index = AddWordToVocab(word);
		mWordEmbeds[index].embedding = (real *)calloc(layer1_size, sizeof(real));
		if (mWordEmbeds[index].embedding == NULL) {
                debug("%d\n", word_size);
                perror("Memory Fail\n");
		}
		memcpy(mWordEmbeds[index].embedding, embedding, sizeof(real) * layer1_size);
		return index;
	}


	vector<real *> getAllEmbedding() {
		return All;
	}

};
