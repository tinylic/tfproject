#include "cluster.h"

#define MAX_TAGS 20
const char rootaddr[] = "/home/tinylic/workspace/tfproject/20_newsgroups";
class test_20news {
private:
	map<string, int> Hash;
public:
	vector < Document* > Groups;
	int *belongs;
	real **embeds;
	cluster Cluster;
	void Run() {
		Groups.clear();
		Hash.clear();
		Cluster = cluster("vectors.bin", true);
		Cluster.Kmeans(max_w);
		DIR *dir, *curdir;
		int tag_count = 0;
		int doc_count = 0;
		int tot_doc = 0;
		struct dirent *ptr;
		struct dirent *curptr;
		struct stat info;
		dir = opendir(rootaddr);
		belongs = (int *)calloc(MAX_TAGS * MAX_DOC_PER_TAG, sizeof(int));
		embeds = (real **)calloc(MAX_TAGS * MAX_DOC_PER_TAG, sizeof(real *));
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
					//cout << curaddr << endl;
					Document doc;
					doc.Init();
					doc.ReadFile(tag_count, curaddr);
					Groups.push_back(&doc);
					int cur_id = tag_count * MAX_DOC_PER_TAG + doc_count;
					embeds[cur_id] = Cluster.Transform(&doc);
					//for (int i = 0; i < max_w; i++)
						//printf("%.6f ", embeds[cur_id][i]);
					//cout << endl;
					belongs[cur_id] = tot_doc ++;
					doc_count ++;
					if (doc_count >= MAX_DOC_PER_TAG) break;
				}
				tag_count ++;
			}
			cout << tot_doc << endl;
		}
	}
};
