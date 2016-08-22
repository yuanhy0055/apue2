/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                        MESSAGE QUEUE MANAGEMENT
*
*                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* File : OS_Q.C
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#ifndef  OS_MASTER_FILE//��ֹ����������һЩ����
#include "includes.h"
#endif

#if (OS_Q_EN > 0) && (OS_MAX_QS > 0)
/*
*********************************************************************************************************
*                                      ACCEPT MESSAGE FROM QUEUE
*
* Description: This function checks the queue to see if a message is available.  Unlike OSQPend(),
*              OSQAccept() does not suspend the calling task if a message is not available.
*
* Arguments  : pevent        is a pointer to the event control block
*
* Returns    : != (void *)0  is the message in the queue if one is available.  The message is removed
*                            from the so the next time OSQAccept() is called, the queue will contain
*                            one less entry.
*              == (void *)0  if the queue is empty or,
*                            if 'pevent' is a NULL pointer or,
*                            if you passed an invalid event type

*********************************************************************************************************
*/

#if OS_Q_ACCEPT_EN > 0
void  *OSQAccept (OS_EVENT *pevent)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif
    void      *msg;
    OS_Q      *pq;


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {               /* Validate 'pevent'                                  */
        return ((void *)0);
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {/* Validate event block type                          */
        return ((void *)0);
    }
#endif
    OS_ENTER_CRITICAL();
    pq = (OS_Q *)pevent->OSEventPtr;             /* Point at queue control block                       */
    if (pq->OSQEntries > 0) {                    /* See if any messages in the queue                   */
        msg = *pq->OSQOut++;                     /* Yes, extract oldest message from the queue         */
        pq->OSQEntries--;                        /* Update the number of entries in the queue          */
        if (pq->OSQOut == pq->OSQEnd) {          /* Wrap OUT pointer if we are at the end of the queue */
            pq->OSQOut = pq->OSQStart;
        }
    } else {
        msg = (void *)0;                         /* Queue is empty                                     */
    }
    OS_EXIT_CRITICAL();
    return (msg);                                /* Return message received (or NULL)                  */
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                        CREATE A MESSAGE QUEUE
*
* Description: This function creates a message queue if free event control blocks are available.
*
* Arguments  : start         is a pointer to the base address of the message queue storage area.  The
*                            storage area MUST be declared as an array of pointers to 'void' as follows
*
*                            void *MessageStorage[size]
*
*              size          is the number of elements in the storage area
*
* Returns    : != (OS_EVENT *)0  is a pointer to the event control clock (OS_EVENT) associated with the
*                                created queue
*              == (OS_EVENT *)0  if no event control blocks were available or an error was detected
                                                      ����һ����Ϣ����
����������п����¼����ƿ飬�ͽ���һ����Ϣ����
������start��ָ����Ϣ���д洢�ռ����ַ���洢�ռ���붨���void �͵�
                            һϵ��ָ�룬��ʽ���ң�void *MessageStorage[size]
                 size���洢�ռ��ڵ�Ԫ����Ŀ
���أ�!= (OS_EVENT *)0  ��ָ���Ͻ����Ķ��е��¼�����ʱ��(OS_EVENT)��ָ��
                 == (OS_EVENT *)0���û���ʺϵ��¼����ƿ�����д���
*********************************************************************************************************
*/

OS_EVENT  *OSQCreate (void **start, INT16U size)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;//������Ϊcpu_sr �ľֲ�����������֧��OS_CRITICAL_METHOD����3
#endif
    OS_EVENT  *pevent;
    OS_Q      *pq;


    if (OSIntNesting > 0) {                      /* See if called from ISR ...                         */
        return ((OS_EVENT *)0);                  /* ... can't CREATE from an ISR                       */
    }//������ISR�н�����Ϣ����
    OS_ENTER_CRITICAL();
    pevent = OSEventFreeList;                    /* Get next free event control block                  */
	//�ӿ����ECB������ȡ��һ���¼����ƿ顣��ʣ�µ�����Ӧ����
	//ECB�ǵ�������
    if (OSEventFreeList != (OS_EVENT *)0) {      /* See if pool of free ECB pool was empty             */
        OSEventFreeList = (OS_EVENT *)OSEventFreeList->OSEventPtr;
    }//��ʣ�µĽ�����Ӧ�ĵ���
    OS_EXIT_CRITICAL();
    if (pevent != (OS_EVENT *)0) {               /* See if we have an event control block              */
		//������ǵõ����¼����ƿ�
        OS_ENTER_CRITICAL();
        pq = OSQFreeList;                        /* Get a free queue control block                     */
		//�õ�һ���µĶ��п��ƿ�
        if (pq != (OS_Q *)0) {                   /* Were we able to get a queue control block ?        */
			//����õ��Ĳ�Ϊ�գ��������õ��ˡ�
            OSQFreeList         = OSQFreeList->OSQPtr;    /* Yes, Adjust free list pointer to next free*/
			//������ͷ
            OS_EXIT_CRITICAL();
			//��ʼ���µõ��Ķ��п��ƿ�
            pq->OSQStart        = start;                  /*      Initialize the queue                 */
            pq->OSQEnd          = &start[size];
            pq->OSQIn           = start;
            pq->OSQOut          = start;
            pq->OSQSize         = size;
            pq->OSQEntries      = 0;//��ʼ����Ϣ��Ϊ��
            pevent->OSEventType = OS_EVENT_TYPE_Q;//�����¼����ƿ�����
            pevent->OSEventCnt  = 0;//�ź���
            pevent->OSEventPtr  = pq;//���˿�ָ����Ϣ���нṹָ��
            OS_EventWaitListInit(pevent);   /*      Initalize the wait list  *///��ʼ���ȴ��б�
        } else {//���û�еõ����п��ƿ�
            pevent->OSEventPtr = (void *)OSEventFreeList; /* No,  Return event control block on error  */
			//�Դ�����ʽ���ض��п��ƿ�
            OSEventFreeList    = pevent;//�˻��¼����ƿ飬�����ˡ�
            OS_EXIT_CRITICAL();
            pevent = (OS_EVENT *)0;//���㣬����һ��ʹ�á�
        }
    }
    return (pevent);//�����Ϣ���н����ɹ�����᷵��һ��ָ�룬���򣬾ͷ��ؿ�ָ�롣
    //��Ϣ���гɹ��󷵻ص����ָ�������Ժ����Ϣ���еĲ�����ˣ���ָ��
    //���Կ�������Ӧ��Ϣ���еľ����
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        DELETE A MESSAGE QUEUE
*
* Description: This function deletes a message queue and readies all tasks pending on the queue.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            queue.
*
*              opt           determines delete options as follows:
*                            opt == OS_DEL_NO_PEND   Delete the queue ONLY if no task pending
*                            opt == OS_DEL_ALWAYS    Deletes the queue even if tasks are waiting.
*                                                    In this case, all the tasks pending will be readied.
*
*              err           is a pointer to an error code that can contain one of the following values:
*                            OS_NO_ERR               The call was successful and the queue was deleted
*                            OS_ERR_DEL_ISR          If you tried to delete the queue from an ISR
*                            OS_ERR_INVALID_OPT      An invalid option was specified
*                            OS_ERR_TASK_WAITING     One or more tasks were waiting on the queue
*                            OS_ERR_EVENT_TYPE       If you didn't pass a pointer to a queue
*                            OS_ERR_PEVENT_NULL      If 'pevent' is a NULL pointer.
*
* Returns    : pevent        upon error
*              (OS_EVENT *)0 if the queue was successfully deleted.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of
*                 the queue MUST check the return code of OSQPend().
*              2) OSQAccept() callers will not know that the intended queue has been deleted unless
*                 they check 'pevent' to see that it's a NULL pointer.
*              3) This call can potentially disable interrupts for a long time.  The interrupt disable
*                 time is directly proportional to the number of tasks waiting on the queue.
*              4) Because ALL tasks pending on the queue will be readied, you MUST be careful in
*                 applications where the queue is used for mutual exclusion because the resource(s)
*                 will no longer be guarded by the queue.
*              5) If the storage for the message queue was allocated dynamically (i.e. using a malloc()
*                 type call) then your application MUST release the memory storage by call the counterpart
*                 call of the dynamic allocation scheme used.  If the queue storage was created statically
*                 then, the storage can be reused.
                                                  ɾ��һ����Ϣ����
������ɾ��һ�����У�ʹ�ڶ����Ϲ��������ȫ������
������pevent ��ָ���¼����ƿ��Ŀ����е�ָ��
                 opt:����ɾ��ѡ����£�
*                            opt == OS_DEL_NO_PEND   û����������ɾ��
*                            opt == OS_DEL_ALWAYS    ���������Ҳɾ�����������ȫ������
*              err          ָ��������´�����Ϣ��ָ��
*                            OS_NO_ERR              ���óɹ�������ɾ��
*                            OS_ERR_DEL_ISR         ������ISR��ɾ��
*                            OS_ERR_INVALID_OPT      ָ���˷Ƿ�ѡ��
*                            OS_ERR_TASK_WAITING     �������ڶ����еȴ�
*                            OS_ERR_EVENT_TYPE      �����û�д�����Ϣ������
*                            OS_ERR_PEVENT_NULL     ���pevent��һ����ָ��
���أ�pevent���д�
                 (OS_EVENT *)0������гɹ�ɾ��
��ע��1���˺���ҪС��ʹ�ã�����ϣ���ֳ����м��OSQPend()�ķ��ش��룬ʲô��˼����֪��
                2��OSQAccept()�ĵ����߲�֪��Ŀ������Ƿ�ɾ���ˣ����Ǽ��pevent'�Ƿ�Ϊ��ָ��
                3���˵��ý�Ǳ�ڹ��ж�һ��ʱ�䣬ʱ�䳤���������������ٳ�����
                4����Ϊ���ж����й��������������ڶ������ʱ��ҪС�ģ���Ϊ��Щ��Դ
                       ���в��ٿ��ܣ���Ϊɾ���ˣ���
                5�������Ϣ�洢�ö�̬���䣨������malloc()������ôӦ�ó������ͨ��
                        ������Ӧ�Ķ�̬����ȥ�ͷ��ڴ�ռ䣬������д洢�Ǿ�̬������������ٴ�
                ����
*********************************************************************************************************
*/

#if OS_Q_DEL_EN > 0
OS_EVENT  *OSQDel (OS_EVENT *pevent, INT8U opt, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    BOOLEAN    tasks_waiting;
    OS_Q      *pq;


    if (OSIntNesting > 0) {                                /* See if called from ISR ...               */
        *err = OS_ERR_DEL_ISR;                             /* ... can't DELETE from an ISR             */
        return ((OS_EVENT *)0);
    }//�������жϷ��������ɾ��
#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {                         /* Validate 'pevent'                        */
        *err = OS_ERR_PEVENT_NULL;
        return (pevent);//pevent������
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {          /* Validate event block type                */
        *err = OS_ERR_EVENT_TYPE;
        return (pevent);//�Ƿ����¼���ģʽ
    }
#endif
    OS_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0x00) {                      /* See if any tasks waiting on queue        */
		//����������ڶ����еȴ�
        tasks_waiting = TRUE;                              /* Yes                                      */
    } else {
        tasks_waiting = FALSE;                             /* No                                       */
    }
    switch (opt) {
        case OS_DEL_NO_PEND:                               /* Delete queue only if no task waiting     */
			//���ֻ��������ȴ��������ɾ��
             if (tasks_waiting == FALSE) {//������ȴ�
                 pq                  = (OS_Q *)pevent->OSEventPtr;  /* Return OS_Q to free list        */
                 pq->OSQPtr          = OSQFreeList;
                 OSQFreeList         = pq;//���������˿�������
                 pevent->OSEventType = OS_EVENT_TYPE_UNUSED;//���δ��
                 pevent->OSEventPtr  = OSEventFreeList;    /* Return Event Control Block to free list  */
                 OSEventFreeList     = pevent; /* Get next free event control block        *///���¼����ƿ�������������
                 OS_EXIT_CRITICAL();
                 *err = OS_NO_ERR;//�޴�
                 return ((OS_EVENT *)0);                   /* Queue has been deleted                   */
             } else {//������ȴ�
                 OS_EXIT_CRITICAL();
                 *err = OS_ERR_TASK_WAITING;
                 return (pevent);
             }

        case OS_DEL_ALWAYS:                                /* Always delete the queue                  */
			//ʼ��Ҫɾ������
             while (pevent->OSEventGrp != 0x00) {          /* Ready ALL tasks waiting for queue        */
			 	//��������ȴ�
                 OS_EventTaskRdy(pevent, (void *)0, OS_STAT_Q);//��������������
             }
             pq                  = (OS_Q *)pevent->OSEventPtr;      /* Return OS_Q to free list        */
             pq->OSQPtr          = OSQFreeList;
             OSQFreeList         = pq;//ͬ�ϣ������п��ƿ�����������
             pevent->OSEventType = OS_EVENT_TYPE_UNUSED;//���Ϊδ��
             pevent->OSEventPtr  = OSEventFreeList;        /* Return Event Control Block to free list  */
             OSEventFreeList     = pevent;                 /* Get next free event control block        */
			 //���¼����ƿ�����������
             OS_EXIT_CRITICAL();
             if (tasks_waiting == TRUE) {                  /* Reschedule only if task(s) were waiting  */
                 OS_Sched();                               /* Find highest priority task ready to run  */
				 //��Ϊ���������������Ҫ���µ���
             }
             *err = OS_NO_ERR;
             return ((OS_EVENT *)0);                       /* Queue has been deleted                   */

        default://�����쳣���
             OS_EXIT_CRITICAL();
             *err = OS_ERR_INVALID_OPT;
             return (pevent);
    }
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                           FLUSH QUEUE
*
* Description : This function is used to flush the contents of the message queue.
*
* Arguments   : none
*
* Returns     : OS_NO_ERR           upon success
*               OS_ERR_EVENT_TYPE   If you didn't pass a pointer to a queue
*               OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer
                                                   �����Ϣ����
�����������Ϣ����������
��������
���أ�
                 OS_NO_ERR          �ɹ�
*               OS_ERR_EVENT_TYPE   û�д���ָ�������
*               OS_ERR_PEVENT_NULL  ���'pevent' ��һ����ָ��

*********************************************************************************************************
*/

#if OS_Q_FLUSH_EN > 0
INT8U  OSQFlush (OS_EVENT *pevent)
{
#if OS_CRITICAL_METHOD == 3                           /* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr;
#endif
    OS_Q      *pq;


#if OS_ARG_CHK_EN > 0//�����������
    if (pevent == (OS_EVENT *)0) {                    /* Validate 'pevent'                             */
        return (OS_ERR_PEVENT_NULL);
    }//������Ĳ���pevent
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {     /* Validate event block type                     */
        return (OS_ERR_EVENT_TYPE);
    }//�����¼�������
#endif
    OS_ENTER_CRITICAL();
    pq             = (OS_Q *)pevent->OSEventPtr;      /* Point to queue storage structure              */
	//����ṹָ��
    pq->OSQIn      = pq->OSQStart;
    pq->OSQOut     = pq->OSQStart;//�����еĲ���ָ��IN��ȡ��ָ��OUT��λ
    pq->OSQEntries = 0;//��ʼ�����Ϊ��
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                     PEND ON A QUEUE FOR A MESSAGE
*
* Description: This function waits for a message to be sent to a queue
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for a message to arrive at the queue up to the amount of time
*                            specified by this argument.  If you specify 0, however, your task will wait
*                            forever at the specified queue or, until a message arrives.
*
*              err           is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*
*                            OS_NO_ERR           The call was successful and your task received a
*                                                message.
*                            OS_TIMEOUT          A message was not received within the specified timeout
*                            OS_ERR_EVENT_TYPE   You didn't pass a pointer to a queue
*                            OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer
*                            OS_ERR_PEND_ISR     If you called this function from an ISR and the result
*                                                would lead to a suspension.
*
* Returns    : != (void *)0  is a pointer to the message received
*              == (void *)0  if no message was received or,
*                            if 'pevent' is a NULL pointer or,
*                            if you didn't pass a pointer to a queue.
                                                 �ȴ���Ϣ�����е���Ϣ
�������ȴ���Ϣ�еĶ���
������pevent��ָ���¼����ƿ���Ŀ����е�ָ��
                timeout����ʱʱ��ѡ���ʱ�ӽ���������������㣬���񽫰��մ�
                             �����Ķ�ʱ�ڶ����еȴ���Ϣ�������������Ϊ�㣬������Ŀ��
                             ��������Զ�ȴ���ֱ����Ϣ������
                err��ָ����ܵĴ�����Ϣ��ָ�룬����Ϊ��
*                            OS_NO_ERR        ���óɹ���������յ���Ϣ
*                            OS_TIMEOUT         ��ʱʱ������Ϣû����
*                            OS_ERR_EVENT_TYPE   ��û�д���ָ�뵽����
*                            OS_ERR_PEVENT_NULL  ��� 'pevent' ��һ����ָ��
*                            OS_ERR_PEND_ISR    �����ISR�е��ã�����������쳣              
*********************************************************************************************************
*/

void  *OSQPend (OS_EVENT *pevent, INT16U timeout, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif
    void      *msg;
    OS_Q      *pq;


    if (OSIntNesting > 0) {                      /* See if called from ISR ...                         */
        *err = OS_ERR_PEND_ISR;                  /* ... can't PEND from an ISR                         */
        return ((void *)0);//������ISR�й���
    }
#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {               /* Validate 'pevent'                                  */
        *err = OS_ERR_PEVENT_NULL;
        return ((void *)0);//�������pevent
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {/* Validate event block type                          */
        *err = OS_ERR_EVENT_TYPE;
        return ((void *)0);//�����¼�������
    }
#endif
    OS_ENTER_CRITICAL();
    pq = (OS_Q *)pevent->OSEventPtr;             /* Point at queue control block                       */
	//ȡ�����п��ƿ�ָ��
    if (pq->OSQEntries > 0) {                    /* See if any messages in the queue                   */
//�����㣬��ʾ����Ϣ�����á���ʱ��OSQPend�õ��¼����ƿ��.OSQOut����ָ
//�����Ϣ����ָ����Ϣ��ָ�븴�Ƶ�msg�����У�����.OSQOutָ��ָ��
//�����е��¸���Ԫ
        msg = *pq->OSQOut++;                     /* Yes, extract oldest message from the queue         */
        pq->OSQEntries--;                        /* Update the number of entries in the queue          */
// ��������Ч��Ϣ��һ
        if (pq->OSQOut == pq->OSQEnd) {          /* Wrap OUT pointer if we are at the end of the queue */
            pq->OSQOut = pq->OSQStart;
        }//��Ϣ������һ��ѭ������������������˶�������ĩһ����Ԫ������
        //����Խ��ʱ����Ҫ��.OSQOut���µ�����ָ����е���ʼ��Ԫ
        OS_EXIT_CRITICAL();
        *err = OS_NO_ERR;
        return (msg);  /* Return message received      *///���ؽ��յ�����Ϣ
    }
    OSTCBCur->OSTCBStat |= OS_STAT_Q;            /* Task will have to pend for a message to be posted  */
	//���������TCB״̬��־���Ա����ȴ���Ϣ������Ϣ�����񱻹���
    OSTCBCur->OSTCBDly   = timeout;              /* Load timeout into TCB                              */
	//װ�ض�ʱ����TCB
    OS_EventTaskWait(pevent);                    /* Suspend task until event or timeout occurs         */
	//��������ֱ����Ϣ�������߳�ʱ�����û������У����ô˺���������
	//����֪����Ϣû�е���֮ǰ�Լ������𣬶��н��յ�һ����Ϣ���߳�ʱ
	//ʱ���˺����ͻ���õ��Ⱥ����ָ����С�
    OS_EXIT_CRITICAL();
    OS_Sched();                                  /* Find next highest priority task ready to run       */
	//�����Ҫ����������ȡ�
    OS_ENTER_CRITICAL();
    msg = OSTCBCur->OSTCBMsg;//������Ⱥ��������˺����ǲ��ǽ���Ϣ�ŵ���
    //�����TCB�С�
    if (msg != (void *)0) {                      /* Did we get a message?                              */
		//�����Ϣȷʵ���ڡ�
        OSTCBCur->OSTCBMsg      = (void *)0;     /* Extract message from TCB (Put there by QPost)      */
		//�������Ϣ��
        OSTCBCur->OSTCBStat     = OS_STAT_RDY;//����
        OSTCBCur->OSTCBEventPtr = (OS_EVENT *)0; /* No longer waiting for event                        */
		//���ٵȴ��¼���
        OS_EXIT_CRITICAL();
        *err                    = OS_NO_ERR;
        return (msg);                            /* Return message received                            */
    }
    OS_EventTO(pevent);                          /* Timed out                                          */
	//��ʱ�ˡ�
    OS_EXIT_CRITICAL();
    *err = OS_TIMEOUT;                           /* Indicate a timeout occured       */
    return ((void *)0);                          /* No message received      */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        POST MESSAGE TO A QUEUE
*
* Description: This function sends a message to a queue
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              msg           is a pointer to the message to send.  You MUST NOT send a NULL pointer.
*
* Returns    : OS_NO_ERR             The call was successful and the message was sent
*              OS_Q_FULL             If the queue cannot accept any more messages because it is full.
*              OS_ERR_EVENT_TYPE     If you didn't pass a pointer to a queue.
*              OS_ERR_PEVENT_NULL    If 'pevent' is a NULL pointer
*              OS_ERR_POST_NULL_PTR  If you are attempting to post a NULL pointer
                                             ����Ϣ���з���һ����Ϣ��FIFO��
����������һ����Ϣ������
������pevent��ָ���¼����ƿ�����Ŀ����е�ָ��
                msg��ָ��Ҫ���͵���Ϣ�����ܷ���NULL
���أ�OS_NO_ERR            ��Ϣ�ɹ�����
*              OS_Q_FULL             ����������ˣ����ܽ�����Ϣ��
*              OS_ERR_EVENT_TYPE     ���û�з�����Ϣ������
*              OS_ERR_PEVENT_NULL    ��� 'pevent' �ǿ�ָ��
*              OS_ERR_POST_NULL_PTR  ����㷢�Ϳ���Ϣ
*********************************************************************************************************
*/

#if OS_Q_POST_EN > 0
INT8U  OSQPost (OS_EVENT *pevent, void *msg)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif
    OS_Q      *pq;


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {                    /* Validate 'pevent'                             */
        return (OS_ERR_PEVENT_NULL);
    }//�������pevent
    if (msg == (void *)0) {                           /* Make sure we are not posting a NULL pointer   */
        return (OS_ERR_POST_NULL_PTR);
    }//��֤û�з��Ϳ�ָ��
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {     /* Validate event block type                     */
        return (OS_ERR_EVENT_TYPE);
    }//�Ƿ����¼�������
#endif
    OS_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0x00) {                 /* See if any task pending on queue              */
//����������ڵ�
        OS_EventTaskRdy(pevent, msg, OS_STAT_Q);      /* Ready highest priority task waiting on event  */
//ʹ���¼��еȴ���������ȼ��������
        OS_EXIT_CRITICAL();
        OS_Sched();                                   /* Find highest priority task ready to run       */
		//�������
        return (OS_NO_ERR);
    }
    pq = (OS_Q *)pevent->OSEventPtr;                  /* Point to queue control block                  */
    if (pq->OSQEntries >= pq->OSQSize) {              /* Make sure queue is not full                   */
        OS_EXIT_CRITICAL();
        return (OS_Q_FULL);
    }
    *pq->OSQIn++ = msg;                               /* Insert message into queue                     */
	//����Ϣ����
    pq->OSQEntries++;                                 /* Update the nbr of entries in the queue        */
	//��������Ϣ��
    if (pq->OSQIn == pq->OSQEnd) {                    /* Wrap IN ptr if we are at end of queue         */
        pq->OSQIn = pq->OSQStart;//������Ϣ����ѭ������������Խ��ʱ������ָ���µĿ�ͷ
    }
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                   POST MESSAGE TO THE FRONT OF A QUEUE
*
* Description: This function sends a message to a queue but unlike OSQPost(), the message is posted at
*              the front instead of the end of the queue.  Using OSQPostFront() allows you to send
*              'priority' messages.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              msg           is a pointer to the message to send.  You MUST NOT send a NULL pointer.
*
* Returns    : OS_NO_ERR             The call was successful and the message was sent
*              OS_Q_FULL             If the queue cannot accept any more messages because it is full.
*              OS_ERR_EVENT_TYPE     If you didn't pass a pointer to a queue.
*              OS_ERR_PEVENT_NULL    If 'pevent' is a NULL pointer
*              OS_ERR_POST_NULL_PTR  If you are attempting to post to a non queue.
                             ����Ϣ���з���һ����Ϣ��LIFO��
����������һ����Ϣ������OSQPost()��ͬ����Ϣ��ָ��ǰ�˶����Ƕ��еĺ�ˣ�
                //�ô˺��������㷢�����ȼ���Ϣ��
������pevent��ָ���¼����ƿ�����Ŀ����е�ָ��
                smg:ָ����Ҫ���͵���Ϣ�����ܷ��Ϳ�ָ��
���أ�OS_NO_ERR             ���óɹ�����Ϣ�ɹ�����
*              OS_Q_FULL             ����������ˣ����ܽ�����Ϣ��
*              OS_ERR_EVENT_TYPE     ���û�з�����Ϣ������
*              OS_ERR_PEVENT_NULL   ��� 'pevent' �ǿ�ָ��
*              OS_ERR_POST_NULL_PTR  ������뷢�͵��Ĳ��Ƕ���
*********************************************************************************************************
*/

#if OS_Q_POST_FRONT_EN > 0
INT8U  OSQPostFront (OS_EVENT *pevent, void *msg)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif
    OS_Q      *pq;


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {                    /* Validate 'pevent'                             */
        return (OS_ERR_PEVENT_NULL);
    }}//�������pevent
    if (msg == (void *)0) {                           /* Make sure we are not posting a NULL pointer   */
        return (OS_ERR_POST_NULL_PTR);
    }//��֤û�з��Ϳ�ָ��
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {     /* Validate event block type                     */
        return (OS_ERR_EVENT_TYPE);
    }//�Ƿ����¼�������
#endif
    OS_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0x00) {                 /* See if any task pending on queue              */
        OS_EventTaskRdy(pevent, msg, OS_STAT_Q);      /* Ready highest priority task waiting on event  */
        OS_EXIT_CRITICAL();
        OS_Sched();                                   /* Find highest priority task ready to run       */
        return (OS_NO_ERR);//���ͼ��ϸ�����
    }
    pq = (OS_Q *)pevent->OSEventPtr;                  /* Point to queue control block                  */
	//װ�ض��п��ƿ�ָ��
    if (pq->OSQEntries >= pq->OSQSize) {  /* Make sure queue is not full                   */
		//��֤���л�û����
        OS_EXIT_CRITICAL();
        return (OS_Q_FULL);
    }//OSQOutָ��ָ������Ѿ���������Ϣָ��ĵ�Ԫ�������ڲ����µ���Ϣָ��ǰ��
    //Ҫ��OSQOutָ������Ϣ������ǰ��һ����Ԫ��
    if (pq->OSQOut == pq->OSQStart) {                 /* Wrap OUT ptr if we are at the 1st queue entry */
        pq->OSQOut = pq->OSQEnd;//����ѭ�������������pq->OSQOutָ��ĵ�ǰ�����Ƕ�
        //���еĵ�һ����Ԫ������ǰ�ƵĻ�����ζ��ָ���˶��е�ĩβ
    }
    pq->OSQOut--;//OSQEndָ�������Ϣ���������һ����Ԫ����һ����Ԫ�����ԣ�
    //Ҫ���������ָ����е���Ч��Χ�ڡ���ΪOSQPend��������ȡ������Ϣ
    //���ɴ˺����ող���ģ���������ʵ���˺���ȳ���
    *pq->OSQOut = msg;                                /* Insert message into queue                     */
    pq->OSQEntries++;                                 /* Update the nbr of entries in the queue        */
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                        POST MESSAGE TO A QUEUE
*
* Description: This function sends a message to a queue.  This call has been added to reduce code size
*              since it can replace both OSQPost() and OSQPostFront().  Also, this function adds the
*              capability to broadcast a message to ALL tasks waiting on the message queue.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              msg           is a pointer to the message to send.  You MUST NOT send a NULL pointer.
*
*              opt           determines the type of POST performed:
*                            OS_POST_OPT_NONE         POST to a single waiting task
*                                                     (Identical to OSQPost())
*                            OS_POST_OPT_BROADCAST    POST to ALL tasks that are waiting on the queue
*                            OS_POST_OPT_FRONT        POST as LIFO (Simulates OSQPostFront())
*
*                            Below is a list of ALL the possible combination of these flags:
*
*                                 1) OS_POST_OPT_NONE
*                                    identical to OSQPost()
*
*                                 2) OS_POST_OPT_FRONT
*                                    identical to OSQPostFront()
*
*                                 3) OS_POST_OPT_BROADCAST
*                                    identical to OSQPost() but will broadcast 'msg' to ALL waiting tasks
*
*                                 4) OS_POST_OPT_FRONT + OS_POST_OPT_BROADCAST  is identical to
*                                    OSQPostFront() except that will broadcast 'msg' to ALL waiting tasks
*
* Returns    : OS_NO_ERR             The call was successful and the message was sent
*              OS_Q_FULL             If the queue cannot accept any more messages because it is full.
*              OS_ERR_EVENT_TYPE     If you didn't pass a pointer to a queue.
*              OS_ERR_PEVENT_NULL    If 'pevent' is a NULL pointer
*              OS_ERR_POST_NULL_PTR  If you are attempting to post a NULL pointer
*
* Warning    : Interrupts can be disabled for a long time if you do a 'broadcast'.  In fact, the
*              interrupt disable time is proportional to the number of tasks waiting on the queue.
                              ����Ϣ���з���һ����Ϣ��LIFO����FIFO��   
�������Կ�������ģ������ķ�ʽ����Ϣ���з���Ϣ�����Դ���LIFO����FIFO
               ��������Ϣ��������Ϣ�����еȴ���Ϣ�����񣨹㲥��ʽ��
������pevent��ָ���¼����ƿ�����Ŀ����е�ָ��
                smg:ָ����Ҫ���͵���Ϣ�����ܷ��Ϳ�ָ��
                opt:ȷ�����ͷ�ʽ��
*                            OS_POST_OPT_NONE        ���͸����������൱��OSQPost()
*                            OS_POST_OPT_BROADCAST   ���͸����������еȴ�������
*                            OS_POST_OPT_FRONT        ��LIFO ����(�� OSQPostFront()����)
                 ���������б�־���ܵĽ�������  
*                                 1) OS_POST_OPT_NONE
*                                    �൱�� OSQPost()
*
*                                 2) OS_POST_OPT_FRONT
*                                    �൱�� OSQPostFront()
*
*                                 3) OS_POST_OPT_BROADCAST
*                                    �൱�� OSQPost() �������Թ㲥��ʽ���� to ALL waiting tasks
*
*                                 4) OS_POST_OPT_FRONT + OS_POST_OPT_BROADCAST  �൱��
*                                    OSQPostFront() �����Թ㲥��ʽ����
*********************************************************************************************************
*/

#if OS_Q_POST_OPT_EN > 0
INT8U  OSQPostOpt (OS_EVENT *pevent, void *msg, INT8U opt)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif
    OS_Q      *pq;


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {                    /* Validate 'pevent'                             */
        return (OS_ERR_PEVENT_NULL);
    }//�Ƿ�pevent 
    if (msg == (void *)0) {                           /* Make sure we are not posting a NULL pointer   */
        return (OS_ERR_POST_NULL_PTR);
    }//��֤û���Ϳ�ָ��
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {     /* Validate event block type                     */
        return (OS_ERR_EVENT_TYPE);
    }//�����¼�������
#endif
    OS_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0x00) {                 /* See if any task pending on queue              */
		//����������ڶ����й���
        if ((opt & OS_POST_OPT_BROADCAST) != 0x00) {  /* Do we need to post msg to ALL waiting tasks ? */
			//�����������ͣ�
            while (pevent->OSEventGrp != 0x00) {      /* Yes, Post to ALL tasks waiting on queue       */
                OS_EventTaskRdy(pevent, msg, OS_STAT_Q);
            }//�ǵġ������opt�����е�OS_POST_OPT_BROADCASTλ��Ϊ1�����������ڵȴ���Ϣ
            //�������ܽ��յ�������Ϣ�����ұ�OS_EventTaskRdy�ӵȴ��б���ɾ����
        } else {//��Ҫ�����еĵȴ�������Ϣ
            OS_EventTaskRdy(pevent, msg, OS_STAT_Q);  /* No,  Post to HPT waiting on queue             */
			//�Ǿ�ֻ�����ȼ���ߵġ�
        }
        OS_EXIT_CRITICAL();
        OS_Sched();                                   /* Find highest priority task ready to run       */
		//Ҫ�������
        return (OS_NO_ERR);
    }
    pq = (OS_Q *)pevent->OSEventPtr;                  /* Point to queue control block                  */
	//ָ����п��ƿ�
    if (pq->OSQEntries >= pq->OSQSize) {              /* Make sure queue is not full                   */
        OS_EXIT_CRITICAL();//������ˣ����ش������
        return (OS_Q_FULL);
    }
    if ((opt & OS_POST_OPT_FRONT) != 0x00) {          /* Do we post to the FRONT of the queue?         */
		//FIFO����LIFO�������LIFO
        if (pq->OSQOut == pq->OSQStart) {             /* Yes, Post as LIFO, Wrap OUT pointer if we ... */
            pq->OSQOut = pq->OSQEnd;                  /*      ... are at the 1st queue entry           */
        }//����Ǻ���ȳ������൱��OSQPostFront ����������ڶ��е�һ���������
        pq->OSQOut--;
        *pq->OSQOut = msg;                            /*      Insert message into queue                */
		//������Ϣ
    } else {                                          /* No,  Post as FIFO                             */
    //�����FIFO
        *pq->OSQIn++ = msg;                           /*      Insert message into queue                */
        if (pq->OSQIn == pq->OSQEnd) {                /*      Wrap IN ptr if we are at end of queue    */
            pq->OSQIn = pq->OSQStart;//�������ĩβ�������
        }
    }
    pq->OSQEntries++;                                 /* Update the nbr of entries in the queue        */
	//������Ϣ����
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                        QUERY A MESSAGE QUEUE
*
* Description: This function obtains information about a message queue.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              pdata         is a pointer to a structure that will contain information about the message
*                            queue.
*
* Returns    : OS_NO_ERR           The call was successful and the message was sent
*              OS_ERR_EVENT_TYPE   If you are attempting to obtain data from a non queue.
*              OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer
                                       ��ȡ��Ϣ����״̬
��������ȡ��Ϣ���е���Ϣ
������pevent��ָ���¼����ƿ���Ŀ����е�ָ��
                pdata��ָ����Ϣ���а�����Ϣ�Ľṹָ��
���أ�OS_NO_ERR           ���óɹ�����Ϣ�ɹ�����
*              OS_ERR_EVENT_TYPE   �����ȡ�Ƕ��е���Ϣ
*              OS_ERR_PEVENT_NULL  ��� 'pevent' ��һ��NULLָ��
*********************************************************************************************************
*/

#if OS_Q_QUERY_EN > 0
INT8U  OSQQuery (OS_EVENT *pevent, OS_Q_DATA *pdata)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif
    OS_Q      *pq;
    INT8U     *psrc;
    INT8U     *pdest;


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) {                         /* Validate 'pevent'     */
        return (OS_ERR_PEVENT_NULL);
    }//�Ƿ�pevent 
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {          /* Validate event block type   */
        return (OS_ERR_EVENT_TYPE);
    }//��֤û���Ϳ�ָ��
#endif
    OS_ENTER_CRITICAL();
//���Ƶȴ������б�
    pdata->OSEventGrp = pevent->OSEventGrp;           /* Copy message queue wait list  */
    psrc              = &pevent->OSEventTbl[0];
    pdest             = &pdata->OSEventTbl[0];
#if OS_EVENT_TBL_SIZE > 0//��0��ʼ���������ưɡ�
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
    pq = (OS_Q *)pevent->OSEventPtr;
    if (pq->OSQEntries > 0) {//�������Ϣ�ȴ�������ȡ������û��ɾ�����������������
    //���е���Ϣ�������临�Ƶ�OSMsg�У�OSQQuery��û�иĶ�.OSQOut��ָ�롣
        pdata->OSMsg = *pq->OSQOut;                        /* Get next message to return if available  */
    } else {//���û������ȴ����򷵻ؿ�ָ��
        pdata->OSMsg = (void *)0;
    }
    pdata->OSNMsgs = pq->OSQEntries;//��������Ϣ��
    pdata->OSQSize = pq->OSQSize;//�������״�С
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif                                                     /* OS_Q_QUERY_EN                            */

/*$PAGE*/
/*
*********************************************************************************************************
*                                      QUEUE MODULE INITIALIZATION
*
* Description : This function is called by uC/OS-II to initialize the message queue module.  Your
*               application MUST NOT call this function.
*
* Arguments   :  none
*
* Returns     : none
*
* Note(s)    : This function is INTERNAL to uC/OS-II and your application should not call it.
                                                   ��ʼ����Ϣ����
��������ucos���ó�ʼ����Ϣ����ģ�ͣ�Ӧ�ó����ܵ���
��������
���أ���
��ע����ucos���ó�ʼ����Ϣ����ģ�ͣ�Ӧ�ó����ܵ���
*********************************************************************************************************
*/

void  OS_QInit (void)
{
#if OS_MAX_QS == 1//���ֻ��һ������
    OSQFreeList         = &OSQTbl[0];            /* Only ONE queue!     */
    OSQFreeList->OSQPtr = (OS_Q *)0;//ǰ��Ϊ��ָ��
#endif

#if OS_MAX_QS >= 2
    INT16U  i;
    OS_Q   *pq1;
    OS_Q   *pq2;


    pq1 = &OSQTbl[0];
    pq2 = &OSQTbl[1];
    for (i = 0; i < (OS_MAX_QS - 1); i++) {      /* Init. list of free QUEUE control blocks            */
		//��ʼ�����ж��п��ƿ飬���һ����������
        pq1->OSQPtr = pq2;
        pq1++;
        pq2++;
    }
    pq1->OSQPtr = (OS_Q *)0;//������ָ��NULLָ��
    OSQFreeList = &OSQTbl[0];//������ָ��ָ���׵�ַ��
#endif
}
#endif                                                     /* OS_Q_EN                                  */
