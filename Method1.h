
real *embeds[MAX_TAGS * MAX_DOC_PER_TAG];
void RunMethod1(cluster *Cluster) {
	int tot_doc = Groups.size();
	cout << "Method1" << endl;
	Cluster -> Kmeans(max_w);
	for (int i = 0; i < tot_doc; i++){
		Document Doc = Groups[i];
		//cout << i << " " << Doc.AllWord.size() << endl;
		embeds[i] = Transform(max_w, &Doc);
	}
}

