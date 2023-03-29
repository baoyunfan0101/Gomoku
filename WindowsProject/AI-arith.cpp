#include <vector>
#include <algorithm>
#include <limits>
#include <windows.h>
using namespace std;

typedef class state {	//定义一个状态
public:
	/* 该状态的基本信息 */
	int CB[15][15] = { 0 };	//ChessBoard：AI（MAX）1，对手（MIN）-1，空0
	int Last_i = -1;	//上一步横坐标
	int Last_j = -1;	//上一步纵坐标
	int eva = INT_MIN;	//用于存储其评价函数的值
	/* 依据CB[Last_i][Last_j]可以判断上一步行棋方是黑棋/白棋，即MAX/MIN */

	/* 搜索树的建立 */
	state* father;	//父结点
	vector<state*> child;	//子结点（用容器存储）

	/* α-β剪枝相关 */
	int alpha = INT_MIN;
	int beta = INT_MAX;

	/* state类的成员函数 */
	int F();	//评价函数
	int GoalTest();	//目标测试函数：AI（MAX）胜返回1，对手（MIN）胜返回-1，其余返回0
	state* minimax(int depth);	//极小极大值搜索：返回下一步行棋后的状态

	void clear();	//重新初始化其上一次搜索的临时数据
}state;

int f(int a[], int n, int P)	//对于一行/列/斜线的静态估值（n>=6为该行/列/斜线长度，P为棋子种类）
{
	int res = 0;
	for (int i = 0; i <= n - 7; ++i)
		if (a[i] == -P && a[i + 1] == 0 && a[i + 2] == P && a[i + 3] == P && a[i + 4] == P && a[i + 5] == 0 && a[i + 6] == -P)	//眠三
			res += 10;

	for (int i = 0; i <= n - 6; ++i)
		if (a[i] == -P) {
			if (a[i + 1] == P) {
				if (a[i + 2] == P) {
					if (a[i + 3] == P) {
						if (a[i + 4] == P) {
							if (a[i + 5] == 0)	//眠四
								res += 100;
						}
						else if (a[i + 4] == 0) {
							if (a[i + 5] == 0)	//眠三
								res += 10;
						}
					}
					else if (a[i + 3] == 0) {
						if (a[i + 4] == P) {
							if (a[i + 5] == 0)	//眠三
								res += 10;
						}
						else if (a[i + 4] == 0) {
							if (a[i + 5] == 0)	//眠二
								res += 1;
						}
					}
				}
				else if (a[i + 2] == 0) {
					if (a[i + 3] == P) {
						if (a[i + 4] == P) {
							if (a[i + 5] == 0)	//眠三
								res += 10;
						}
						else if (a[i + 4] == 0) {
							if (a[i + 5] == 0)	//眠二
								res += 1;
						}
					}
					else if (a[i + 3] == 0) {
						if (a[i + 4] == P) {
							if (a[i + 5] == 0)	//眠二
								res += 1;
						}
					}
				}
			}
		}
		else if (a[i] == 0) {
			if (a[i + 1] == P) {
				if (a[i + 2] == P) {
					if (a[i + 3] == P) {
						if (a[i + 4] == P) {
							if (a[i + 5] == -P)	//眠四
								res += 100;
							else if (a[i + 5] == 0)	//活四
								res += 1000;
						}
						else if (a[i + 4] == 0) {
							if (a[i + 5] == 0)	//活三
								res += 100;
						}
					}
					else if (a[i + 3] == 0) {
						if (a[i + 4] == P) {
							if (a[i + 5] == -P)	//眠三
								res += 10;
							else if (a[i + 5] == 0)	//活三
								res += 100;
						}
					}
				}
				else if (a[i + 2] == 0) {
					if (a[i + 3] == P) {
						if (a[i + 4] == P) {
							if (a[i + 5] == -P)	//眠三
								res += 10;
							else if (a[i + 5] == 0)	//活三
								res += 100;
						}
					}
					else if (a[i + 3] == 0) {
						if (a[i + 4] == P) {
							if (a[i + 5] == -P)	//眠二
								res += 1;
							else if (a[i + 5] == 0)	//活二
								res += 10;
						}
					}
				}
			}
			else if (a[i + 1] == 0) {
				if (a[i + 2] == P) {
					if (a[i + 3] == P) {
						if (a[i + 4] == P) {
							if (a[i + 5] == -P)	//眠三
								res += 10;
							else if (a[i + 5] == 0)	//活三
								res += 100;
						}
					}
					else if (a[i + 3] == 0) {
						if (a[i + 4] == P) {
							if (a[i + 5] == -P)	//眠二
								res += 1;
						}
					}
				}
				else if (a[i + 2] == 0) {
					if (a[i + 3] == P) {
						if (a[i + 4] == P) {
							if (a[i + 5] == -P)	//眠二
								res += 1;
						}
					}
				}
			}
		}

	for (int i = 0; i <= n - 5; ++i)
		if (a[i] == P) {
			if (a[i + 1] == P) {
				if (a[i + 2] == P) {
					if (a[i + 3] == P) {
						if (a[i + 4] == P)	//连五
							res += 10000;
					}
					else if (a[i + 3] == 0) {
						if (a[i + 4] == P)	//眠四
							res += 100;
					}
				}
				else if (a[i + 2] == 0) {
					if (a[i + 3] == P) {
						if (a[i + 4] == P)	//眠四
							res += 100;
					}
					else if (a[i + 3] == 0) {
						if (a[i + 4] == P)	//眠三
							res += 10;
					}
				}
			}
			else if (a[i + 1] == 0) {
				if (a[i + 2] == P) {
					if (a[i + 3] == P) {
						if (a[i + 4] == P)	//眠四
							res += 100;
					}
					else if (a[i + 3] == 0) {
						if (a[i + 4] == P)	//眠三
							res += 10;
					}
				}
				else if (a[i + 2] == 0) {
					if (a[i + 3] == P) {
						if (a[i + 4] == P)	//眠三
							res += 10;
					}
					else if (a[i + 3] == 0) {
						if (a[i + 4] == P)	//眠二
							res += 1;
					}
				}
			}
		}
		else if (a[i] == 0) {
			if (a[i + 1] == P) {
				if (a[i + 2] == P) {
					if (a[i + 3] == 0) {
						if (a[i + 4] == 0)	//活二
							res += 10;
					}
				}
				else if (a[i + 2] == 0) {
					if (a[i + 3] == P) {
						if (a[i + 4] == 0)	//活二
							res += 10;
					}
				}
			}
			else if (a[i + 1] == 0) {
				if (a[i + 2] == P) {
					if (a[i + 3] == P) {
						if (a[i + 4] == 0)	//活二
							res += 10;
					}
				}
			}
		}

	return res;
}

int state::F()
{
	int res = 0;
	for (int i = 0; i < 15; ++i) {	//"-"方向
		int a[17];
		for (int j = 0; j < 15; ++j)
			a[j + 1] = CB[i][j];
		a[0] = a[16] = -1;	//将边界当作对手棋子处理
		res += f(a, 17, 1);
		a[0] = a[16] = 1;
		res -= f(a, 17, -1);
	}

	for (int j = 0; j < 15; ++j) {	//"|"方向
		int a[17];
		for (int i = 0; i < 15; ++i)
			a[i + 1] = CB[i][j];
		a[0] = a[16] = -1;	//将边界当作对手棋子处理
		res += f(a, 17, 1);
		a[0] = a[16] = 1;
		res -= f(a, 17, -1);
	}

	for (int i = 4; i < 15; ++i) {	//"/"方向
		int a[17];
		for (int j = 0; j <= i; ++j)
			a[j + 1] = CB[i - j][j];
		a[0] = a[i + 2] = -1;	//将边界当作对手棋子处理
		res += f(a, i + 3, 1);
		a[0] = a[i + 2] = 1;
		res -= f(a, i + 3, -1);

		if (i == 14)
			break;
		for (int j = 0; j <= i; ++j)
			a[j + 1] = CB[14 - j][14 - i + j];
		a[0] = a[i + 2] = -1;	//将边界当作对手棋子处理
		res += f(a, i + 3, 1);
		a[0] = a[i + 2] = 1;
		res -= f(a, i + 3, -1);
	}

	for (int i = 0; i < 11; ++i) {	//"\"方向
		int a[17];
		for (int j = 0; j <= 14 - i; ++j)
			a[j + 1] = CB[i + j][j];
		a[0] = a[16 - i] = -1;	//将边界当作对手棋子处理
		res += f(a, 17 - i, 1);
		a[0] = a[16 - i] = 1;
		res -= f(a, 17 - i, -1);

		if (i == 0)
			continue;
		for (int j = 0; j <= 14 - i; ++j)
			a[j + 1] = CB[j][i + j];
		a[0] = a[16 - i] = -1;	//将边界当作对手棋子处理
		res += f(a, 17 - i, 1);
		a[0] = a[16 - i] = 1;
		res -= f(a, 17 - i, -1);
	}

	return res;
}

int state::GoalTest()
{
	int judge = 0;
	for (int i = 0; i < 15; ++i)	//"-"方向
		for (int j = 0; j < 11; ++j) {
			judge = 0;
			for (int k = 0; k < 5; ++k)
				judge += this->CB[i][j + k];
			if (judge == 5)
				return 1;
			else if (judge == -5)
				return -1;
		}

	for (int i = 0; i < 11; ++i)	//"|"方向
		for (int j = 0; j < 15; ++j) {
			judge = 0;
			for (int k = 0; k < 5; ++k)
				judge += this->CB[i + k][j];
			if (judge == 5)
				return 1;
			else if (judge == -5)
				return -1;
		}

	for (int i = 4; i < 15; ++i)	//"/"方向
		for (int j = 0; j < 11; ++j) {
			judge = 0;
			for (int k = 0; k < 5; ++k)
				judge += this->CB[i - k][j + k];
			if (judge == 5)
				return 1;
			else if (judge == -5)
				return -1;
		}

	for (int i = 0; i < 11; ++i)	//"\"方向
		for (int j = 0; j < 11; ++j) {
			judge = 0;
			for (int k = 0; k < 5; ++k)
				judge += this->CB[i + k][j + k];
			if (judge == 5)
				return 1;
			else if (judge == -5)
				return -1;
		}

	return 0;
}

bool GT(state* a, state* b)	//定义compare函数：大于
{
	return a->eva > b->eva;
}

bool LT(state* a, state* b)	//定义compare函数：小于
{
	return a->eva < b->eva;
}

state* state::minimax(int depth)	//调用一次，递归两层
{
	/* AI（MAX）行棋 */
	for (int i = 0; i < 15; ++i)
		for (int j = 0; j < 15; ++j)
			if (this->CB[i][j] == 0) {	//当前该位置为空，可以落子
				int t = 0;	//t为(i, j)周围的棋子数
				for (int m = ((i - 1 > 0) ? i - 1 : 0); m <= ((i + 1 < 14) ? i + 1 : 14); ++m)
					for (int n = ((j - 1 > 0) ? j - 1 : 0); n <= ((j + 1 < 14) ? j + 1 : 14); ++n)
						if (this->CB[m][n] != 0)
							++t;
				if (t == 0)	//若(i, j)周围3*3的范围内都没有棋子，则直接舍弃
					continue;
				/* 一般情况下不会在远离已存在的棋子处行棋，而仅考虑已存在的棋子周围3*3的范围可以大大减少子结点数（尤其是在前几步） */
				state* p = new state;
				for (int i = 0; i < 15; ++i)
					for (int j = 0; j < 15; ++j)
						p->CB[i][j] = this->CB[i][j];
				p->CB[i][j] = 1;
				p->Last_i = i;
				p->Last_j = j;

				if (p->GoalTest() == 1)	//若已经胜利，则无需再搜索
					return p;

				p->eva = p->F();	//计算该子结点评价函数的值
				p->father = this;
				this->child.push_back(p);
			}

	/* 对手（MIN）行棋 */
	sort(this->child.begin(), this->child.end(), GT);	//对所有子结点进行排序，以优化剪枝
	int temp = -1;
	for (unsigned int k = 0; k < this->child.size(); ++k) {	//遍历所有子结点
		bool judge = false;
		for (int i = 0; i < 15; ++i) {
			for (int j = 0; j < 15; ++j) {
				if (this->child[k]->CB[i][j] == 0) {	//当前该位置为空，可以落子
					int t = 0;	//t为(i, j)周围的棋子数
					for (int m = ((i - 1 > 0) ? i - 1 : 0); m <= ((i + 1 < 14) ? i + 1 : 14); ++m)
						for (int n = ((j - 1 > 0) ? j - 1 : 0); n <= ((j + 1 < 14) ? j + 1 : 14); ++n)
							if (this->child[k]->CB[m][n] != 0)
								++t;
					if (t == 0)	//若(i, j)周围3*3的范围内都没有棋子，则直接减枝
						continue;
					state* p = new state;
					for (int i = 0; i < 15; ++i)
						for (int j = 0; j < 15; ++j)
							p->CB[i][j] = this->child[k]->CB[i][j];
					p->CB[i][j] = -1;

					if (p->GoalTest() == -1 && k + 1 != this->child.size()) {	//若已经失败且不是最后一个子结点，则直接舍弃
						judge = true;
						break;
					}

					p->Last_i = i;
					p->Last_j = j;
					p->father = this->child[k];
					if (depth > 2) {
						state* NewState = p->minimax(depth - 2);	//递归调用
						p->eva = p->beta;
					}
					else
						p->eva = p->F();	//计算该子结点评价函数的值
					if (p->eva < this->child[k]->beta) {
						this->child[k]->beta = p->eva;
						if (this->child[k]->beta < this->alpha) {
							judge = true;
							break;
						}
					}
				}
			}
			if (judge)
				break;
		}
		if (judge)
			continue;
		this->child[k]->alpha = this->child[k]->beta;
		if (this->child[k]->alpha > this->alpha) {
			this->alpha = this->child[k]->alpha;
			temp = k;
		}
	}
	this->beta = this->alpha;

	return this->child[temp];
}

void state::clear()
{
	this->alpha = INT_MIN;	//清除上一次搜索的临时数据
	this->beta = INT_MAX;
	for (unsigned int k = 0; k < this->child.size(); ++k)	//释放内存
		delete this->child[k];
	this->child.clear();	//清空其子结点（其中存在上一次MIN搜索的子结点）
}