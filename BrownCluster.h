#include <stdlib.h>
#include <string.h>
char brownoutput[] = "output.txt";
void RunMethodBrown(const int &classes) {
	char word[255];
	int ID;
	//memset(opt2, 0, sizeof opt2);
	//sprintf(opt2, "/brown-cluster/./wcluster --text %s --c %d", browninput, classes);
	mWordEmbedding -> Init();
	FILE *infile = fopen(brownoutput, "r");
	while (1) {
		fscanf(infile, "%d%s", &ID, word);
		if (feof(infile)) break;
		mWordEmbedding -> AddCluster(word, ID);
	}
	int tot_doc = Groups.size();
	for (int i = 0; i < tot_doc; i++){
		Document Doc = Groups[i];
		//cout << i << " " << Doc.AllWord.size() << endl;
		embeds[i] = Transform(max_w, &Doc);
	}
}
