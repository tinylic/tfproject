#include <cstdio>
#include <algorithm>

using namespace std;

class WordEmbedding {
private:
	unsigned nDim;  //the dimensionality of word embedding
	embed_word* mWordEmbeds;
	
	int * word_hash;
public:
	struct embed_word {
		char32_t *word, 
		double *embedding;
	};
	
	WordEmbedding() {
		vocab = (struct embed_word *)calloc(vocab_max_size, sizeof(struct embed_word));
	}
	WordEmbedding(const string & fn, bool isBinary){
		//read in from a word embedding file 
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
	
	int getEmbedding(const u32string& word, double* embedding){
		
	}
	
	vector<double*> getAllEmbedding() {
		 
	}
	
};
struct wordvector{
		int dim;
		int word_id;
		double vec[layer1size];
		u32string word;
		wordvector(){}
		wordvector(int _word_id, double *_vec){
			word_id = _word_id;
			for (int i = 0; i < layer1size; i++)
			vec[i] = _vec[i];
			word = Dict.GetWord(_word_id);
		}
};