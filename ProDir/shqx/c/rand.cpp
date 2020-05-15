/* 
 * 此程序用于生成一组随机数, 
 * 若不允许重复, 则生成区域内所有非重复的随机数
 * 若允许重复, 则生成指定组数的随机数.
 */
#include"_public.h"

int main()
{
	CRand CrtRand;
	CrtRand.Rand(5, 11, false, 10);
	for(int ii=0;ii<CrtRand.m_val.size();ii++) { printf("%d\n",CrtRand.m_val[ii]); }

	return 0;
}
