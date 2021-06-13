/*********
  Luo Wei ,9/11/2007



  *************/




#ifndef ROUT_ING
#define ROUT_ING

#include "Allrouting.h"
#include "Message.h"
#include "Q2DTorusNode.h"
class Q2DTorus;

class Buffer;

class Routing:public Allrouting{

public:
	Routing(Q2DTorus* torus1){
		torus = torus1;
		k = torus1->getNode();
		next = new NodeInfo();
	}

int chkWrplnk(Q2DTorusNode* cur, Q2DTorusNode* dst);//check how many wrap links need from cur to dst.


NodeInfo*	forward(Message&);
NodeInfo*	forward(Q2DTorusNode* cur, Q2DTorusNode* dst);//return the nextnode of cur. 
NodeInfo*	noWrapLinkrt(Q2DTorusNode* cur, Q2DTorusNode* dst);//from cur to do not need wraplink.
NodeInfo*	oneWrapLinkrt(Q2DTorusNode* cur, Q2DTorusNode* dst);//from cur to  need  one wraplink. 
NodeInfo*	twoWrapLinkrt(Q2DTorusNode* cur, Q2DTorusNode* dst);//from cur to  need  two wraplink.


int		prefer(Buffer *buff1,Buffer *buff2, int xdis, int ydis, int& channel, Buffer*& buff);

};														   









#endif