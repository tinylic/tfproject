#include "cluster.h"
#include "transform.h"
#include "wmd.h"
#include "ReadFile.h"

const char rootaddr[] = "/home/tinylic/workspace/tfproject/20_newsgroups";
vector < Document > Groups;

#include "Method1.h"
#include "BrownCluster.h"
#include "distance.h"
//#include "../brown/wcluster.h"


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

vector < DocCmp > dis[MAX_TAGS * MAX_DOC_PER_TAG];


void *RunMethod(void *arg) {
	pair<int, int> *temp = (pair<int, int> *)arg;
	int i = temp -> first;
	int id = temp -> second;
	int num = (Groups.size() - QUERY_DOC) / MAX_THREADS;
	for (int j = 0; j < num; j++) {
		int k = QUERY_DOC + id * num + j;
		real sum = DistCluster(i, k);
		dis[i].push_back(DocCmp(k, sum));
	}
	pthread_exit(NULL);
}

class test_20news {
public:
	FILE *fout = fopen("result.txt", "w");
	int *belongs;
	int *tags;
	cluster Cluster;
	real MAP(int doc_id) {
		int len = dis[doc_id].size();
		real result = 0;
		for (int i = 1; i < len; i++)
			if (Groups[dis[doc_id][i].doc_id].mtag == Groups[doc_id].mtag)
				result += 1 / (real)(i + 1);
		return result;
	}
	void Run() {
		Groups.clear();
		Cluster = cluster("vectors.bin", true);
		cout << "reading" << endl;
		DIR *dir, *curdir;
		int tag_count = 0;
		int doc_count = 0;
		int tot_doc = 0;
		struct dirent *ptr;
		struct dirent *curptr;
		struct stat info;
		dir = opendir(rootaddr);
		belongs = new int[MAX_TAGS * MAX_DOC_PER_TAG];

		tags = new int[MAX_TAGS * MAX_DOC_PER_TAG];
		while ((ptr = readdir(dir)) != NULL){
			if (ptr -> d_name[0] == '.') continue;
			char *addr = (char *)calloc(255, sizeof(char));
				sprintf(addr, "%s/%s", rootaddr, ptr -> d_name);
			stat(addr, &info);
			if (S_ISDIR(info.st_mode)) {
				curdir = opendir(addr);
				doc_count = 0;
				while ((curptr = readdir(curdir)) != NULL) {
					if (curptr -> d_name[0] == '.') continue;
					char *curaddr = (char *)calloc(255, sizeof(char));
					sprintf(curaddr, "%s/%s", addr, curptr -> d_name);
					cout << curaddr << endl;
					read_file(curaddr, browninput);
					Document doc;
					doc.Init();
					doc.ReadFile(tag_count, curaddr);
					Cluster.GetAllEmbedding(&doc);
					//cout << doc.AllWord.size() << endl;
					Groups.push_back(doc);
					tags[tot_doc] = tag_count;
					tot_doc ++;
					doc_count ++;
					if (doc_count >= MAX_DOC_PER_TAG) break;
				}
				tag_count ++;
			}
		}
		//random_shuffle(Groups.begin(), Groups.end());
		//for (int i = 0; i < tot_doc; i++)
			//printf("%d\n", Groups[i].AllEmbed.size());
		//RunMethodBrown(max_w);
		RunMethod1(&Cluster);
		pthread_t *pt = (pthread_t *)malloc(MAX_THREADS * sizeof(pthread_t));
		for (int i = 0; i < QUERY_DOC; i++) {
			dis[i].clear();
			for (int j = 0; j < MAX_THREADS; j++) {
				pair<int, int> arg = make_pair(i, j);
				pthread_create(&pt[j], NULL, RunMethod, (void *)&arg);
			}
			for (int j = 0; j < MAX_THREADS; j++)
				pthread_join(pt[j], NULL);
			//for (int j = QUERY_DOC; j < tot_doc; j++) {
				//real sum = DistCluster(i, j);
				//printf("%.6f\n", sum);
				//dis[i].push_back(DocCmp(j, sum));
			//}
			sort(dis[i].begin(), dis[i].end());
			//for (int j = 0; j < dis[i].size(); j++)
				//printf("%d ", dis[i][j].doc_id);
			//cout << endl;
			printf("MAP = %.6f\n", MAP(i));
		}
		//WMD(Groups[0], Groups[0]);
	}
};
