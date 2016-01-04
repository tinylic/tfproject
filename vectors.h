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

	int * word_hash;
	int word_size;

	AllEmbeds All;

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
		// check;
		return -1;
	}

	Char* GetWord(unsigned int hash) {
		// Return the word in position hash
		if (word_hash[hash] == -1) return NULL;
		return mWordEmbeds[word_hash[hash]].word;
	}

	real* GetEmbedding(unsigned int hash) {
		// Return the embedding in position hash
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
		return word_size - 1;
	}


	WordEmbedding() {

		// check;
	}

	~WordEmbedding() {
		// free(mWordEmbeds);
		// free(word_hash);
		// word_size = 0;
	}

	WordEmbedding(const char *fn, bool IsBinary) {
		long long words, size, a, b, c, d, bi[100], index;
		real len;
		char ch;
		real *M;
		char *vocab;
		char *temp;
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
		fscanf(f, "%lld", &words);
		fscanf(f, "%lld", &size);
		layer1_size = size;
		if (words > vocab_hash_size) words = vocab_hash_size;
		mWordEmbeds = (struct embed_word *)calloc(vocab_hash_size, sizeof(struct embed_word));
		word_size = 0;
		word_hash = (int *)calloc(vocab_hash_size, sizeof(int));
		for (unsigned i = 0; i < vocab_hash_size; i++)
            word_hash[i] = -1;

		// for (; fgetc(f) != '\n';);
		for (b = 0; b < words; b++) {
			vocab = (char *)malloc(max_w * sizeof(char));
			M = (real *)malloc((long long) size * sizeof(real));
			a = 0;
			while (1) {
				vocab[a] = fgetc(f);
				if (feof(f) || (vocab[a] == ' ')) break;
				if ((a < max_w) && (vocab[a] != '\n')) a++;
			}
			vocab[a] = 0;
			printf("%s\n", vocab);
			//debug("%s\n", vocab);
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
			index = AddEmbedding(vocab, M);
		}
	}
	// WordEmbedding(const char *fn){
		// read in from a word embedding file
		// first line word_numbers && vector_dimensions
		// in the form of word array[embedding] separated in spaces
		// Char word[MAX_STRING];
		// unsigned index, i, word_numbers;
		// double *embedding;
		// FILE *fin = fopen(fn, "rb");
		// if (fin == NULL) {
			// printf("ERROR: training data file not found!\n");
			// exit(1);
		// }
		// fscanf(fin, "%d%d", &word_numbers, &layer1_size);
        // vocab_hash_size = word_numbers;
		// mWordEmbeds = (struct embed_word *)calloc(vocab_hash_size, sizeof(struct embed_word));
		// word_size = 0;
		// word_hash = (int *)calloc(vocab_hash_size * 2 + 1, sizeof(int));
		// for (i = 0; i < vocab_hash_size; i++)
            // word_hash[i] = -1;

		// printf("%d %d\n", word_numbers, layer1_size);
		// for (; fgetc(fin) != '\n';);
        // embedding = (double *)calloc(layer1_size, sizeof(double));
		// while (word_numbers --) {
			// ReadWord(word, fin);
			// printf("%s ", word);
			// if (feof(fin)) break;
			// for (i = 0; i < layer1_size; i++) {
				// fscanf(fin, "%lf", &embedding[i]);
			// }
			// for (; fgetc(fin) != '\n';);
			// index = AddEmbedding(word, embedding);
			// mWordEmbeds[index].cn++;
		// }

	// }

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
