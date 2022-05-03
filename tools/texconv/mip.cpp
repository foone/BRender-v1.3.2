#include <brender.h>

#include "mip.h"
#include "log.h"


class PixelmapNode{
public:
	PixelmapNode(){_next=NULL,_pixelmap=NULL;}
	PixelmapNode(br_pixelmap *pixelmap){_next=NULL,_pixelmap=pixelmap;}
	void attatch(PixelmapNode * pixelmapNode){_next=pixelmapNode;}
	br_pixelmap * value(){return _pixelmap;}
	PixelmapNode * next(){return _next;}
private:
	PixelmapNode * _next;
	br_pixelmap * _pixelmap;
};

class PixelmapList{
public:
	PixelmapList(){head=NULL,tail=NULL;}
	void addTail(br_pixelmap *pixelmap);
	br_pixelmap * removeHead();

private:
	PixelmapNode * head;
	PixelmapNode * tail;
};

inline void PixelmapList::addTail(br_pixelmap *pixelmap){
	PixelmapNode *temp;
	temp=new PixelmapNode(pixelmap);
	if(tail)
		tail->attatch(temp);
	tail=temp;
	if(!head)
		head=tail;
}

inline br_pixelmap * PixelmapList::removeHead(){
	br_pixelmap *pixelmap;
	PixelmapNode *temp;
	if(head){
		pixelmap=head->value();
		temp=head;head=head->next();
		return pixelmap;
	}else{
		return NULL;
	}
}
	
static PixelmapList newPixelmapList,oldPixelmapList;
static paramBase,paramRange;
br_uint_32 BR_CALLBACK mipGeneration(br_pixelmap * source,void * arg){
	br_pixelmap * destination;
	if(source->identifier)
		T_Log("Generating mip levels for %s.\n",source->identifier);
	else
		T_Log("Generating mip levels.\n",source->identifier);

	destination=BrPixelmapMakeMipMap(source,source->type,source->map,paramBase,paramRange,BR_QUANTIZE_YIQ);
	newPixelmapList.addTail(destination);
	oldPixelmapList.addTail(source);
	return 0;
}


void generateMipLevels(br_uint_32 base,br_uint_32 range){
	br_pixelmap * pixelmap;

	paramBase=base,paramRange=range;

	BrMapEnum("*",mipGeneration,NULL);

	while(pixelmap=oldPixelmapList.removeHead()){
		BrMapRemove(pixelmap);
		BrPixelmapFree(pixelmap);
	}
	
	while(pixelmap=newPixelmapList.removeHead())
		BrMapAdd(pixelmap);
}