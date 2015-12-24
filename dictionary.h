#include <cstdio>
#include <algorithm>
#include "head.h"

using namespace std;

// Returns hash value of a word
class  Dictionary {
private:
	struct vocab_word *vocab;
	int *vocab_hash;
	int vocab_size;
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
	struct vocab_word {
		char *word;
	};
	
	Dictionary(){
		vocab = (struct vocab_word *)calloc(vocab_max_size, sizeof(struct vocab_word));
		vocab_hash = (int *)calloc(vocab_hash_size, sizeof(int));		
		vocab_size = 0;
	}
	
	Dictionary(const string & fn){
		// read in a dictionary from a file

		Char word[MAX_STRING];
		FILE *fin = fopen(fn, "rb");
		if (fin == NULL) {
			printf("ERROR: training data file not found!\n");
			exit(1);
		}

		while (1) {
			ReadWord(word, fin);
			if (feof(fin)) break;
			i = SearchVocab(word);
			if (i == -1) {
				a = AddWordToVocab(word);
				vocab[a].cn = 1;
			} else vocab[i].cn++;
		}
		
	}
	
	~Dictionary(){
		free(vocab);
		free(vocab_hash);
		vocab_size = 0;
	}
	
	int GetWordHash(Char *word) {
		unsigned long long a, hash = 0;
		int len = word.length();
		for (a = 0; a < len; a++) hash = hash * 257 + word[a];
		hash = hash % vocab_hash_size;
		return hash;
	}

// Returns position of a word in the vocabulary; if the word is not found, returns -1

	int SearchVocab(Char *word) {
		unsigned int hash = GetWordHash(word);
		while (1) {
			if (vocab_hash[hash] == -1) return -1;
			if (!strcmp(word, vocab[vocab_hash[hash]].word)) return vocab_hash[hash];
			hash = (hash + 1) % vocab_hash_size;
		}
		return -1;
	}
	
	Char* GetWord(unsigned int hash) {
		if (vocab_hash[hash] == -1) return -1;
		return (u32string)vocab[vocab_hash[hash]].word;
	}
	int AddWordToVocab(Char *word) {
		unsigned int hash, length = strlen(word) + 1;
		if (length > MAX_STRING) length = MAX_STRING;
		vocab[vocab_size].word = (Char *)calloc(length, sizeof(Char));
		strcpy(vocab[vocab_size].word, word);
		vocab[vocab_size].cn = 0;
		vocab_size++;
		hash = GetWordHash(word);
		while (vocab_hash[hash] != -1) hash = (hash + 1) % vocab_hash_size;
		vocab_hash[hash] = vocab_size - 1;
		return vocab_size - 1;
	}

}Dict;