#include "cluster.h"
#include "emd.c"
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
real _Cost(int *i, int *j) {
	return cost[*i][*j];
}
class test_20news {
private:
	map<string, int> Hash;
public:
	vector < Document* > Groups;
	vector <DocCmp> dis[MAX_TAGS * MAX_DOC_PER_TAG];
	FILE *fout = fopen("result.txt", "w");
	int *belongs;
	int *tags;
	real **embeds;
	cluster Cluster;
	real MAP(int doc_id) {
		int len = dis[doc_id].size();
		real result = 0;
		for (int i = 1; i < len; i++)
			if (tags[dis[doc_id][i].doc_id] == tags[doc_id])
				result += 1 / (real)(i + 1);
		return result;
	}
	real WordDistance(const Embeds &a, const Embeds &b) {
		real result = 0;
		for (int i = 0; i < layer1_size; i++)
			result += SQR(a[i] - b[i]);
		return sqrt(result);
	}
	real Nearest(const Embeds &a, const Document &doc) {
		real result = 1e9;;
		unsigned i;
		for (i = 0; i < doc.AllWord.size(); i++) {
			Char *mWord = doc.mDict -> GetWord(doc.AllWord[i].first);
			int index = Cluster.mWordEmbedding -> SearchVocab(mWord);
			Embeds vec = Cluster.mWordEmbedding -> GetEmbedding(index);
			real temp = WordDistance(a, vec);
			if (temp < result) result = temp;
		}
		return result;
	}
	Embeds GetDocCenter(const Document &a) {
		Embeds result;
		result = new real[layer1_size];
		int *cnt = new int[layer1_size];
		unsigned i, j;
		memset(cnt, 0, sizeof cnt);
		for (i = 0; i < layer1_size; i++)
			result[i] = cnt[i] = 0;
			for (i = 0; i < a.AllWord.size(); i++) {
				Char *mWord = a.mDict -> GetWord(a.AllWord[i].first);
	            int index = Cluster.mWordEmbedding -> SearchVocab(mWord);
	            Embeds vec = Cluster.mWordEmbedding -> GetEmbedding(index);
	            for (j = 0; j < layer1_size; j++) {
	            	result[j] += vec[j] * a.AllWord[i].second;
	            	cnt[j] += a.AllWord[i].second;
	            }
			}
		for (i = 0; i < layer1_size; i++)
			result[i] /= cnt[j];
		return result;
	}
	real WMD(const Document &a, const Document &b) {
		unsigned lena, lenb;
		lena = a.AllWord.size();
		lenb = b.AllWord.size();
		if (lena > MAX_DOC_LENGTH) lena = MAX_DOC_LENGTH;
		if (lenb > MAX_DOC_LENGTH) lenb = MAX_DOC_LENGTH;
		unsigned i, j;
		real *DA = new real[MAX_DOC_LENGTH];
		real *DB = new real[MAX_DOC_LENGTH];
		int *IDA = new int[MAX_DOC_LENGTH];
		int *IDB = new int[MAX_DOC_LENGTH];
		real suma = 0, sumb = 0;
		for (i = 0; i < lena; i++) {
			suma += a.AllWord[i].second;
			IDA[i] = i;
		}
		for (i = 0; i < lenb; i++) {
			sumb += b.AllWord[i].second;
			IDB[i] = i;
		}
		for (i = 0; i < lena; i++)
			DA[i] = (real)a.AllWord[i].second / suma;
		for (i = 0; i < lenb; i++)
			DB[i] = (real)b.AllWord[i].second / sumb;
		cost = new real *[MAX_DOC_LENGTH];
		for (i = 0; i < MAX_DOC_LENGTH; i++)
			cost[i] = new real[MAX_DOC_LENGTH];
		for (i = 0; i < lena; i++)
			for (j = 0; j < lenb; j++) {
				Char *mWord;
				int index;
				Embeds veca, vecb;
				mWord = a.mDict -> GetWord(a.AllWord[i].first);
				index = Cluster.mWordEmbedding -> SearchVocab(mWord);
				veca = Cluster.mWordEmbedding -> GetEmbedding(index);
				mWord = b.mDict -> GetWord(b.AllWord[j].first);
				index = Cluster.mWordEmbedding -> SearchVocab(mWord);
				vecb = Cluster.mWordEmbedding -> GetEmbedding(index);
				cost[i][j] = WordDistance(veca, vecb);
			}
		signature_t doca = signature_t{lena, IDA, DA};
		signature_t docb = signature_t{lenb, IDB, DB};
		real result = emd(&doca, &docb, _Cost, 0, 0);
		return result;
	}
	real WCD(const Document &a, const Document &b) {
		Embeds CentA, CentB;
		CentA = GetDocCenter(a);
		CentB = GetDocCenter(b);
		return WordDistance(CentA, CentB);
	}

	real RWMD(const Document &a, const Document &b) {
		real result = 1e9;
		real temp = 0;
		unsigned i;
		for (i = 0; i < a.AllWord.size(); i++) {
			Char *mWord = a.mDict -> GetWord(a.AllWord[i].first);
			int index = Cluster.mWordEmbedding -> SearchVocab(mWord);
			Embeds vec = Cluster.mWordEmbedding -> GetEmbedding(index);
			temp += a.AllWord[i].second * Nearest(vec, b);
		}
		if (result > temp) result = temp;
		temp = 0;
		for (i = 0; i < b.AllWord.size(); i++) {
			Char *mWord = b.mDict -> GetWord(b.AllWord[i].first);
			int index = Cluster.mWordEmbedding -> SearchVocab(mWord);
			Embeds vec = Cluster.mWordEmbedding -> GetEmbedding(index);
			temp += b.AllWord[i].second * Nearest(vec, a);
		}
		if (result > temp) result = temp;
		return result;
	}
	void Run() {
		Groups.clear();
		Hash.clear();
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
					if (curptr -> d_name[0] == '.') continue;
					char *curaddr = (char *)calloc(255, sizeof(char));
					sprintf(curaddr, "%s/%s", addr, curptr -> d_name);
					//out << curaddr << endl;
					Document doc;
					doc.Init();
					doc.ReadFile(tag_count, curaddr);
					Groups.push_back(&doc);
					embeds[tot_doc] = Cluster.Transform(max_w, &doc);
					for (int i = 0; i < max_w; i++)
						fprintf(fout, "%.6f ", embeds[tot_doc][i]);
					fprintf(fout, "\n");
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
				}
				sort(dis[i].begin(), dis[i].end());
				//for (int j = 0; j < dis[i].size(); j++)
					//printf("%d ", dis[i][j].doc_id);
				//cout << endl;
				printf("MAP = %.6f\n", MAP(i));
			}
		}
	}
};
