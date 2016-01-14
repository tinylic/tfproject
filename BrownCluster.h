#include <stdlib.h>
#include <string.h>
char cluster_out[] = "";
void RunMethodBrown(const int &classes) {
	char opt1[255], opt2[255];
	memset(opt2, 0, sizeof opt2);
	sprintf(opt2, "/brown-cluster/./wcluster --text %s --c %d", browninput, classes);
	mWordEmbedding->Init();
}
