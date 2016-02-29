#include "emd.c"


real Nearest(const Embeds &a, Document *doc) {
	real result = 1e9;;
	unsigned i, len = doc -> GetEmbedSize();
	for (i = 0; i < len; i++) {
		Embeds vec = doc -> GetEmbed(i);
		real temp = WordDistance(a, vec);
		if (temp < result) result = temp;
	}
	return result;
}
Embeds GetDocCenter(Document *a) {
	Embeds result;
	result = new real[layer1_size];
	int *cnt = new int[layer1_size];
	unsigned i, j;
	for (i = 0; i < layer1_size; i++)
		result[i] = cnt[i] = 0;
	for (i = 0; i < a -> GetEmbedSize(); i++) {
		Embeds vec = a -> GetEmbed(i);
		if (vec == NULL) continue;
		for (j = 0; j < layer1_size; j++) {
			result[j] += vec[j] * (a -> GetAllWordByID(i)).second;
			cnt[j] += a -> GetAllWordByID(i).second;
		}
	}
	for (i = 0; i < layer1_size; i++)
		result[i] /= max(cnt[i], 1);
	return result;
}
real WMD(Document *a, Document *b) {
	unsigned lena, lenb;
	lena = a -> GetEmbedSize();
	lenb = b -> GetEmbedSize();
	if (lena > MAX_DOC_LENGTH) lena = MAX_DOC_LENGTH;
	if (lenb > MAX_DOC_LENGTH) lenb = MAX_DOC_LENGTH;
	unsigned i, j;
	real *DA = new real[lena];
	real *DB = new real[lenb];
	real suma = 0, sumb = 0;
	for (i = 0; i < lena; i++)
		suma += a -> GetAllWordByID(i).second;
	for (i = 0; i < lenb; i++)
		sumb += b -> GetAllWordByID(i).second;
	for (i = 0; i < lena; i++)
		DA[i] = (real)a -> GetAllWordByID(i).second / suma;
	for (i = 0; i < lenb; i++)
		DB[i] = (real)b -> GetAllWordByID(i).second / sumb;
	cost = new real *[lena];
	for (i = 0; i < lena; i++)
		cost[i] = new real[lenb];
	for (i = 0; i < lena; i++) {
		for (j = 0; j < lenb; j++) {
			Embeds veca, vecb;
			veca = a -> GetEmbed(i);
			vecb = b -> GetEmbed(j);
			cost[i][j] = WordDistance(veca, vecb);
		}
	}
	signature_t doca = signature_t{lena, DA};
	signature_t docb = signature_t{lenb, DB};
	real result = emd(&doca, &docb, cost, 0, 0);
	if (!isfinite(result)) result = 1e9;
	return result;
}
real WCD(Document *a, Document *b) {
	Embeds CentA, CentB;
	CentA = GetDocCenter(a);
	CentB = GetDocCenter(b);
	return WordDistance(CentA, CentB);
}
real RWMD(Document *a, Document *b) {
	real result = 0;
	real temp = 0;
	unsigned i;
	unsigned lena = a -> GetEmbedSize();
	unsigned lenb = b -> GetEmbedSize();
	real *DA = new real[lena];
	real *DB = new real[lenb];
	real suma = 0, sumb = 0;
	for (i = 0; i < lena; i++)
		suma += a -> GetAllWordByID(i).second;
	for (i = 0; i < lenb; i++)
		sumb += b -> GetAllWordByID(i).second;
	for (i = 0; i < lena; i++)
		DA[i] = (real)(a -> GetAllWordByID(i).second) / suma;
	for (i = 0; i < lenb; i++)
		DB[i] = (real)(b -> GetAllWordByID(i).second) / sumb;

	for (i = 0; i < lena; i++) {
		Embeds vec = a -> GetEmbed(i);
		if (vec == NULL) continue;
		temp += DA[i] * Nearest(vec, b);
	}
	if (result < temp) result = temp;

	temp = 0;
	for (i = 0; i < lenb; i++) {
		Embeds vec = b -> GetEmbed(i);
		if (vec == NULL) continue;
		temp += DB[i] * Nearest(vec, a);
	}
	if (result < temp) result = temp;
	return result;
}



