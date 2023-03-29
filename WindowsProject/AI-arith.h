#include "resource.h"
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