/*
THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING
	A TUTOR OR CODE WRITTEN BY OTHER STUDENTS OR FOUND ONLINE 
	- SABRINA MARTINEZ
	- SMART57@EMORY.EDU / 928-285-3381
	- 2375865 / SMART57
*/



//
//  my_malloc.c
//  Lab1: Malloc
//



#include "my_malloc.h"
#include <stddef.h>//needed for NULL
#include <unistd.h>//needed for sbrk
#include <stdio.h>


MyErrorNo my_errno=MYNOERROR;

FreeListNode head;//global var: pointer that points to first node



/*****************HELPER FUNCTIONS*****************************/

void nosplitSew(FreeListNode ptr){
	FreeListNode prev;
	FreeListNode currentNode= free_list_begin();

	if(currentNode!=NULL &&currentNode->flink !=NULL){
		prev=currentNode;
		currentNode= currentNode->flink;
	}

	if(prev==head && prev==ptr){
		head= currentNode;
		return;
	}

	if(currentNode==ptr){
		prev->flink=currentNode->flink;
		return;
	}

	while(currentNode!=ptr){

		prev=currentNode;
		currentNode=currentNode->flink;

			if(currentNode==ptr){
				prev->flink= currentNode->flink;
				return;
			}

	}

}


//uses sbrk to <title drop> 
void * makeSpace(unsigned int paddedSize){
	void *ptr;


	//check if more space than 8192 is needed

	if(paddedSize<= 8192){

		ptr= sbrk(8192);
		*((unsigned int*)ptr)= 8192; //store size of sbrked space to chunk

	}else{

		ptr= sbrk(paddedSize);
		*((unsigned int*)ptr)= paddedSize;//store size of sbrked space to chunk

	}

	return ptr;
}


//goes through free list until finds suitable node or the end (or nowhere if list is empty)
FreeListNode traverseNodes(FreeListNode currentNode, unsigned int paddedSize){
	
	FreeListNode temp= currentNode;

	/*traversal cases:
		- empty list
		- first appropriate node found
		- reach end of list
	*/

	while(temp!=NULL && temp->size < paddedSize && temp->flink!=NULL){
		temp = temp->flink;
	}

	return temp;
}

//add header to malloced space, move pointer along to after header
void * addHeader(void * ptr, unsigned int paddedSize){

	*((unsigned int*)ptr)= 			paddedSize; //first part of header: full size
	ptr+=							sizeof(unsigned int);//go to magic number address
	*((unsigned int*)ptr)= 			39;//second part of header: magic number
	ptr+=							sizeof(unsigned int);	//go to first part of malloced space 

	return ptr;

}


void newFreeListNode(FreeListNode ptr, unsigned int freeSize){ //insert free node at designated address

	FreeListNode prev;
	FreeListNode currentNode= free_list_begin();
	void * endmysuffering;
	unsigned int magicNum=0;

	if(currentNode==NULL){ //CASE: EMPTY LIST
		currentNode= ptr;
		currentNode->flink=NULL; //null  because no other node
		currentNode->size= freeSize;
		head= currentNode;

		return;
	}

	if(ptr<currentNode){//CASE: INSERT AT START
		head= ptr;
		ptr->flink= currentNode;
		ptr->size= freeSize;

		return;
	}

	while(currentNode->flink!=NULL){//CASE: TRAVERSAL, MIDDLE STITCH

		prev=currentNode;
		currentNode= currentNode->flink;

		if(currentNode!=NULL){
		
				endmysuffering= currentNode;
				endmysuffering+= sizeof(unsigned int);
				magicNum=*((unsigned int*)endmysuffering);


				if(magicNum==39){
					break;
				}
		
				if(ptr<currentNode){ // ptrs are at right places
					//STITCH
		
					if(prev<ptr){
					prev->flink=ptr;
					ptr->flink=currentNode;
					ptr->size=freeSize;
		
					return;
					}
		
				}
			}
	}


	//CASE:APPEND
	if(magicNum==39){


		prev->flink=ptr;
		ptr->flink=NULL;
		ptr->size=freeSize;
		return;

	}else if(currentNode->flink==NULL){

		currentNode->flink=ptr;
		ptr->flink=NULL;
		ptr->size=freeSize;
		return;
	}
}

/*****************ASSIGNMENT FUNCTIONS************************/

void *my_malloc(uint32_t size){
	
	FreeListNode currentNode;//used for linked list transversal 
	void * ptr;
	unsigned int sbrked= 0;
	unsigned int actualsize;

	FreeListNode temp=NULL;


	/*PAD SPACE ACCORDINGLY*/

		unsigned int paddedSize= CHUNKHEADERSIZE; //starts with 8 for header


		paddedSize+= size;//add size of header to size 

		if(paddedSize%8 != 0){ //if size+header is not a multiple of 8
			paddedSize+= 8 - (paddedSize%8);//make it a multiple of 8
		}

	/*CHECK IF SIZE OF CHUNK MEETS THE MIN REQ*/
		if(paddedSize<16 || paddedSize< sizeof(struct freelistnode)||size==0){//also check if person does not want to malloc anything
			my_errno= MYENOMEM;
			return NULL;
		}



	/*TRAVERSE FREE LIST*/

		currentNode= free_list_begin();
		currentNode= traverseNodes(currentNode, paddedSize);



	/*ON CURRENT NODE, DO WE NEED MORE SPACE?*/


		if(currentNode==NULL){ //empty list, have to make space



			ptr= makeSpace(paddedSize);
			sbrked=1;

		}else if(currentNode->size < paddedSize){  //reach end of list, no applicable free space found, need to make space


			ptr= makeSpace(paddedSize);
			sbrked=1;

		}else{ //current node has the space we're looking for

			ptr= currentNode;


		}

	/*TO SPLIT OR NOT TO SPLIT*/


		/*first if statement to check where size is located*/

		if(sbrked==1){ //memory was sbrked 
			actualsize= *((unsigned int*)ptr);

		}else{//node was found
			actualsize= currentNode->size;
			

		}



		/*check size to see if need to split*/





		unsigned int freeSize= actualsize-paddedSize;
		unsigned int fragSize=0;

		if(paddedSize==actualsize){//no split


			if(currentNode==head && head->flink==NULL){//end reached, no split, just return 
				ptr= addHeader(ptr,paddedSize);// add header, return as is
				return ptr;

			}else{//sew

			nosplitSew(currentNode);}

			ptr= addHeader(ptr,paddedSize);// add header, return as is

		}else if(freeSize<16 || freeSize<sizeof(struct freelistnode)){// internal fragmentation, no split

			fragSize=paddedSize+freeSize;
			nosplitSew(currentNode);

			ptr= addHeader(ptr,fragSize);// add header, return as is


		}else{//split

			if(currentNode==head){//CASE: ONLY ONE NODE IN LIST AFTER TRAVERSING

				if(head!=NULL){
				temp= head->flink;
				}
				
				head=NULL;

			}



			ptr=addHeader(ptr,paddedSize);//add header, THIS IS WHAT GETS RETURNED


			
			if(freeSize>=16 || freeSize>= sizeof(struct freelistnode)){ //check if leftover space is over the minimum requirement to split
				FreeListNode splitter= ptr-CHUNKHEADERSIZE+paddedSize;//go to end of malloced space

				newFreeListNode(splitter, freeSize);//split and make new free
				if(temp!=NULL){
					newFreeListNode(temp, temp->size);
				}

			}


		}

		

	return ptr;

}


      
void my_free(void *ptr){

	//throw error, null pointer
	if(ptr==NULL){
		my_errno= MYBADFREEPTR;
	}

	if(my_errno!=MYBADFREEPTR){
		//go to space that should have size of space we want to free and grab it
		void * magicPtr= ptr- CHUNKHEADERSIZE;
	
		unsigned int freeSize= *((unsigned int*)magicPtr);
	
		//go to space that should have magic number and grab it
		magicPtr= magicPtr+ sizeof(unsigned int);
	
		unsigned int num= *((unsigned int*)magicPtr); 
	
		//go back to start
		magicPtr= magicPtr- sizeof(unsigned int);
	
	
		//if statements check for magic number
	
		if(num==39){// found space that we malloced, so we can free it
	
			newFreeListNode(magicPtr, freeSize);
	
		}else{//throw error, trying to free space that isn't malloced by us
	
			my_errno= MYBADFREEPTR;
	
		}
	}
}


FreeListNode free_list_begin(){ //return head
	//global variable initializes as NULL
	return head;
}

void coalesce_free_list(){

	FreeListNode currentNode= free_list_begin();
	FreeListNode next;
	void * temp;
	//ELSE: TRAVERSE LIST

	while(currentNode!=NULL){//TRAVERSE, IF LIST IS NOT EMPTY OR NEXT NODE IS NOT EMPTY


		if((void *)currentNode+ currentNode->size == (void *) currentNode->flink){
			currentNode->size+= currentNode->flink->size;
			currentNode->flink= currentNode->flink->flink;
		}else{
			currentNode= currentNode->flink;
		}
	}
}


