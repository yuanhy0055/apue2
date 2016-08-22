/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                          SEMAPHORE MANAGEMENT
*
*                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* File : OS_SEM.C
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#ifndef  OS_MASTER_FILE
#include "includes.h"
#endif

#if OS_SEM_EN > 0
/*
*********************************************************************************************************
*                                           ACCEPT SEMAPHORE�����ź���
*
* Description: This function checks the semaphore to see if a resource is available or, if an event
*              occurred.  Unlike OSSemPend(), OSSemAccept() does not suspend the calling task if the
*              resource is not available or the event did not occur.
*
* Arguments  : pevent     is a pointer to the event control block
*
* Returns    : >  0       if the resource is available or the event did not occur the semaphore is
*                         decremented to obtain the resource.
*              == 0       if the resource is not available or the event did not occur or,
*                         if 'pevent' is a NULL pointer or,
*                         if you didn't pass a pointer to a semaphore
                                                        
*********************************************************************************************************
*/

#if OS_SEM_ACCEPT_EN > 0
INT16U  OSSemAccept (OS_EVENT *pevent)
{
#if OS_CRITICAL_METHOD == 3                           /* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr;
#endif    
    INT16U     cnt;


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {                    /* Validate 'pevent'                             */
        return (0);
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_SEM) {   /* Validate event block type                     */
        return (0);
    }
#endif
    OS_ENTER_CRITICAL();
    cnt = pevent->OSEventCnt;
    if (cnt > 0) {                                    /* See if resource is available                  */
        pevent->OSEventCnt--;                         /* Yes, decrement semaphore and notify caller    */
    }
    OS_EXIT_CRITICAL();
    return (cnt);                                     /* Return semaphore count                        */
}
#endif    

/*$PAGE*/
/*
*********************************************************************************************************
*                                           CREATE A SEMAPHORE
*
* Description: This function creates a semaphore.
*
* Arguments  : cnt           is the initial value for the semaphore.  If the value is 0, no resource is
*                            available (or no event has occurred).  You initialize the semaphore to a
*                            non-zero value to specify how many resources are available (e.g. if you have
*                            10 resources, you would initialize the semaphore to 10).
*
* Returns    : != (void *)0  is a pointer to the event control clock (OS_EVENT) associated with the
*                            created semaphore
*              == (void *)0  if no event control blocks were available
                                                             ����һ���ź���
����������һ���ź���
������cnt���ź����ĳ�ʼ��ֵ�����Ϊ�㣬��û���ź������ã�����û���¼�������
                          �÷���ֵ��ʼ���ź�������ʾ���ж��ٿ�����Դ���������ʮ��
                          ��Դ�����ʼ���ź���Ϊ10
���أ�!= (void *)0  ָ���¼����ƿ��Ͻ������ź�����ָ��
*              == (void *)0  ���û�п����¼����ƿ�
*********************************************************************************************************
*/

OS_EVENT  *OSSemCreate (INT16U cnt)
{
#if OS_CRITICAL_METHOD == 3                       /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif    
    OS_EVENT  *pevent;


    if (OSIntNesting > 0) {                                /* See if called from ISR ...               */
        return ((OS_EVENT *)0);                            /* ... can't CREATE from an ISR             */
    }//�жϷ����ӳ����ܵ��ô˺��������н����źŵ����Ĺ�������������
    //�����л��߶���������ǰ���
    OS_ENTER_CRITICAL();
    pevent = OSEventFreeList;                              /* Get next free event control block        */
	//�ӿ����¼����ƿ������л��һ���¼����ƿ�ECB
    if (OSEventFreeList != (OS_EVENT *)0) {                /* See if pool of free ECB pool was empty   */
//����ɹ���ã�����������¼����ƿ�����ʹָ֮����һ��������¼����ƿ�
        OSEventFreeList = (OS_EVENT *)OSEventFreeList->OSEventPtr;
    }
    OS_EXIT_CRITICAL();
    if (pevent != (OS_EVENT *)0) {                         /* Get an event control block               */
		//����õ����¼����ƿ���ã��ͽ�����¼��������ó��ź�����
        pevent->OSEventType = OS_EVENT_TYPE_SEM;
        pevent->OSEventCnt  = cnt;                         /* Set semaphore value                      */
		//���ź����ĳ�ʼֵ�����¼����ƿ�ECB�С�
        pevent->OSEventPtr  = (void *)0;                   /* Unlink from ECB free list                */
		//��.OSEventPtr��ʼ��ΪNULL����Ϊ���������ڿ����¼����ƿ������ˡ�
        OS_EventWaitListInit(pevent);                      /* Initialize to 'nobody waiting' on sem.   */
		//�Եȴ������б���г�ʼ������Ϊ�ź������ڳ�ʼ������ʱû���κ�
		//����ȴ����ź��������Ժ�����.OSEventGrp()��.OSEventTbl[]����
    }
    return (pevent);//���ص��ú���һ��ָ���¼����ƿ�ECB��ָ��
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                         DELETE A SEMAPHORE
*
* Description: This function deletes a semaphore and readies all tasks pending on the semaphore.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            semaphore.
*
*              opt           determines delete options as follows:
*                            opt == OS_DEL_NO_PEND   Delete semaphore ONLY if no task pending
*                            opt == OS_DEL_ALWAYS    Deletes the semaphore even if tasks are waiting.
*                                                    In this case, all the tasks pending will be readied.
*
*              err           is a pointer to an error code that can contain one of the following values:
*                            OS_NO_ERR               The call was successful and the semaphore was deleted
*                            OS_ERR_DEL_ISR          If you attempted to delete the semaphore from an ISR
*                            OS_ERR_INVALID_OPT      An invalid option was specified
*                            OS_ERR_TASK_WAITING     One or more tasks were waiting on the semaphore
*                            OS_ERR_EVENT_TYPE       If you didn't pass a pointer to a semaphore
*                            OS_ERR_PEVENT_NULL      If 'pevent' is a NULL pointer.
*
* Returns    : pevent        upon error
*              (OS_EVENT *)0 if the semaphore was successfully deleted.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of
*                 the semaphore MUST check the return code of OSSemPend().
*              2) OSSemAccept() callers will not know that the intended semaphore has been deleted unless
*                 they check 'pevent' to see that it's a NULL pointer.
*              3) This call can potentially disable interrupts for a long time.  The interrupt disable
*                 time is directly proportional to the number of tasks waiting on the semaphore.
*              4) Because ALL tasks pending on the semaphore will be readied, you MUST be careful in
*                 applications where the semaphore is used for mutual exclusion because the resource(s)
*                 will no longer be guarded by the semaphore.
                                              ɾ��һ���ź���
������ɾ��һ���ź���
������pevent��ָ���¼����ƿ���Ŀ���ź�����ָ��
                opt:����ɾ��ѡ�
 *                            opt == OS_DEL_NO_PEND   û����������ʱ���ɾ����
*                            opt == OS_DEL_ALWAYS    ��ʹ������ȴ�Ҳɾ���ź�������������£�
                                   ���й�������񽫾���
 *              err           ���ܰ���ָ����Ϣ�Ĵ�������ָ��
*                            OS_NO_ERR               ���óɹ����ź���ɾ��
*                            OS_ERR_DEL_ISR          �������жϷ����ӳ�����ɾ���ź���
*                            OS_ERR_INVALID_OPT      ָ���˲������ѡ��
*                            OS_ERR_TASK_WAITING    һ�����߶���������ź����еȴ�
*                            OS_ERR_EVENT_TYPE      û�д���ָ�뵽�ź���
*                            OS_ERR_PEVENT_NULL      ��� 'pevent'��һ����ָ��************
��ע��ɾ���ź���֮ǰ����������ɾ���������ź�������������
*/

#if OS_SEM_DEL_EN > 0
OS_EVENT  *OSSemDel (OS_EVENT *pevent, INT8U opt, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif    
    BOOLEAN    tasks_waiting;


    if (OSIntNesting > 0) {                                /* See if called from ISR ...               */
        *err = OS_ERR_DEL_ISR;                             /* ... can't DELETE from an ISR             */
        return (pevent);
    }//�ǲ�����ISR�е���
#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {                         /* Validate 'pevent'                        */
        *err = OS_ERR_PEVENT_NULL;
        return (pevent);//�Ƿ���pevent
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_SEM) {        /* Validate event block type                */
        *err = OS_ERR_EVENT_TYPE;
        return (pevent);//�����¼�������
    }
#endif
    OS_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0x00) {                      /* See if any tasks waiting on semaphore    */
		//����������ڵȴ�
        tasks_waiting = TRUE;                              /* Yes                                      */
    } else {
        tasks_waiting = FALSE;                             /* No                                       */
    }
    switch (opt) {//ɾ��ѡ��
        case OS_DEL_NO_PEND:                               /* Delete semaphore only if no task waiting */
//���ֻ��û������ȴ���ʱ���ɾ��
             if (tasks_waiting == FALSE) {//��û������ȴ�
                 pevent->OSEventType = OS_EVENT_TYPE_UNUSED;//���¼����ƿ���Ϊδ�ã����˻ص�
                 //����ECB�У��˲���������¼����ƿ����ڽ�����һ�ź���
                 pevent->OSEventPtr  = OSEventFreeList;    /* Return Event Control Block to free list  */
				 //ԭ���б�ͷ����϶��ˡ�
                 OSEventFreeList     = pevent;             /* Get next free event control block        */
				 //peventָ���µĿ��б�ͷ
                 OS_EXIT_CRITICAL();
                 *err = OS_NO_ERR;//�����޴�
                 return ((OS_EVENT *)0);                   /* Semaphore has been deleted               */
             } else {//���������ȴ�
                 OS_EXIT_CRITICAL();
                 *err = OS_ERR_TASK_WAITING;
                 return (pevent);
             }

        case OS_DEL_ALWAYS:                                /* Always delete the semaphore              */
			//������ζ�Ҫɾ��
             while (pevent->OSEventGrp != 0x00) {          /* Ready ALL tasks waiting for semaphore    */
                 OS_EventTaskRdy(pevent, (void *)0, OS_STAT_SEM);
             }//ʹ���еȴ����������
             pevent->OSEventType = OS_EVENT_TYPE_UNUSED;//���Ϊδ��
             pevent->OSEventPtr  = OSEventFreeList;        /* Return Event Control Block to free list  */
			 //ԭ���б�ͷ����϶��ˡ�
             OSEventFreeList     = pevent;                 /* Get next free event control block        */
             //peventָ���µĿ��б�ͷ
             OS_EXIT_CRITICAL();
             if (tasks_waiting == TRUE) {                  /* Reschedule only if task(s) were waiting  */
                 OS_Sched();                               /* Find highest priority task ready to run  */
				 //������ȴ��Ļ�����Ϊ���������������Ҫ�������
             }
             *err = OS_NO_ERR;
             return ((OS_EVENT *)0);                       /* Semaphore has been deleted               */

        default://�����Ƿ�ѡ��
             OS_EXIT_CRITICAL();
             *err = OS_ERR_INVALID_OPT;
             return (pevent);
    }
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                           PEND ON SEMAPHORE
*
* Description: This function waits for a semaphore.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            semaphore.
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for the resource up to the amount of time specified by this argument.
*                            If you specify 0, however, your task will wait forever at the specified
*                            semaphore or, until the resource becomes available (or the event occurs).
*
*              err           is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*
*                            OS_NO_ERR           The call was successful and your task owns the resource
*                                                or, the event you are waiting for occurred.
*                            OS_TIMEOUT          The semaphore was not received within the specified
*                                                timeout.
*                            OS_ERR_EVENT_TYPE   If you didn't pass a pointer to a semaphore.
*                            OS_ERR_PEND_ISR     If you called this function from an ISR and the result
*                                                would lead to a suspension.
*                            OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer.
*
* Returns    : none
                                                            �ȴ�һ���ź���
�������ȴ�һ���ź���
������pevent��ָ���¼����ƿ���Ŀ���ź�����ָ��
                timeout����ʱ��ʱѡ���ʱ�ӽ���Ϊ��λ����������㣬������񽫵ȴ�
                             //��Դ��ʱ��ֵ����������������Ϊ�㣬����Զ�ȴ�ֱ����Դ��ɿ��ã����������¼�������
		  err			ָ�����������Ϣָ��
			   *							��ϢΪ��
			   *
			   *							OS_NO_ERR			���óɹ�������ӵ����Դ����Ŀ���¼�����
			   *							OS_TIMEOUT			�涨ʱ�����ź���û�з���
			   *							OS_ERR_EVENT_TYPE	���û�д���ָ�뵽�ź���
			   *							OS_ERR_PEND_ISR 	���ISR���ô˺�����������쳣
			   *							OS_ERR_PEVENT_NULL	��� 'pevent' ��һ����ָ��

*********************************************************************************************************
*/

void  OSSemPend (OS_EVENT *pevent, INT16U timeout, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                           /* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr;
#endif    


    if (OSIntNesting > 0) {                           /* See if called from ISR ...                    */
        *err = OS_ERR_PEND_ISR;                       /* ... can't PEND from an ISR                    */
        return;
    }//�ǲ�����ISR�е���
#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {                    /* Validate 'pevent'                             */
        *err = OS_ERR_PEVENT_NULL;
        return;
    }//�Ƿ���pevent
    if (pevent->OSEventType != OS_EVENT_TYPE_SEM) {   /* Validate event block type                     */
        *err = OS_ERR_EVENT_TYPE;
        return;
    }//�����¼�������
#endif
    OS_ENTER_CRITICAL();
    if (pevent->OSEventCnt > 0) {                     /* If sem. is positive, resource available ...   */
        pevent->OSEventCnt--;                         /* ... decrement semaphore only if positive.     */
        OS_EXIT_CRITICAL();
        *err = OS_NO_ERR;
        return;
    }
//����ź�����Ч�����ź�������ֵ�ݼ��������޴���������ĺ����������
//�������ڵȴ�һ������Դ���ź�������ôͨ�����ش�ֵ��֪�������Ƿ���ȷ���С�
//����ź����ļ���ֵΪ0������ô˺��������񽫽���˯��״̬���ȴ���һ������
//����ISR�����ź�����
                                                      /* Otherwise, must wait until event occurs       */
    OSTCBCur->OSTCBStat |= OS_STAT_SEM;               /* Resource not available, pend on semaphore     */
//��Դ�����ã���������
    OSTCBCur->OSTCBDly   = timeout;                   /* Store pend timeout in TCB                     */
//�������ʱ��TCB
    OS_EventTaskWait(pevent);                         /* Suspend task until event or timeout occurs    */
//ʹ����ȴ��¼����������������
    OS_EXIT_CRITICAL();
    OS_Sched();                                       /* Find next highest priority task ready         */
	//���ڵò����ź�������ǰ�����ٴ��ھ���״̬��������ȣ�����һ��
	//���ȼ���ߵ��������С����������ô˺��������񱻹���ֱ���ź������֣�
	//���ܼ������С�
    OS_ENTER_CRITICAL();
    if (OSTCBCur->OSTCBStat & OS_STAT_SEM) {          /* Must have timed out if still waiting for event*/
//�ٴμ��������ƿ��е�״̬��־���Ƿ��Դ��ڵȴ��ź�����״̬������ǣ�
//��˵��������û�б��˺����������ź������ѣ���ʵ���ϸ���������Ϊ�ȴ�
//��ʱ������TimeTick()������Ϊ����̬��
        OS_EventTO(pevent);//�ȴ��¼���ʱ��������ӵȴ��б���ɾ����������������С�
        OS_EXIT_CRITICAL();
        *err = OS_TIMEOUT;     /* Indicate that didn't get event within TO      *///���س�ʱ�������
        return;//��ʱ��
    }
    OSTCBCur->OSTCBEventPtr = (OS_EVENT *)0;//��ָ���ź���ECB��ָ��Ӹ������������ƿ���ɾ��
    OS_EXIT_CRITICAL();
    *err = OS_NO_ERR;
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                         POST TO A SEMAPHORE
*
* Description: This function signals a semaphore
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            semaphore.
*
* Returns    : OS_NO_ERR           The call was successful and the semaphore was signaled.
*              OS_SEM_OVF          If the semaphore count exceeded its limit.  In other words, you have
*                                  signalled the semaphore more often than you waited on it with either
*                                  OSSemAccept() or OSSemPend().
*              OS_ERR_EVENT_TYPE   If you didn't pass a pointer to a semaphore
*              OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer.
                                                      ����һ���ź���
����������һ���ź���
������pevent ��ָ���¼����ƿ���Ŀ���ź�����ָ��
���أ�OS_NO_ERR           ���óɹ����ź�������
*              OS_SEM_OVF       �ź�����Ŀ������Χ. Ҳ����˵�㷢�͵��ź���������either
*                                  OSSemAccept() or OSSemPend()�еȴ��� 
*              OS_ERR_EVENT_TYPE   û�д���ָ����ź���
*              OS_ERR_PEVENT_NULL  ��� 'pevent' �ǿ�ָ��
*********************************************************************************************************
*/

INT8U  OSSemPost (OS_EVENT *pevent)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;                               
#endif    


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {                         /* Validate 'pevent'                        */
        return (OS_ERR_PEVENT_NULL);
    }//�Ƿ���pevent
    if (pevent->OSEventType != OS_EVENT_TYPE_SEM) {        /* Validate event block type                */
        return (OS_ERR_EVENT_TYPE);
    }//�����¼�������
#endif
    OS_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0x00) {                      /* See if any task waiting for semaphore    */
		//�ǲ���������ȴ�
        OS_EventTaskRdy(pevent, (void *)0, OS_STAT_SEM);   /* Ready highest prio task waiting on event */
		//����������ȼ��ĵȴ�����
        OS_EXIT_CRITICAL();
        OS_Sched(); /* Find highest priority task ready to run  *///������ȣ���������ȼ�����������                                       
        return (OS_NO_ERR);
    }
    if (pevent->OSEventCnt < 65535) {                 /* Make sure semaphore will not overflow         */
		//��֤�ź���û�����
        pevent->OSEventCnt++;                         /* Increment semaphore count to register event   */
		//�����ź�����Ŀ
        OS_EXIT_CRITICAL();
        return (OS_NO_ERR);
    }
    OS_EXIT_CRITICAL();    /* Semaphore value has reached its maximum   *///�ź������ˡ�
    return (OS_SEM_OVF);
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                          QUERY A SEMAPHORE
*
* Description: This function obtains information about a semaphore
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            semaphore
*
*              pdata         is a pointer to a structure that will contain information about the
*                            semaphore.
*
* Returns    : OS_NO_ERR           The call was successful and the message was sent
*              OS_ERR_EVENT_TYPE   If you are attempting to obtain data from a non semaphore.
*              OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer.
                                                        ��ѯһ���ź����ĵ�ǰ״̬
���������һ���ź�������Ϣ
������pevent��ָ���¼����ƿ���Ŀ���ź�����ָ��
                pdata��ָ������ź�����Ϣ�Ľṹָ��
���أ�OS_NO_ERR           ���óɹ�����Ϣ����
*              OS_ERR_EVENT_TYPE   �����ӷ��ź����������
*              OS_ERR_PEVENT_NULL  ��� 'pevent' �ǿ�ָ��
*********************************************************************************************************
*/

#if OS_SEM_QUERY_EN > 0
INT8U  OSSemQuery (OS_EVENT *pevent, OS_SEM_DATA *pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif    
    INT8U     *psrc;
    INT8U     *pdest;


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {                         /* Validate 'pevent'                        */
        return (OS_ERR_PEVENT_NULL);
    }//�Ƿ���pevent
    if (pevent->OSEventType != OS_EVENT_TYPE_SEM) {        /* Validate event block type                */
        return (OS_ERR_EVENT_TYPE);
    }//�����¼�������
#endif
    OS_ENTER_CRITICAL();
    pdata->OSEventGrp = pevent->OSEventGrp;                /* Copy message mailbox wait list           */
	//��OS_EVENT�ṹ�еĵȴ������б��Ƶ�OS_SEM_DATA���ݽṹ�У�
	//�˴�ʹ���������벻����ѭ����䣬Ŀ����ʹ�������еø���
    psrc              = &pevent->OSEventTbl[0];
    pdest             = &pdata->OSEventTbl[0];//ָ���ͷ
#if OS_EVENT_TBL_SIZE > 0
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 1
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 2
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 3
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 4
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 5
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 6
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 7
    *pdest            = *psrc;
#endif
    pdata->OSCnt      = pevent->OSEventCnt; /* Get semaphore count       */
//����ź�������
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif                                                     /* OS_SEM_QUERY_EN                          */
#endif                                                     /* OS_SEM_EN                                */
