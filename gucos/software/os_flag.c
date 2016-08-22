/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                         EVENT FLAG  MANAGEMENT
*
*                          (c) Copyright 2001-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* File : OS_FLAG.C
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#ifndef  OS_MASTER_FILE
#include "INCLUDES.H"
#endif

#if (OS_VERSION >= 251) && (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0)
/*
*********************************************************************************************************
*                                            LOCAL PROTOTYPES
                                                    �ֲ�����
*********************************************************************************************************
*/

static  void     OS_FlagBlock(OS_FLAG_GRP *pgrp, OS_FLAG_NODE *pnode, OS_FLAGS flags, INT8U wait_type, INT16U timeout);
static  BOOLEAN  OS_FlagTaskRdy(OS_FLAG_NODE *pnode, OS_FLAGS flags_rdy);

/*$PAGE*/
/*
*********************************************************************************************************
*                              CHECK THE STATUS OF FLAGS IN AN EVENT FLAG GROUP
*
* Description: This function is called to check the status of a combination of bits to be set or cleared
*              in an event flag group.  Your application can check for ANY bit to be set/cleared or ALL
*              bits to be set/cleared.
*
*              This call does not block if the desired flags are not present.
*
* Arguments  : pgrp          is a pointer to the desired event flag group.
*
*              flags         Is a bit pattern indicating which bit(s) (i.e. flags) you wish to check.
*                            The bits you want are specified by setting the corresponding bits in
*                            'flags'.  e.g. if your application wants to wait for bits 0 and 1 then
*                            'flags' would contain 0x03.
*
*              wait_type     specifies whether you want ALL bits to be set/cleared or ANY of the bits
*                            to be set/cleared.
*                            You can specify the following argument:
*
*                            OS_FLAG_WAIT_CLR_ALL   You will check ALL bits in 'flags' to be clear (0)
*                            OS_FLAG_WAIT_CLR_ANY   You will check ANY bit  in 'flags' to be clear (0)
*                            OS_FLAG_WAIT_SET_ALL   You will check ALL bits in 'flags' to be set   (1)
*                            OS_FLAG_WAIT_SET_ANY   You will check ANY bit  in 'flags' to be set   (1)
*
*                            NOTE: Add OS_FLAG_CONSUME if you want the event flag to be 'consumed' by
*                                  the call.  Example, to wait for any flag in a group AND then clear
*                                  the flags that are present, set 'wait_type' to:
*
*                                  OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME
*
*              err           is a pointer to an error code and can be:
*                            OS_NO_ERR              No error
*                            OS_ERR_EVENT_TYPE      You are not pointing to an event flag group
*                            OS_FLAG_ERR_WAIT_TYPE  You didn't specify a proper 'wait_type' argument.
*                            OS_FLAG_INVALID_PGRP   You passed a NULL pointer instead of the event flag
*                                                   group handle.
*                            OS_FLAG_ERR_NOT_RDY    The desired flags you are waiting for are not
*                                                   available.
*
* Returns    : The state of the flags in the event flag group.
*
* Called from: Task or ISR
                                    �޵ȴ��ػ���¼���־���е��¼���־
������ȥ����¼���־���н��λ��״̬����λ���Ǳ����������
                ����κν�����λ���������λ����ȫ��λ
                ����ȴ��¼��������������¼�����������OSFlagPend()Ψһ��ͬ��
������pgrp��ָ��Ŀ���¼���־���ָ��
                flags����һ��λ��ģʽ��ʾҪ����λ�����磺��Ҫ���0λ��1λ����ô�㽫��
                           ����Ϊ0x03
                wait_type ���������ȫ����⻹��ֻ������ʵһ���֡����ܱ�����²�����
*                            OS_FLAG_WAIT_CLR_ALL   �㽫���flags��ȫ��������λ
*                            OS_FLAG_WAIT_CLR_ANY   �㽫���flags���κ�����λ
*                            OS_FLAG_WAIT_SET_ALL   �㽫���flags��ȫ������һλ
*                            OS_FLAG_WAIT_SET_ANY   �㽫���flags���κ���һλ
               ������¼���־�����ú�������Ļ���Ҫ����OS_FLAG_CONSUME�����磺���
               ��Ҫ������������ô��wait_type���ó�OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME
               err ָ���������ָ�룬����Ϊ��
*                            OS_NO_ERR             ����
*                            OS_ERR_EVENT_TYPE      û��ָ�������¼���־��
*                            OS_FLAG_ERR_WAIT_TYPE  ��û��������ȷ�� 'wait_type' ����
*                            OS_FLAG_INVALID_PGRP   �㴫������ָ��������¼���־�����
*                            OS_FLAG_ERR_NOT_RDY    ��ȴ���Ŀ���־������
���أ��¼���־��ı�־״̬
�������ISR�е���
*********************************************************************************************************
*/

#if OS_FLAG_ACCEPT_EN > 0
OS_FLAGS  OSFlagAccept (OS_FLAG_GRP *pgrp, OS_FLAGS flags, INT8U wait_type, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR     cpu_sr;
#endif
    OS_FLAGS      flags_cur;
    OS_FLAGS      flags_rdy;
    BOOLEAN       consume;


#if OS_ARG_CHK_EN > 0
    if (pgrp == (OS_FLAG_GRP *)0) {                        /* Validate 'pgrp'                          */
        *err = OS_FLAG_INVALID_PGRP;
        return ((OS_FLAGS)0);//�ǿյ�pgrp'
    }
    if (pgrp->OSFlagType != OS_EVENT_TYPE_FLAG) {          /* Validate event block type                */
        *err = OS_ERR_EVENT_TYPE;
        return ((OS_FLAGS)0);//������¼����ƿ�����
    }
#endif
    if (wait_type & OS_FLAG_CONSUME) {                     /* See if we need to consume the flags      */
        wait_type &= ~OS_FLAG_CONSUME;
        consume    = TRUE;
    } else {
        consume    = FALSE;
    }//�Ƿ���Ҫ���λ
/*$PAGE*/
    *err = OS_NO_ERR;                                      /* Assume NO error until proven otherwise.  */
//��ʼ���޴�
    OS_ENTER_CRITICAL();
    switch (wait_type) {
        case OS_FLAG_WAIT_SET_ALL:                         /* See if all required flags are set        */
             flags_rdy = pgrp->OSFlagFlags & flags;        /* Extract only the bits we want            */
             if (flags_rdy == flags) {                     /* Must match ALL the bits that we want     */
                 //����պ�������Ҫ��
                 if (consume == TRUE) {                    /* See if we need to consume the flags      */
                     pgrp->OSFlagFlags &= ~flags_rdy;      /* Clear ONLY the flags that we wanted      */
                 }//�Ƿ�Ҫ���
             } else {
                 *err  = OS_FLAG_ERR_NOT_RDY;//û��������Ҫ�ģ����ز�����
             }
             flags_cur = pgrp->OSFlagFlags;                /* Will return the state of the group       */
			 //���ر�־
             OS_EXIT_CRITICAL();
             break;

        case OS_FLAG_WAIT_SET_ANY:
             flags_rdy = pgrp->OSFlagFlags & flags;        /* Extract only the bits we want            */
             if (flags_rdy != (OS_FLAGS)0) {               /* See if any flag set                      */
			 	//�Ƿ��б�־��λ
                 if (consume == TRUE) {                    /* See if we need to consume the flags      */
                     pgrp->OSFlagFlags &= ~flags_rdy;      /* Clear ONLY the flags that we got         */
                 }//�Ƿ���Ҫ���λ
             } else {//û�б�־��λ
                 *err  = OS_FLAG_ERR_NOT_RDY;
             }
             flags_cur = pgrp->OSFlagFlags;                /* Will return the state of the group       */
			 //������״̬��
             OS_EXIT_CRITICAL();
             break;

#if OS_FLAG_WAIT_CLR_EN > 0
        case OS_FLAG_WAIT_CLR_ALL:                         /* See if all required flags are cleared    */
             flags_rdy = ~pgrp->OSFlagFlags & flags;       /* Extract only the bits we want            */
			 //����λ
             if (flags_rdy == flags) {                     /* Must match ALL the bits that we want     */
			 	//�������е�λ��ƥ��
                 if (consume == TRUE) {                    /* See if we need to consume the flags      */
                     pgrp->OSFlagFlags |= flags_rdy;       /* Set ONLY the flags that we wanted        */
                 }//�Ƿ�Ҫ����λ
             } else {
                 *err  = OS_FLAG_ERR_NOT_RDY;//��������λ��ƥ��
             }
             flags_cur = pgrp->OSFlagFlags;                /* Will return the state of the group       */
			 //���ر�־��״̬
             OS_EXIT_CRITICAL();
             break;

        case OS_FLAG_WAIT_CLR_ANY:
             flags_rdy = ~pgrp->OSFlagFlags & flags;       /* Extract only the bits we want            */
             if (flags_rdy != (OS_FLAGS)0) {               /* See if any flag cleared                  */
                 if (consume == TRUE) {                    /* See if we need to consume the flags      */
                     pgrp->OSFlagFlags |= flags_rdy;       /* Set ONLY the flags that we got           */
                 }
             } else {
                 *err  = OS_FLAG_ERR_NOT_RDY;
             }
             flags_cur = pgrp->OSFlagFlags;                /* Will return the state of the group       */
             OS_EXIT_CRITICAL();
             break;
#endif

        default://�����쳣���
             OS_EXIT_CRITICAL();
             flags_cur = (OS_FLAGS)0;
             *err      = OS_FLAG_ERR_WAIT_TYPE;
             break;
    }
    return (flags_cur);//����״̬
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                           CREATE AN EVENT FLAG
*
* Description: This function is called to create an event flag group.
*
* Arguments  : flags         Contains the initial value to store in the event flag group.
*
*              err           is a pointer to an error code which will be returned to your application:
*                               OS_NO_ERR                if the call was successful.
*                               OS_ERR_CREATE_ISR        if you attempted to create an Event Flag from an
*                                                        ISR.
*                               OS_FLAG_GRP_DEPLETED     if there are no more event flag groups
*
* Returns    : A pointer to an event flag group or a NULL pointer if no more groups are available.
*
* Called from: Task ONLY
                                                     ����һ���¼���־��
����������һ���¼���־��
������flags�������洢���¼���־���еĳ�ʼֵ
                err�������ص���Ӧ�ó���Ĵ�����Ϣ
 *                               OS_NO_ERR                ����ɹ�
*                               OS_ERR_CREATE_ISR       ��������ISR�н���
*                               OS_FLAG_GRP_DEPLETED    ���û�ж�����¼���־����               
*********************************************************************************************************
*/

OS_FLAG_GRP  *OSFlagCreate (OS_FLAGS flags, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                         /* Allocate storage for CPU status register        */
    OS_CPU_SR    cpu_sr;
#endif
    OS_FLAG_GRP *pgrp;


    if (OSIntNesting > 0) {                         /* See if called from ISR ...                      */
        *err = OS_ERR_CREATE_ISR;                   /* ... can't CREATE from an ISR                    */
        return ((OS_FLAG_GRP *)0);//���ܴ�ISR�н���
    }
    OS_ENTER_CRITICAL();
    pgrp = OSFlagFreeList;                          /* Get next free event flag                        */
	//ָ�������Ŀյ�ַ ��ȡ��һ�������¼���־
    if (pgrp != (OS_FLAG_GRP *)0) {                 /* See if we have event flag groups available      */
                  /* Adjust free list       *///���Ϊ0������û�п��е��¼���־����
        OSFlagFreeList       = (OS_FLAG_GRP *)OSFlagFreeList->OSFlagWaitList;
		//����ϵͳ�Ŀ����¼���־������ָ�룬ʹָ֮���µı�ͷ
        pgrp->OSFlagType     = OS_EVENT_TYPE_FLAG;  /* Set to event flag group type                    */
		//���������¼���־�飬ȷ����ϵͳ��������
        pgrp->OSFlagFlags    = flags;               /* Set to desired initial value                    */
		//����ʼ��ֵ��������¼���־��
        pgrp->OSFlagWaitList = (void *)0;           /* Clear list of tasks waiting on flags            */
		//��Ϊ�Ǹոս������¼���־�飬û���κ�����ȴ�����¼���־�飬
		//���Եȴ���������ָ���ʼ��Ϊ0
        OS_EXIT_CRITICAL();
        *err                 = OS_NO_ERR;
    } else {//���û�п��е��¼���־����
        OS_EXIT_CRITICAL();
        *err                 = OS_FLAG_GRP_DEPLETED;
    }
    return (pgrp);                                  /* Return pointer to event flag group     */
	//���ظոս������¼���־��ָ�룬���û�п��е��¼���־�飬
	//������NULLָ��
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                     DELETE AN EVENT FLAG GROUP
*
* Description: This function deletes an event flag group and readies all tasks pending on the event flag
*              group.
* Arguments  : pgrp          is a pointer to the desired event flag group.
*
*              opt           determines delete options as follows:
*                            opt == OS_DEL_NO_PEND   Deletes the event flag group ONLY if no task pending
*                            opt == OS_DEL_ALWAYS    Deletes the event flag group even if tasks are
*                                                    waiting.  In this case, all the tasks pending will be
*                                                    readied.
*
*              err           is a pointer to an error code that can contain one of the following values:
*                            OS_NO_ERR               The call was successful and the event flag group was
*                                                    deleted
*                            OS_ERR_DEL_ISR          If you attempted to delete the event flag group from
*                                                    an ISR
*                            OS_FLAG_INVALID_PGRP    If 'pgrp' is a NULL pointer.
*                            OS_ERR_EVENT_TYPE       If you didn't pass a pointer to an event flag group
*                            OS_ERR_INVALID_OPT      An invalid option was specified
*                            OS_ERR_TASK_WAITING     One or more tasks were waiting on the event flag
*                                                    group.
* Returns    : pevent        upon error
*              (OS_EVENT *)0 if the semaphore was successfully deleted.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of
*                 the event flag group MUST check the return code of OSFlagAccept() and OSFlagPend().
*              2) This call can potentially disable interrupts for a long time.  The interrupt disable
*                 time is directly proportional to the number of tasks waiting on the event flag group.
                                                                ɾ��һ���¼���־��
������ɾ��һ���¼���־�飬���¼���־�������й�������������
������pgrp��ָ��Ŀ���¼���־���ָ��
                opt����������ɾ��ѡ�
*                            opt == OS_DEL_NO_PEND   û����������ʱ���ɾ�¼���־��
*                            opt == OS_DEL_ALWAYS    ��ʹ������ȴ�Ҳɾ������������й����
                                               ���񽫾�����
 *              err           �������´�����Ϣ֮һ��ָ��
*                            OS_NO_ERR              ���óɹ����¼���־��ɹ�ɾ��
*                            OS_ERR_DEL_ISR     ������ISR��ɾ���¼���־��
*                            OS_FLAG_INVALID_PGRP    ���pgrp��һ����ָ��
*                            OS_ERR_EVENT_TYPE      ���û�д���ָ�뵽�¼���־��
*                            OS_ERR_INVALID_OPT      �зǷ�ѡ��
*                            OS_ERR_TASK_WAITING     һ�������������¼���־���еȴ�
*����    : pevent        �д�
*              (OS_EVENT *)0 ����ɹ�ɾ��

                                               
*********************************************************************************************************
*/

#if OS_FLAG_DEL_EN > 0
OS_FLAG_GRP  *OSFlagDel (OS_FLAG_GRP *pgrp, INT8U opt, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR     cpu_sr;
#endif
    BOOLEAN       tasks_waiting;
    OS_FLAG_NODE *pnode;


    if (OSIntNesting > 0) {                                /* See if called from ISR ...               */
        *err = OS_ERR_DEL_ISR;                             /* ... can't DELETE from an ISR             */
        return (pgrp);//�������ISR��ɾ��
    }
#if OS_ARG_CHK_EN > 0
    if (pgrp == (OS_FLAG_GRP *)0) {                        /* Validate 'pgrp'                          */
        *err = OS_FLAG_INVALID_PGRP;
        return (pgrp);//pgrp������Ч������Ϊ�㣬��ָ���¼���־��
    }
    if (pgrp->OSFlagType != OS_EVENT_TYPE_FLAG) {          /* Validate event group type                */
        *err = OS_ERR_EVENT_TYPE;
        return (pgrp);//��������¼�������
    }
#endif
    OS_ENTER_CRITICAL();
    if (pgrp->OSFlagWaitList != (void *)0) {     /* See if any tasks waiting on event flags  */
		//�Ƿ��������ڴ˵ȴ���
        tasks_waiting = TRUE;                              /* Yes                                      */
    } else {//
        tasks_waiting = FALSE;                             /* No                                       */
    }
    switch (opt) {
        case OS_DEL_NO_PEND:    /* Delete group if no task waiting    */
             //���û������ȴ���ɾ��
             if (tasks_waiting == FALSE) {//û������ȴ�
                 pgrp->OSFlagType     = OS_EVENT_TYPE_UNUSED;//�������Ϊδʹ��
                 pgrp->OSFlagWaitList = (void *)OSFlagFreeList; /* Return group to free list           */
				 //���䷵�ص�����������
                 OSFlagFreeList       = pgrp;
                 OS_EXIT_CRITICAL();
                 *err                 = OS_NO_ERR;//�޴�
                 return ((OS_FLAG_GRP *)0);                /* Event Flag Group has been deleted        */
             } else {//������ȴ�
                 OS_EXIT_CRITICAL();
                 *err                 = OS_ERR_TASK_WAITING;
                 return (pgrp);//�����¼���־��ָ��
             }

        case OS_DEL_ALWAYS:                                /* Always delete the event flag group       */
			//������ȴ�Ҳɾ��
             pnode = (OS_FLAG_NODE *)pgrp->OSFlagWaitList;//ָ���¼���־�еȴ��������ָ��
             while (pnode != (OS_FLAG_NODE *)0) {          /* Ready ALL tasks waiting for flags        */
			 	//�������е�����
                 OS_FlagTaskRdy(pnode, (OS_FLAGS)0);//ʹ����������¼�����
                 pnode = (OS_FLAG_NODE *)pnode->OSFlagNodeNext;//ֻҪ��Ϊ�㣬�ͼ���ָ����һ����㡣��������
             }
             pgrp->OSFlagType     = OS_EVENT_TYPE_UNUSED;//���Ϊδ��
             pgrp->OSFlagWaitList = (void *)OSFlagFreeList;/* Return group to free list                */
             OSFlagFreeList       = pgrp;//���䷵�ص���������
             OS_EXIT_CRITICAL();
             if (tasks_waiting == TRUE) {                  /* Reschedule only if task(s) were waiting  */
			 	//������ȴ�������£�Ҫ����������ȡ���Ϊ�������������ˡ�
                 OS_Sched();                               /* Find highest priority task ready to run  */
             }
             *err = OS_NO_ERR;//�޴�
             return ((OS_FLAG_GRP *)0);                    /* Event Flag Group has been deleted        */

        default://�����쳣���
             OS_EXIT_CRITICAL();
             *err = OS_ERR_INVALID_OPT;
             return (pgrp);
    }
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                        WAIT ON AN EVENT FLAG GROUP
*
* Description: This function is called to wait for a combination of bits to be set in an event flag
*              group.  Your application can wait for ANY bit to be set or ALL bits to be set.
*
* Arguments  : pgrp          is a pointer to the desired event flag group.
*
*              flags         Is a bit pattern indicating which bit(s) (i.e. flags) you wish to wait for.
*                            The bits you want are specified by setting the corresponding bits in
*                            'flags'.  e.g. if your application wants to wait for bits 0 and 1 then
*                            'flags' would contain 0x03.
*
*              wait_type     specifies whether you want ALL bits to be set or ANY of the bits to be set.
*                            You can specify the following argument:
*
*                            OS_FLAG_WAIT_CLR_ALL   You will wait for ALL bits in 'mask' to be clear (0)
*                            OS_FLAG_WAIT_SET_ALL   You will wait for ALL bits in 'mask' to be set   (1)
*                            OS_FLAG_WAIT_CLR_ANY   You will wait for ANY bit  in 'mask' to be clear (0)
*                            OS_FLAG_WAIT_SET_ANY   You will wait for ANY bit  in 'mask' to be set   (1)
*
*                            NOTE: Add OS_FLAG_CONSUME if you want the event flag to be 'consumed' by
*                                  the call.  Example, to wait for any flag in a group AND then clear
*                                  the flags that are present, set 'wait_type' to:
*
*                                  OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME
*
*              timeout       is an optional timeout (in clock ticks) that your task will wait for the
*                            desired bit combination.  If you specify 0, however, your task will wait
*                            forever at the specified event flag group or, until a message arrives.
*
*              err           is a pointer to an error code and can be:
*                            OS_NO_ERR              The desired bits have been set within the specified
*                                                   'timeout'.
*                            OS_ERR_PEND_ISR        If you tried to PEND from an ISR
*                            OS_FLAG_INVALID_PGRP   If 'pgrp' is a NULL pointer.
*                            OS_ERR_EVENT_TYPE      You are not pointing to an event flag group
*                            OS_TIMEOUT             The bit(s) have not been set in the specified
*                                                   'timeout'.
*                            OS_FLAG_ERR_WAIT_TYPE  You didn't specify a proper 'wait_type' argument.
*
* Returns    : The new state of the flags in the event flag group when the task is resumed or,
*              0 if a timeout or an error occurred.
*
* Called from: Task ONLY
                                                         �ȴ��¼���־����¼���־λ
������ȥ����¼���־���н��λ��״̬����λ���Ǳ����������
                ����κν�����λ���������λ����ȫ��λ 
������pgrp��ָ��Ŀ���¼���־���ָ��
                flags����һ��λ��ģʽ��ʾҪ����λ�����磺��Ҫ���0λ��1λ����ô�㽫��
                           ����Ϊ0x03
                wait_type ���������ȫ����⻹��ֻ������ʵһ���֡����ܱ�����²�����
*                            OS_FLAG_WAIT_CLR_ALL   �ȴ�����ָ���¼���־���е��¼���־λ��0
*                            OS_FLAG_WAIT_CLR_ANY   �ȴ�����һ��ָ���¼���־���е��¼���־λ��1
*                            OS_FLAG_WAIT_SET_ALL   �ȴ�����ָ���¼���־���е��¼���־λ��1
*                            OS_FLAG_WAIT_SET_ANY  �ȴ�����һ��ָ���¼���־���е��¼���־λ��1
               ������¼���־�����ú�������Ļ���Ҫ����OS_FLAG_CONSUME�����磺���
               ��Ҫ������������ô��wait_type���ó�OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME
               timeout:�������ȴ�Ŀ����λ��ʱ��������־�㣬���������ָ��
                         �¼���־�����õȴ�ֱ��һ����Ϣ����
               err ָ���������ָ�룬����Ϊ��
*                            OS_NO_ERR             ����
*                            OS_ERR_PEND_ISR       ������ISR�й���
*                            OS_FLAG_INVALID_PGRP   ��� 'pgrp' ��NULLָ��
*                            OS_ERR_EVENT_TYPE      ��û��ָ�������¼�������
*                            OS_TIMEOUT             ��ָ��ʱ����λû�б���λ
*                            OS_FLAG_ERR_WAIT_TYPE  ��û��ָ�����ʵ� 'wait_type' ����

���أ����������ʱ����־������״̬��������߳�ʱ������
ֻ�ܴ������е���

*********************************************************************************************************
*/

OS_FLAGS  OSFlagPend (OS_FLAG_GRP *pgrp, OS_FLAGS flags, INT8U wait_type, INT16U timeout, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR     cpu_sr;
#endif
    OS_FLAG_NODE  node;
    OS_FLAGS      flags_cur;
    OS_FLAGS      flags_rdy;
    BOOLEAN       consume;


    if (OSIntNesting > 0) {                                /* See if called from ISR ...               */
        *err = OS_ERR_PEND_ISR;                            /* ... can't PEND from an ISR               */
        return ((OS_FLAGS)0);//���������ж�Ƕ���е��ñ�����
    }
#if OS_ARG_CHK_EN > 0//�����麯������
    if (pgrp == (OS_FLAG_GRP *)0) {                        /* Validate 'pgrp'                          */
        *err = OS_FLAG_INVALID_PGRP;
        return ((OS_FLAGS)0);//��pgrp�ǲ���һ��NULLָ��
    }
    if (pgrp->OSFlagType != OS_EVENT_TYPE_FLAG) {          /* Validate event block type                */
        *err = OS_ERR_EVENT_TYPE;
        return ((OS_FLAGS)0);//���Ƿ�ָ���¼���־�����ݽṹ��
    }
#endif
    if (wait_type & OS_FLAG_CONSUME) {                     /* See if we need to consume the flags      */
//���Ƿ���Ҫ�ֲ�����������OSFlagPend ����ָ��������ȴ��¼�����������
//��λ�������Ӧ���¼���־λ���������ڵ��ô˺���ʱ����һ�����������
//wait_type���
        wait_type &= ~OS_FLAG_CONSUME;
        consume    = TRUE;
    } else {
        consume    = FALSE;
    }
/*$PAGE*/
    OS_ENTER_CRITICAL();
    switch (wait_type) {
        case OS_FLAG_WAIT_SET_ALL:                         /* See if all required flags are set        */
             flags_rdy = pgrp->OSFlagFlags & flags;        /* Extract only the bits we want            */
//���wait_typeΪOS_FLAG_WAIT_SET_ALL����OS_FLAG_WAIT_SET_ANDʱ������ȡ����flags����ָ��
//���¼���־λ
             if (flags_rdy == flags) {                     /* Must match ALL the bits that we want     */
	//���ȡ����־λ״̬ǡ�÷���Ԥ��״̬��˵��������Ҫ�ȴ����¼���־λ���Ѿ���λ
	//������£�PEND�����������ص��ú���
                 if (consume == TRUE) {                    /* See if we need to consume the flags      */
                     pgrp->OSFlagFlags &= ~flags_rdy;      /* Clear ONLY the flags that we wanted      */
                 }//����ǰ����ǲ���Ҫ�����������
                 flags_cur = pgrp->OSFlagFlags;            /* Will return the state of the group       */
				 //������״̬
                 OS_EXIT_CRITICAL();                       /* Yes, condition met, return to caller     */
				 //��ȡ�¼���־������¼���־״ֵ̬�������ص��ú���
                 *err      = OS_NO_ERR;
                 return (flags_cur);
             } else {                                      /* Block task until events occur or timeout */
             //���ָ�����¼���־λû����ȫ��λ����ô���ú���������
             //ֱ����ʱ�����¼�����
                 OS_FlagBlock(pgrp, &node, flags, wait_type, timeout);
                 OS_EXIT_CRITICAL();
             }
             break;

        case OS_FLAG_WAIT_SET_ANY:
             flags_rdy = pgrp->OSFlagFlags & flags;        /* Extract only the bits we want            */
			 //����պ�������Ҫ��λ���ȴ������������������ص��ú���
             if (flags_rdy != (OS_FLAGS)0) {               /* See if any flag set                      */
                 if (consume == TRUE) {                    /* See if we need to consume the flags      */
                     pgrp->OSFlagFlags &= ~flags_rdy;      /* Clear ONLY the flags that we got         */
                 }//�Ƿ���Ҫ�����
                 flags_cur = pgrp->OSFlagFlags;            /* Will return the state of the group       */
				 //��ȡ�¼���־���µ�״̬�������ص��ú���
                 OS_EXIT_CRITICAL();                       /* Yes, condition met, return to caller     */
                 *err      = OS_NO_ERR;
                 return (flags_cur);
             } else {                                      /* Block task until events occur or timeout */
                 OS_FlagBlock(pgrp, &node, flags, wait_type, timeout);
                 OS_EXIT_CRITICAL();
             }//������ж�û����λ���򽫵��ú�������ֱ���¼��������߳�ʱ
             break;

#if OS_FLAG_WAIT_CLR_EN > 0
        case OS_FLAG_WAIT_CLR_ALL:                         /* See if all required flags are cleared    */
             flags_rdy = ~pgrp->OSFlagFlags & flags;       /* Extract only the bits we want            */
             if (flags_rdy == flags) {                     /* Must match ALL the bits that we want     */
                 if (consume == TRUE) {                    /* See if we need to consume the flags      */
                     pgrp->OSFlagFlags |= flags_rdy;       /* Set ONLY the flags that we wanted        */
                 }
                 flags_cur = pgrp->OSFlagFlags;            /* Will return the state of the group       */
                 OS_EXIT_CRITICAL();                       /* Yes, condition met, return to caller     */
                 *err      = OS_NO_ERR;
                 return (flags_cur);
             } else {                                      /* Block task until events occur or timeout */
                 OS_FlagBlock(pgrp, &node, flags, wait_type, timeout);
                 OS_EXIT_CRITICAL();
             }
             break;

        case OS_FLAG_WAIT_CLR_ANY:
             flags_rdy = ~pgrp->OSFlagFlags & flags;       /* Extract only the bits we want            */
             if (flags_rdy != (OS_FLAGS)0) {               /* See if any flag cleared                  */
                 if (consume == TRUE) {                    /* See if we need to consume the flags      */
                     pgrp->OSFlagFlags |= flags_rdy;       /* Set ONLY the flags that we got           */
                 }
                 flags_cur = pgrp->OSFlagFlags;            /* Will return the state of the group       */
                 OS_EXIT_CRITICAL();                       /* Yes, condition met, return to caller     */
                 *err      = OS_NO_ERR;
                 return (flags_cur);
             } else {                                      /* Block task until events occur or timeout */
                 OS_FlagBlock(pgrp, &node, flags, wait_type, timeout);
                 OS_EXIT_CRITICAL();
             }
             break;
#endif

        default://�������
             OS_EXIT_CRITICAL();
             flags_cur = (OS_FLAGS)0;
             *err      = OS_FLAG_ERR_WAIT_TYPE;//����ĵȴ�����
             return (flags_cur);
    }
    OS_Sched();   /* Find next HPT ready to run  *///os_FlagBlock()����ʱ��������������
    //����Ϊ���ô�����ĺ�����Ϊ�¼�û�з��������ܼ�������
    OS_ENTER_CRITICAL();
    if (OSTCBCur->OSTCBStat & OS_STAT_FLAG) {              /* Have we timed-out?                       */
        OS_FlagUnlink(&node);//��ʱ���������ʱ���ʹ�˫��ȴ��б���ɾ��
        OSTCBCur->OSTCBStat = OS_STAT_RDY;/* Yes, make task ready-to-run   */
		//��ʱ��ʹ����������״̬
        OS_EXIT_CRITICAL();
        flags_cur           = (OS_FLAGS)0;
        *err                = OS_TIMEOUT;                  /* Indicate that we timed-out waiting       */
		//���أ����ǳ�ʱ����
    } else {//���û�г�ʱ��һ���ǰ���Ԥ�ڵķ�����
        if (consume == TRUE) {         /* See if we need to consume the flags      */
			//�Ƿ�Ҫ�������
            switch (wait_type) {
                case OS_FLAG_WAIT_SET_ALL:
                case OS_FLAG_WAIT_SET_ANY:                 /* Clear ONLY the flags we got              */
					//ֻ������ǵõ��ġ�
                     pgrp->OSFlagFlags &= ~OSTCBCur->OSTCBFlagsRdy;
                     break;

#if OS_FLAG_WAIT_CLR_EN > 0
                case OS_FLAG_WAIT_CLR_ALL:
                case OS_FLAG_WAIT_CLR_ANY:                 /* Set   ONLY the flags we got              */
					//ֻ��λ���ǵõ��ġ�
                     pgrp->OSFlagFlags |= OSTCBCur->OSTCBFlagsRdy;
                     break;
#endif
            }
        }
        flags_cur = pgrp->OSFlagFlags;//�˺�����ȡ�¼���־��ĵ�ǰ�¼���־״̬��
        //�����ظ�����״̬
        OS_EXIT_CRITICAL();
        *err      = OS_NO_ERR;  /* Event(s) must have occurred              */
    }
    return (flags_cur);
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                         POST EVENT FLAG BIT(S)
*
* Description: This function is called to set or clear some bits in an event flag group.  The bits to
*              set or clear are specified by a 'bit mask'.
*
* Arguments  : pgrp          is a pointer to the desired event flag group.
*
*              flags         If 'opt' (see below) is OS_FLAG_SET, each bit that is set in 'flags' will
*                            set the corresponding bit in the event flag group.  e.g. to set bits 0, 4
*                            and 5 you would set 'flags' to:
*
*                                0x31     (note, bit 0 is least significant bit)
*
*                            If 'opt' (see below) is OS_FLAG_CLR, each bit that is set in 'flags' will
*                            CLEAR the corresponding bit in the event flag group.  e.g. to clear bits 0,
*                            4 and 5 you would specify 'flags' as:
*
*                                0x31     (note, bit 0 is least significant bit)
*
*              opt           indicates whether the flags will be:
*                                set     (OS_FLAG_SET) or
*                                cleared (OS_FLAG_CLR)
*
*              err           is a pointer to an error code and can be:
*                            OS_NO_ERR              The call was successfull
*                            OS_FLAG_INVALID_PGRP   You passed a NULL pointer
*                            OS_ERR_EVENT_TYPE      You are not pointing to an event flag group
*                            OS_FLAG_INVALID_OPT    You specified an invalid option
*
* Returns    : the new value of the event flags bits that are still set.
*
* Called From: Task or ISR
*
* WARNING(s) : 1) The execution time of this function depends on the number of tasks waiting on the event
*                 flag group.
*              2) The amount of time interrupts are DISABLED depends on the number of tasks waiting on
*                 the event flag group.
                                                ��λ����0�¼���־���е��¼���־
�������˺������û�����¼���־���е�һЩλ��ͨ��������������ɡ�
������pgrp��Ŀ���¼���־��ָ��
                flags�����opt��OS_FLAG_SET��flags�е�ÿһλ�������¼���־���еĶ�Ӧλ��
                �磺Ҫ����0��4��5λ����ôflagsΪ0x31�����'opt' (see below) �� OS_FLAG_CLR����ô
                flags�е�ÿһλ��λ������¼���־���еĶ�Ӧλ��
                �磺���0��4��5λ����ôflagsΪ0x31��
*              opt          ��ʾflags�Ƿ�Ϊ��
*                                set     (OS_FLAG_SET) ����
*                                cleared (OS_FLAG_CLR)
*
*              err           is a pointer to an error code and can be:
*                            OS_NO_ERR              �ɹ�
*                            OS_FLAG_INVALID_PGRP   �㴫���˿�ָ��
*                            OS_ERR_EVENT_TYPE     û��ָ���¼���־��
*                            OS_FLAG_INVALID_OPT    �д���ѡ��
*********************************************************************************************************
*/
OS_FLAGS  OSFlagPost (OS_FLAG_GRP *pgrp, OS_FLAGS flags, INT8U opt, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                          /* Allocate storage for CPU status register       */
    OS_CPU_SR     cpu_sr;
#endif
    OS_FLAG_NODE *pnode;
    BOOLEAN       sched;
    OS_FLAGS      flags_cur;
    OS_FLAGS      flags_rdy;


#if OS_ARG_CHK_EN > 0
    if (pgrp == (OS_FLAG_GRP *)0) {                  /* Validate 'pgrp'                                */
        *err = OS_FLAG_INVALID_PGRP;
        return ((OS_FLAGS)0);//pgrp�Ƿ����
    }
    if (pgrp->OSFlagType != OS_EVENT_TYPE_FLAG) {    /* Make sure we are pointing to an event flag grp */
        *err = OS_ERR_EVENT_TYPE;
        return ((OS_FLAGS)0);//��֤��ָ���¼���־��
    }
#endif
/*$PAGE*/
    OS_ENTER_CRITICAL();
    switch (opt) {
        case OS_FLAG_CLR://����Ǹ������������෴��
             pgrp->OSFlagFlags &= ~flags;            /* Clear the flags specified in the group         */
             break;

        case OS_FLAG_SET://�������������������ͬ��
             pgrp->OSFlagFlags |=  flags;            /* Set   the flags specified in the group         */
             break;

        default://���쳣����
             OS_EXIT_CRITICAL();                     /* INVALID option                                 */
             *err = OS_FLAG_INVALID_OPT;
             return ((OS_FLAGS)0);
    }
    sched = FALSE;                                   /* Indicate that we don't need rescheduling       */
	//��ʼ�����費�ᵼ�¸������ȼ�����������̬�����Բ���Ҫ����
    pnode = (OS_FLAG_NODE *)pgrp->OSFlagWaitList;//ȡ���ȴ��б��еĵ�һ�����
    while (pnode != (OS_FLAG_NODE *)0) {             /* Go through all tasks waiting on event flag(s)  */
		//���������ȴ��������¼���־����������
        switch (pnode->OSFlagNodeWaitType) {//�ȴ���������ʲô
            case OS_FLAG_WAIT_SET_ALL:               /* See if all req. flags are set for current node */
                 flags_rdy = pgrp->OSFlagFlags & pnode->OSFlagNodeFlags;
                 if (flags_rdy == pnode->OSFlagNodeFlags) {//�����ȫ������ȴ����¼���־λ���㣬
                     if (OS_FlagTaskRdy(pnode, flags_rdy) == TRUE) { /* Make task RTR, event(s) Rx'd   */
					 	//��ôʹ����������״̬
                         sched = TRUE;                               /* When done we will reschedule   */
            //������ÿ���һ��OSFLAG_NODE�ͽ���һ��������ȣ������ڱ�����ȫ���ȴ������
            //����һ���ܵ�������ȡ�
                     }
                 }
                 break;

            case OS_FLAG_WAIT_SET_ANY:               /* See if any flag set                            */
				//����һ��ָ�� ���¼���־λ��λ
                 flags_rdy = pgrp->OSFlagFlags & pnode->OSFlagNodeFlags;
                 if (flags_rdy != (OS_FLAGS)0) {//ֻҪ��Ϊ�㣨��λ���У���ִ��
                     if (OS_FlagTaskRdy(pnode, flags_rdy) == TRUE) { /* Make task RTR, event(s) Rx'd   */
                         sched = TRUE;                               /* When done we will reschedule   */
                     }
                 }
                 break;

#if OS_FLAG_WAIT_CLR_EN > 0
            case OS_FLAG_WAIT_CLR_ALL:               /* See if all req. flags are set for current node */
				//ָ���¼���־ȫ������
                 flags_rdy = ~pgrp->OSFlagFlags & pnode->OSFlagNodeFlags;
                 if (flags_rdy == pnode->OSFlagNodeFlags) {
                     if (OS_FlagTaskRdy(pnode, flags_rdy) == TRUE) { /* Make task RTR, event(s) Rx'd   */
                         sched = TRUE;                               /* When done we will reschedule   */
                     }
                 }
                 break;

            case OS_FLAG_WAIT_CLR_ANY:               /* See if any flag set                            */
				//����һ��ָ�����¼���־����
                 flags_rdy = ~pgrp->OSFlagFlags & pnode->OSFlagNodeFlags;
                 if (flags_rdy != (OS_FLAGS)0) {
                     if (OS_FlagTaskRdy(pnode, flags_rdy) == TRUE) { /* Make task RTR, event(s) Rx'd   */
                         sched = TRUE;                               /* When done we will reschedule   */
                     }
                 }
                 break;
#endif
        }
        pnode = (OS_FLAG_NODE *)pnode->OSFlagNodeNext; /* Point to next task waiting for event flag(s) */
		//ͨ��˫������õ���һ��OSFlagNode

    }
    OS_EXIT_CRITICAL();
    if (sched == TRUE) {
		//�����������Ϊ�棬�����������
        OS_Sched();
    }
    OS_ENTER_CRITICAL();
    flags_cur = pgrp->OSFlagFlags;//���ص�ǰ�¼���־����¼���־״̬
    OS_EXIT_CRITICAL();
    *err      = OS_NO_ERR;
    return (flags_cur);
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                           QUERY EVENT FLAG
*
* Description: This function is used to check the value of the event flag group.
*
* Arguments  : pgrp         is a pointer to the desired event flag group.
*
*              err           is a pointer to an error code returned to the called:
*                            OS_NO_ERR              The call was successfull
*                            OS_FLAG_INVALID_PGRP   You passed a NULL pointer
*                            OS_ERR_EVENT_TYPE      You are not pointing to an event flag group
*
* Returns    : The current value of the event flag group.
*
* Called From: Task or ISR
                                                         ��ѯ�¼���־���״̬
���������ڲ�ѯ�¼���־���״̬
������ pgrp��ָ��Ŀ���¼���־���״̬
                  err�����ظ����ú����ĳ������ָ�룺
*                            OS_NO_ERR              ���óɹ�
*                            OS_FLAG_INVALID_PGRP   �㴫���˿�ָ��
*                            OS_ERR_EVENT_TYPE      ��û��ָ��һ���¼���־��                    
*********************************************************************************************************
*/

#if OS_FLAG_QUERY_EN > 0//���� OSFlagQuery()����
OS_FLAGS  OSFlagQuery (OS_FLAG_GRP *pgrp, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                       /* Allocate storage for CPU status register          */
    OS_CPU_SR  cpu_sr;
#endif
    OS_FLAGS   flags;


#if OS_ARG_CHK_EN > 0
    if (pgrp == (OS_FLAG_GRP *)0) {               /* Validate 'pgrp'                                   */
        *err = OS_FLAG_INVALID_PGRP;
        return ((OS_FLAGS)0);//�������pgrp
    }
    if (pgrp->OSFlagType != OS_EVENT_TYPE_FLAG) { /* Validate event block type                         */
        *err = OS_ERR_EVENT_TYPE;
        return ((OS_FLAGS)0);//�����¼���־����
    }
#endif
    OS_ENTER_CRITICAL();
    flags = pgrp->OSFlagFlags;//��ȡ��ǰ�¼���־��״̬
    OS_EXIT_CRITICAL();
    *err = OS_NO_ERR;
    return (flags);      /* Return the current value of the event flags       */
	//����״̬��־�����ú���
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                         SUSPEND TASK UNTIL EVENT FLAG(s) RECEIVED OR TIMEOUT OCCURS
*
* Description: This function is internal to uC/OS-II and is used to put a task to sleep until the desired
*              event flag bit(s) are set.
*
* Arguments  : pgrp          is a pointer to the desired event flag group.
*
*              pnode         is a pointer to a structure which contains data about the task waiting for
*                            event flag bit(s) to be set.
*
*              flags         Is a bit pattern indicating which bit(s) (i.e. flags) you wish to check.
*                            The bits you want are specified by setting the corresponding bits in
*                            'flags'.  e.g. if your application wants to wait for bits 0 and 1 then
*                            'flags' would contain 0x03.
*
*              wait_type     specifies whether you want ALL bits to be set/cleared or ANY of the bits
*                            to be set/cleared.
*                            You can specify the following argument:
*
*                            OS_FLAG_WAIT_CLR_ALL   You will check ALL bits in 'mask' to be clear (0)
*                            OS_FLAG_WAIT_CLR_ANY   You will check ANY bit  in 'mask' to be clear (0)
*                            OS_FLAG_WAIT_SET_ALL   You will check ALL bits in 'mask' to be set   (1)
*                            OS_FLAG_WAIT_SET_ANY   You will check ANY bit  in 'mask' to be set   (1)
*
*              timeout       is the desired amount of time that the task will wait for the event flag
*                            bit(s) to be set.
*
* Returns    : none
*
* Called by  : OSFlagPend()  OS_FLAG.C
*
* Note(s)    : This function is INTERNAL to uC/OS-II and your application should not call it.
                                           ��������ֱ���¼���־�������߳�ʱ
����������һ��ucos�ڲ���������һ������˯��ֱ��Ŀ���־λ��λ
������pgrp��ָ��Ŀ���¼����ƿ��ָ��
                 pnode��ָ��ȴ��¼���־λ��λ����������ݵĽṹָ��
                 flags����һ��λģʽ������������λ��ͨ������flags����Ӧλ����������Ҫ
                             ��λ�����磺���Ӧ������ȴ�0��1λ����ôflags��Ϊ0x03��
                 wait_type����־��������λ����/���㻹���κ�λ����/����
                 �����������²�����
*                            OS_FLAG_WAIT_CLR_ALL   You will check ALL bits in 'mask' to be clear (0)
*                            OS_FLAG_WAIT_CLR_ANY   You will check ANY bit  in 'mask' to be clear (0)
*                            OS_FLAG_WAIT_SET_ALL   You will check ALL bits in 'mask' to be set   (1)
*                            OS_FLAG_WAIT_SET_ANY   You will check ANY bit  in 'mask' to be set   (1)
                 timeout������ȴ��¼���־λ��Ŀ��ʱ��
*********************************************************************************************************
*/

static  void  OS_FlagBlock (OS_FLAG_GRP *pgrp, OS_FLAG_NODE *pnode, OS_FLAGS flags, INT8U wait_type, INT16U timeout)
{
    OS_FLAG_NODE  *pnode_next;


    OSTCBCur->OSTCBStat      |= OS_STAT_FLAG;
    OSTCBCur->OSTCBDly        = timeout;              /* Store timeout in task's TCB                   */
	//��TCB�б��泬ʱʱ��
#if OS_TASK_DEL_EN > 0//�����������ɾ������
    OSTCBCur->OSTCBFlagNode   = pnode;                /* TCB to link to node                           */
//	��OSTCBFlagNode���ݽṹ������TCB��һ�������У�ǰ����OS_TASK_DEL_ENΪ1�����Ը������
//���Ӱ�Ҫɾ��������Ӷ�Ӧ���¼���־��ĵȴ��б���ɾ����ɾ������������������ɡ�
#endif
    pnode->OSFlagNodeFlags    = flags;                /* Save the flags that we need to wait for       */
//��������ȴ���־����ָ���¼���־λ�͵ȴ���ʽ����Ϣ��
    pnode->OSFlagNodeWaitType = wait_type;            /* Save the type of wait we are doing            */
//�������ǵȴ��ķ�ʽ 
    pnode->OSFlagNodeTCB      = (void *)OSTCBCur;     /* Link to task's TCB                            */
//����ǰ��������ȴ��¼���TCB
    pnode->OSFlagNodeNext     = pgrp->OSFlagWaitList; /* Add node at beginning of event flag wait list */
//��������OS_FLAG_NODE����ӵ�˫������Ŀ�ʼ��
    pnode->OSFlagNodePrev     = (void *)0;
    pnode->OSFlagNodeFlagGrp  = (void *)pgrp;         /* Link to Event Flag Group                      */
	//�����¼���־�鷴�����ӵ�OS_FLAG_NODE���¼���־���У���ɾ��һ������ʱ��
	//��Ҫ����������Ӱѱ�ɾ��������Ӷ�Ӧ���¼���־��ĵȴ������б���ɾ��
    pnode_next                = (OS_FLAG_NODE *)pgrp->OSFlagWaitList;
	//��ǰ��һ��OS_FLAG_NODEָ�����ӵ�����ӵ�OS_FLAG_NODE
    if (pnode_next != (void *)0) {                    /* Is this the first NODE to insert?             */
        pnode_next->OSFlagNodePrev = pnode;           /* No, link in doubly linked list                */
    }
    pgrp->OSFlagWaitList = (void *)pnode;//��һ�����ͱ��pnode��
                                                      /* Suspend current task until flag(s) received   */
    if ((OSRdyTbl[OSTCBCur->OSTCBY] &= ~OSTCBCur->OSTCBBitX) == 0) {
        OSRdyGrp &= ~OSTCBCur->OSTCBBitY;//����ǰ����ֱ���յ��¼���־
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                    INITIALIZE THE EVENT FLAG MODULE
*
* Description: This function is called by uC/OS-II to initialize the event flag module.  Your application
*              MUST NOT call this function.  In other words, this function is internal to uC/OS-II.
*
* Arguments  : none
*
* Returns    : none
*
* WARNING    : You MUST NOT call this function from your code.  This is an INTERNAL function to uC/OS-II.
                                                    ��ʼ���¼���־ģ��
��������ucos���ó�ʼ���¼���־ģ�飬Ӧ�ó����ܵ���
��������
���أ���
��ע�����ڲ�������Ӧ�ó����ܵ���
*********************************************************************************************************
*/

void  OS_FlagInit (void)
{
#if OS_MAX_FLAGS == 1//ֻ��һ���¼���־��
    OSFlagFreeList                 = (OS_FLAG_GRP *)&OSFlagTbl[0];  /* Only ONE event flag group!      */
//ֻ��һ����
    OSFlagFreeList->OSFlagType     = OS_EVENT_TYPE_UNUSED;
//��ʼ�����ó�û��ʹ��
    OSFlagFreeList->OSFlagWaitList = (void *)0;
//û�������ڵ���
#endif

#if OS_MAX_FLAGS >= 2//����¼���־��
    INT8U        i;
    OS_FLAG_GRP *pgrp1;
    OS_FLAG_GRP *pgrp2;


    pgrp1 = &OSFlagTbl[0];
    pgrp2 = &OSFlagTbl[1];
    for (i = 0; i < (OS_MAX_FLAGS - 1); i++) {                      /* Init. list of free EVENT FLAGS  */
		//��ʼ�����е��¼���־
        pgrp1->OSFlagType     = OS_EVENT_TYPE_UNUSED;//���ó�û��ʹ��
        pgrp1->OSFlagWaitList = (void *)pgrp2;//������������
        pgrp1++;
        pgrp2++;
    }
    pgrp1->OSFlagWaitList = (void *)0;//û�����������
    OSFlagFreeList        = (OS_FLAG_GRP *)&OSFlagTbl[0];//��ͷ
#endif
}

/*$PAGE*/
/*
*********************************************************************************************************
*                              MAKE TASK READY-TO-RUN, EVENT(s) OCCURRED
*
* Description: This function is internal to uC/OS-II and is used to make a task ready-to-run because the
*              desired event flag bits have been set.
*
* Arguments  : pnode         is a pointer to a structure which contains data about the task waiting for
*                            event flag bit(s) to be set.
*
*              flags_rdy     contains the bit pattern of the event flags that cause the task to become
*                            ready-to-run.
*
* Returns    : none
*
* Called by  : OSFlagsPost() OS_FLAG.C
*
* Note(s)    : 1) This function assumes that interrupts are disabled.
*              2) This function is INTERNAL to uC/OS-II and your application should not call it.
                                         //�¼�������ʹ�������
������Ucos�ڲ�����������Ŀ���¼���־λ��λ�ˣ�ʹ�������
������pnode��ָ������ȴ��¼���־λ��λ���������ݵ�ָ��
                 flags_rdy�������¼���־��λģʽ�����������¼�����
���أ���
��OSFlagsPost() OS_FLAG.C����
��ע��1�����ô˺���Ҫ���ж�
                 2���ڲ�������Ӧ�ó����ܵ���
*********************************************************************************************************
*/

static  BOOLEAN  OS_FlagTaskRdy (OS_FLAG_NODE *pnode, OS_FLAGS flags_rdy)
{
    OS_TCB   *ptcb;
    BOOLEAN   sched;


    ptcb                = (OS_TCB *)pnode->OSFlagNodeTCB;  /* Point to TCB of waiting task             */
	//ȡ��ָ��ȴ��¼���PCBָ��
    ptcb->OSTCBDly      = 0;//����ʱ
    ptcb->OSTCBFlagsRdy = flags_rdy;//����Ҫ��������ı�־ȡ����
    ptcb->OSTCBStat    &= ~OS_STAT_FLAG;//�����¼���־���й���״̬
    if (ptcb->OSTCBStat == OS_STAT_RDY) {      /* Put task into ready list                 *///������������б�
        OSRdyGrp               |= ptcb->OSTCBBitY;
        OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;//��������б������㷨��BLOG
        sched                   = TRUE;//��Ҫ�������
    } else {//������Ҫ�������
        sched                   = FALSE;
    }
    OS_FlagUnlink(pnode);//�����ӵȴ��б���ɾ��
    return (sched);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                  UNLINK EVENT FLAG NODE FROM WAITING LIST
*
* Description: This function is internal to uC/OS-II and is used to unlink an event flag node from a
*              list of tasks waiting for the event flag.
*
* Arguments  : pnode         is a pointer to a structure which contains data about the task waiting for
*                            event flag bit(s) to be set.
*
* Returns    : none
*
* Called by  : OS_FlagTaskRdy() OS_FLAG.C
*              OSFlagPend()     OS_FLAG.C
*              OSTaskDel()      OS_TASK.C
*
* Note(s)    : 1) This function assumes that interrupts are disabled.
*              2) This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/
/*
*********************************************************************************************************
                                                     ���¼���־���ӵȴ��б���ɾ��
���������������ucos�ڲ��ģ����ڽ��¼���־���͵ȴ��¼���־��һϵ�����������
������pnode:����һ���ṹָ�룬��������ȴ��¼���־λ�������õ����ݡ�
���أ���
   �����溯�����ã� OS_FlagTaskRdy() OS_FLAG.C
*                                               OSFlagPend()     OS_FLAG.C
*                                               OSTaskDel()      OS_TASK.C
��ע��1����������ٶ��жϲ���ʹ
                2���˺�������ucos�ڲ���Ӧ�ú������ܵ��á�

*********************************************************************************************************
*/


void  OS_FlagUnlink (OS_FLAG_NODE *pnode)
{
#if OS_TASK_DEL_EN > 0//���� ��������ɾ������
    OS_TCB       *ptcb;//������ƿ�
#endif
    OS_FLAG_GRP  *pgrp;//�¼�����Ⱥ
    OS_FLAG_NODE *pnode_prev;//�¼���־�ȴ�����ӵ�ǰ��
    OS_FLAG_NODE *pnode_next;//�¼���־�ȴ�����ӵ���


    pnode_prev = (OS_FLAG_NODE *)pnode->OSFlagNodePrev;//�ȴ������еȴ����ָ��ǰ�˸���pnode_prev
    pnode_next = (OS_FLAG_NODE *)pnode->OSFlagNodeNext;//�ȴ������еȴ����ָ���˸���pnode_next
    if (pnode_prev == (OS_FLAG_NODE *)0) {                      /* Is it first node in wait list?      */
		//����ǵ�һ�����
        pgrp                 = (OS_FLAG_GRP *)pnode->OSFlagNodeFlagGrp;
		//�ȴ��������¼�����Ⱥָ�븳���¼�����Ⱥ
        pgrp->OSFlagWaitList = (void *)pnode_next;              /*      Update list for new 1st node   */
		//�¼�����Ⱥ����һ��ָ�����ڵȴ�����ĵ�һ�����ָ��
        if (pnode_next != (OS_FLAG_NODE *)0) {
            pnode_next->OSFlagNodePrev = (OS_FLAG_NODE *)0;     /*      Link new 1st node PREV to NULL */
			//�½���ǰ��ָ���ָ��
        }
    } else {                                                    /* No,  A node somewhere in the list   */
    //������ǵ�һ�����    
        pnode_prev->OSFlagNodeNext = pnode_next;                /*      Link around the node to unlink */
	//ǰ�˵ĺ��Ϊpnode_next
        if (pnode_next != (OS_FLAG_NODE *)0) {                  /*      Was this the LAST node?        */
			//��������Ǻ�һ�����
            pnode_next->OSFlagNodePrev = pnode_prev;            /*      No, Link around current node   */
			//��˵�ǰ��Ϊpnode��ǰ��
        }
    }
#if OS_TASK_DEL_EN > 0//�ܱ�ɾ���������
    ptcb                = (OS_TCB *)pnode->OSFlagNodeTCB;//�ȴ�������ƿ�ָ�븳��������ƿ�
    ptcb->OSTCBFlagNode = (OS_FLAG_NODE *)0;//�¼���־�������
#endif
}
#endif
