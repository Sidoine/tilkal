#ifndef _SKILL_INC
#define _SKILL_INC

namespace Tilkal
{

class CSkill:public CMyArrayMember
{
public:
	int type,owner,val;
	int prev,next;
	void Remove();
	float Compute();
	void Test_Progress();
};

}
#endif
