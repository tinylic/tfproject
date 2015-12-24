#include <cstdio>
#include <cstring>
#include <cmath>

using namespace std;

struct cluster{
	int clcn, iter, closeid;
	int vec_size;
	int *centcn;
	int *cl;
	double closev,x;
	double *cent;
	void Kmeans(int classes, vector<wordvector> &vectors){
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
	clcn = classes, iter = 10;
	vec_size = (int)vectors.size();
	*centcn = (int *)malloc(classes * sizeof(int));
	*cl = (int *)calloc(vec_size, sizeof(int));
	*cent = (real *)calloc(classes * layer1_size, sizeof(real));
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
}Cluster;