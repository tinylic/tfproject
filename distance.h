real DistCluster(const int &i, const int &j) {
	real sum = 0.0;
	for (int k = 0; k < max_w; k++)
		sum += SQR(embeds[i][k] - embeds[j][k]);
	return sum;
}
