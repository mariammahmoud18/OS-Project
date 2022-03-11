#include <inc/lib.h>

// malloc()
//	This function use FIRST FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
#define HeapSize (USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE
struct Heap_Info {
	uint32 num_of_blocks;
	int empty;
	uint32 va;
};
struct Heap_Info Heap[HeapSize];
uint32 S_alloc=USER_HEAP_START;
uint32 E_alloc=USER_HEAP_MAX;
int index=0;
void* malloc(uint32 size)
{
	//TODO: [PROJECT 2021 - [2] User Heap] malloc() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("malloc() is not implemented yet...!!");
	size=ROUNDUP(size,PAGE_SIZE);
	uint32 min_size=9999999;
	uint32 Available_Size=E_alloc - S_alloc;
	//////////////////////////////////////////////////////////
	//This function should find the space of the required range
	//using the BEST FIT strategy
	///////////////////////////////////////////////////////////
	//Check if the size equal zero or the size entered is greater than the heap size
	if(size>Available_Size || size>(HeapSize*PAGE_SIZE) || size==0)
	{
			return NULL;
	}
	int j=-1;
	for(int i=0;i<index;i++)//Search For Min Size Of Spaces
	{
		if(Heap[i].empty==0 && size<=Heap[i].num_of_blocks && Heap[i].num_of_blocks<min_size)
		{
			min_size=Heap[i].num_of_blocks;
			j=i;
		}
	}
	if(j!=-1)//BEST FIT STRATEGY IS USED
	{
		if(Available_Size>=size && Available_Size<=min_size)
		{
			Heap[index].empty = 1;
			Heap[index].num_of_blocks = size;
			Heap[index].va=S_alloc;
			S_alloc+=size;
			index++;
			sys_allocateMem(Heap[index-1].va, size);
			return (void*)Heap[index-1].va;
		}
		else
		{
			if((Heap[j].num_of_blocks-size)>0)
			{
				Heap[index].empty=0;
				Heap[index].num_of_blocks=Heap[j].num_of_blocks-size;
				Heap[index].va=Heap[j].va+size;
				index++;
			}
			Heap[j].empty = 1;
			Heap[j].num_of_blocks = size;
			sys_allocateMem(Heap[j].va, size);
			return (void*)Heap[j].va;

		}
	}
	else//BEST FIT STRATEGY NOT USED
	{
		if(Available_Size>=size)
		{
			Heap[index].empty = 1;
			Heap[index].num_of_blocks = size;
			Heap[index].va=S_alloc;
			S_alloc+=size;
			index++;
			sys_allocateMem(Heap[index-1].va, size);
			return (void*)Heap[index-1].va;
		}
		else
		{
			return NULL;
		}
	}
	return NULL;
}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address)
{
	//TODO: [PROJECT 2021 - [2] User Heap] free() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("free() is not implemented yet...!!");
	///////////////////////////////////////////////////////////////
	//you should get the size of the given allocation using its address
	///////////////////////////////////////////////////////////////
	int j=0;
	int check1=-1,check2=-1;
	uint32 Size;
	for(int i = 0 ;i<index;i++)
	{
		if((uint32)virtual_address==Heap[i].va && Heap[i].empty==1)
		{
			Heap[i].empty=0;
			Size=Heap[i].num_of_blocks;
			sys_freeMem((uint32)virtual_address ,Size);
			j=i;
		}
	}//End Of FOR Loop FOR FREE HEAP
	/////////////////////////////////////////////////////////////////
	if(j!=0)//CHECK FOR CONT FREE SIZE
	{
		for(int i=0 ; i<index ; i++)//CHECK FOR PREVIOUS <- FREE SIZE
		{
			if(Heap[i].empty==0 && Heap[i].va+Heap[i].num_of_blocks==Heap[j].va)
			{
			   check1=j;
			   Heap[i].num_of_blocks+=Heap[j].num_of_blocks;
			   break;
			}
		}//End Of FOR Loop
		for(int i=0 ; i<index ; i++)//CHECK FOR NEXT -> FREE SIZE
		{
			if(Heap[i].empty==0 && Heap[j].va+Heap[j].num_of_blocks==Heap[i].va)
			{
				check2=i;
				Heap[j].num_of_blocks+=Heap[i].num_of_blocks;
				break;
			}
		}//End Of FOR Loop
		////////////////////////////////////////////////////////////////////////
		//FREE NEXT AND PREVIOUS -> <-
		if((check2!=-1 && check1!=-1) || (check2!=-1 && check1==-1) || (check2==-1 && check1!=-1))
		{
			Heap[check1].empty=0;
			Heap[check2].empty=0;
			Heap[check1].num_of_blocks=0;
			Heap[check2].num_of_blocks=0;
		}
		//////////////////////////////////////////////////////////////////////////
	}//End Of if(j!=0){....}
}

//==================================================================================//
//================================ OTHER FUNCTIONS =================================//
//==================================================================================//

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	panic("this function is not required...!!");
	return 0;
}

void* sget(int32 ownerEnvID, char *sharedVarName)
{
	panic("this function is not required...!!");
	return 0;
}

void sfree(void* virtual_address)
{
	panic("this function is not required...!!");
}

void *realloc(void *virtual_address, uint32 new_size)
{
	panic("this function is not required...!!");
	return 0;
}

void expand(uint32 newSize)
{
	panic("this function is not required...!!");
}
void shrink(uint32 newSize)
{
	panic("this function is not required...!!");
}

void freeHeap(void* virtual_address)
{
	panic("this function is not required...!!");
}
