#include "cluster.h"

const char rootaddr[] = "/home/tinylic/workspace/tfproject/20_newsgroups";
class test_20news {
private:
	map<string, int> Hash;
public:
	vector < Document* > Groups;
	void Run() {
		Groups.clear();
		Hash.clear();
		DIR *dir, *curdir;
		int tag_count = 0;
		int doc_count = 0;
		int tot_doc = 0;
		struct dirent *ptr;
		struct dirent *curptr;
		struct stat info;
		dir = opendir(rootaddr);
		while ((ptr = readdir(dir)) != NULL){
			if (ptr -> d_name[0] == '.') continue;
			stat(ptr -> d_name, &info);
			if (S_ISDIR(info.st_mode)) {
				char *addr = (char *)calloc(255, sizeof(char));
				sprintf(addr, "%s/%s", rootaddr, ptr -> d_name);
				curdir = opendir(addr);
				tag_count ++;
				doc_count = 0;
				while ((curptr = readdir(curdir)) != NULL) {
					//printf("%s\n", curptr -> d_name);
					if (curptr -> d_name[0] == '.') continue;
					char *curaddr = (char *)calloc(255, sizeof(char));
					sprintf(curaddr, "%s/%s", addr, curptr -> d_name);
					cout << curaddr  << " " << ++ doc_count << endl;
					tot_doc ++;
					if (doc_count >= MAX_DOC_PER_TAG) break;
					Document doc;
					doc.Init();
					doc.ReadFile(tag_count, curaddr);
					Groups.push_back(&doc);
				}
			}
			cout << tot_doc << endl;
		}
	}
};
