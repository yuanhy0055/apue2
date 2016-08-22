/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                            TASK MANAGEMENT
*
*                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* File : OS_TASK.C
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#ifndef  OS_MASTER_FILE
#include "includes.h"
#endif

/*
*********************************************************************************************************
*                                        CHANGE PRIORITY OF A TASK
*
* Description: This function allows you to change the priority of a task dynamically.  Note that the new
*              priority MUST be available.
*
* Arguments  : oldp     is the old priority
*
*              newp     is the new priority
*
* Returns    : OS_NO_ERR        is the call was successful
*              OS_PRIO_INVALID  if the priority you specify is higher that the maximum allowed
*                               (i.e. >= OS_LOWEST_PRIO)
*              OS_PRIO_EXIST    if the new priority already exist.
*              OS_PRIO_ERR      there is no task with the specified OLD priority (i.e. the OLD task does
*                               not exist.
*********************************************************************************************************
*/
/*
*********************************************************************************************************
                                                   �ı��������ȼ�
�������˺��������㶯̬�ı�һ����������ȼ������µ����ȼ��������
������oldp ���ɵ����ȼ�
                 newp���µ����ȼ�
���أ�OS_NO_ERR���ı�ɹ�
                OS_PRIO_INVALID��ָ�������ȼ����Ϸ����������ֵ
                OS_PRIO_EXIST�������ȼ��Ѿ�����
                OS_PRIO_ERR�������ȼ����񲻴���

*********************************************************************************************************
*/


#if OS_TASK_CHANGE_PRIO_EN > 0     //���OS_TASK_CHANGE_PRIO_EN����Ϊ1,��ʹ�����������
INT8U  OSTaskChangePrio (INT8U oldprio, INT8U newprio)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
                                                 //ΪCPU״̬�Ĵ�������洢��
    OS_CPU_SR    cpu_sr;     //OS_CPU_SR��Ϊunsigned int 
#endif

#if OS_EVENT_EN > 0
//OS_EVENT_EN ����Ϊ��(((OS_Q_EN > 0) && (OS_MAX_QS > 0)) || (OS_MBOX_EN > 0) || (OS_SEM_EN > 0) || (OS_MUTEX_EN > 0))
//OS_EVENT_EN ����Ϊ����ʹ���д������&&������п��ƿ��������Ϊ��||��ʹ����������||
//��ʹ�ź����������||��ʹ������������� 
    OS_EVENT    *pevent;
#endif

    OS_TCB      *ptcb;
    INT8U        x;
    INT8U        y;
    INT8U        bitx;
    INT8U        bity;



#if OS_ARG_CHK_EN > 0//����������
    if ((oldprio >= OS_LOWEST_PRIO && oldprio != OS_PRIO_SELF)  ||
         newprio >= OS_LOWEST_PRIO) {//�������ȼ������Ϸ�
        return (OS_PRIO_INVALID);
    }
#endif
    OS_ENTER_CRITICAL();//����Ϸ�
    if (OSTCBPrioTbl[newprio] != (OS_TCB *)0) {                 /* New priority must not already exist */
        OS_EXIT_CRITICAL();
        return (OS_PRIO_EXIST);//�����ȼ����벻����,���ھ��ظ���
    } else {
        OSTCBPrioTbl[newprio] = (OS_TCB *)1;                    /* Reserve the entry to prevent others */
		//�������,��ֹ��������ռ�ô����ȼ�
        OS_EXIT_CRITICAL();
        y    = newprio >> 3;                                    /* Precompute to reduce INT. latency   */
		//�˺�����Ԥ�ȼ��������ȼ������������ƿ��е�ĳЩֵ,ʹ����Щֵ
		//���Խ����������������ߴӸñ����Ƴ�����.
        bity = OSMapTbl[y];
        x    = newprio & 0x07;
        bitx = OSMapTbl[x];
        OS_ENTER_CRITICAL();
        if (oldprio == OS_PRIO_SELF) {                          /* See if changing self                */
			//����ı��Լ�
            oldprio = OSTCBCur->OSTCBPrio;                      /* Yes, get priority                   */
        }//�ǵ�,�õ����ȼ�
        ptcb = OSTCBPrioTbl[oldprio];//�õ������ȼ�TCBָ��
        if (ptcb != (OS_TCB *)0) {                              /* Task to change must exist           */
			//���ȼ�����,���Ҫ�ı���ǵ�ǰ����,��һ����ɹ�
            OSTCBPrioTbl[oldprio] = (OS_TCB *)0;                /* Remove TCB from old priority        */
		//ͨ���������ָ��,��ָ��ǰ�����TCBָ������ȼ��б���ɾ��,
		//ʹ��ǰ�ɵ����ȼ�����,���Ա���������ռ��.
            if ((OSRdyTbl[ptcb->OSTCBY] & ptcb->OSTCBBitX) != 0x00) {  /* If task is ready make it not */
                if ((OSRdyTbl[ptcb->OSTCBY] &= ~ptcb->OSTCBBitX) == 0x00) {	//���Ҫ�ı����ȼ����������
                    OSRdyGrp &= ~ptcb->OSTCBBitY;//������������,
                }
                OSRdyGrp    |= bity;                            /* Make new priority ready to run      */
                OSRdyTbl[y] |= bitx;//�Ӿ��������Ƴ�,Ȼ���������ȼ���,��������������,
                //ע��:OSTaskChangePrio������Ԥ�ȼ����ֵ(��ǰ��)���������������е�.
#if OS_EVENT_EN > 0
//#define  OS_EVENT_EN       (((OS_Q_EN > 0) && (OS_MAX_QS > 0)) || (OS_MBOX_EN > 0) || (OS_SEM_EN > 0) || (OS_MUTEX_EN > 0))
			//��ʹ���д������&&������п��ƿ��������Ϊ��||��ʹ����������||
			//��ʹ�ź����������||��ʹ�������������

            } else {
                pevent = ptcb->OSTCBEventPtr;
                if (pevent != (OS_EVENT *)0) {                  /* Remove from event wait list  */
	//�������û�о���,��ô�����ڵ�һ���ź���,һ���������ź���,һ������,����
	//��,���OSTCBEventPtr�ǿ�,��ô�˺�����֪���������ڵ����ϵ�ĳ����.
                    if ((pevent->OSEventTbl[ptcb->OSTCBY] &= ~ptcb->OSTCBBitX) == 0) {
                        pevent->OSEventGrp &= ~ptcb->OSTCBBitY;
                    }
                    pevent->OSEventGrp    |= bity;              /* Add new priority to wait list       */
	//����������ڵ�ĳ�¼�����,OSTCBEventPtr���뽫������¼����ƿ�ĵȴ�����(��
	//�����ȼ���)���Ƴ�,�����µ����ȼ��½��¼����뵽�ȴ�������.����Ҳ������
	//�ڵȴ���ʱʱ�䵽,�򱻹���,���漸�п���ʡ��
                    pevent->OSEventTbl[y] |= bitx;
                }
#endif
            }
            OSTCBPrioTbl[newprio] = ptcb;                       /* Place pointer to TCB @ new priority */
			//��ָ�������OS-TCB��ָ��浽OSTCBPrioTbl[]��.
            ptcb->OSTCBPrio       = newprio;                    /* Set new task priority               */
			//�µ����ȼ�������OSTCB��,Ԥ��ֵҲ������OSTCB��.
            ptcb->OSTCBY          = y;
            ptcb->OSTCBX          = x;
            ptcb->OSTCBBitY       = bity;
            ptcb->OSTCBBitX       = bitx;
            OS_EXIT_CRITICAL();
            OS_Sched();                                         /* Run highest priority task ready     */
            //�������,����������ȼ�����,���µ����ȼ����ھɵ������л����µ����ȼ����ڵ�
            //�ô˺����������ȼ���ʱ��,�˺����ᱻ����
            return (OS_NO_ERR);
        } else {
            OSTCBPrioTbl[newprio] = (OS_TCB *)0;                /* Release the reserved prio.          */
			//������񲻴���,�ͷ������ȼ���TCB
            OS_EXIT_CRITICAL();
            return (OS_PRIO_ERR);                               /* Task to change didn't exist         */
        }//����
    }
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                            CREATE A TASK
*
* Description: This function is used to have uC/OS-II manage the execution of a task.  Tasks can either
*              be created prior to the start of multitasking or by a running task.  A task cannot be
*              created by an ISR.
*
* Arguments  : task     is a pointer to the task's code
*
*              pdata    is a pointer to an optional data area which can be used to pass parameters to
*                       the task when the task first executes.  Where the task is concerned it thinks
*                       it was invoked and passed the argument 'pdata' as follows:
*
*                           void Task (void *pdata)
*                           {
*                               for (;;) {
*                                   Task code;
*                               }
*                           }
*
*              ptos     is a pointer to the task's top of stack.  If the configuration constant
*                       OS_STK_GROWTH is set to 1, the stack is assumed to grow downward (i.e. from high
*                       memory to low memory).  'pstk' will thus point to the highest (valid) memory
*                       location of the stack.  If OS_STK_GROWTH is set to 0, 'pstk' will point to the
*                       lowest memory location of the stack and the stack will grow with increasing
*                       memory locations.
*
*              prio     is the task's priority.  A unique priority MUST be assigned to each task and the
*                       lower the number, the higher the priority.
*
* Returns    : OS_NO_ERR        if the function was successful.
*              OS_PRIO_EXIT     if the task priority already exist
*                               (each task MUST have a unique priority).
*              OS_PRIO_INVALID  if the priority you specify is higher that the maximum allowed
*                               (i.e. >= OS_LOWEST_PRIO)
*********************************************************************************************************
*/
/*
*********************************************************************************************
                                                           ����һ������
 �����������������ucosII�������һ��������Ҫô�ڶ�������֮ǰ������
                  Ҫô���������������������жϷ����������
������task: ָ����������ָ�롣
                 pdata:��һ��ָ���ǿ�������������ָ�룬��������������ʱ����
                 ���������������йز��ּ����������ѣ�Ȼ�������·�ʽ����pdata��
*                           void Task (void *pdata)
*                           {
*                               for (;;) {
*                                   Task code;
*                               }
*                           }
                ptos��ָ�������ջ������ָ�룬������ó��� OS_STK_GROWTH ����Ϊ1�Ļ�����ջ����ɸߵ����������ɸߵ�ַ��͵�ַ�洢��������
��pstk����ָ���ջ�洢��λ�õ���ߵ�ַ����� OS_STK_GROWTH ����Ϊ0�Ļ�����pstk����ָ���ջ
��ʹ洢��λ�ã���ջ�����洢��λ�õ�����
             prio������������ȼ���һ�����ص����ȼ�����ָ����ÿ��������С������Ӧ������ȼ���

                 ���أ�OS_NO_ERR   ����������ɹ���
                                  OS_PRIO_EXIT ��������ȼ��Ѿ����ڡ�
                                  OS_PRIO_INVALID������������ȼ������������ֵ
                 
*********************************************************************************************
*/

#if OS_TASK_CREATE_EN > 0   //��ʹ�������񴴽�����
INT8U  OSTaskCreate (void (*task)(void *pd), void *pdata, OS_STK *ptos, INT8U prio)//������˵��
{
#if OS_CRITICAL_METHOD == 3                  /* Allocate storage for CPU status register               */
                                //  ΪCPU״̬�Ĵ�������洢�ռ�
    OS_CPU_SR  cpu_sr;         //CPU״̬����ʮ��λ OS_CPU_SRΪunsigned int 
#endif
    OS_STK    *psp;
    INT8U      err;


#if OS_ARG_CHK_EN > 0
//���OS_ARG_CHK_EN ��Ϊ1��OSTaskCreate����������������ȼ��Ƿ���Ч��
//ϵͳ��ִ�г�ʼ����ʱ���Ѿ���������ȼ�������˿�������
//���Բ�����������ȼ�����������
    if (prio > OS_LOWEST_PRIO) {             /* Make sure priority is within allowable range           */
		                                 //��֤���ȼ�������Χ��
        return (OS_PRIO_INVALID);
    }
#endif
    OS_ENTER_CRITICAL();                  //�����ٽ�״̬
    if (OSTCBPrioTbl[prio] == (OS_TCB *)0) { /* Make sure task doesn't already exist at this priority  */
		                                   //��֤���ȼ�û�б���������ռ��
        OSTCBPrioTbl[prio] = (OS_TCB *)1;    /* Reserve the priority to prevent others from doing ...  */
                                             /* ... the same thing until task is created.              */
								//����һ���ǿ�ָ�룬��ʾ�Ѿ�ռ��
        OS_EXIT_CRITICAL();        //�˳��ٽ�״̬
        psp = (OS_STK *)OSTaskStkInit(task, pdata, ptos, 0);    /* Initialize the task's stack         */
		                     //��ʼ�������ջ,�����������ջ
        err = OS_TCBInit(prio, psp, (OS_STK *)0, 0, 0, (void *)0, 0);
							 //��ʼ��������ƿ飬�ӿ��е�OS_TCB�����
							 //�л�ò���ʼ��һ��������ƿ�
        if (err == OS_NO_ERR) {         //�����ʼ��û�д�
            OS_ENTER_CRITICAL();//�����ٽ�״̬
            OSTaskCtr++;                                        /* Increment the #tasks counter        */
			//����������һ
            OS_EXIT_CRITICAL();//�˳��ٽ�״̬
            if (OSRunning == TRUE) {         /* Find highest priority task if multitasking has started */
				//���������ʼ��Ѱ��������ȼ�����
                OS_Sched();
            }
        } else {              //�����ʼ��������ƿ��д�
            OS_ENTER_CRITICAL();//�����ٽ�״̬
            OSTCBPrioTbl[prio] = (OS_TCB *)0;/* Make this priority available to others                 */
			//����һ���ȼ�����������
            OS_EXIT_CRITICAL();//�˳��ٽ�״̬
        }
        return (err);//���ش�����Ϣ
    }
    OS_EXIT_CRITICAL();//������ȼ�ռ�ã��˳��ٽ�״̬
    return (OS_PRIO_EXIST);//�������ȼ�����
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                     CREATE A TASK (Extended Version)
*
* Description: This function is used to have uC/OS-II manage the execution of a task.  Tasks can either
*              be created prior to the start of multitasking or by a running task.  A task cannot be
*              created by an ISR.  This function is similar to OSTaskCreate() except that it allows
*              additional information about a task to be specified.
*
* Arguments  : task     is a pointer to the task's code
*
*              pdata    is a pointer to an optional data area which can be used to pass parameters to
*                       the task when the task first executes.  Where the task is concerned it thinks
*                       it was invoked and passed the argument 'pdata' as follows:
*
*                           void Task (void *pdata)
*                           {
*                               for (;;) {
*                                   Task code;
*                               }
*                           }
*
*              ptos     is a pointer to the task's top of stack.  If the configuration constant
*                       OS_STK_GROWTH is set to 1, the stack is assumed to grow downward (i.e. from high
*                       memory to low memory).  'pstk' will thus point to the highest (valid) memory
*                       location of the stack.  If OS_STK_GROWTH is set to 0, 'pstk' will point to the
*                       lowest memory location of the stack and the stack will grow with increasing
*                       memory locations.  'pstk' MUST point to a valid 'free' data item.
*
*              prio     is the task's priority.  A unique priority MUST be assigned to each task and the
*                       lower the number, the higher the priority.
*
*              id       is the task's ID (0..65535)
*
*              pbos     is a pointer to the task's bottom of stack.  If the configuration constant
*                       OS_STK_GROWTH is set to 1, the stack is assumed to grow downward (i.e. from high
*                       memory to low memory).  'pbos' will thus point to the LOWEST (valid) memory
*                       location of the stack.  If OS_STK_GROWTH is set to 0, 'pbos' will point to the
*                       HIGHEST memory location of the stack and the stack will grow with increasing
*                       memory locations.  'pbos' MUST point to a valid 'free' data item.
*
*              stk_size is the size of the stack in number of elements.  If OS_STK is set to INT8U,
*                       'stk_size' corresponds to the number of bytes available.  If OS_STK is set to
*                       INT16U, 'stk_size' contains the number of 16-bit entries available.  Finally, if
*                       OS_STK is set to INT32U, 'stk_size' contains the number of 32-bit entries
*                       available on the stack.
*
*              pext     is a pointer to a user supplied memory location which is used as a TCB extension.
*                       For example, this user memory can hold the contents of floating-point registers
*                       during a context switch, the time each task takes to execute, the number of times
*                       the task has been switched-in, etc.
*
*              opt      contains additional information (or options) about the behavior of the task.  The
*                       LOWER 8-bits are reserved by uC/OS-II while the upper 8 bits can be application
*                       specific.  See OS_TASK_OPT_??? in uCOS-II.H.
*
* Returns    : OS_NO_ERR        if the function was successful.
*              OS_PRIO_EXIT     if the task priority already exist
*                               (each task MUST have a unique priority).
*              OS_PRIO_INVALID  if the priority you specify is higher that the maximum allowed
*                               (i.e. > OS_LOWEST_PRIO)
*********************************************************************************************************
*/
/*
*****************************************************************************
                                                     ����һ���������չ�溯��
�����������������ucosII�������һ��������Ҫô�ڶ�������֮ǰ������
				 Ҫô���������������������жϷ����������
                   ����OSTaskCreate()�������ƣ���������һ����������ĸ�����Ϣ��
������task: ָ����������ָ��
      pdata������ʼִ��ʱ�����ݸ����������ָ�룬�÷����ϡ�
      ptos������������ջ��ջ��ָ��
      prio:�������������ȼ�
      id:Ϊ���񴴽�һ�������־������չ�汾���á���������Ϊ�����ȼ�һ�����ɡ�
      pbos��ָ�������ջջ��ָ�룬���ڶ�ջ����
      stk_size:����ָ����ջ���������������ջ��ڿ��Ϊ4B����ôstk_sizeΪ1000��
               ˵����ջ��4000B
      pext��ָ���û����ӵ�������ָ�룬������չ�����������ƿ�OS_TCB
      opt���趨OSTaskCreateExt��ѡ�ָ���Ƿ������ջ���飬�Ƿ񽫶�ջ���㣬�Ƿ�
           ���и����������ȡ������uCOS_II��ÿһλ���塣ֻҪ����Ӧλ��opt��򼴿�
���أ�OS_NO_ERR   ����������ɹ���
		  OS_PRIO_EXIT ��������ȼ��Ѿ����ڡ�
		  OS_PRIO_INVALID������������ȼ������������ֵ



*****************************************************************************
*/
/*$PAGE*/
#if OS_TASK_CREATE_EXT_EN > 0 //��ʹ������������
INT8U  OSTaskCreateExt (void   (*task)(void *pd),
                        void    *pdata,
                        OS_STK  *ptos,
                        INT8U    prio,
                        INT16U   id,
                        OS_STK  *pbos,
                        INT32U   stk_size,
                        void    *pext,
                        INT16U   opt)//���庬�����
{
#if OS_CRITICAL_METHOD == 3                  /* Allocate storage for CPU status register               */
//ΪCPU״̬�Ĵ�������洢�ռ�
    OS_CPU_SR  cpu_sr;
#endif
    OS_STK    *psp;//�����ջ����ָ��
    INT8U      err;//�������


#if OS_ARG_CHK_EN > 0//�������ʲô������û�п���
    if (prio > OS_LOWEST_PRIO) {             /* Make sure priority is within allowable range           */
		//���ȼ�������Χ
        return (OS_PRIO_INVALID);//�������ȼ�������
    }
#endif
    OS_ENTER_CRITICAL();//������У������ٽ�״̬
    if (OSTCBPrioTbl[prio] == (OS_TCB *)0) { /* Make sure task doesn't already exist at this priority  */
		//��֤�����ȼ�������
        OSTCBPrioTbl[prio] = (OS_TCB *)1;    /* Reserve the priority to prevent others from doing ...  */
                                             /* ... the same thing until task is created.              */
											 //�������ȼ���˵�������ȼ��Ѿ�ռ��
        OS_EXIT_CRITICAL();//�˳��ٽ�״̬����������ʹ�˺����ڶ����������ݽṹ
                            //�������ֵ�ʱ���ܹ����ж�

        if (((opt & OS_TASK_OPT_STK_CHK) != 0x0000) ||   /* See if stack checking has been enabled     */
			//��������ջ
            ((opt & OS_TASK_OPT_STK_CLR) != 0x0000)) {   /* See if stack needs to be cleared           */
            //������ʱ�����ջ
            #if OS_STK_GROWTH == 1//��ջ�ӵ͵�������
            (void)memset(pbos, 0, stk_size * sizeof(OS_STK));
			//memset��һ����׼��ANSI�������������̼һ�ʹ֮���Ż�
            #else//�������������ʲô��֪����
            (void)memset(ptos, 0, stk_size * sizeof(OS_STK));
            #endif
        }

        psp = (OS_STK *)OSTaskStkInit(task, pdata, ptos, opt); /* Initialize the task's stack          */
		//��ʼ�������ջ
        err = OS_TCBInit(prio, psp, pbos, id, stk_size, pext, opt);
		//������ƿ��ʼ��
        if (err == OS_NO_ERR) {//����ɹ���OS_TCBInit����OS_NO_ERR
            OS_ENTER_CRITICAL();//�����ٽ�״̬
            OSTaskCtr++;                                       /* Increment the #tasks counter         */
			//������������һ
            OS_EXIT_CRITICAL();//�˳��ٽ�״̬
            if (OSRunning == TRUE) {                           /* Find HPT if multitasking has started */
				//������������еĻ����������ȼ�
                OS_Sched();//�������
            }
        } else {//���������ƿ��ʼ��ʧ��
            OS_ENTER_CRITICAL();
            OSTCBPrioTbl[prio] = (OS_TCB *)0;                  /* Make this priority avail. to others  */
			//����һ���ȼ�����������
            OS_EXIT_CRITICAL();
        }
        return (err);//�������������ջ
    }
    OS_EXIT_CRITICAL();
    return (OS_PRIO_EXIST);//��������ȼ����ڣ����ء�
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                            DELETE A TASK
*
* Description: This function allows you to delete a task.  The calling task can delete itself by
*              its own priority number.  The deleted task is returned to the dormant state and can be
*              re-activated by creating the deleted task again.
*
* Arguments  : prio    is the priority of the task to delete.  Note that you can explicitely delete
*                      the current task without knowing its priority level by setting 'prio' to
*                      OS_PRIO_SELF.
*
* Returns    : OS_NO_ERR           if the call is successful
*              OS_TASK_DEL_IDLE    if you attempted to delete uC/OS-II's idle task
*              OS_PRIO_INVALID     if the priority you specify is higher that the maximum allowed
*                                  (i.e. >= OS_LOWEST_PRIO) or, you have not specified OS_PRIO_SELF.
*              OS_TASK_DEL_ERR     if the task you want to delete does not exist
*              OS_TASK_DEL_ISR     if you tried to delete a task from an ISR
*
* Notes      : 1) To reduce interrupt latency, OSTaskDel() 'disables' the task:
*                    a) by making it not ready
*                    b) by removing it from any wait lists
*                    c) by preventing OSTimeTick() from making the task ready to run.
*                 The task can then be 'unlinked' from the miscellaneous structures in uC/OS-II.
*              2) The function OS_Dummy() is called after OS_EXIT_CRITICAL() because, on most processors,
*                 the next instruction following the enable interrupt instruction is ignored.  
*              3) An ISR cannot delete a task.
*              4) The lock nesting counter is incremented because, for a brief instant, if the current
*                 task is being deleted, the current task would not be able to be rescheduled because it
*                 is removed from the ready list.  Incrementing the nesting counter prevents another task
*                 from being schedule.  This means that an ISR would return to the current task which is
*                 being deleted.  The rest of the deletion would thus be able to be completed.
*********************************************************************************************************
*/
/*
*****************************************************************************************************
                                                                 ɾ��һ������
�������˺���������ɾ��һ������������ڵ��õ�������ͨ�����Լ������ȼ���ɾ�����Լ���
                 ��ɾ�������񷵻�˯��״̬������ͨ������һ����ɾ�������ٴμ��
������prio����ɾ����������ȼ�������prio���OS_PRIO_SELFʱ�����ǲ�֪���������ȼ�Ҳ
                   �ܹ�ɾ����
���أ�OS_NO_ERR:ɾ���ɹ�
                 OS_TASK_DEL_IDLE�������ͼɾ����������
                 OS_PRIO_INVALID�����ָ�����ȼ��߹�����ֵ
                 OS_TASK_DEL_ERR�������ɾ�������񲻴���
                 OS_TASK_DEL_ISR���������жϷ��������ɾ������
��ע��1��Ϊ�����ж���ʱ��OSTaskDel()ͨ�������ֶβ���ʹ����
                   a��ָ����û�о���b���ӵȴ��б���ɾ��c����OSTimeTick()��������ֹ����׼�����С�
                Ȼ������ᱻ��ucos�ĸ��ӽṹ�н���
                    2������OS_Dummy() ��OS_EXIT_CRITICAL()����֮���������Ϊ�ڶ����������У�����ʹ�ж�
                       ָ�����һ��ָ������ԡ�
                      3��һ���жϷ��������ɾ��һ������
                     4������Ƕ������������Ϊ��Ϊһ����ʱ�����������ǰ����ɾ���������ǰ���������±�
                       ������Ϊ���Ѿ��Ӿ����б���ɾ��������Ƕ������ֹ��һ�����񱻵��ȡ�����ζ���жϷ������
                    �����ر�ɾ���ĵ�ǰ�������µ�ɾ������������ֹ��


*********************************************************************************************************
*/

/*$PAGE*/
#if OS_TASK_DEL_EN > 0//�����������ɾ������
INT8U  OSTaskDel (INT8U prio)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR     cpu_sr;//ΪCPU״̬�Ĵ�������洢�ռ�
#endif

#if OS_EVENT_EN > 0
	//��ʹ���д������&&������п��ƿ��������Ϊ��||��ʹ����������||
	//��ʹ�ź����������||��ʹ������������� 

    OS_EVENT     *pevent;//OS_EVENT���¼����ƿ�
#endif    
#if (OS_VERSION >= 251) && (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0)
//OS�汾���ڵ���251����ʹ�¼���־���¼���־����ֵ������
    OS_FLAG_NODE *pnode;//�¼���־�ȴ��б�
#endif
    OS_TCB       *ptcb;//������ƿ�
    BOOLEAN       self;//#define BOOLEAN  unsigned  char



    if (OSIntNesting > 0) {                                     /* See if trying to delete from ISR    */
		//���ǲ���Ҫ��ISR��ɾ��
        return (OS_TASK_DEL_ISR);//����
    }
#if OS_ARG_CHK_EN > 0//���������⣿
    if (prio == OS_IDLE_PRIO) {                                 /* Not allowed to delete idle task     */
		//��֤ɾ���Ĳ��ǿ�������
        return (OS_TASK_DEL_IDLE);
    }
    if (prio >= OS_LOWEST_PRIO && prio != OS_PRIO_SELF) {       /* Task priority valid ?               */
		//�����ɾ���������ȼ�Υ��
        return (OS_PRIO_INVALID);
    }
#endif
    OS_ENTER_CRITICAL();//�����ٽ�״̬
    if (prio == OS_PRIO_SELF) {                                 /* See if requesting to delete self    */
		//���ɾ������
        prio = OSTCBCur->OSTCBPrio;                             /* Set priority to delete to current   */
		//���õ�ǰɾ������״̬���ȼ�
    }
    ptcb = OSTCBPrioTbl[prio];//ȡ���ѽ���TCB��ָ���
    if (ptcb != (OS_TCB *)0) {                                       /* Task to delete must exist      */
		//�����Ϊ�գ���ʾָ�����
        if ((OSRdyTbl[ptcb->OSTCBY] &= ~ptcb->OSTCBBitX) == 0x00) {  /* Make task not ready            */
            OSRdyGrp &= ~ptcb->OSTCBBitY;//����������գ���֤��������
        }//��������ھ������У������ھ�������ɾ��
#if OS_EVENT_EN > 0
		//��ʹ���д������&&������п��ƿ��������Ϊ��||��ʹ����������||
		//��ʹ�ź����������||��ʹ������������� 

        pevent = ptcb->OSTCBEventPtr;//���¼����ƿ�ָ������¼����ƿ�
        if (pevent != (OS_EVENT *)0) {                          /* If task is waiting on event         */
   //����������¼��еȴ�������������ڻ������ź��������䣬
   //��Ϣ���л����ź����ĵȴ��������ʹ��Լ������ı��б�ȥ��
            if ((pevent->OSEventTbl[ptcb->OSTCBY] &= ~ptcb->OSTCBBitX) == 0) { /* ... remove task from */
                pevent->OSEventGrp &= ~ptcb->OSTCBBitY;                        /* ... event ctrl block */
            }//��������¼����ƿ���ɾ����
        }
#endif
#if (OS_VERSION >= 251) && (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0)
		//OS�汾���ڵ���251����ʹ�¼���־���¼���־����ֵ������

        pnode = ptcb->OSTCBFlagNode;//�¼���־��ϵ㸳���¼���־���ƿ�
        //����������¼���־�ĵȴ����У��ͻ�Ӵ˱���ɾ��
        if (pnode != (OS_FLAG_NODE *)0) {                       /* If task is waiting on event flag    */
			//����������¼���־�еȴ�
            OS_FlagUnlink(pnode);                               /* Remove from wait list               */
			//�ӵȴ��б���ɾ��
        }
#endif
        ptcb->OSTCBDly  = 0;                                    /* Prevent OSTimeTick() from updating  */
//ɾ������󣬽�����ʱ�ӽ����ӳ�����0��ȷ���Լ����¿��ж�
//ʱ���жϷ����ӳ���ISR����ʹ���������
        ptcb->OSTCBStat = OS_STAT_RDY;                          /* Prevent task from being resumed     */
//����״̬��Ϊ����̬��OSTaskDel()�������OSTCBStat��־ΪOS_STAT_RDY
//OSTaskDel()������ͼʹ�����ھ���̬��ֻ����ֹ������������жϷ���
//�����ø��������¿�ʼִ�У�ͨ������OSTaskResume()��.�������������OSTCBStat��
//־ΪOS_STAT_RDY��Ҳ�������OS_STAT_SUSPEND() ��
		if (OSLockNesting < 255) {//����������Ƕ����С��255
            OSLockNesting++;//Ƕ������һ
		}
//���ˣ���ɾ��������Ͳ��ᱻ������������жϷ����ӳ������ھ���̬
//��Ϊ�������Ѿ��Ӿ���������б�ɾ�ˣ�Ϊ������ɾ������Ŀ�ģ�����
//����������״̬������Ϊ����������״̬��OSTaskDel()Ҫ��ֹ�������
//������ɾ���������л�������������ȥ����Ϊ��ǰ����������ڱ�ɾ����
//�ǲ����ܱ��ٴε��ȡ�
        OS_EXIT_CRITICAL();                                     /* Enabling INT. ignores next instruc. */
//���¿��жϣ���������Ӧʱ�䣬������OSTaskDel()���ܹ������ж��ˣ�����
//���ڸ�OSLockNesting����һ��ISRִ����󣬻᷵�ص����ж��˵����񣬴Ӷ�
//���������ɾ��������
        OS_Dummy();                                             /* ... Dummy ensures that INTs will be */
//���ж�ָ�����Ҫ��ִ��һ��ָ����������жϣ�����ִ��һ����ָ�
        OS_ENTER_CRITICAL();                                    /* ... disabled HERE!                  */
		//��ʹ�жϣ�������һ��ָ���ٺ�����֤���ж������ﲻ��ʹ
		if (OSLockNesting > 0) {//�������������Ƕ�״�����
            OSLockNesting--;//Ƕ������һ
		}//���Լ���ִ��ɾ�������ˣ���OSTaskDel()���¹��жϺ���ͨ������Ƕ��
		 //��������һ����������������ȡ�
        OSTaskDelHook(ptcb);                                    /* Call user defined hook              */
		 //�����Զ���ɾ������ӿں�������������ɾ�������ͷ��Զ����
		 //TCB����������
        OSTaskCtr--;                                            /* One less task being managed         */
		 //�����������һ��������������������һ����
        OSTCBPrioTbl[prio] = (OS_TCB *)0;                       /* Clear old priority entry            */
		 //OSTaskDel()�����򵥵�ָ��ɾ�������TCB��ָ����ΪNULL���ʹ����ȼ�
		 //���а�OS_TCB��ɾ���ˡ�
        if (ptcb->OSTCBPrev == (OS_TCB *)0) {                   /* Remove from TCB chain               */
		//����OSTCBList��ͷ��OS_TCB˫��������ɾ����ɾ�����TCB��
            ptcb->OSTCBNext->OSTCBPrev = (OS_TCB *)0;//����Ǳ�ͷ
            OSTCBList                  = ptcb->OSTCBNext;
        } else {//������Ǳ�ͷ
            ptcb->OSTCBPrev->OSTCBNext = ptcb->OSTCBNext;
            ptcb->OSTCBNext->OSTCBPrev = ptcb->OSTCBPrev;
        }//����û�б�Ҫ�����β���������Ϊ������ɾ����������
        ptcb->OSTCBNext = OSTCBFreeList;                        /* Return TCB to free TCB list         */
		//��ɾ�����OS_TCB���˻ص����е�OS_TCB�У�����������ʹ��
        OSTCBFreeList   = ptcb;//ptcb����µı�ͷ
        OS_EXIT_CRITICAL();
        OS_Sched();                                             /* Find new highest priority task      */
        //������ȣ����˺������ж�ʱ��ESRʱ����ʹ�������ȼ����������
        //�˾���̬
        return (OS_NO_ERR);//ɾ���ɹ�
    }
    OS_EXIT_CRITICAL();
    return (OS_TASK_DEL_ERR);//���ָ��Ϊ�գ�ɾ�����ִ���
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                    REQUEST THAT A TASK DELETE ITSELF
*
* Description: This function is used to:
*                   a) notify a task to delete itself.
*                   b) to see if a task requested that the current task delete itself.
*              This function is a little tricky to understand.  Basically, you have a task that needs
*              to be deleted however, this task has resources that it has allocated (memory buffers,
*              semaphores, mailboxes, queues etc.).  The task cannot be deleted otherwise these
*              resources would not be freed.  The requesting task calls OSTaskDelReq() to indicate that
*              the task needs to be deleted.  Deleting of the task is however, deferred to the task to
*              be deleted.  For example, suppose that task #10 needs to be deleted.  The requesting task
*              example, task #5, would call OSTaskDelReq(10).  When task #10 gets to execute, it calls
*              this function by specifying OS_PRIO_SELF and monitors the returned value.  If the return
*              value is OS_TASK_DEL_REQ, another task requested a task delete.  Task #10 would look like
*              this:
*
*                   void Task(void *data)
*                   {
*                       .
*                       .
*                       while (1) {
*                           OSTimeDly(1);
*                           if (OSTaskDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ) {
*                               Release any owned resources;
*                               De-allocate any dynamic memory;
*                               OSTaskDel(OS_PRIO_SELF);
*                           }
*                       }
*                   }
*
* Arguments  : prio    is the priority of the task to request the delete from
*
* Returns    : OS_NO_ERR          if the task exist and the request has been registered
*              OS_TASK_NOT_EXIST  if the task has been deleted.  This allows the caller to know whether
*                                 the request has been executed.
*              OS_TASK_DEL_IDLE   if you requested to delete uC/OS-II's idle task
*              OS_PRIO_INVALID    if the priority you specify is higher that the maximum allowed
*                                 (i.e. >= OS_LOWEST_PRIO) or, you have not specified OS_PRIO_SELF.
*              OS_TASK_DEL_REQ    if a task (possibly another task) requested that the running task be
*                                 deleted.
*********************************************************************************************************
*/
/*
*********************************************************************************************************
                                                 ��������ɾ���Լ�
�������˺����������ڣ�
          a��֪ͨ����ɾ���Լ���
          b���鿴�Ƿ�һ����������ǰ����Ҫɾ�����Լ���
          �������Ҫ�Ƚ����ȥ��⡣�����ϣ�����һ��������Ҫɾ�������������з������Դ����          �磺 �洢�����������ź��������䡢���еȣ�����Щ�����ܱ�ɾ����������Щ��Դ������          �š������������OSTaskDelReq()ȥ������ǰ������Ҫ��ɾ����Ȼ��ɾ�����񽫱��ӳ٣��� 
          �磺��������ʮ����ɾ�����������񣨱����壩������OSTaskDelReq(10)��������ʮ���е�            ʱ���������ô˺����е�OS_PRIO_SELF�ټ��ӷ���ֵ��������ص�ֵ��OS_TASK_DEL_REQ
          ����һ����������ɾ����������ʮ������������
*                   void Task(void *data)
*                   {
*                       .
*                       .
*                       while (1) {
*                           OSTimeDly(1);
*                           if (OSTaskDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ) {
*                               Release any owned resources;
*                               De-allocate any dynamic memory;
*                               OSTaskDel(OS_PRIO_SELF);
*                           }
*                       }
*                   }//������ʲô��˼
������   prio������ɾ����������ȼ�
����ֵ�� OS_NO_ERR :���������ڣ�������ͨ����
             OS_TASK_NOT_EXIST����������Ѿ�ɾ��������Ҫ������֪�������Ƿ��Ѿ�ִ��
             OS_TASK_DEL_IDLE �����Ҫɾ����������
             OS_PRIO_INVALID��������ȼ���ֵ�����������ֵ�����߲���ָ��OS_PRIO_SELF.
             OS_TASK_DEL_REQ�����һ������Ҳ�����������������������е�����Ҫɾ����

*********************************************************************************************************
*/


/*$PAGE*/
#if OS_TASK_DEL_EN > 0
INT8U  OSTaskDelReq (INT8U prio)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;//����CPU״̬�Ĵ����洢�ռ�
#endif
    BOOLEAN    stat;//typedef unsigned char BOOLEAN
    INT8U      err;
    OS_TCB    *ptcb;


#if OS_ARG_CHK_EN > 0//����������
    if (prio == OS_IDLE_PRIO) {                                 /* Not allowed to delete idle task     */
        return (OS_TASK_DEL_IDLE);//�����ɾ�������������ǲ�����ġ�
    }
    if (prio >= OS_LOWEST_PRIO && prio != OS_PRIO_SELF) {       /* Task priority valid ?               */
        return (OS_PRIO_INVALID);//���ȼ����Ϸ�
    }
#endif
    if (prio == OS_PRIO_SELF) {                                 /* See if a task is requesting to ...  */
        OS_ENTER_CRITICAL();                                    /* ... this task to delete itself      */
        stat = OSTCBCur->OSTCBDelReq;                           /* Return request status to caller     */
		//�����ɾ���Լ�����ô��������״̬��������
        OS_EXIT_CRITICAL();
        return (stat);//��ô��������״̬��������
    }
    OS_ENTER_CRITICAL();
    ptcb = OSTCBPrioTbl[prio];//�Ѵ���TCBָ���
    if (ptcb != (OS_TCB *)0) {                                  /* Task to delete must exist           */
		//�����Ҫɾ�����������
        ptcb->OSTCBDelReq = OS_TASK_DEL_REQ;                    /* Set flag indicating task to be DEL. */
     //��������ȼ���������OS_PRIO_SELFָ��������������ڣ����ñ�־��ʾ����ɾ��
        err               = OS_NO_ERR;//���سɹ�
    } else {
        err               = OS_TASK_NOT_EXIST;                  /* Task must be deleted                */
    }//��Ҫɾ�������񲻴��ڣ�������������Ѿ�ɾ���Լ���
    OS_EXIT_CRITICAL();
    return (err);//��������
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                        RESUME A SUSPENDED TASK
*
* Description: This function is called to resume a previously suspended task.  This is the only call that
*              will remove an explicit task suspension.
*
* Arguments  : prio     is the priority of the task to resume.
*
* Returns    : OS_NO_ERR                if the requested task is resumed
*              OS_PRIO_INVALID          if the priority you specify is higher that the maximum allowed
*                                       (i.e. >= OS_LOWEST_PRIO)
*              OS_TASK_RESUME_PRIO      if the task to resume does not exist
*              OS_TASK_NOT_SUSPENDED    if the task to resume has not been suspended
*********************************************************************************************************
*/
/*
*********************************************************************************************************
                                               �ָ�����
�������������������ȥ�ָ�һ����ǰ����������������ֻ����ȥ������ʱ����á�
          �����������ֻ��ͨ�����������ܹ����ָ���
������prio�� ��Ҫ�ָ����������ȼ�
���أ�OS_NO_ERR�������������񱻻ָ�
          OS_PRIO_INVALID��������ȼ���Ч
          OS_TASK_RESUME_PRIO�����Ҫ�ָ����������ȼ�������
          OS_TASK_NOT_SUSPENDED�����Ҫ�ָ�������û�б�����

*********************************************************************************************************
*/



#if OS_TASK_SUSPEND_EN > 0//�������������������ָ�����
INT8U  OSTaskResume (INT8U prio)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;//����CPU״̬�Ĵ����洢�ռ�
#endif
    OS_TCB    *ptcb;


#if OS_ARG_CHK_EN > 0
    if (prio >= OS_LOWEST_PRIO) {                               /* Make sure task priority is valid    */
        return (OS_PRIO_INVALID);//��֤�������ȼ���Ч
    }
#endif
    OS_ENTER_CRITICAL();
    ptcb = OSTCBPrioTbl[prio];//���ô������TCBָ��
    if (ptcb == (OS_TCB *)0) {                                  /* Task to suspend must exist          */
        OS_EXIT_CRITICAL();//������񲻴���
        return (OS_TASK_RESUME_PRIO);
    }
    if ((ptcb->OSTCBStat & OS_STAT_SUSPEND) != OS_STAT_RDY) {              /* Task must be suspended   */
		//�����������ұ�����
        if (((ptcb->OSTCBStat &= ~OS_STAT_SUSPEND) == OS_STAT_RDY) &&      /* Remove suspension        */
		//��ͨ�����OSTCBStat���е�OS_STAT_SUSPENDλ��ȡ�������
             (ptcb->OSTCBDly  == 0)) {                                     /* Must not be delayed      */
       //Ҫʹ�����ھ���̬��OSTCBDly��Ϊ0����Ϊû���κα�־��������
       //���ڵȴ��ӳ�ʱ�䵽
            OSRdyGrp               |= ptcb->OSTCBBitY;                     /* Make task ready to run   */
            OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;//������������������ʱ������Ŵ��ھ���״̬
            OS_EXIT_CRITICAL();
            OS_Sched();//������Ȼ��鱻�ָ�������ӵ�е����ȼ��Ƿ�ȵ��ñ�����������
                      //���ȼ��ߡ�
        } else {
            OS_EXIT_CRITICAL();
        }
        return (OS_NO_ERR);//�ָ��ɹ�
    }
    OS_EXIT_CRITICAL();
    return (OS_TASK_NOT_SUSPENDED);//�������û�б�����
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                             STACK CHECKING
*
* Description: This function is called to check the amount of free memory left on the specified task's
*              stack.
*
* Arguments  : prio     is the task priority
*
*              pdata    is a pointer to a data structure of type OS_STK_DATA.
*
* Returns    : OS_NO_ERR           upon success
*              OS_PRIO_INVALID     if the priority you specify is higher that the maximum allowed
*                                  (i.e. > OS_LOWEST_PRIO) or, you have not specified OS_PRIO_SELF.
*              OS_TASK_NOT_EXIST   if the desired task has not been created
*              OS_TASK_OPT_ERR     if you did NOT specified OS_TASK_OPT_STK_CHK when the task was created
*********************************************************************************************************
*/
/*
*********************************************************************************************************
                                                                                 ��ջ����
����������������ڼ���ָ�������ջ��ʣ��洢�ռ�����
������prio���������ȼ�
           pdata��OS_STK_DATA�ṹ���͵�����ָ�롣
���أ�OS_NO_ERR ���ɹ���
            OS_PRIO_INVALID�����ȼ���ֵ�������ֵ����û��ָ��OS_PRIO_SELF
            OS_TASK_NOT_EXIST�����ָ������û�б�����
            OS_TASK_OPT_ERR��������񴴽�ʱû��ָ��OS_TASK_OPT_STK_CHK

*********************************************************************************************************
*/


#if OS_TASK_CREATE_EXT_EN > 0
INT8U  OSTaskStkChk (INT8U prio, OS_STK_DATA *pdata)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif
    OS_TCB    *ptcb;
    OS_STK    *pchk;
    INT32U     free;
    INT32U     size;


#if OS_ARG_CHK_EN > 0
    if (prio > OS_LOWEST_PRIO && prio != OS_PRIO_SELF) {        /* Make sure task priority is valid    */
        return (OS_PRIO_INVALID);//���OS_ARG_CHK_EN����Ϊ1�����ȼ�ֵ�������ֵ�Ҳ��� OS_PRIO_SELF
    }
#endif
    pdata->OSFree = 0;                                          /* Assume failure, set to 0 size       */
    pdata->OSUsed = 0;//��ʼ��Ϊ0
    OS_ENTER_CRITICAL();
    if (prio == OS_PRIO_SELF) {                        /* See if check for SELF                        */
        prio = OSTCBCur->OSTCBPrio;//�����֪����ǰ�����ջ��Ϣ
    }
    ptcb = OSTCBPrioTbl[prio];//��ȡ���ȼ���������㣬�����������
    if (ptcb == (OS_TCB *)0) {                         /* Make sure task exist                         */
        OS_EXIT_CRITICAL();
        return (OS_TASK_NOT_EXIST);//���񲻴���
    }
    if ((ptcb->OSTCBOpt & OS_TASK_OPT_STK_CHK) == 0) { /* Make sure stack checking option is set       */
		//Ҫ��֤������飬Ҫ��֤�Ѿ����������񣬲������˲���OS_TASK_OPT_STK_CHK
        //��������������OSTaskCreate(),������OSTaskCreateExt(),��ô��Ϊ����optΪ�㣬
        //���Լ���ʧ�ܡ�
        OS_EXIT_CRITICAL();
        return (OS_TASK_OPT_ERR);
    }
    free = 0;
    size = ptcb->OSTCBStkSize;//��������������㣬OSTaskStkChk�ͻ���ǰ�������������Ӷ�ջջ��
                              //��ʼͳ�ƶ�ջ�Ŀ��пռ䣬ֱ������һ������ֵ����Ķ�ջ��ڡ�
    pchk = ptcb->OSTCBStkBottom;
    OS_EXIT_CRITICAL();
#if OS_STK_GROWTH == 1//�����ջ���óɴӸߵ�������
    while (*pchk++ == (OS_STK)0) {                    /* Compute the number of zero entries on the stk */
        free++;//����տ�
    }
#else//����ӵ͵�������
    while (*pchk-- == (OS_STK)0) {
        free++;
    }
#endif
    pdata->OSFree = free * sizeof(OS_STK);            /* Compute number of free bytes on the stack     */
//����ն�ջ�ֽ�
    pdata->OSUsed = (size - free) * sizeof(OS_STK);   /* Compute number of bytes used on the stack     */
//�������ö�ջ�ֽ�
    return (OS_NO_ERR);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                            SUSPEND A TASK
*
* Description: This function is called to suspend a task.  The task can be the calling task if the
*              priority passed to OSTaskSuspend() is the priority of the calling task or OS_PRIO_SELF.
*
* Arguments  : prio     is the priority of the task to suspend.  If you specify OS_PRIO_SELF, the
*                       calling task will suspend itself and rescheduling will occur.
*
* Returns    : OS_NO_ERR                if the requested task is suspended
*              OS_TASK_SUSPEND_IDLE     if you attempted to suspend the idle task which is not allowed.
*              OS_PRIO_INVALID          if the priority you specify is higher that the maximum allowed
*                                       (i.e. >= OS_LOWEST_PRIO) or, you have not specified OS_PRIO_SELF.
*              OS_TASK_SUSPEND_PRIO     if the task to suspend does not exist
*
* Note       : You should use this function with great care.  If you suspend a task that is waiting for
*              an event (i.e. a message, a semaphore, a queue ...) you will prevent this task from
*              running when the event arrives.
*********************************************************************************************************
*/
/*
*********************************************************************************************************
                                                    ����һ������
���������ô˺���ȥ����һ������������͵�OSTaskSuspend()����������ȼ���Ҫ��������������
           OS_PRIO_SELF����ô������񽫱�����
������ prio����Ҫ������������ȼ������ָ��OS_PRIO_SELF����ô��������Լ������ٷ�����
                 �ε��ȡ�
���أ�OS_NO_ERR�������������񱻹���
            OS_TASK_SUSPEND_IDLE�����������������
            OS_PRIO_INVALID  ��������������ȼ�������
            OS_TASK_SUSPEND_PRIO����Ҫ��������񲻴��ڡ�
��ע������ʱҪʮ��С�ģ�����������һ���ȴ��¼������䣬��Ϣ�����У��������¼�������ʱ��
            �㽫��ֹ����������С�

*********************************************************************************************************
*/



#if OS_TASK_SUSPEND_EN > 0
INT8U  OSTaskSuspend (INT8U prio)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif
    BOOLEAN    self;
    OS_TCB    *ptcb;


#if OS_ARG_CHK_EN > 0
    if (prio == OS_IDLE_PRIO) {                                 /* Not allowed to suspend idle task    */
        return (OS_TASK_SUSPEND_IDLE);//���ܹ����������
    }
    if (prio >= OS_LOWEST_PRIO && prio != OS_PRIO_SELF) {       /* Task priority valid ?               */
        return (OS_PRIO_INVALID);//�������ȼ�������
    }
#endif
    OS_ENTER_CRITICAL();
    if (prio == OS_PRIO_SELF) {                                 /* See if suspend SELF                 */
		//�ǲ���Ҫ�����Լ���������OSTaskSuspend ��ӵ�ǰ�����������ƿ���
		//��õ�ǰ��������ȼ�
        prio = OSTCBCur->OSTCBPrio;
        self = TRUE;
    } else if (prio == OSTCBCur->OSTCBPrio) {                   /* See if suspending self              */
        self = TRUE;//Ҳ����ͨ��ָ�����ȼ���������ñ���������������������£�
        //������ȶ������ã�����ΪʲôҪ����ֲ�����self��ԭ�򣬸ñ�����
        //�ʵ���ʱ��ᱻ���ԣ����û�й�����ñ�����������OSTaskSus_pend()
        //��û�б�Ҫ����������ȳ�����Ϊ���øú������������ڹ���һ��
        //���ȼ��Ƚϵ͵�����
    } else {
        self = FALSE;                                           /* No suspending another task          */
    //����Ҫ�����Լ�
    }
    ptcb = OSTCBPrioTbl[prio];//ȡ��Ҫ���������TCB
    if (ptcb == (OS_TCB *)0) {                                  /* Task to suspend must exist          */
        OS_EXIT_CRITICAL();
        return (OS_TASK_SUSPEND_PRIO);//���Ҫ��������񲻴���
    }
    if ((OSRdyTbl[ptcb->OSTCBY] &= ~ptcb->OSTCBBitX) == 0x00) { /* Make task not ready                 */
        OSRdyGrp &= ~ptcb->OSTCBBitY;//�����������ڣ��ͻ�Ӿ�������ȥ����
//Ҫ�����������ܲ��ھ������У��п����ڵȴ��¼����������ӳ١�Ҫ
//�����������OSRdyTbl[]�ж�Ӧλ�ѱ�������ٴ������λ�����ȼ����λ�Ƿ�
//��������û������������ö࣬���Ծ�û�м����ˡ�
    }
    ptcb->OSTCBStat |= OS_STAT_SUSPEND;                         /* Status of task is 'SUSPENDED'       */
	//���ڹ�������
    OS_EXIT_CRITICAL();
    if (self == TRUE) {                                         /* Context switch only if SELF         */
        OS_Sched();//�����ڹ��������Լ�������²ŵ����������
    }
    return (OS_NO_ERR);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                            QUERY A TASK
*
* Description: This function is called to obtain a copy of the desired task's TCB.
*
* Arguments  : prio     is the priority of the task to obtain information from.
*
* Returns    : OS_NO_ERR       if the requested task is suspended
*              OS_PRIO_INVALID if the priority you specify is higher that the maximum allowed
*                              (i.e. > OS_LOWEST_PRIO) or, you have not specified OS_PRIO_SELF.
*              OS_PRIO_ERR     if the desired task has not been created
*********************************************************************************************************
*/
/*
*********************************************************************************************************
                                                                                                  ��ѯһ������
�������˺�������ȥ���һ��ָ������TCB�ĸ���
������prio:ָ�����������ȼ�
����:    OS_NO_ERR:����ĺ���������
                 OS_PRIO_INVALID:�������ȼ����Ϸ�
                 OS_PRIO_ERR���ָ���ĺ�����û�д���
*********************************************************************************************************
*/


#if OS_TASK_QUERY_EN > 0
INT8U  OSTaskQuery (INT8U prio, OS_TCB *pdata)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif
    OS_TCB    *ptcb;


#if OS_ARG_CHK_EN > 0
    if (prio > OS_LOWEST_PRIO && prio != OS_PRIO_SELF) {   /* Task priority valid ?                    */
        return (OS_PRIO_INVALID);//���ȼ����Ϸ�
    }
#endif
    OS_ENTER_CRITICAL();
    if (prio == OS_PRIO_SELF) {                            /* See if suspend SELF                      */
        prio = OSTCBCur->OSTCBPrio;//�����Լ�
    }
    ptcb = OSTCBPrioTbl[prio];//�������Լ�
    if (ptcb == (OS_TCB *)0) {                             /* Task to query must exist                 */
		//���񲻴���
        OS_EXIT_CRITICAL();
        return (OS_PRIO_ERR);//���ش���
    }
    memcpy(pdata, ptcb, sizeof(OS_TCB));                   /* Copy TCB into user storage area          */
	//�����TCB���Ƶ��û��Ĵ洢�ռ�
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif
