/*
 * CEmbeddingHIstogramInformationRetrieval.cpp
 *
 *  Created on: 2016年3月3日
 *      Author: xiezp
 */

#include "CEmbeddingHistogramInformationRetrieval.h"


CEmbeddingHistogramInformationRetrieval::CEmbeddingHistogramInformationRetrieval(
		WordLibrary& dict, Corpus& train) :
		CInformationRetrieval(dict, train) {
	// TODO Auto-generated constructor stub
	vector<unsigned> vecIDs;
	vector<Real*> vecEmbeddings;

	trainCorpus.GetAllEmbeddings(vecEmbeddings, vecIDs);
	int vecsize = vecEmbeddings.size();
	double *ArrayEmbeddings = new double[vecsize * layer1_size];
	for (int i = 0; i < vecsize; i++) {
		for (int j = 0; j < layer1_size; j++)
			ArrayEmbeddings[i * layer1_size + j] = (double)vecEmbeddings[i][j];
	}
	pCluster = new cluster(max_w, 10, vecEmbeddings);
	/*gmm -> Train(ArrayEmbeddings, vecsize);
	for (int i = 0; i < max_w; i++) {
		double* mmean = gmm -> Mean(i);
		printf("\nthe %dth cluster mean is ", i);
		for (int j = 0; j < layer1_size; j++)
			printf("%f ", mmean[j]);
	}
	for (int i = 0; i < max_w; i++) {
		double* mvar = gmm ->Variance(i);
		printf("\nthe %dth cluster var is ", i);
		for (int j = 0; j < layer1_size; j++) {
			printf("%f ", mvar[j]);
		}
	}
	for (int i = 0; i < vecsize; i++) {
		double* mdistri = new double[max_w];
		double* tEmbeddings = new double[layer1_size];
		for (int j = 0; j < layer1_size; j++) {
			tEmbeddings[j] = ArrayEmbeddings[i * layer1_size + j];
		}
		double probsum = 0;
		for (int j = 0; j < max_w; j++) {
			mdistri[j] = gmm ->GetProbability(tEmbeddings, j);
			//printf("%d %.6lf\n", j, gmm ->GetProbability(tEmbeddings, j));
			//GetProbability(sample, kcluster)
			probsum += mdistri[j];
		}
		if (probsum == 0) probsum = 1e-9;
		for (int j = 0; j < max_w; j++) {
			mdistri[j] /= probsum;
		//	printf("%.6lf ", mdistri[j]);
		}
		//printf("%f \n", probsum);
		mDict.ChangeEmbedWordGMM(vecIDs[i], mdistri);
	}*/
/*
	vector<pair<Real, int>> RelativeDistance;//<distance to cur node, index>
	for (int i = 0; i < max_w; i++)
		RelativeDistance.push_back(make_pair(0.0, i));
	DistanceMatrix = new Real*[max_w];
	for (int i = 0; i < max_w; i++) {
		DistanceMatrix[i] = new Real[max_w];
		for (int j = 0; j < max_w; j++) {
			Real tDistance = pCluster->CalcDistance(i, j, layer1_size);
			RelativeDistance[j] = make_pair(tDistance, j);
		}
		sort(RelativeDistance.begin(), RelativeDistance.end());
		debug("\n i = %d\n", i);
		for (int j = 0; j < 5; j++)
			debug("%d : %.6f\n", RelativeDistance[j].second, RelativeDistance[j].first);
		Real DistCoef = max_w * DIST_STEP;//1;
		for (int j = 0; j < max_w; j++) {
			DistanceMatrix[i][RelativeDistance[j].second] = DistCoef;// / (j + 1);
			//DistCoef -= DIST_STEP;
			DistCoef /= 2;
			//DistanceMatrix[i][j] = i == j;
		}
	}*/
	mDict.ClearAllLabels(); //clear all the cluster assignment of the words in the library

	int* cl = pCluster -> GetLabels();
	for (unsigned a = 0; a < vecEmbeddings.size(); a++)
		mDict.ChangeEmbedWordCl(vecIDs[a], cl[a]);
	//construct the observation matrix(N * D)
	// N observations, embedding numbers
	// D dimensions
	//MatrixXd Observations(vecEmbeddings.size(), layer1_size);
	//for (int i = 0; i < vecEmbeddings.size(); i++)
	//	for (int j = 0; j < layer1_size; j++)
	//		Observations(i, j) = vecEmbeddings[i][j];
	//construct the prob matrix(N * K)
	// N observations
	// K cluster numbers
	// initialize with k-means results
	//MatrixXd qZ(vecEmbeddings.size(), max_w);
	//for (int i = 0; i < vecEmbeddings.size(); i++) {
	//	for (int j = 0; j < max_w; j++)
	//		qZ(i, j) = 0.0;
	//	qZ(i, cl[i]) = 1.0;
	//}
	//cerr << Observations.size() << endl;
	//cerr << qZ.size() << endl;
	//Dirichlet weights;
	//vector<GaussWish> clusters;

	//Learn Bayesian GMM
	//double F = learnBGMM(Observations, qZ, weights, clusters, PRIORVAL, max_w, true);
	//for (int i = 0; i < vecEmbeddings.size(); i++) {
	//	for (int j = 0; j < max_w; j++)
	//		cerr << qZ(i, j) << " ";
	//	cerr << endl;
	//}

	//Assign Cluster Distributions
	//Real *tDistri = new Real[max_w];
	//for (int i = 0; i < vecEmbeddings.size(); i++) {
	//	for (int j = 0; j < max_w; j++)
	//		tDistri[j] = qZ(i, j);
	//	mDict.SetDistributions(vecIDs[i], tDistri, max_w);
	//}
	cout << "End Labels" << endl;
	cout << trainCorpus.size() << endl;
	for (int i = 0; i < trainCorpus.size(); i++) {
		trainCorpus.getDocument(i) -> ClusterTransform(max_w);
	}
	cerr << "Transform Finished" << endl;
}

CEmbeddingHistogramInformationRetrieval::~CEmbeddingHistogramInformationRetrieval() {
	// TODO Auto-generated destructor stub
}

void CEmbeddingHistogramInformationRetrieval::Transform(Document *querydoc) {
	querydoc->ClusterTransform(max_w);
}
void CEmbeddingHistogramInformationRetrieval::rank(Document* queryDoc) {
	std::thread mThreads[MAX_THREADS];
	Transform(queryDoc);
	/*for (int i = 0; i < trainCorpus.size(); i++) {
		Real dist = distance(trainCorpus.getDocument(i), queryDoc);
		//printf("%.6f\n", dist);
		DocCmp mDocCmp = DocCmp(trainCorpus.getDocument(i), dist);
		dis.push_back(mDocCmp);
	}*/
	for (int i = 0; i < MAX_THREADS; i++)
		mThreads[i] = std::thread(&CInformationRetrieval::ThreadFunction, *this, queryDoc, i);
	for (int i = 0; i < MAX_THREADS; i++)
		mThreads[i].join();
	sort(dis, dis + trainCorpus.size());
	return;
}
Real CEmbeddingHistogramInformationRetrieval::distance(Document* doc1,
		Document* doc2) {
	Real* vec1 = doc1->GetTransformed();
	Real* vec2 = doc2->GetTransformed();
	//return ImprovedDistance(vec1, vec2, max_w);
	//return CenterDistance(doc1, doc2, max_w, layer1_size);
	//return SquaredEuclideanDistance(vec1, vec2, max_w);
	return WMDDistance(doc1, doc2, max_w, layer1_size);
}

Real CEmbeddingHistogramInformationRetrieval::ImprovedDistance(Real* vec1, Real* vec2,	int size) {
	if (vec1 == NULL || vec2 == NULL)
		return DIS_INF;

	Real* vec = new Real[size];
	for (int i = 0; i < size; i++)
		vec[i] = vec1[i] - vec2[i];

	Real* left_hand = new Real[size];
	for (int i = 0; i < size; i++) {
		left_hand[i] = 0;
		for (int j = 0; j < size; j++)
			left_hand[i] += vec[j] * DistanceMatrix[i][j];
	}

	Real result = 0;
	for (int i = 0; i < size; i++)
		result += left_hand[i] * vec[i];

	return result;
}
Real CEmbeddingHistogramInformationRetrieval::CenterDistance(Document* doc1, Document* doc2, int cluster_size, int embedding_size) {
	Real* ClusterVec1 = doc1->GetTransformed();
	Real* ClusterVec2 = doc2->GetTransformed();
	Real* vec1 = new Real[embedding_size];
	for (int i = 0; i < embedding_size; i++) {
		vec1[i] = 0;
		for (int j = 0; j < cluster_size; j++)
			vec1[i] += ClusterVec1[j] * ((pCluster->GetCentroid(j))[i]);
	}
	Real* vec2 = new Real[embedding_size];
	for (int i = 0; i < embedding_size; i++) {
		vec2[i] = 0;
		for (int j = 0; j < cluster_size; j++)
			vec2[i] += ClusterVec2[j] * ((pCluster->GetCentroid(j))[i]);
	}
	return SquaredEuclideanDistance(vec1, vec2, embedding_size);
}

Real CEmbeddingHistogramInformationRetrieval::WMDDistance(Document* doc1, Document* doc2, int cluster_size, int embedding_size) {
	Real* DA = doc1->GetTransformed();
	Real* DB = doc2->GetTransformed();
	Real **cost = new Real *[cluster_size];
	for (int i = 0; i < cluster_size; i++) {
		cost[i] = new Real[cluster_size];
		for (int j = 0; j < cluster_size; j++)
			cost[i][j] = pCluster->CalcDistance(i, j, layer1_size);
	}

	signature_t doca = signature_t { cluster_size, DA };
	signature_t docb = signature_t { cluster_size, DB };
	emd_node mEmd_node;
	Real result = mEmd_node.emd(&doca, &docb, cost, 0, 0);
	if (!isfinite(result))
		result = 1e9;
	//cerr << result << endl;
	return result;

}
