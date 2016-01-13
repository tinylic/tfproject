#include <cstdio>
#include <cstring>
#include <cmath>
#include "vectors.h"

using namespace std;
WordEmbedding *mWordEmbedding;
struct cluster{
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
	vector<int> IDvectors;
	void Init() {
		mWordEmbedding = new WordEmbedding;
		vectors.clear();
		IDvectors.clear();
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
			// Run K-means on the word vectors
			unsigned a, b, c, d;
			clcn = classes, iter = 10;
			unsigned vec_size = (int)vectors.size();
			//cout << "vec_size == " << vec_size << endl;
			//cout << clcn << endl;
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
			for (a = 0; a < vec_size; a++)
				mWordEmbedding -> mWordEmbeds[IDvectors[a]].cl = cl[a];
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

	void GetAllEmbedding(Document *Doc) {
		vector<Upair> All = Doc -> GetAllWord();
		Doc -> AllEmbed.resize(All.size());
		for (int i = 0; i < (int)All.size(); i++) {
			Doc -> AllEmbed[i] = NULL;
            Char *mWord = Doc -> mDict -> GetWord(All[i].first);
            //cout << mWord << endl;
            // Word in the document
            int index = mWordEmbedding -> SearchVocab(mWord);
            if (index == -1) continue;
            // Word ID in the Embedding
            Doc -> AllEmbed[i] = new real[layer1_size];
            real *mEmbed = (*mWordEmbedding).GetEmbedding(index);
            for (int j = 0; j < All[i].second; j++) {
            	vectors.push_back(mEmbed);
            	IDvectors.push_back(index);
            }
            for (int j = 0; j < layer1_size; j++)
            	Doc -> AllEmbed[i][j] = mEmbed[j];
		}
	}
};
