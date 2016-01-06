#include <cstdio>
#include <algorithm>
#include "heads.h"

using namespace std;

// Returns hash value of a word
struct vocab_word {
		Char *word;
};

class  Dictionary {
private:
	vocab_word *vocab;
	int *vocab_hash;
	int vocab_size;

public:

	Dictionary(){
		// Initialization
		vocab = (struct vocab_word *)calloc(dict_hash_size, sizeof(struct vocab_word));
		vocab_hash = (int *)calloc(dict_hash_size, sizeof(int));
		for (int i = 0; i < dict_hash_size; i++)
            vocab_hash[i] = -1;
		vocab_size = 0;
	}
	void Init() {
		vocab = (struct vocab_word *)calloc(dict_hash_size, sizeof(struct vocab_word));
		vocab_hash = (int *)calloc(dict_hash_size, sizeof(int));
		for (int i = 0; i < dict_hash_size; i++)
            vocab_hash[i] = -1;
		vocab_size = 0;
	}
	void ReadWord(Char *word, FILE *f) {
			int a = 0;
			while (1) {
				word[a] = fgetc(f);
				if (!isalpha(word[a])) break;
							if ((a < max_w) && (word[a] != '\n')) a++;
						}
			word[a] = 0;
		}
	
	Dictionary(const char *fn){
		// read in a dictionary from a file
		Init();
		Char word[MAX_STRING];
		int index;
		FILE *fin = fopen(fn, "r");
		if (fin == NULL) {
			printf("ERROR: training data file not found!\n");
			exit(1);
		}

		while (1) {
			ReadWord(word, fin);
			if (strlen(word) < MIN_WORDS) continue;
			if (feof(fin)) break;
			index = SearchVocab(word);
			if (index == -1) {
				index = AddWordToVocab(word);
			}
		}

	}

	~Dictionary(){
		free(vocab);
		free(vocab_hash);
		vocab_size = 0;
	}

	int GetWordHash(Char *word) {
		unsigned long long hash = 0;
		int a, len = strlen(word);
		for (a = 0; a < len; a++) hash = hash * 257 + word[a];
		hash = hash % dict_hash_size;
		return hash;
	}

// Returns position of a word in the vocabulary; if the word is not found, returns -1

	int SearchVocab(Char *word) {
		unsigned int hash = GetWordHash(word);
		while (1) {
			if (vocab_hash[hash] == -1) return -1;
			if (!strcmp(word, vocab[vocab_hash[hash]].word)) return vocab_hash[hash];
			hash = (hash + 1) % dict_hash_size;
		}
		return -1;
	}
	// Return the word in the position [hash]
	Char* GetWord(int hash) {
		if (hash == -1) return NULL;
		return vocab[hash].word;
	}
	int AddWordToVocab(Char *word) {
		unsigned int hash, length = strlen(word) + 1;
		if (length > MAX_STRING) length = MAX_STRING;
		vocab[vocab_size].word = (Char *)calloc(length, sizeof(Char));
		strcpy(vocab[vocab_size].word, word);
		vocab_size++;
		hash = GetWordHash(word);
		while (vocab_hash[hash] != -1) hash = (hash + 1) % dict_hash_size;
		vocab_hash[hash] = vocab_size - 1;
		return vocab_size - 1;
	}

}Dict;
