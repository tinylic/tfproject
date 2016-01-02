#include <cstdio>
#include <cstring>
#include <cmath>
#include "vectors.h"

using namespace std;

struct cluster{
private:
	WordEmbedding *mWordEmbedding;

public:
	int clcn, iter, closeid;
	// clcn : class number
	// closeid : closest cluster
	int vec_size;
	int *centcn;
	int *cl;
	real closev,x;
	real *cent;
	// cent : coordinates of the cluster centroid
	AllEmbeds vectors;
	cluster(){
		// Initialization
		mWordEmbedding = (WordEmbedding *)calloc(1, sizeof(WordEmbedding));
		vectors.clear();
	}

	~cluster(){
		free(mWordEmbedding);
	}

	cluster(WordEmbedding *mEmbedding) {
		mWordEmbedding = (WordEmbedding *)calloc(1, sizeof(WordEmbedding));
		vectors.clear();
		mWordEmbedding = mEmbedding;
	}
	cluster(const char *fn, bool IsBinary) {
		mWordEmbedding = (WordEmbedding *)calloc(1, sizeof(WordEmbedding));
		vectors.clear();
		(*mWordEmbedding) = WordEmbedding(fn, IsBinary);
	}
	void Kmeans(int classes){
		if (classes == 0) {
			// Save the word vectors
			// fprintf(fo, "%lld %lld\n", vec_size, layer1_size);
			// for (a = 0; a < vec_size; a++) {
				// fprintf(fo, "%s ", vocab[a].word);
				// if (binary) for (b = 0; b < layer1_size; b++) fwrite(&syn0[a * layer1_size + b], sizeof(real), 1, fo);
				// else for (b = 0; b < layer1_size; b++) fprintf(fo, "%lf ", syn0[a * layer1_size + b]);
				// fprintf(fo, "\n");
			// }
		}
		else {
			// Run K-means on the word vectors
			unsigned a, b, c, d;
			clcn = classes, iter = 10;
			vectors = (*mWordEmbedding).getAllEmbedding();
			vec_size = (int)vectors.size();
			centcn = (int *)malloc(classes * sizeof(int));
			cl = (int *)calloc(vec_size, sizeof(int));
			cent = (real *)calloc(classes * layer1_size, sizeof(real));
			if (cent == NULL) perror("Memory fail\n");
			for (a = 0; a < vec_size; a++) cl[a] = a % clcn;
			for (a = 0; a < iter; a++) {
				for (b = 0; b < clcn * layer1_size; b++) cent[b] = 0;
				for (b = 0; b < clcn; b++) centcn[b] = 1;
				for (c = 0; c < vec_size; c++) {
					for (d = 0; d < layer1_size; d++)
						cent[layer1_size * cl[c] + d] += vectors[c][d];
					centcn[cl[c]]++;
				}
				for (b = 0; b < clcn; b++) {
					closev = 0;
					for (c = 0; c < layer1_size; c++) {
						cent[layer1_size * b + c] /= centcn[b];
						closev += cent[layer1_size * b + c] * cent[layer1_size * b + c];
					}
					closev = sqrt(closev);
					for (c = 0; c < layer1_size; c++) cent[layer1_size * b + c] /= closev;
				}
				for (c = 0; c < vec_size; c++) {
					closev = -10;
					closeid = 0;
					for (d = 0; d < clcn; d++) {
						x = 0;
						for (b = 0; b < layer1_size; b++)
							x += cent[layer1_size * d + b] * vectors[c][b];
						if (x > closev) {
							closev = x;
							closeid = d;
						}
					}
					cl[c] = closeid;
				}
			}
    // Save the K-means classes
	// for (a = 0; a < vec_size; a++) fprintf(fo, "%s %d\n", vocab[a].word, cl[a]);
		}
	}
		vector<real *> GetCentroid() {
			// Return the centroid coordinates
			vector<real *> result;
			result.clear();
			for (int i = 0; i < clcn; i++) {
				real *centroid = (real *)calloc(layer1_size, sizeof(real));
				for (int j = 0; j < layer1_size; j++)
					centroid[j] = cent[i * layer1_size + j];
				result.push_back(centroid);
			}
			return result;
		}
	real *Transform(vector<Upair> doc){
		// Documents are represented in word_id
		int len = (int)doc.size();
		//debug("len = %d\n", len);
		int *ans = (int *)calloc(classes, sizeof(int));
		real *result = (real *)calloc(clcn, sizeof(real));
		memset(ans, 0, sizeof ans);
		for (int i = 0; i < len; i++)
			ans[cl[doc[i].first]] += doc[i].second;
		for (int i = 0; i < classes; i++)
			result[i] = (real)ans[i] / len;
		return result;
	}
	real *Transform(Document *Doc) {
		// Read documents in the form of <w1, c1>
		vector<Upair> All = Doc -> GetAllWord();
		vector<Upair> NewAll;
		NewAll.clear();
		for (int i = 0; i < (int)All.size(); i++) {
			cout << All[i].first << " " << All[i].second << endl;
            Char *mWord = Doc -> mDict -> GetWord(All[i].first);
            cout << "asdas" << endl;
            if (mWord == NULL)
            	cout << "fuck" << endl;
            else
            	cout << mWord << endl;
            cout << "asd" << endl;
            // Word in the document
            int index = mWordEmbedding -> SearchVocab(mWord);
            // Word ID in the Embedding
            cout << index;
            cout << " === " << endl;
            if (index != -1)
                NewAll.push_back(make_pair(index, All[i].second));
		}
		cout << "asdas" << endl;
		cout << "asd" << endl;
		return Transform(NewAll);
	}
	// double *transform(const Document &doc){
	// Documents are represented in word_id
	// int len = (int)doc.size();
	// int *ans = (int *)calloc(classes, sizeof(int));
	// memset(ans, 0, sizeof ans);
	// for (int i = 0; i < len; i++)
		// ans[cl[doc[i]]] ++;
	// for (int i = 0; i < classes; i++)
		// result[i] = (double)ans[i] / len;
};
