#include "Routing.h"
#include "assert.h"
/*******

  if ALGORITHM == 1, west-first 
  if ALGORITHM == 2, dor-routing


  ***********/
extern int ALGORITHM;

NodeInfo*	Routing::forward(Message& s){

	return forward((*torus)[s.routpath[0].node], (*torus)[s.dst]);
}

int Routing::chkWrplnk(Q2DTorusNode* cur, Q2DTorusNode* dst){ 
	int curx = cur->x;
	int cury = cur->y;

	
	int dstx = dst->x;
	int dsty = dst->y;


	int wraplink = 0;

  if(((curx < dstx) && (dstx - curx) > (k/2)) || ((curx > dstx) && (curx - dstx) > (k/2)))
	  wraplink ++;

  if(((cury < dsty) && (dsty - cury) > (k/2)) || ((cury > dsty) && (cury - dsty) > (k/2)))
	  wraplink ++;

 

  return wraplink;
}


NodeInfo*	Routing::forward(Q2DTorusNode* cur, Q2DTorusNode* dst){
	assert(cur && dst && (cur != dst));
	
	next->node = -1;
	next->buff = NULL;

	//int wraplink = chkWrplnk( cur,  dst);
	int wraplink = 0;//实现的是mesh网络，所以没有环链路，wraplink为0

	if (wraplink == 0)  return noWrapLinkrt( cur,  dst);

	if (wraplink == 1)  return oneWrapLinkrt(cur, dst);

	if (wraplink == 2)  return twoWrapLinkrt( cur, dst);




}


/**********

		 check whether buff1 has enough buffer,
		 n	is the buffer number , if buff1 used, then record it:
		 chn is R1 or R2 used,and record is the buffer(such as bufferxneg)
		 
		 ***********/






int		Routing::prefer(Buffer* buff1, Buffer* buff2, int xdis, int ydis, int& chn, Buffer*& record) {
	int bufferslc = 0;
	int var;
	bool k = false;
	bool xflag =( xdis == 0 || buff1->linkused);
	bool yflag =( ydis == 0 || buff2 == NULL || buff2->linkused);
	if (xflag == true && yflag == true) {
		bufferslc = 0;
	}
	else if (xflag == true && yflag == false) {
		if (buff2->r1 >= MESSLENGTH || buff2->r2 >= MESSLENGTH) {
			if (buff2->r1 >= MESSLENGTH) chn = R1; else chn = R2;
			if (chn == R1 || xdis == 0) {
				record = buff2;
				buff2->bufferMin(chn, MESSLENGTH);
				bufferslc = 2;
				buff2->linkused = true;
			}
		}
	}
	else if (xflag == false && yflag == true) {
		if (buff1->r1 >= MESSLENGTH || buff1->r2 >= MESSLENGTH) {
			if (buff1->r1 >= MESSLENGTH) chn = R1; else chn = R2;
			record = buff1;
			buff1->bufferMin(chn, MESSLENGTH);
			bufferslc = 1;
			buff1->linkused = true;
		}
	}
	else if (xflag == false && yflag == false) {
		if (buff1->r1 >= MESSLENGTH || buff2->r1 >= MESSLENGTH || buff1->r2 >= MESSLENGTH || buff2->r2 >= MESSLENGTH) {
			if (buff1->r1 >= MESSLENGTH || buff2->r1 >= MESSLENGTH) chn = R1; else chn = R2;
			if (buff1->r1 >= MESSLENGTH || (buff1->r2 >= MESSLENGTH && chn == R2)) {
				record = buff1;
				buff1->bufferMin(chn, MESSLENGTH);
				bufferslc = 1;
				buff1->linkused = true;
			}
			else {
				if (chn == R1 || xdis == 0) {
					record = buff2;
					buff2->bufferMin(chn, MESSLENGTH);
					bufferslc = 2;
					buff2->linkused = true;
				}
			}
		}
	}
	return bufferslc;
}



NodeInfo*	Routing::noWrapLinkrt(Q2DTorusNode* cur, Q2DTorusNode* dst){  //routing without wraplink.
	assert( cur && dst && (cur != dst));
	int curx = cur->x;
	int cury = cur->y;

   	//R1 preferred, leave more space for escape channel R2.
	int dstx = dst->x;
	int dsty = dst->y;

	int xdis = dstx - curx;//x方向的偏移
	int ydis = dsty - cury;//y方向的偏移

    int bufferslc ; // 0 no buffer available, 1: select  x direction buffer,2 y direction, 3 z direction

	Buffer* xlink[3] = {cur->bufferxneglink, NULL, cur->bufferxposlink};
	Buffer* ylink[3] = {cur->bufferyneglink, NULL, cur->bufferyposlink};

	
	int xlinknode[3] = {cur->linkxneg, -1, cur->linkxpos};
	int ylinknode[3] = {cur->linkyneg, -1, cur->linkypos};

	int virtualch[3] = {R2, 0, R1};

/**********************************************


	ALGORITHM == 2   dor ROUTING 路由算法的实现，这里实现的xy

  *****************************************************/


	if(ALGORITHM == 2){
		/*if( var1 != 1) {
			vchx = R2;
			vchy = R1;
		
		}
		else{
			vchy = R2;
		}*/

		bufferslc =	prefer(xlink[var1], ylink[var2], xdis, ydis, next->channel, next->buff);
				switch(bufferslc){
					case 0:
					next->node = -1;
						break;
					case 1:
						next->node = xlinknode[var1];
						break;
					case 2:
						next->node = ylinknode[var2];
						break;
				
					}
					return next;
		
	}
}



NodeInfo* Routing::oneWrapLinkrt(Q2DTorusNode* cur, Q2DTorusNode* dst){  //routing one wraplink. 
	assert( cur && dst && (cur != dst));
	int curx = cur->x;
	int cury = cur->y;

	//R1 preferred, leave more space for escape channel R2.
	int dstx = dst->x;
	int dsty = dst->y;

	int xdis = dstx - curx;
	int ydis = dsty - cury;

	
	
    int bufferslc ; // 0 no buffer available, 1: select  x direction buffer,2 y direction, 3 z direction
	int var1;
	int var2;

	

	int vchx = R1;   // virtual channel of x axis
	int vchy = R1;


	if( xdis < 0) var1 = 0;
	else { 
		if( xdis == 0) var1 = 1;
		else if( xdis > 0) var1 = 2;
	}

	if( ydis < 0) var2 = 0;
	else { 
		if( ydis == 0) var2 = 1;
		else if( ydis > 0) var2 = 2;
	}

	

	Buffer* xlink[3] = {cur->bufferxneglink, NULL, cur->bufferxposlink};
	Buffer* ylink[3] = {cur->bufferyneglink, NULL, cur->bufferyposlink};

	
	int xlinknode[3] = {cur->linkxneg, -1, cur->linkxpos};
	int ylinknode[3] = {cur->linkyneg, -1, cur->linkypos};
	

	if(xdis > k/2  || xdis < -k/2){
		Buffer* temp = xlink[0];
		xlink[0] = xlink[2];
		xlink[2] = temp;

		int temp2 = xlinknode[0];
		xlinknode[0] = xlinknode[2];
		xlinknode[2] = temp2;
		if( xdis > k/2 && cur->x == 0) vchx = R2;
		if( xdis < -k/2 && cur->x == k-1) vchx = R2;
	}

	
	if(ydis > k/2  || ydis < -k/2){
		 Buffer* temp = ylink[0];
		ylink[0] = ylink[2];
		ylink[2] = temp;

		int temp2 = ylinknode[0];
		ylinknode[0] = ylinknode[2];
		ylinknode[2] = temp2;
		if( ydis > k/2 && cur->y == 0) vchy = R2;
		if( ydis < -k/2 && cur->y == k-1) vchy = R2;
	}

	
	

	bufferslc = prefer(xlink[var1], ylink[var2], xdis, ydis, next->channel, next->buff);
		//bufferslc =	prefer(xlink[var1], ylink[var2], vchx, vchy,  next->buff);
				switch(bufferslc){
					case 0:
					next->node = -1;
						break;
					case 1:
						next->node = xlinknode[var1];
						next->channel = vchx;
						break;
					case 2:
						next->node = ylinknode[var2];
						next->channel = vchy;
						break;
				
					}
	return next;
	}
    


		

NodeInfo*	Routing::twoWrapLinkrt(Q2DTorusNode* cur, Q2DTorusNode* dst){
			assert( cur && dst && (cur != dst));
	int curx = cur->x;
	int cury = cur->y;
	
	//R1 preferred, leave more space for escape channel R2.
	int dstx = dst->x;
	int dsty = dst->y;

	int xdis = dstx - curx;
	int ydis = dsty - cury;


	
	
    int bufferslc ; // 0 no buffer available, 1: select  x direction buffer,2 y direction, 3  direction
	int var1;
	int var2;
	
	

	int vchx = R1;   // virtual channel of x axis
	int vchy = R1;


	if( xdis < 0) var1 = 0;
	else { 
		if( xdis == 0) var1 = 1;
		else if( xdis > 0) var1 = 2;
	}

	if( ydis < 0) var2 = 0;
	else { 
		if( ydis == 0) var2 = 1;
		else if( ydis > 0) var2 = 2;
	}



	Buffer* xlink[3] = {cur->bufferxposlink, NULL, cur->bufferxneglink};
	Buffer* ylink[3] = {cur->bufferyposlink, NULL, cur->bufferyneglink};

	
	int xlinknode[3] = {cur->linkxpos, -1, cur->linkxneg};
	int ylinknode[3] = {cur->linkypos, -1, cur->linkyneg};



	bufferslc = prefer(xlink[var1], ylink[var2], xdis, ydis, next->channel, next->buff);
		//bufferslc =	prefer(xlink[var1], ylink[var2], vchx, vchy, next->buff);
				switch(bufferslc){
					case 0:
					next->node = -1;
						break;
					case 1:
						next->node = xlinknode[var1];
						next->channel = vchx;
						break;
					case 2:
						next->node = ylinknode[var2];
						next->channel = vchy;
						break;
					
					}
	return next;
}


