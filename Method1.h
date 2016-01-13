
real *embeds[MAX_TAGS * MAX_DOC_PER_TAG];
void RunMethod1(cluster *Cluster) {
	int tot_doc = Groups.size();
	Cluster -> Kmeans(max_w);
	for (int i = 0; i < tot_doc; i++){
		Document Doc = Groups[i];
		//cout << i << " " << Doc.AllWord.size() << endl;
		embeds[i] = Cluster -> Transform(&Doc);
	}
}
real DistMethod1(const int &i, const int &j) {
	real sum = 0.0;
	for (int k = 0; k < max_w; k++)
		sum += SQR(embeds[i][k] - embeds[j][k]);
	return sum;
}
