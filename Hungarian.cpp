#include <cstdio>
#include <algorithm>
#include <queue>
#include <string.h>
using namespace std;
#define N 3	// size of array.
#define INF 987654321	//INF value.

int cost[N][N];
int n, max_match;	//n : a number of people, max_match is a number of matching.
int lx[N], ly[N];	//label values
int xy[N], yx[N];	//xy is number of y which is matched from x. yx is same too.
bool S[N], T[N];	//S, T group
int slack[N], slackx[N];	//to reduce time complexity
int pre[N];	//final update

void init_labels()
{
	memset(lx, 0, sizeof(lx));	//init label
	memset(ly, 0, sizeof(ly));	//init label

	for (int x = 0; x < n; ++x)
		for (int y = 0; y < n; ++y)
			lx[x] = max(lx[x], cost[x][y]);	//first, setting the greatest value to label
	return;
}
void update_labels()
{
	int x, y, delta = INF;
	for (y = 0; y < n; ++y)
		if (!T[y])
			delta = min(delta, slack[y]);

	for (x = 0; x < n; ++x)
		if (S[x])	lx[x] -= delta;	//rule

	for (y = 0; y < n; ++y)
		if (T[y])	ly[y] += delta;	//rule

	for (y = 0; y < n; ++y)
		if (!T[y])	slack[y] -= delta;	//because we minus min of slacks 

	return;
}
void add_to_tree(int x, int prevx)
{	//(prevx, xy[x]) (xy[x], x) will be connected.
	S[x] = true;
	pre[x] = prevx;

	for (int y = 0; y < n; ++y) //update slacks, because we add new vertex to S
		if (lx[x] + ly[y] - cost[x][y] < slack[y])
		{
			slack[y] = lx[x] + ly[y] - cost[x][y];
			slackx[y] = x;
		}
	return;
}
void augment()
{
	if (max_match == n)	return;	//finished.

	int x, y, root;
	queue <int> q;

	memset(S, false, sizeof(S));	//init
	memset(T, false, sizeof(T));	//init
	memset(pre, -1, sizeof(pre));	//init	

	for (x = 0; x < n; ++x)
		if (xy[x] == -1)	//find any free vertex x in X
		{
			q.push(x);
			root = x;	//x will be root if alternating tree 
			pre[x] = -2;	//because there isn't exist previous vertex of root.
			S[x] = true;	// rule 
			break;
		}

	for (y = 0; y < n; ++y) {	//initializing slack array because S is changed.
		slack[y] = lx[root] + ly[y] - cost[root][y];	//the rest of labeling value.
		slackx[y] = root;	//slack[y] is calculated from root which is x in X.
	}

	while (true)
	{
		while (!q.empty()) {	//bfs loop
			x = q.front(); q.pop();

			for (y = 0; y < n; ++y)
				if (cost[x][y] == lx[x] + ly[y] && !T[y])	//find edge E_l 
				{
					if (yx[y] == -1)	break;	//find free vertex y = find augment path

					T[y] = true;
					q.push(yx[y]);
					add_to_tree(yx[y], x);	//next x, cur x
				}
			//if we couldn't find free vertex in this loop before y<n, there isn't exist  augment path.
			if (y < n)	break;	//augment path found
		}
		if (y < n)	break;	//augment path found

		//step2
		update_labels();	// N_l(S) = T , augment path not found, so improve labeling.

		q = queue<int>();	//clear

		//step 3
		for (y = 0; y < n; ++y)
			if (!T[y] && slack[y] == 0) {	//slack[y] == 0 is new E_l
				if (yx[y] == -1)	//free vertex y in Y. we find argument path
				{
					x = slackx[y];	//x is will be matched to y.
					break;
				}
				else
				{
					T[y] = true;
					if (!S[yx[y]]) //to expand alternating tree
					{
						q.push(yx[y]);
						add_to_tree(yx[y], slackx[y]);
					}
				}
			}
		if (y < n)	break;	//augment path found.
	}

	if (y < n)	//augmeht path found
	{
		max_match++;

		//x, y will be matched.
		//so , we update connection information until metting root.
		for (int cx = x, cy = y, ty; cx != -2; cx = pre[cx], cy = ty) {
			ty = xy[cx];
			yx[cy] = cx;
			xy[cx] = cy;
		}
		augment();	//augment경로를 1개 찾음.
	}
	return;
}

