#include <iostream>
#include <algorithm>
using namespace std;

int n;
int w[20];

int side[20];

int solve(int N, int w_left, int w_right)
{
	if ( N == 0 )
	{
		for(int i=0;i<n;i++)
			cout << side[i];
		cout << ":" << abs ( w_left- w_right ) << endl;
		return abs ( w_left- w_right );
	}
	
	int res;
	
	side[N-1] = 1;
	res = solve ( N-1, w_left + w[N-1], w_right );
	
	side[N-1] = 2;
	res = min(res, solve ( N-1, w_left, w_right + w[N-1] ) );
	
	return res;
}

int main()
{
	cin >> n;
	for(int i=0;i<n;i++)
		cin >> w[i];
		
	cout << solve( n, 0, 0 ) << endl;
	
	return 0;
}
