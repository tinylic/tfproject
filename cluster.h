#ifndef TFPROJECT_CLUSTER_H_
#define TFPROJECT_CLUSTER_H_
#include <WordLibrary.h>
#include <cstdio>
#include <cstring>
#include <cmath>

using namespace std;
//WordEmbedding *mWordEmbedding;

struct cluster {

private:
	cluster(const cluster &other);
protected:
	unsigned numClusters, numIterations;
	// clcn : specified number of clusters
	// iter : specified number of iterations for K-means

	unsigned closeid;
	// closeid : closest cluster

	int *centcn;
	int *cl;
	Real **centroid;

	Real closev, x;

	// cent : coordinates of the cluster centroid

	unsigned vec_size;
	AllEmbeds& vectors;

	//vector<int> IDvectors;
public:
	Real *cent;
	cluster(unsigned nClusters, unsigned iterations, AllEmbeds& vecEmbeddings) :
			numClusters(nClusters), numIterations(iterations), vectors(
					vecEmbeddings) {

		vec_size = vectors.size();  //the number of embeddings

		cerr << "vec_size == " << vec_size << endl;
		centcn = new int[numClusters];
		cl = new int[vec_size];
		cent = new Real[numClusters * layer1_size];
		if (cent == NULL)
			perror("Memory fail\n");
		Kmeans();
	}

	~cluster() {
		//delete centcn;
		//delete cent;
		//delete cl;
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
			cerr << a << endl;
		}

		//for (a = 0; a < vec_size; a++)
		//	mWordEmbedding -> ChangeEmbedWordCl(IDvectors[a], cl[a]);
		cout << "End Kmeans" << endl;
	}

	Real* GetCentroid(int index) {
		// Return the centroid coordinates
		return cent + (index * layer1_size);
	}

	Real CalcDistance(int a, int b, int size) {
		Real result = 0;
		Real *veca = GetCentroid(a);
		Real *vecb = GetCentroid(b);
		for (int i = 0; i < size; i++)
			result += (veca[i] - vecb[i]) * (veca[i] - vecb[i]);
		return result;
	}

	int* GetLabels() {
		return cl;
	}

};
#endif
