
struct TF_IDF_node {
	Char *word;
	real TF_IDF;
	friend bool operator <(const TF_IDF_node &a, const TF_IDF_node &b) {
		return a.TF_IDF > b.TF_IDF;
	}
};

vector < TF_IDF_node > Twords;
real TF(Char *word, Document *doc) {
	int ID = doc -> SearchVocab(word);
	if (ID == -1) return 0;
	return ((real)doc -> GetWordCount(ID)) / doc -> GetTotalWord();
}

real IDF(char *word) {
	real result = Groups.size();
	//debug("%.6f\n", result);
	int cnt = 1;
	for (int i = 0; i < (int)Groups.size(); i++)
		if (Groups[i].SearchVocab(word) != -1)
			cnt ++;
	result /= cnt;
	result = log(result);
	return result;
}
Embeds TFIDF_Transform(Document *doc) {
	int *ans = new int[MAX_KEY_WORDS];
	Embeds result = new real[MAX_KEY_WORDS];
	int total = 1;
	for (int i = 0; i < MAX_KEY_WORDS; i++) {
		ans[i] = 0;
		Char *word = Twords[i].word;
		int index = doc -> SearchVocab(word);
		if (index == -1) continue;
		ans[i] = doc -> mWordCount[index];
		total += ans[i];
	}
	for (int i = 0; i < MAX_KEY_WORDS; i++)
		result[i] = (real)ans[i] / total;
	return result;
}
void Calc_TF_IDF() {
	Twords.clear();
	int total_doc = Groups.size();
	for (int i = 0; i < total_doc; i++) {
		Document *now = &Groups[i];
		int AllWordSize = now -> GetWordSize();
		for (int j = 0; j < AllWordSize; j++) {
			int CurId = now -> GetAllWordByID(j).first;
			Char *word = now -> GetWord(CurId);
			real tTF = TF(word, &Groups[i]);
			real tIDF = IDF(word);
			int k;
			for (k = 0; k < (int)Twords.size(); k++)
				if (strcmp(Twords[k].word, word) == 0)
					break;
			if (k == (int)Twords.size()) {
				Twords.push_back(TF_IDF_node{word, tTF * tIDF});
			}
			else  if (Twords[k].TF_IDF < (tTF * tIDF)){
				Twords[k].TF_IDF = tTF * tIDF;
			}
		}
	}
	sort(Twords.begin(), Twords.end());
	for (int i = 0; i < MAX_KEY_WORDS; i++)
		printf("%s %.6f\n", Twords[i].word, Twords[i].TF_IDF);
	for (int i = 0; i < total_doc; i++)
		embeds[i] = TFIDF_Transform(&Groups[i]);
}

real DistTFIDF(int i, int j) {
	real lena = 0, lenb = 0, sum = 0;
	for (int k = 0; k < MAX_KEY_WORDS; k++) {
		lena += SQR(embeds[i][k]);
		lenb += SQR(embeds[j][k]);
		sum += embeds[i][k] * embeds[j][k];
	}
	lena = sqrt(lena);
	lenb = sqrt(lenb);
	real result = sum / (lena * lenb);
	return acos(result);
}
