#ifndef TFPROJECT_CLUSTER_H_
#define TFPROJECT_CLUSTER_H_
#include <WordLibrary.h>
#include <cstdio>
#include <cstring>
#include <cmath>

using namespace std;
//WordEmbedding *mWordEmbedding;

struct cluster {

protected:
	unsigned numClusters, numIterations;
	// clcn : specified number of clusters
	// iter : specified number of iterations for K-means

	unsigned closeid;
	// closeid : closest cluster

	int *centcn;
	int *cl;

	real closev, x;
	real *cent;
	// cent : coordinates of the cluster centroid

	int vec_size;
	AllEmbeds& vectors;

	//vector<int> IDvectors;
public:



	cluster(unsigned nClusters, unsigned iterations, AllEmbeds& vecEmbeddings) :
			numClusters(nClusters), numIterations(iterations), vectors(
					vecEmbeddings) {

		vec_size = (int) vectors.size();  //the number of embeddings

		cout << "vec_size == " << vec_size << endl;
		centcn = new int[numClusters];
		cl = new int[vec_size];
		cent = new real[numClusters * layer1_size];
		if (cent == NULL)
			perror("Memory fail\n");
		Kmeans();
	}

	~cluster() {
		delete centcn;
		delete cent;
		delete cl;
	}

	void Kmeans() {
		// Run K-means on the word vectors
		unsigned a, b, c, d;
		cout << "Kmeans" << endl;
		for (a = 0; a < vec_size; a++)
			cl[a] = a % numClusters;

		for (a = 0; a < numIterations; a++) {
			for (b = 0; b < numClusters; b++)
				for (c = 0; c < layer1_size; c++)
					cent[b * layer1_size + c] = 0;
			for (b = 0; b < numClusters; b++)
				centcn[b] = 1;
			for (c = 0; c < vec_size; c++) {
				for (d = 0; d < layer1_size; d++)
					cent[layer1_size * cl[c] + d] += vectors[c][d];
				centcn[cl[c]]++;
			}
			for (b = 0; b < numClusters; b++) {
				closev = 0;
				for (c = 0; c < layer1_size; c++) {
					cent[layer1_size * b + c] /= centcn[b];
					closev += cent[layer1_size * b + c]
							* cent[layer1_size * b + c];
				}
				closev = sqrt(closev);
				for (c = 0; c < layer1_size; c++)
					cent[layer1_size * b + c] /= closev;
			}
			for (c = 0; c < vec_size; c++) {
				closev = -10;
				closeid = 0;
				for (d = 0; d < numClusters; d++) {
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
			cout << a << endl;
		}

		//for (a = 0; a < vec_size; a++)
		//	mWordEmbedding -> ChangeEmbedWordCl(IDvectors[a], cl[a]);
		cout << "End Kmeans" << endl;
	}

	vector<real *> GetCentroid() {
		// Return the centroid coordinates
		vector<real *> result;
		result.clear();
		unsigned i;
		for (i = 0; i < numClusters; i++) {
			real *centroid = new real[layer1_size];
			for (int j = 0; j < layer1_size; j++)
				centroid[j] = cent[i * layer1_size + j];
			result.push_back(centroid);
		}
		return result;
	}

	int* GetLabels() {
		return cl;
	}

};
#endif
