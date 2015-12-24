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
	
	WordEmbedding(const string & fn, bool isBinary){
		//read in from a word embedding file 
		
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