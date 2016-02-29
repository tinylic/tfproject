#include "cluster.h"
#include "transform.h"
#include "wmd.h"
#include "ReadFile.h"

const char rootaddr[] = "20_newsgroups";
vector < Document > Groups;
real *embeds[MAX_TAGS * MAX_DOC_PER_TAG];
#include "Method1.h"
#include "BrownCluster.h"
#include "distance.h"
#include "../brown/wcluster.h"
#include "TF_IDF.h"

struct DocCmp {
	int doc_id;
	real distance;
	DocCmp(){
		doc_id = 0;
		distance = 0;
	}
	DocCmp(int _doc_id, real _distance) {
		doc_id = _doc_id;
		distance = _distance;
	}
	friend bool operator <(DocCmp a, DocCmp b) {
		return a.distance < b.distance;
	}
};

DocCmp dis[MAX_DOCS];


class test_corpus {
private:
	DIR *dir, *curdir;
	int tag_count;
	int doc_count;
	int tot_doc;
	struct dirent *ptr;
	struct dirent *curptr;
	struct stat info;
	pthread_t *pt;

public:
	static int query_id;

	FILE *fout = fopen("result.txt", "w");
	FILE *frepo = fopen("WMD_100NearestResult.txt", "w");
	cluster Cluster;

	void Run() {

		fprintf(frepo, "Using %d Threads\n", MAX_THREADS);
		clock_t start_time = clock();
		Groups.clear();
		Cluster = cluster("vectors.bin", true);
		clock_t read_corpus_time = clock();
		fprintf(frepo, "Reading Corpus : %.6lf seconds.\n", (double)(read_corpus_time - start_time) / CLOCKS_PER_SEC);
		cout << "reading" << endl;

		tag_count = 0;
		doc_count = 0;
		tot_doc = 0;

		//Read 20news total 2000 docs

		Read20News();

		random_shuffle(Groups.begin(), Groups.end());

		clock_t read_news_time = clock();
		fprintf(frepo, "Reading 20news : %.6lf seconds.\n", (double)(read_news_time - read_corpus_time) / CLOCKS_PER_SEC);

		//Choose different methods

		//RunMethod1(&Cluster);
		//Calc_TF_IDF();
		//RunMethodBrown(max_w, "output.txt");

		clock_t cluster_time = clock();
		fprintf(frepo, "Clustering : %.6lf seconds.\n", (double)(cluster_time - read_news_time) / CLOCKS_PER_SEC);
		pt = (pthread_t *)malloc(MAX_THREADS * sizeof(pthread_t));
		real total = 0;

		for (int i = 0; i < QUERY_DOC; i++) {
			query_id = i;
			memset(dis, 0, sizeof dis);
			assert(Groups[i].GetEmbedSize() == Groups[i].GetWordSize());

			//Run Method1 or tf-idf with multi threads
			//Run_Threads();

			//WMD method

			//prefetch & prune

			Run_WMD_Prefetch_And_Prune(query_id);

			//or calculate all wmd

			//Run_WMD_Brute(query_id);


			real tmap = MAP(i);
			total += tmap;
			if (i % 10 == 0) cout << i << endl;
			fprintf(frepo, "MAP = %.6f\n", tmap);
			printf("Current MAP : %.6f\n", tmap);
			printf("Average MAP : %.6f\n", total / (i + 1));
		}
		fprintf(frepo, "Average MAP : %.6f\n", total / QUERY_DOC);
		clock_t dist_time = clock();
		fprintf(frepo, "Calculating Similarities : %.6lf seconds.\n", (double)(dist_time - cluster_time) / CLOCKS_PER_SEC);
	}

	void Read20News() {
		dir = opendir(rootaddr);
		while ((ptr = readdir(dir)) != NULL) {
			if (ptr -> d_name[0] == '.') continue;
			char *addr = (char *)calloc(255, sizeof(char));
			sprintf(addr, "%s/%s", rootaddr, ptr -> d_name);
			stat(addr, &info);
			cout << addr << endl;
			if (S_ISDIR(info.st_mode)) {
				curdir = opendir(addr);
				doc_count = 0;
				while ((curptr = readdir(curdir)) != NULL) {
					if (curptr -> d_name[0] == '.') continue;
					char *curaddr = (char *)calloc(255, sizeof(char));
					sprintf(curaddr, "%s/%s", addr, curptr -> d_name);
					cout << curaddr << endl;
					read_file(curaddr, totalinput);
					Document doc;
					doc.Init();
					doc.ReadFile(tag_count, curaddr);
					Cluster.GetAllEmbedding(&doc);
					cout << doc.GetWordSize() << endl;
					Groups.push_back(doc);
					tot_doc ++;
					doc_count ++;
					if (doc_count >= MAX_DOC_PER_TAG) break;
				}
				tag_count ++;
			}
		}
	}

	static void *RunMethod(void *arg) {
		int i = query_id;
		int id = (intptr_t)arg;
		int num = (Groups.size() - QUERY_DOC) / MAX_THREADS;
		for (int j = 0; j < num; j++) {
			int k = QUERY_DOC + id * num + j;
			//real sum = RWMD(&Groups[i], &Groups[j]);
			real sum = DistCluster(i, k);
			//real sum = DistTFIDF(i, k);
			dis[k - QUERY_DOC] = DocCmp(k, sum);
		}
		pthread_exit(NULL);
	}


	void Run_Threads() {
		for (int j = 0; j < MAX_THREADS; j++) {
			pthread_create(&pt[j], NULL, RunMethod, (void *)j);
		}
		for (int j = 0; j < MAX_THREADS; j++)
			pthread_join(pt[j], NULL);
		sort(dis, dis + Groups.size() - QUERY_DOC);
	}


	void Run_WMD_Prefetch_And_Prune(int i) {
		//select nearest threshold_k documents
		for (int j = QUERY_DOC; j < Groups.size(); j++)
			dis[j - QUERY_DOC] = DocCmp(j, WCD(&Groups[i], &Groups[j]));
		sort(dis, dis + Groups.size() - QUERY_DOC);
		for (int j = 0; j < THRESHOLD_K; j++)
			dis[j] = DocCmp(dis[j].doc_id, WMD(&Groups[i], &Groups[dis[j].doc_id]));
		sort(dis, dis + THRESHOLD_K);
		int ToUpdate = 0;
		for (int j = THRESHOLD_K; j < Groups.size() - QUERY_DOC; j++) {
			real LowerBound = RWMD(&Groups[i], &Groups[dis[j].doc_id]);
			if (LowerBound <= dis[THRESHOLD_K - 1].distance) {
				ToUpdate ++;
				dis[j].distance = WMD(&Groups[i], &Groups[dis[j].doc_id]);
				if (dis[j].distance <= dis[THRESHOLD_K - 1].distance) {
					swap(dis[j], dis[THRESHOLD_K - 1]);
					sort(dis, dis + THRESHOLD_K);
				}
			}
		}
		printf("In total %d Documents to be updated WMD.\n", ToUpdate);
		sort(dis + THRESHOLD_K, dis + Groups.size() - QUERY_DOC);

	}


	void Run_WMD_Brute(int i) {
		for (int j = QUERY_DOC; j < Groups.size(); j++)
			dis[j - QUERY_DOC] = DocCmp(j, WMD(&Groups[i], &Groups[j]));
		sort(dis, dis + Groups.size() - QUERY_DOC);
	}

	real MAP(int doc_id) {
		int len = MAX_DOCS - QUERY_DOC;
		real result = 0;
		for (int i = 1; i < len; i++)
			if (Groups[dis[i].doc_id].Getmtag() == Groups[doc_id].Getmtag())
				result += 1 / (real)(i + 1);
		return result;
	}

};

int test_corpus::query_id = 0;

