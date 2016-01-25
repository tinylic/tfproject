#include "emd.c"

real _Cost(int *i, int *j) {
	return cost[*i][*j];
}
real Nearest(const Embeds &a, const Document &doc) {
	real result = 1e9;;
	unsigned i;
	for (i = 0; i < doc.AllWord.size(); i++) {
		Embeds vec = doc.AllEmbed[i];
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
	//cout << a.AllWord.size() << endl;
	for (i = 0; i < layer1_size; i++)
		result[i] = cnt[i] = 0;
	for (i = 0; i < a.AllWord.size(); i++) {
		Embeds vec = a.AllEmbed[i];
		if (vec == NULL) continue;
		for (j = 0; j < layer1_size; j++) {
			//printf("%.6f ", vec[j]);
			result[j] += vec[j] * a.AllWord[i].second;
			cnt[j] += a.AllWord[i].second;
		}
		//cout << endl;
	}
	for (i = 0; i < layer1_size; i++)
		result[i] /= max(cnt[i], 1);
	return result;
}
real WMD(const Document &a, const Document &b) {
	unsigned lena, lenb;
	lena = a.AllWord.size();
	lenb = b.AllWord.size();
	unsigned i, j;
	real *DA = new real[lena];
	real *DB = new real[lenb];
	int *IDA = new int[lena];
	int *IDB = new int[lenb];
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
	cost = new real *[lena];
	for (i = 0; i < lena; i++)
		cost[i] = new real[lenb];
	for (i = 0; i < lena; i++) {
		for (j = 0; j < lenb; j++) {
			Embeds veca, vecb;
			veca = a.AllEmbed[i];
			vecb = b.AllEmbed[j];
			cost[i][j] = WordDistance(veca, vecb);
		}
	}
	signature_t doca = signature_t{lena, IDA, DA};
	signature_t docb = signature_t{lenb, IDB, DB};
	real result = emd(&doca, &docb, _Cost, 0, 0);
	if (!isfinite(result)) result = 1e9;
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
		Embeds vec = a.AllEmbed[i];
		if (vec == NULL) continue;
		temp += a.AllWord[i].second * Nearest(vec, b);
	}
	if (result > temp) result = temp;
	temp = 0;
	for (i = 0; i < b.AllWord.size(); i++) {
		Embeds vec = b.AllEmbed[i];
		if (vec == NULL) continue;
		temp += b.AllWord[i].second * Nearest(vec, a);
	}
	if (result > temp) result = temp;
	return result;
}
