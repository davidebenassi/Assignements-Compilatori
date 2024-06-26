void foo(int N, int a[5], int b[5], int c[5], int d[5]) {

	for (int i=0; i<N; i++)
	       a[i] = b[i]*c[i];

	for (int i=0; i<N; i++)
		d[i] = a[i]+c[i];

	for (int i=0; i<N; i++)
		a[i] = c[i]-1;
}
