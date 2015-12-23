#include <cstdio>
#include <vector>
#include <cstring>

using namespace std;

typedef pair<u32string, int> word_count;

vector<word_count> document_processing(vector<u32string> doc) {
	int cnt[vocab_hash_size];
	vector<word_count> result;
	int i, j;
	int hash;
	int doc_len = (int)doc.size();
	memset(cnt, 0, sizeof cnt);
	result.clear();
	for (i = 0; i < doc_len; i++) {
		int cur_id = SearchVocab(doc[i]);
		if (cur_id != -1) cnt[cur_id] ++;
	}
	for (i = 0; i < vocab_hash_size; i++)
		if (cnt[i] > 0)
			result.push_back(make_pair(GetWord(i), cnt[i]));
	return result;
}