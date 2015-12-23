#include <cstdio>
#include <algorithm>

using namespace std;

void transform(vector<int> &doc, int classes, double *result){
	// Documents are represented in word_id
	int len = (int)doc.size();
	int *ans = (int *)calloc(classes, sizeof(int));
	memset(ans, 0, sizeof ans);
	for (int i = 0; i < len; i++)
		ans[cl[doc[i]]] ++;
	for (int i = 0; i < classes; i++)
		result[i] = (double)ans[i] / len;
}