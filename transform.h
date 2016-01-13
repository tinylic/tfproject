real *Transform(const int &clcn, const vector<Upair> &doc){
		// Documents are represented in word_id
		unsigned len = doc.size();
		unsigned i;
		long long total = 0;
		int *ans = new int[clcn];
		real *result = new real[clcn];
		for (i = 0; i < clcn; i++)
			ans[i] = 0;
		for (i = 0; i < len; i++) {
			//cout << cl[doc[i].first] << endl;
			ans[mWordEmbedding ->mWordEmbeds[doc[i].first].cl] += doc[i].second;
			total += doc[i].second;
		}
		total ++;//Avoid total == 0
		for (i = 0; i < clcn; i++)
			result[i] = (real)ans[i] / total;
		return result;
	}
	real *Transform(const int &clcn, Document *Doc) {
		// Read documents in the form of <w1, c1>
		vector<Upair> All = Doc -> GetAllWord();
		vector<Upair> NewAll;
		NewAll.clear();
		for (int i = 0; i < (int)All.size(); i++) {
            Char *mWord = Doc -> mDict -> GetWord(All[i].first);
            //cout << mWord << endl;
            // Word in the document
            int index = mWordEmbedding -> SearchVocab(mWord);
            if (index == -1) continue;
            // Word ID in the Embedding
            NewAll.push_back(make_pair(index, All[i].second));
		}
		return Transform(clcn, NewAll);
	}
