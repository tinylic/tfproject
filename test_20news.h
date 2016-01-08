#include "cluster.h"

#define MAX_TAGS 20
const char rootaddr[] = "/home/tinylic/workspace/tfproject/20_newsgroups";
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
class test_20news {
private:
	map<string, int> Hash;
public:
	vector < Document* > Groups;
	vector <DocCmp> dis[MAX_TAGS * MAX_DOC_PER_TAG];
	int *belongs;
	int *tags;
	real **embeds;
	cluster Cluster;
	real MAP(int doc_id) {
		int len = dis[doc_id].size();
		real result = 0;
		for (int i = 0; i < len; i++)
			if (tags[dis[doc_id][i].doc_id] == tags[doc_id])
				result += 1 / (real)(i + 1);
		return result;
	}
	void Run() {
		Groups.clear();
		Hash.clear();
		Cluster = cluster("vectors.bin", true);
		cout << "reading" << endl;
		Cluster.Kmeans(max_w);
		cout << "Kmeans" << endl;
		vector<real *> cent = Cluster.GetCentroid();
		for (int i = 0; i < max_w; i++) {
			for (int j = 0; j < layer1_size; j++)
				printf("%.6f ", cent[i][j]);
			cout << endl;
		}
		DIR *dir, *curdir;
		int tag_count = 0;
		int doc_count = 0;
		int tot_doc = 0;
		struct dirent *ptr;
		struct dirent *curptr;
		struct stat info;
		dir = opendir(rootaddr);
		belongs = new int[MAX_TAGS * MAX_DOC_PER_TAG];
		embeds = new real *[MAX_TAGS * MAX_DOC_PER_TAG];
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
					//printf("%s\n", curptr -> d_name);
					if (curptr -> d_name[0] == '.') continue;
					char *curaddr = (char *)calloc(255, sizeof(char));
					sprintf(curaddr, "%s/%s", addr, curptr -> d_name);
					//out << curaddr << endl;
					Document doc;
					doc.Init();
					doc.ReadFile(tag_count, curaddr);
					Groups.push_back(&doc);
					embeds[tot_doc] = Cluster.Transform(&doc);
					for (int i = 0; i < max_w; i++)
						printf("%.6f ", embeds[tot_doc][i]);
					cout << endl;
					tags[tot_doc] = tag_count;
					tot_doc ++;
					doc_count ++;
					if (doc_count >= MAX_DOC_PER_TAG) break;
				}
				tag_count ++;
			}
			for (int i = 0; i < tot_doc; i++) {
				dis[i].clear();
				for (int j = 0; j < tot_doc; j++) {
					real sum = 0.0;
					for (int k = 0; k < max_w; k++)
						sum += SQR(embeds[i][k] - embeds[j][k]);
					dis[i].push_back(DocCmp(j, sum));
					//if (i == j) printf("fuck %d %.6f\n", i, sum);
				}
				sort(dis[i].begin(), dis[i].end());
				if (dis[i][0].doc_id != i) cout << "fuck " << i << endl;
				//printf("MAP = %.6f\n", MAP(i));
			}
			//cout << tot_doc << endl;
		}
	}
};
