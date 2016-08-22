/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                            MEMORY MANAGEMENT
*
*                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* File : OS_MEM.C
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#ifndef  OS_MASTER_FILE
#include "includes.h"
#endif

#if (OS_MEM_EN > 0) && (OS_MAX_MEM_PART > 0)//����������ڹ�����������ڷ���������0
/*
*********************************************************************************************************
*                                        CREATE A MEMORY PARTITION
*
* Description : Create a fixed-sized memory partition that will be managed by uC/OS-II.
*
* Arguments   : addr     is the starting address of the memory partition
*
*               nblks    is the number of memory blocks to create from the partition.
*
*               blksize  is the size (in bytes) of each block in the memory partition.
*
*               err      is a pointer to a variable containing an error message which will be set by
*                        this function to either:
*
*                        OS_NO_ERR            if the memory partition has been created correctly.
*                        OS_MEM_INVALID_ADDR  you are specifying an invalid address for the memory 
*                                             storage of the partition.
*                        OS_MEM_INVALID_PART  no free partitions available
*                        OS_MEM_INVALID_BLKS  user specified an invalid number of blocks (must be >= 2)
*                        OS_MEM_INVALID_SIZE  user specified an invalid block size
*                                             (must be greater than the size of a pointer)
* Returns    : != (OS_MEM *)0  is the partition was created
*              == (OS_MEM *)0  if the partition was not created because of invalid arguments or, no
*                              free partition is available.
*********************************************************************************************************
*/
/*
*********************************************************************************************************
                                                           ����һ���ڴ����
����������һ������ucosII����Ĵ�С�̶����ڴ��
������addr ���ڴ��������ʼ��ַ
          nblks���ӷָ�����Ҫ�������ڿ����Ŀ
          blksize�����ڷָ�����ÿ���ڴ�Ĵ�С�����ֽڱ�ʾ��
           err���������к��������ģ�����������Ϣ�Ŀɱ�ָ�룺
               OS_NO_ERR������ڴ�齨����ȷ
               OS_MEM_INVALID_ADDR����Ϊ�ָ��������ڿռ�ָ���˲����õ�ַ
               OS_MEM_INVALID_PART��û�ж���������ˡ�
               OS_MEM_INVALID_BLKS���û�ָ���ָ���Ŀ���Ϸ���������ڵ���2��
               OS_MEM_INVALID_SIZE���û�ָ���˷Ƿ����ڴ���С���������ָ��Ĵ�С��
���أ� != (OS_MEM *)0�������Ѿ�����
            == (OS_MEM *)0����Ϊ�Ƿ���������û�п�����û�н�������

*********************************************************************************************************
*/


OS_MEM  *OSMemCreate (void *addr, INT32U nblks, INT32U blksize, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                           /* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr;//typedef unsigned int OS_CPU_SR
#endif    
    OS_MEM    *pmem;//�ڴ���ƿ����ݽṹ
    INT8U     *pblk;
    void     **plink;//ָ��ָ���ָ��
    INT32U     i;


#if OS_ARG_CHK_EN > 0//�����������
    if (addr == (void *)0) {                          /* Must pass a valid address for the memory part. */
        *err = OS_MEM_INVALID_ADDR;//ҪΪ�ڴ�齨��һ���Ϸ���ַ������ַҪ��Ч
        return ((OS_MEM *)0);//����
    }
    if (nblks < 2) {                                  /* Must have at least 2 blocks per partition      */
        *err = OS_MEM_INVALID_BLKS;//�ڴ�ֿ�̫���ˣ�������С����
        return ((OS_MEM *)0);
    }
    if (blksize < sizeof(void *)) {                   /* Must contain space for at least a pointer      */
        *err = OS_MEM_INVALID_SIZE;//ÿ���ڴ������Ҫ�ݲ��õ���һ��ָ�룬��Ϊ
        //ͬһ�����е����п������ڿ�����ָ�������������ġ�
        return ((OS_MEM *)0);
    }
#endif
    OS_ENTER_CRITICAL();
    pmem = OSMemFreeList;                             /* Get next free memory partition                */
	//��ϵͳ�еĿ����ڴ���ƿ�������ȡ��һ���ڴ���ƿ飬�����ڴ�
	//���ƿ��а�����Ӧ���ڷ���������״̬��Ϣ��
    if (OSMemFreeList != (OS_MEM *)0) {               /* See if pool of free partitions was empty      */
		//�ڴ�����Ŀ�����ָ���Ƿ�Ϊ�գ��粻Ϊ�գ�
        OSMemFreeList = (OS_MEM *)OSMemFreeList->OSMemFreeList;//����ʲô��˼��������鸳�������ڴ�飿
    }
    OS_EXIT_CRITICAL();
    if (pmem == (OS_MEM *)0) {                        /* See if we have a memory partition             */
        *err = OS_MEM_INVALID_PART;//���Ƿ��ж�����ڴ���ƿ�
        return ((OS_MEM *)0);
    }
    plink = (void **)addr;             /* Create linked list of free memory blocks  ������������ʱ    */
    pblk  = (INT8U *)addr + blksize;//��Ҫ�������ڴ�����ڵ������ڴ��������һ������
    //������Ϊ�ڵ��������в����ɾ��Ԫ�ض��Ǵ�����Ķ��˿�ʼ����ִ�еġ�
    for (i = 0; i < (nblks - 1); i++) {
        *plink = (void *)pblk;
        plink  = (void **)pblk;//������ΪʲôҪ��ôд��
        pblk   = pblk + blksize;//����Ҷ���
    }
    *plink              = (void *)0;                  /* Last memory block points to NULL              */
	//����������Ϸֵ��£����һ��ָ���㡣
	//�����⼸���Ǳ�����ص���Ϣ
    pmem->OSMemAddr     = addr;                       /* Store start address of memory partition       */
    pmem->OSMemFreeList = addr;                       /* Initialize pointer to pool of free blocks     */
    pmem->OSMemNFree    = nblks;                      /* Store number of free blocks in MCB            */
    pmem->OSMemNBlks    = nblks;
    pmem->OSMemBlkSize  = blksize;                    /* Store block size of each memory blocks        */
	//����ÿһ���ڴ��Ĵ�С
    *err                = OS_NO_ERR;//û�д���
    return (pmem);//������ɺ󣬷���ָ����ڴ���ƿ��ָ�롣��ָ�����Ժ�Ը�
    //�ڴ�����Ĳ�����ʹ�á�
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                          GET A MEMORY BLOCK
*
* Description : Get a memory block from a partition
*
* Arguments   : pmem    is a pointer to the memory partition control block
*
*               err     is a pointer to a variable containing an error message which will be set by this
*                       function to either:
*
*                       OS_NO_ERR           if the memory partition has been created correctly.
*                       OS_MEM_NO_FREE_BLKS if there are no more free memory blocks to allocate to caller
*                       OS_MEM_INVALID_PMEM if you passed a NULL pointer for 'pmem'
*
* Returns     : A pointer to a memory block if no error is detected
*               A pointer to NULL if an error is detected
*********************************************************************************************************
*/
 /*
 *********************************************************************************************************
                                             �����ڴ��
 �������ӷָ����õ�һ���ڴ��
 ������pmem��һ��ָ���ڴ�ָ���ƿ��ָ��
			err�����԰������´�����Ϣ��ָ�룺
					OS_NO_ERR������ڴ�ָ���ȷ����
					OS_MEM_NO_FREE_BLKS��û�ж�����ڴ���ƿ����
					OS_MEM_INVALID_PMEM�������һ����ָ���"pmem"
 
 ���أ�����޴����򷵻�һ��ָ���ڴ���ƿ��ָ�룬����д��󷵻�һ����ָ��	
��ע��Ӧ�ó���Ӧ��֪���������ڴ�С������ʹ�õ�ʱ���ܳ������Ĵ�С
Ӧ�ó����õ�ʱ��Ӧ�������ͷţ����»ص���Ӧ����Ӧ�����ڷ�����
 *********************************************************************************************************
*/


void  *OSMemGet (OS_MEM *pmem, INT8U *err)
{//����ָ��pmem����ָ��ϣ�����з��䵽���ڴ����ڴ����
#if OS_CRITICAL_METHOD == 3                           /* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr;
#endif    
    void      *pblk;


#if OS_ARG_CHK_EN > 0
    if (pmem == (OS_MEM *)0) {                        /* Must point to a valid memory partition         */
        *err = OS_MEM_INVALID_PMEM;//�ڴ���ַΪ�㣬������
        return ((OS_MEM *)0);
    }
#endif
    OS_ENTER_CRITICAL();
    if (pmem->OSMemNFree > 0) {                       /* See if there are any free memory blocks       */
		//���ǲ����ж�����ڴ��
        pblk                = pmem->OSMemFreeList;    /* Yes, point to next free memory block          */
		//�ж���ģ�ָ������һ���ڴ�飬��ɾ����һ��
        pmem->OSMemFreeList = *(void **)pblk;         /*      Adjust pointer to new free list          */
       //�����µ�����ָ��
        pmem->OSMemNFree--;                           /*      One less memory block in this partition  */
	   //������һ
        OS_EXIT_CRITICAL();
        *err = OS_NO_ERR;                             /*      No error                                 */
		//û�д���
        return (pblk);                                /*      Return memory block to caller            */
		//���صõ����ڴ��
    }
    OS_EXIT_CRITICAL();
    *err = OS_MEM_NO_FREE_BLKS;                       /* No,  Notify caller of empty memory partition  */
	//û�ж���ģ�
    return ((void *)0);                               /*      Return NULL pointer to caller            */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                         RELEASE A MEMORY BLOCK
*
* Description : Returns a memory block to a partition
*
* Arguments   : pmem    is a pointer to the memory partition control block
*
*               pblk    is a pointer to the memory block being released.
*
* Returns     : OS_NO_ERR            if the memory block was inserted into the partition
*               OS_MEM_FULL          if you are returning a memory block to an already FULL memory 
*                                    partition (You freed more blocks than you allocated!)
*               OS_MEM_INVALID_PMEM  if you passed a NULL pointer for 'pmem'
*               OS_MEM_INVALID_PBLK  if you passed a NULL pointer for the block to release.
*********************************************************************************************************
*/
/*
*********************************************************************************************************
                                                        �ͷ�һ���ڴ��
����������һ���ڴ�鵽�ָ�����
������pmem��ָ���ڴ�ָ���ƿ��ָ��
                pblk��ָ�򽫱��ͷ��ڴ���ָ��
 ���أ�OS_NO_ERR������ڴ��ɹ����뵽�ָ���
                  OS_MEM_FULL����������ڴ�鵽һ���������ڴ�ָ�����
                  OS_MEM_INVALID_PMEM������㴫��һ����ָ�뵽��pmem��
                  OS_MEM_INVALID_PBLK�������һ����ָ�뵽���ͷŵĿ�
��ע���˺�������֪���ڴ���������ĸ������ģ�Ҳ����˵������û�����
��һ������32B�ڴ��ķ����з�����һ���ڴ�飬�����ǧ��Ҫ��һ������
120B�ڴ�������������Ϊ����´�һ����������120B�ڴ�ʱ��ֻ��õ�32B
*********************************************************************************************************
*/



INT8U  OSMemPut (OS_MEM  *pmem, void *pblk)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    
    
#if OS_ARG_CHK_EN > 0
    if (pmem == (OS_MEM *)0) {                   /* Must point to a valid memory partition             */
        return (OS_MEM_INVALID_PMEM);//�����ǿ��е��ڴ�ָ���
    }
    if (pblk == (void *)0) {                     /* Must release a valid block                         */
        return (OS_MEM_INVALID_PBLK);//�����ͷ�һ�����е��ڴ��
    }
#endif
    OS_ENTER_CRITICAL();
    if (pmem->OSMemNFree >= pmem->OSMemNBlks) {  /* Make sure all blocks not already returned          */
        OS_EXIT_CRITICAL();//����ڴ�����Ƿ�������
        return (OS_MEM_FULL);//������ˣ���������
    }
    *(void **)pblk      = pmem->OSMemFreeList;   /* Insert released block into free block list         */
	//���û�������Ͳ���
    pmem->OSMemFreeList = pblk;//����ʲô��˼�����������ˡ�������µı�ͷ��
    pmem->OSMemNFree++;                          /* One more memory block in this partition            */
	//������һ
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);                          /* Notify caller that memory block was released       */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                          QUERY MEMORY PARTITION
*
* Description : This function is used to determine the number of free memory blocks and the number of
*               used memory blocks from a memory partition.
*
* Arguments   : pmem    is a pointer to the memory partition control block
*
*               pdata   is a pointer to a structure that will contain information about the memory
*                       partition.
*
* Returns     : OS_NO_ERR            If no errors were found.
*               OS_MEM_INVALID_PMEM  if you passed a NULL pointer for 'pmem'
*               OS_MEM_INVALID_PDATA if you passed a NULL pointer for the block to release.
*********************************************************************************************************
*/
/*
*********************************************************************************************************
                                                ��ѯһ���ڴ����״̬
��������ѯһ���ڴ�ָ����Ŀ��ڴ��������ڴ��
������pmem��ָ��һ���ڴ�ָ���ƿ��ָ��
                 pdata:    ��Ҫ�����ڴ�ָ�����Ϣ�Ľṹָ��
���أ�OS_NO_ERR��û�з��ִ���
                 OS_MEM_INVALID_PMEM ���������һ����ָ�����pmem��
                 OS_MEM_INVALID_PDATA:�����һ����ָ�뵽���ͷŵĿ�
*********************************************************************************************************
*/



#if OS_MEM_QUERY_EN > 0
INT8U  OSMemQuery (OS_MEM *pmem, OS_MEM_DATA *pdata)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    
    
#if OS_ARG_CHK_EN > 0
    if (pmem == (OS_MEM *)0) {                   /* Must point to a valid memory partition             */
        return (OS_MEM_INVALID_PMEM);//�����ǿ��еķָ���
    }
    if (pdata == (OS_MEM_DATA *)0) {             /* Must release a valid storage area for the data     */
        return (OS_MEM_INVALID_PDATA);//����Ϊ�����ͷ�һ�����еĴ洢�ռ�
    }//OS_MEM_DATA�д�����ض��ڴ�������ڴ��Ĵ�С�������ڴ����Ŀ��
    //�����ڴ����Ŀ����Ϣ��
#endif
    OS_ENTER_CRITICAL();
    pdata->OSAddr     = pmem->OSMemAddr;//��ָ���ڴ��������Ϣȫ�����Ƶ�ָ��������
    pdata->OSFreeList = pmem->OSMemFreeList;//��Ӧ���򣬴˹����У��жϱ��أ���ֹ�ڴ˹�����
    pdata->OSBlkSize  = pmem->OSMemBlkSize;//���ݱ���
    pdata->OSNBlks    = pmem->OSMemNBlks;
    pdata->OSNFree    = pmem->OSMemNFree;
    OS_EXIT_CRITICAL();
    pdata->OSNUsed    = pdata->OSNBlks - pdata->OSNFree;//�������ÿ�
    return (OS_NO_ERR);
}
#endif                                           /* OS_MEM_QUERY_EN                                    */
/*$PAGE*/
/*
*********************************************************************************************************
*                                    INITIALIZE MEMORY PARTITION MANAGER
*
* Description : This function is called by uC/OS-II to initialize the memory partition manager.  Your
*               application MUST NOT call this function.
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)    : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/
/*
*********************************************************************************************************
                                                            ��ʼ���ڴ�ָ�������
������ucos�ڲ����ã�����Ӧ�ó����ܵ���
��������
���أ�Ԫ
��ע��ucos�ڲ����ã�����Ӧ�ó����ܵ���
*********************************************************************************************************
*/



void  OS_MemInit (void)
{
#if OS_MAX_MEM_PART == 1//�ָ���Ϊһ��
    OSMemFreeList                = (OS_MEM *)&OSMemTbl[0]; /* Point to beginning of free list          */
//ָ������б�ʼ��ָ��
    OSMemFreeList->OSMemFreeList = (void *)0;              /* Initialize last node                     */
//��ʼ������㣿ʲô��˼������ָ����ڴ��ָ���
    OSMemFreeList->OSMemAddr     = (void *)0;              /* Store start address of memory partition  */
//�����ڴ�ָ����Ŀ�ʼ��ַ
    OSMemFreeList->OSMemNFree    = 0;                      /* No free blocks                           */
//Ϊ�����û�пտ�
    OSMemFreeList->OSMemNBlks    = 0;                      /* No blocks                                */
//�ָ�����û���ڴ��
    OSMemFreeList->OSMemBlkSize  = 0;                      /* Zero size                                */
//��СΪ��
#endif

#if OS_MAX_MEM_PART >= 2//����ָ�������������
    OS_MEM  *pmem;//�ڴ�ṹ
    INT16U   i;


    pmem = (OS_MEM *)&OSMemTbl[0];                    /* Point to memory control block (MCB)           */
	//ָ���ڴ���ƿ��ָ��
    for (i = 0; i < (OS_MAX_MEM_PART - 1); i++) {     /* Init. list of free memory partitions          */
		//��ÿһ���ڴ�ָ������г�ʼ��
        pmem->OSMemFreeList = (void *)&OSMemTbl[i+1]; /* Chain list of free partitions                 */
		//�ڴ����ռ�����
        pmem->OSMemAddr     = (void *)0;              /* Store start address of memory partition       */
		//�ڴ�ָ����洢��ʼ��ַ
        pmem->OSMemNFree    = 0;                      /* No free blocks                                */
		//�޿��п�
        pmem->OSMemNBlks    = 0;                      /* No blocks                                     */
		//�޿�
        pmem->OSMemBlkSize  = 0;                      /* Zero size                                     */
		//��СΪ��
        pmem++;//�ڴ���ƿ��һ��
    }
    pmem->OSMemFreeList = (void *)0;                  /* Initialize last node                          */
//��ʼ�����һ����㣬�����ڴ���б�ָ��
    pmem->OSMemAddr     = (void *)0;                  /* Store start address of memory partition       */
//��ʼ��ַ
    pmem->OSMemNFree    = 0;                          /* No free blocks                                */
    pmem->OSMemNBlks    = 0;                          /* No blocks                                     */
    pmem->OSMemBlkSize  = 0;                          /* Zero size                                     */

    OSMemFreeList       = (OS_MEM *)&OSMemTbl[0];     /* Point to beginning of free list               */
	//�ձ�ʼָ��
#endif
}
#endif                                           /* OS_MEM_EN                                          */
