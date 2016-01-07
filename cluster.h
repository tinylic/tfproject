#include <cstdio>
#include <cstring>
#include <cmath>
#include "vectors.h"

using namespace std;

struct cluster{
private:
	WordEmbedding *mWordEmbedding;

public:
	unsigned clcn, iter, closeid;
	// clcn : class number
	// closeid : closest cluster

	int *centcn;
	int *cl;
	real closev,x;
	real *cent;
	// cent : coordinates of the cluster centroid
	AllEmbeds vectors;
	void Init() {
		mWordEmbedding = new WordEmbedding;
		vectors.clear();
	}
	cluster(){
		// Initialization
		Init();
	}

	~cluster(){
		//delete mWordEmbedding;
		//delete centcn;
		//delete cent;
	}

	cluster(WordEmbedding *mEmbedding) {
		Init();
		mWordEmbedding = mEmbedding;
	}
	cluster(const char *fn, bool IsBinary) {
		Init();
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
			clcn = classes, iter = 2;
			vectors = (*mWordEmbedding).getAllEmbedding();
			unsigned vec_size = (int)vectors.size();
			cout << "vec_size == " << vec_size << endl;
			cout << clcn << endl;
			centcn = new int[classes];
			cl = new int[vec_size];
			cent = new real[classes * layer1_size];
			if (cent == NULL) perror("Memory fail\n");
			for (a = 0; a < vec_size; a++) cl[a] = a % clcn;
			for (a = 0; a < iter; a++) {
				memset(cent, 0, sizeof cent);
				memset(centcn, 0, sizeof centcn);
				for (b = 0; b < clcn; b++) centcn[b] = 1;
				for (c = 0; c < vec_size; c++) {
					for (d = 0; d < layer1_size; d++)
						cent[layer1_size * cl[c] + d] += vectors[c][d];
					centcn[cl[c]]++;
				}
				for (b = 0; b < clcn; b++) {
					closev = 0;
					//cout << "centcn = " << centcn[b] << endl;
					for (c = 0; c < layer1_size; c++) {
						cent[layer1_size * b + c] /= centcn[b];
						closev += cent[layer1_size * b + c] * cent[layer1_size * b + c];
					}
					//printf("====\nclosev = %.6f\n", closev);
					//closev = sqrt(closev);
					for (c = 0; c < layer1_size; c++) cent[layer1_size * b + c] /= closev;
				}
				for (c = 0; c < vec_size; c++) {
					//for (d = 0; d < layer1_size; d++)
						//printf("%.6f ", vectors[c][d]);
					//cout << endl;
					closev = -1e9;
					closeid = 0;
					for (d = 0; d < clcn; d++) {
						x = 0;
						for (b = 0; b < layer1_size; b++){
							x += cent[layer1_size * d + b] * vectors[c][b];
							//printf("x = %.6f\n", x);
						}
						//cout << "=====\n";
						if (x > closev) {
							closev = x;
							closeid = d;
						}
						//printf("x = %.6f\n", x);
					}
					//cout << "closest = " << closeid << endl;
					cl[c] = closeid;
				}
			}
    // Save the K-means classes
	 //for (a = 0; a < vec_size; a++) printf("%d\n", cl[a]);
		}
	}
		vector<real *> GetCentroid() {
			// Return the centroid coordinates
			vector<real *> result;
			result.clear();
			unsigned i;
			for (i = 0; i < clcn; i++) {
				real *centroid = new real[layer1_size];
				for (int j = 0; j < layer1_size; j++)
					centroid[j] = cent[i * layer1_size + j];
				result.push_back(centroid);
			}
			return result;
		}
	real *Transform(const vector<Upair> &doc){
		// Documents are represented in word_id
		unsigned len = doc.size();
		unsigned i;
		long long total = 0;
		int *ans = new int[clcn];
		real *result = new real[clcn];
		for (i = 0; i < clcn; i++)
			ans[i] = 0;
		for (i = 0; i < len; i++) {
			//cout << "doc " << doc[i].first << " cluster " << cl[doc[i].first] << endl;
			ans[cl[doc[i].first]] += doc[i].second;
			total += doc[i].second;
		}
		total ++;//Avoid total == 0
		for (i = 0; i < clcn; i++){
			//printf("%d ", ans[i]);
			result[i] = (real)ans[i] / total;
		}
		return result;
	}
	real *Transform(Document *Doc) {
		// Read documents in the form of <w1, c1>
		vector<Upair> All = Doc -> GetAllWord();
		vector<Upair> NewAll;
		NewAll.clear();
		//cout << All.size() << endl;
		//cout << "vec_size == " << mWordEmbedding -> word_size << endl;
		//cout << sizeof(mWordEmbedding -> word_hash) / sizeof(int) << endl;
		for (int i = 0; i < (int)All.size(); i++) {
            Char *mWord = Doc -> mDict -> GetWord(All[i].first);
            //cout << mWord << " == ";
            // Word in the document
            int index = mWordEmbedding -> SearchVocab(mWord);
            if (index == -1) continue;
            //cout << mWordEmbedding -> GetWord(index) << endl;
            // Word ID in the Embedding
            NewAll.push_back(make_pair(index, All[i].second));
		}
		return Transform(NewAll);
	}
};
