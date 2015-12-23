#include <cstdio>
#include <algorithm>

using namespace std;

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