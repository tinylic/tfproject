#include <cstdio>
#include <algorithm>

using namespace std;

// Returns hash value of a word
struct Dictionary {
	int GetWordHash(u32string word) {
		unsigned long long a, hash = 0;
		int len = word.length();
		for (a = 0; a < len; a++) hash = hash * 257 + word[a];
		hash = hash % vocab_hash_size;
		return hash;
	}

// Returns position of a word in the vocabulary; if the word is not found, returns -1

	int SearchVocab(u32string word) {
		unsigned int hash = GetWordHash(word);
		while (1) {
			if (vocab_hash[hash] == -1) return -1;
			if (!strcmp(word, vocab[vocab_hash[hash]].word)) return vocab_hash[hash];
			hash = (hash + 1) % vocab_hash_size;
		}
		return -1;
	}
	
	u32string GetWord(unsigned int hash) {
		if (vocab_hash[hash] == -1) return -1;
		return (u32string)vocab[vocab_hash[hash]].word;
	}
}Dict;