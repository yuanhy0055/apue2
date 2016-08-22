/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                             CORE FUNCTIONS
*
*                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* File : OS_CORE.C
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#ifndef  OS_MASTER_FILE//��ֹһЩ�ļ����뵽includes.h����
#define  OS_GLOBALS
#include "includes.h"
#endif

/*
*********************************************************************************************************
*                              MAPPING TABLE TO MAP BIT POSITION TO BIT MASK
*
* Note: Index into table is desired bit position, 0..7
*       Indexed value corresponds to bit mask
*********************************************************************************************************
*/
/*
*********************************************************************************************************
                                                ӳ��λλ��ӳ�䵽λ�����ӳ���  
��ע�������������õ���λ��λ�ã�0��7��
                 �����õ���ֵ��λ������
*********************************************************************************************************
*/


INT8U  const  OSMapTbl[]   = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

/*
*********************************************************************************************************
*                                       PRIORITY RESOLUTION TABLE
*
* Note: Index into table is bit pattern to resolve highest priority
*       Indexed value corresponds to highest priority bit position (i.e. 0..7)
*********************************************************************************************************
*/
/*
**********************************************************************************************
                                       ���ȼ��ֱ��
��ע����������������ȼ���λģʽ
                 ����ֵ��Ӧ��������ȼ���λλ�ã�0��7��
***********************************************************************************************
*/

INT8U  const  OSUnMapTbl[] = {
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x00 to 0x0F                             */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x10 to 0x1F                             */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x20 to 0x2F                             */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x30 to 0x3F                             */
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x40 to 0x4F                             */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x50 to 0x5F                             */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x60 to 0x6F                             */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x70 to 0x7F                             */
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x80 to 0x8F                             */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x90 to 0x9F                             */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xA0 to 0xAF                             */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xB0 to 0xBF                             */
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xC0 to 0xCF                             */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xD0 to 0xDF                             */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xE0 to 0xEF                             */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0        /* 0xF0 to 0xFF                             */
};//256�������������أ�

/*
*********************************************************************************************************
*                                       FUNCTION PROTOTYPES
*********************************************************************************************************
*/
//����Ϊ��ʼ������
static  void  OS_InitEventList(void);//��ʼ���¼����ƿ�Ŀձ�
static  void  OS_InitMisc(void);//��ʼ������Լ����ֵ
static  void  OS_InitRdyList(void);//��ʼ�������б�
static  void  OS_InitTaskIdle(void);//������������
static  void  OS_InitTaskStat(void);//����һ��ͳ������
static  void  OS_InitTCBList(void);//��ʼ��������ƿ�Ŀ��б�

/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*
* Description: This function is used to initialize the internals of uC/OS-II and MUST be called prior to
*              creating any uC/OS-II object and, prior to calling OSStart().
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/
/*
*********************************************************************************************************
                                                             ��ʼ��
 ���ܣ�Ҫ���ڵ���  OSStart()���ٳ�ʼ��uxosII�ںˣ�Ҫ�Ƚ���ucos�κ�һ������                                                        
*********************************************************************************************************
*/



void  OSInit (void)
{
#if OS_VERSION >= 204
    OSInitHookBegin();                                           /* Call port specific initialization code   */
//�����ض�ͨ�ſڳ�ʼ������
#endif

    OS_InitMisc();                                               /* Initialize miscellaneous variables       */
//��ʼ�����ֱ���
    OS_InitRdyList();                                            /* Initialize the Ready List                */
//��ʼ����������
    OS_InitTCBList();                                            /* Initialize the free list of OS_TCBs      */
//��ʼ��TCB��������
    OS_InitEventList();                                          /* Initialize the free list of OS_EVENTs    */
//��ʼ���¼���������

#if (OS_VERSION >= 251) && (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0)
    OS_FlagInit();                                               /* Initialize the event flag structures     */
//��ʼ���¼���־�ṹ
#endif

#if (OS_MEM_EN > 0) && (OS_MAX_MEM_PART > 0)
    OS_MemInit();                                                /* Initialize the memory manager            */
//��ʼ�����ڹ���
#endif

#if (OS_Q_EN > 0) && (OS_MAX_QS > 0)
    OS_QInit();                                                  /* Initialize the message queue structures  */
//��ʼ����Ϣ���нṹ
#endif

    OS_InitTaskIdle();                                           /* Create the Idle Task                     */
//������������
#if OS_TASK_STAT_EN > 0
    OS_InitTaskStat();                                           /* Create the Statistic Task                */
//����ͳ������
#endif

#if OS_VERSION >= 204
    OSInitHookEnd();                                             /* Call port specific init. code            */
//����ר��ͨ�ſڳ�ʼ������
#endif
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                              ENTER ISR
*
* Description: This function is used to notify uC/OS-II that you are about to service an interrupt
*              service routine (ISR).  This allows uC/OS-II to keep track of interrupt nesting and thus
*              only perform rescheduling at the last nested ISR.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) This function should be called ith interrupts already disabled
*              2) Your ISR can directly increment OSIntNesting without calling this function because
*                 OSIntNesting has been declared 'global'.  
*              3) You MUST still call OSIntExit() even though you increment OSIntNesting directly.
*              4) You MUST invoke OSIntEnter() and OSIntExit() in pair.  In other words, for every call
*                 to OSIntEnter() at the beginning of the ISR you MUST have a call to OSIntExit() at the
*                 end of the ISR.
*              5) You are allowed to nest interrupts up to 255 levels deep.
*              6) I removed the OS_ENTER_CRITICAL() and OS_EXIT_CRITICAL() around the increment because
*                 OSIntEnter() is always called with interrupts disabled.
                                             �����жϷ������
������֪ͨucosII�������жϷ������������ucos�����ж�Ƕ�ף�����ֻ��
                 //�����һ���ж�Ƕ�ײ����°��š�
��������
���أ���
ע�⣺1��ֻ���ڹ��жϵ�ʱ��ŵ���
                 2����Ϊ�ж�Ƕ�׶���Ϊȫ�ֱ�����������û�е��ô˺�����ʱ��
                         Ҳ���������ж�Ƕ����
                 3����ʹ���������ж�Ƕ��������ҲҪ����OSIntExit()
                 4�������ɶԵ���OSIntEnter() and OSIntExit()������ÿ����ISR��ʼʱ����OSIntEnter() 
                        �㽫��ISR����ʱ����OSIntExit()
                 5�������Ƕ��255����
                 6����Ϊÿ�ε���ʱ�ж��ǹصģ�������ȥ����OS_ENTER_CRITICAL() and OS_EXIT_CRITICAL()
*********************************************************************************************************
*/

void  OSIntEnter (void)
{
    if (OSRunning == TRUE) {
        if (OSIntNesting < 255) {
            OSIntNesting++;                      /* Increment ISR nesting level                        */
        }//�����ж�Ƕ����
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                               EXIT ISR
*
* Description: This function is used to notify uC/OS-II that you have completed serviving an ISR.  When
*              the last nested ISR has completed, uC/OS-II will call the scheduler to determine whether
*              a new, high-priority task, is ready to run.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) You MUST invoke OSIntEnter() and OSIntExit() in pair.  In other words, for every call
*                 to OSIntEnter() at the beginning of the ISR you MUST have a call to OSIntExit() at the
*                 end of the ISR.
*                 2) Rescheduling is prevented when the scheduler is locked (see OS_SchedLock())
                                       �˳��ж�
������֪ͨucos���Ѿ�����жϷ�����򣬵����һ��Ƕ����ɺ�ucos��
                 ���µ��ȿ��Ƿ�һ���µģ������ȼ����񽫾�����
��������
���أ���
��ע��1�������ɶԵ���OSIntEnter() and OSIntExit()������ÿ����ISR��ʼʱ����OSIntEnter() 
                        �㽫��ISR����ʱ����OSIntExit()
                 2�������������Ļ������µ����ǲ�����ġ���OS_SchedLock()(��������)
*********************************************************************************************************
*/

void  OSIntExit (void)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    
    
    if (OSRunning == TRUE) {//�����������
        OS_ENTER_CRITICAL();
        if (OSIntNesting > 0) {                            /* Prevent OSIntNesting from wrapping       */
			//��ֹ������0��С
            OSIntNesting--;
        }
        if ((OSIntNesting == 0) && (OSLockNesting == 0)) { /* Reschedule only if all ISRs complete ... */
            OSIntExitY    = OSUnMapTbl[OSRdyGrp];          /* ... and not locked.                      */
            OSPrioHighRdy = (INT8U)((OSIntExitY << 3) + OSUnMapTbl[OSRdyTbl[OSIntExitY]]);
			//���������ˣ����ǲ����㰡���Ľ�Ҫ����
			//���������ˣ���������������������������
            if (OSPrioHighRdy != OSPrioCur) {              /* No Ctx Sw if current task is highest rdy */
                OSTCBHighRdy  = OSTCBPrioTbl[OSPrioHighRdy];
				//�����ǰ������������ȼ�����������ȼ�����������ȼ���־�С�
                OSCtxSwCtr++;                              /* Keep track of the number of ctx switches */
				//��������ת������
                OSIntCtxSw();                              /* Perform interrupt level ctx switch       */
				//ִ���ж�ˮƽ�����л�������
            }
        }
        OS_EXIT_CRITICAL();
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                          PREVENT SCHEDULING
*
* Description: This function is used to prevent rescheduling to take place.  This allows your application
*              to prevent context switches until you are ready to permit context switching.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) You MUST invoke OSSchedLock() and OSSchedUnlock() in pair.  In other words, for every
*                 call to OSSchedLock() you MUST have a call to OSSchedUnlock().
                                                    ������������
��������ֹ�ٴε��ȷ�����������׼��ִ�������л���ʱ��Ž��������л�
//��������
���أ���
��ע��1���������OSSchedLock() and OSSchedUnlock()�ɶ�
*********************************************************************************************************
*/

#if OS_SCHED_LOCK_EN > 0
void  OSSchedLock (void)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    
    
    if (OSRunning == TRUE) {                     /* Make sure multitasking is running                  */
//��֤������������
        OS_ENTER_CRITICAL();
        if (OSLockNesting < 255) {               /* Prevent OSLockNesting from wrapping back to 0      */
			//��ֹǶ�������ص�0
            OSLockNesting++;                     /* Increment lock nesting level                       */
        }
        OS_EXIT_CRITICAL();
    }
}
#endif    

/*$PAGE*/
/*
*********************************************************************************************************
*                                          ENABLE SCHEDULING
*
* Description: This function is used to re-allow rescheduling.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) You MUST invoke OSSchedLock() and OSSchedUnlock() in pair.  In other words, for every
*                 call to OSSchedLock() you MUST have a call to OSSchedUnlock().
                                                  ��ʹ�������
�����������ٴ������������
��������
���أ���
��ע��1�����뽫OSSchedLock() and OSSchedUnlock()�ɶԵ���
*********************************************************************************************************
*/

#if OS_SCHED_LOCK_EN > 0//��ʹ��������OSSchedLock() and OSSchedUnlock()
void  OSSchedUnlock (void)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif    
    
    
    if (OSRunning == TRUE) {                                   /* Make sure multitasking is running    */
		//��֤����������
        OS_ENTER_CRITICAL();
        if (OSLockNesting > 0) {                               /* Do not decrement if already 0        */
			//�������Ļ��Ͳ�Ҫ�ټ���
            OSLockNesting--;                                   /* Decrement lock nesting level         */
            if ((OSLockNesting == 0) && (OSIntNesting == 0)) { /* See if sched. enabled and not an ISR */
				//������������ж�Ƕ����Ϊ�㣬����Ҫ������ȣ���������
				//�������Ƿ����
                OS_EXIT_CRITICAL();
                OS_Sched();                                    /* See if a HPT is ready                */
            } else {
                OS_EXIT_CRITICAL();//����Ͳ�Ҫ���������
            }
        } else {
            OS_EXIT_CRITICAL();//��������Ļ����Ϳ��ж��ˡ�
        }
    }
}
#endif    

/*$PAGE*/
/*
*********************************************************************************************************
*                                          START MULTITASKING
*
* Description: This function is used to start the multitasking process which lets uC/OS-II manages the
*              task that you have created.  Before you can call OSStart(), you MUST have called OSInit()
*              and you MUST have created at least one task.
*
* Arguments  : none
*
* Returns    : none
*
* Note       : OSStartHighRdy() MUST:
*                 a) Call OSTaskSwHook() then,
*                 b) Set OSRunning to TRUE.
*                 c) Load the context of the task pointed to by OSTCBHighRdy.
*                 d_ Execute the task.
                                                  ��ʼ��������
��������ʼ��������ʹucos�����㽨���������ڵ���OSStart()ǰ�������
                 �ȵ���OSInit()�������ٽ�����һ������
��������
���أ���
��ע��OSStartHighRdy()����
                 1���ȵ���Call OSTaskSwHook()
                 2��������OSRunningΪ��
                 3��װ��ָ��OSTCBHighRdy�����ݵ�ָ��
                 4��ִ������
*********************************************************************************************************
*/

void  OSStart (void)
{
    INT8U y;
    INT8U x;


    if (OSRunning == FALSE) {//���û������
        y             = OSUnMapTbl[OSRdyGrp];        /* Find highest priority's task priority number   */
        x             = OSUnMapTbl[OSRdyTbl[y]];//���������ǣ�����FUCK��
        OSPrioHighRdy = (INT8U)((y << 3) + x);
        OSPrioCur     = OSPrioHighRdy;//�����ȼ���������Ϊ��ǰ����
        OSTCBHighRdy  = OSTCBPrioTbl[OSPrioHighRdy]; /* Point to highest priority task ready to run    */
		//��һ��������Ϊ�������е�����
        OSTCBCur      = OSTCBHighRdy;
        OSStartHighRdy();                            /* Execute target specific code to start task     */
		//ִ���ض�����ȥ��ʼ����
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        STATISTICS INITIALIZATION
*
* Description: This function is called by your application to establish CPU usage by first determining
*              how high a 32-bit counter would count to in 1 second if no other tasks were to execute
*              during that time.  CPU usage is then determined by a low priority task which keeps track
*              of this 32-bit counter every second but this time, with other tasks running.  CPU usage is
*              determined by:
*
*                                                            OSIdleCtr
*                 CPU Usage (%) = 100 * (1 - ��������)
*                                                          OSIdleCtrMax
*
* Arguments  : none
*
* Returns    : none
                                                 ͳ�������ʼ��
����������û���������������ʱ�����У�����һ����һ��32λ��������
                 �Ƶ�������������CPUʹ����CPUʹ������һ��ÿ�����32λ�������ĵ�����
                 ���������������������������У�CPUʹ���������溯��������
  *                                                            OSIdleCtr
*                 CPU Usage (%) = 100 * (1 - ��������)
*                                                          OSIdleCtrMax      
��������
���أ���
*********************************************************************************************************
*/

#if OS_TASK_STAT_EN > 0
void  OSStatInit (void)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    
    
    OSTimeDly(2);                                /* Synchronize with clock tick                        */
//��ʱ�ӽ���ͬ��
    OS_ENTER_CRITICAL();
    OSIdleCtr    = 0L;                           /* Clear idle counter                                 */
	//������м����� 
    OS_EXIT_CRITICAL();
    OSTimeDly(OS_TICKS_PER_SEC);                 /* Determine MAX. idle counter value for 1 second     */
    //����һ���ڿ��м������ܼƶ���
    OS_ENTER_CRITICAL();
    OSIdleCtrMax = OSIdleCtr;                    /* Store maximum idle counter count in 1 second       */
	//�������ֵ
    OSStatRdy    = TRUE;//ͳ���������
    OS_EXIT_CRITICAL();
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                         PROCESS SYSTEM TICK
*
* Description: This function is used to signal to uC/OS-II the occurrence of a 'system tick' (also known
*              as a 'clock tick').  This function should be called by the ticker ISR but, can also be
*              called by a high priority task.
*
* Arguments  : none
*
* Returns    : none
                                               ����ϵͳʱ��
���������������ucos���źŲ���ʱ�ӽ��ģ����ܱ�ISR���ĵ��ã�Ҳ������
                 �����ȼ��������
��������
���أ���
*********************************************************************************************************
*/

void  OSTimeTick (void)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif    
    OS_TCB    *ptcb;


    OSTimeTickHook();                                      /* Call user definable hook                 */
	//�û������ʱ�ӽ����������������ɽ�ʱ�ӽ��ĺ���OSTimeTick��չ�����ô�
	//�����Ǵ������ж�һ��ʼ���û�һ����������ʲô�Ļ��ᣬ
#if OS_TIME_GET_SET_EN > 0   //��ʹ��������OSTimeGet() and OSTimeSet()
    OS_ENTER_CRITICAL();                                   /* Update the 32-bit tick counter           */
    OSTime++;//ϵͳ�����ֽ׶�ֵ��������ϵͳ�ϵ�������ʱ�ӽ�����
    OS_EXIT_CRITICAL();
#endif
    if (OSRunning == TRUE) {    
        ptcb = OSTCBList;                                  /* Point at first TCB in TCB list           */
		//ָ��PCB˫�������еĵ�һ��
        while (ptcb->OSTCBPrio != OS_IDLE_PRIO) {          /* Go through all TCBs in TCB list          */
			//��PCB�����е�TCB����һ�飬һֱ������������
            OS_ENTER_CRITICAL();
            if (ptcb->OSTCBDly != 0) {                     /* Delayed or waiting for event with TO     */
                if (--ptcb->OSTCBDly == 0) {               /* Decrement nbr of ticks to end of delay   */
                    if ((ptcb->OSTCBStat & OS_STAT_SUSPEND) == OS_STAT_RDY) { /* Is task suspended?    */
						//ȷʵ����������񲻻�������̬
                        OSRdyGrp               |= ptcb->OSTCBBitY; /* No,  Make task R-to-R (timed out)*/
			//���ĳ�����TCB�е�ʱ����ʱ��OSTCBDly��Ϊ0ʱ���������ͽ����˾���̬
                        OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
                    } else {                               /* Yes, Leave 1 tick to prevent ...         */
   //����һ�����ķ�ֹȥ�������ʱ�������ȶ�
                        ptcb->OSTCBDly = 1;                /* ... loosing the task when the ...        */
                    }                                      /* ... suspension is removed.               */
                }
            }
            ptcb = ptcb->OSTCBNext;                        /* Point at next TCB in TCB list            */
			//ָ��TCB�������һ��
            OS_EXIT_CRITICAL();
        }
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                             GET VERSION
*
* Description: This function is used to return the version number of uC/OS-II.  The returned value
*              corresponds to uC/OS-II's version number multiplied by 100.  In other words, version 2.00
*              would be returned as 200.
*
* Arguments  : none
*
* Returns    : the version number of uC/OS-II multiplied by 100.
                                                  �汾��
����������UCOS�汾�źţ�����ֵΪUCOS�汾*100�����仰˵��2.00�汾��
                 ����200
 ��������
 ���أ��汾��*100
*********************************************************************************************************
*/

INT16U  OSVersion (void)
{
    return (OS_VERSION);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                            DUMMY FUNCTION
*
* Description: This function doesn't do anything.  It is called by OSTaskDel().
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/
/*
*********************************************************************************************************
                                                         ���⺯����
�������˺��������������飬��OSTaskDel().����
��������
���أ���
*********************************************************************************************************
*/


#if OS_TASK_DEL_EN > 0
void  OS_Dummy (void)
{           //�����κ���
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                             MAKE TASK READY TO RUN BASED ON EVENT OCCURING
*
* Description: This function is called by other uC/OS-II services and is used to ready a task that was
*              waiting for an event to occur.
*
* Arguments  : pevent    is a pointer to the event control block corresponding to the event.
*
*              msg       is a pointer to a message.  This pointer is used by message oriented services
*                        such as MAILBOXEs and QUEUEs.  The pointer is not used when called by other
*                        service functions.
*
*              msk       is a mask that is used to clear the status byte of the TCB.  For example,
*                        OSSemPost() will pass OS_STAT_SEM, OSMboxPost() will pass OS_STAT_MBOX etc.
*
* Returns    : none
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
                                                     �����¼�����ʹ����׼������
�������ú�����UCOS����������ã�����ʹһ������������ȴ�һ���¼�����
������pevent��ָ���Ӧ�¼���ECB��ָ��
                 msg����Ϣָ�룬��������е���Ϣ�������ʹ�ã���ָ�벻��������
                              ����������
                msk�����������TCB״̬�ֽڵ����룬���磺
                             OSSemPost() ������TAT_SEM, OSMboxPost() ������OS_STAT_MBOX

����������ò��Ǻܶ�
*********************************************************************************************************
*/
#if OS_EVENT_EN > 0  
//��ʹ���д������&&������п��ƿ��������Ϊ��||��ʹ����������||
//��ʹ�ź����������||��ʹ������������� 

INT8U  OS_EventTaskRdy (OS_EVENT *pevent, void *msg, INT8U msk)
{
    OS_TCB *ptcb;
    INT8U   x;
    INT8U   y;
    INT8U   bitx;
    INT8U   bity;
    INT8U   prio;


    y    = OSUnMapTbl[pevent->OSEventGrp];            /* Find highest prio. task waiting for message   */
    bity = OSMapTbl[y];//Ѱ�ҵȴ���Ϣ��������ȼ�����
    x    = OSUnMapTbl[pevent->OSEventTbl[y]];
    bitx = OSMapTbl[x];
    prio = (INT8U)((y << 3) + x);                     /* Find priority of task getting the msg         */
    if ((pevent->OSEventTbl[y] &= ~bitx) == 0x00) {   /* Remove this task from the waiting list        */
		//��������ӵȴ��б����Ƴ���
        pevent->OSEventGrp &= ~bity;                  /* Clr group bit if this was only task pending   */
		//�����Ψһ�����������ô�����λ��
    }
    ptcb                 =  OSTCBPrioTbl[prio];       /* Point to this task's OS_TCB                   */
	//ָ��ָ��ǰ�����OS_TCB 
    ptcb->OSTCBDly       =  0;                        /* Prevent OSTimeTick() from readying task       */
	//��ֹʱ�ӽ���ʹ����������������У�
    ptcb->OSTCBEventPtr  = (OS_EVENT *)0;             /* Unlink ECB from this task                     */
	//�����������϶Ͽ�ECB
#if ((OS_Q_EN > 0) && (OS_MAX_QS > 0)) || (OS_MBOX_EN > 0)
    ptcb->OSTCBMsg       = msg;                       /* Send message directly to waiting task         */
//����Ϣֱ�ӷ��͵��ȴ�������
#else
    msg                  = msg;                       /* Prevent compiler warning if not used          */
//��ֹ����������
#endif
    ptcb->OSTCBStat     &= ~msk;                      /* Clear bit associated with event type          */
//�¼�״̬λ���
    if (ptcb->OSTCBStat == OS_STAT_RDY) {             /* See if task is ready (could be susp'd)        */
		//�����������������ܱ�����
        OSRdyGrp        |=  bity;                     /* Put task in the ready to run list             */
        OSRdyTbl[y]     |=  bitx;
    }
    return (prio);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                   MAKE TASK WAIT FOR EVENT TO OCCUR
*
* Description: This function is called by other uC/OS-II services to suspend a task because an event has
*              not occurred.
*
* Arguments  : pevent   is a pointer to the event control block for which the task will be waiting for.
*
* Returns    : none
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
                                  ʹ����ȴ��¼�����
��������ucosII����������ȥ����һ��������Ϊһ���¼���û�з���
������pevent  ָ��Ҫ�ȴ��������ECB��ָ��
���أ���
��ע��ucos�ڲ����ã�����Ӧ�ó����ܵ�����
*********************************************************************************************************
*/

#if OS_EVENT_EN > 0
#define  OS_EVENT_EN       (((OS_Q_EN > 0) && (OS_MAX_QS > 0)) || (OS_MBOX_EN > 0) || (OS_SEM_EN > 0) || (OS_MUTEX_EN > 0))
//��ʹ���д������&&������п��ƿ��������Ϊ��||��ʹ����������||
//��ʹ�ź����������||��ʹ������������� 

void  OS_EventTaskWait (OS_EVENT *pevent)
{
    OSTCBCur->OSTCBEventPtr = pevent;            /* Store pointer to event control block in TCB        */
//����ECBָ�뵽TCB
    if ((OSRdyTbl[OSTCBCur->OSTCBY] &= ~OSTCBCur->OSTCBBitX) == 0x00) {   /* Task no longer ready      */
		//�������û�о������Ǿ͹����𣿣�
        OSRdyGrp &= ~OSTCBCur->OSTCBBitY;        /* Clear event grp bit if this was only task pending  */
		//�����û��һ���������Ļ�������¼�Ⱥλ��
    }
    pevent->OSEventTbl[OSTCBCur->OSTCBY] |= OSTCBCur->OSTCBBitX;          /* Put task in waiting list  */
    pevent->OSEventGrp                   |= OSTCBCur->OSTCBBitY;
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                              MAKE TASK READY TO RUN BASED ON EVENT TIMEOUT
*
* Description: This function is called by other uC/OS-II services to make a task ready to run because a
*              timeout occurred.
*
* Arguments  : pevent   is a pointer to the event control block which is readying a task.
*
* Returns    : none
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
                                                             ���¼���ʱ��������׼������
���������ڳ�ʱ��������ucos�����ø�����׼�����У�
������pevent�����������ECBָ��
���أ���
��ע��ucos�ڲ�����������Ӧ�ú������ܵ��á�
*********************************************************************************************************
*/
#if OS_EVENT_EN > 0
void  OS_EventTO (OS_EVENT *pevent)
{
    if ((pevent->OSEventTbl[OSTCBCur->OSTCBY] &= ~OSTCBCur->OSTCBBitX) == 0x00) {
        pevent->OSEventGrp &= ~OSTCBCur->OSTCBBitY;
    }//���һ����û��һ�����������������
    OSTCBCur->OSTCBStat     = OS_STAT_RDY;       /* Set status to ready         */
	//����ǰ��������Ϊ��������
    OSTCBCur->OSTCBEventPtr = (OS_EVENT *)0;     /* No longer waiting for event   */
	//���оͲ��ٵȴ�������
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                 INITIALIZE EVENT CONTROL BLOCK'S WAIT LIST
*
* Description: This function is called by other uC/OS-II services to initialize the event wait list.
*
* Arguments  : pevent    is a pointer to the event control block allocated to the event.
*
* Returns    : none
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
                                                        ��ʼ��ECB�ȴ��б�
��������ucos���ó�ʼ���¼��ȴ��б�
������pevent��ָ��ָ���¼���ECB��ָ��
���أ���
��ע���ڲ�������Ӧ�ú������ܵ���
*********************************************************************************************************
*/
#if ((OS_Q_EN > 0) && (OS_MAX_QS > 0)) || (OS_MBOX_EN > 0) || (OS_SEM_EN > 0) || (OS_MUTEX_EN > 0)
void  OS_EventWaitListInit (OS_EVENT *pevent)
{
    INT8U  *ptbl;


    pevent->OSEventGrp = 0x00;                   /* No task waiting on event     */
	//��ʼ��ʱ�¼����޵ȴ�������
    ptbl               = &pevent->OSEventTbl[0];//ȡ��ַ

#if OS_EVENT_TBL_SIZE > 0//������ѭ����ԭ�������������ٶȸ���
    *ptbl++            = 0x00;
#endif

#if OS_EVENT_TBL_SIZE > 1
    *ptbl++            = 0x00;
#endif

#if OS_EVENT_TBL_SIZE > 2
    *ptbl++            = 0x00;
#endif

#if OS_EVENT_TBL_SIZE > 3
    *ptbl++            = 0x00;
#endif

#if OS_EVENT_TBL_SIZE > 4
    *ptbl++            = 0x00;
#endif

#if OS_EVENT_TBL_SIZE > 5
    *ptbl++            = 0x00;
#endif

#if OS_EVENT_TBL_SIZE > 6
    *ptbl++            = 0x00;
#endif

#if OS_EVENT_TBL_SIZE > 7
    *ptbl              = 0x00;
#endif
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                           INITIALIZE THE FREE LIST OF EVENT CONTROL BLOCKS
*
* Description: This function is called by OSInit() to initialize the free list of event control blocks.
*
* Arguments  : none
*
* Returns    : none
��������ʼ���¼����ƿ�����б�
��������
���أ���
*********************************************************************************************************
*/

static  void  OS_InitEventList (void)
{
#if (OS_EVENT_EN > 0) && (OS_MAX_EVENTS > 0)
#if (OS_MAX_EVENTS > 1)//�������������һ
    INT16U     i;
    OS_EVENT  *pevent1;
    OS_EVENT  *pevent2;


    pevent1 = &OSEventTbl[0];//�¼����ƿ鷽���
    pevent2 = &OSEventTbl[1];
    for (i = 0; i < (OS_MAX_EVENTS - 1); i++) {                  /* Init. list of free EVENT control blocks  */
		//��ʼ����ECB����
        pevent1->OSEventType = OS_EVENT_TYPE_UNUSED;//״̬����Ϊ����
        pevent1->OSEventPtr  = pevent2;//��������������������
        pevent1++;
        pevent2++;//�Ӱɣ��Ӱɣ��ӵ�OS_MAX_EVENTS - 1
    }
    pevent1->OSEventType = OS_EVENT_TYPE_UNUSED;
    pevent1->OSEventPtr  = (OS_EVENT *)0;//ǰ��ָ����ָ��
    OSEventFreeList      = &OSEventTbl[0];//�ձ�ָ��OSEventTbl��һ��
#else//���ֻ��һ���¼����ƿ�
    OSEventFreeList              = &OSEventTbl[0];    /* Only have ONE event control block        */
    OSEventFreeList->OSEventType = OS_EVENT_TYPE_UNUSED;
    OSEventFreeList->OSEventPtr  = (OS_EVENT *)0;//ǰ��ָ��NULL
#endif
#endif
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                                    INITIALIZE MISCELLANEOUS VARIABLES
*
* Description: This function is called by OSInit() to initialize miscellaneous variables.
*
* Arguments  : none
*
* Returns    : none
��������OSInit()����ȥ��ʼ�����ֱ���
��������
���أ���
*********************************************************************************************************
*/

static  void  OS_InitMisc (void)
{
#if OS_TIME_GET_SET_EN > 0   
    OSTime        = 0L;                                          /* Clear the 32-bit system clock            */
#endif//���32λϵͳʱ��

    OSIntNesting  = 0;   /* Clear the interrupt nesting counter      *///�ж�Ƕ�׳�ʼ��Ϊ0
    OSLockNesting = 0;  /* Clear the scheduling lock counter        */
	//�����������������

    OSTaskCtr     = 0;                                           /* Clear the number of tasks                */
       //��ʼ�����������
    OSRunning     = FALSE;                                       /* Indicate that multitasking not started   */
    //��ʾ��������û�п�ʼ
    OSCtxSwCtr    = 0;                                           /* Clear the context switch counter         */
	//����ת��������Ϊ0
    OSIdleCtr     = 0L;                                          /* Clear the 32-bit idle counter            */
    //���32λ���м�����
#if (OS_TASK_STAT_EN > 0) && (OS_TASK_CREATE_EXT_EN > 0)
    OSIdleCtrRun  = 0L;
    OSIdleCtrMax  = 0L;
    OSStatRdy     = FALSE;                                       /* Statistic task is not ready              */
	//ͳ������û��׼�����أ�
#endif
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                                       INITIALIZE THE READY LIST
*
* Description: This function is called by OSInit() to initialize the Ready List.
*
* Arguments  : none
*
* Returns    : none
                                                 ��ʼ�������б�
��������OSInit()���ã���ʼ�������б�
��������
���أ���
*********************************************************************************************************
*/

static  void  OS_InitRdyList (void)
{
    INT16U   i;
    INT8U   *prdytbl;


    OSRdyGrp      = 0x00;                    /* Clear the ready list                     */
	//�ȴ��б�����
    prdytbl       = &OSRdyTbl[0];//�õ��������񷽿���׵�ַ
    for (i = 0; i < OS_RDY_TBL_SIZE; i++) {
        *prdytbl++ = 0x00;//ÿһ�鶼����
    }

    OSPrioCur     = 0;//��ǰ�������ȼ�
    OSPrioHighRdy = 0;//������ȼ���������ȼ�

    OSTCBHighRdy  = (OS_TCB *)0; //ָ��������ȼ�TCBָ��                                
    OSTCBCur      = (OS_TCB *)0;//��ǰ��������TCBָ��
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                                         CREATING THE IDLE TASK
*
* Description: This function creates the Idle Task.
*
* Arguments  : none
*
* Returns    : none
                                                 ������������
�������������������������
��������
���أ���
*********************************************************************************************************
*/

static  void  OS_InitTaskIdle (void)
{
#if OS_TASK_CREATE_EXT_EN > 0//���������������OS_TASK_CREATE_EXT_EN 
    #if OS_STK_GROWTH == 1//��ջ�Ӹߵ���
    (void)OSTaskCreateExt(OS_TaskIdle,//������������
                          (void *)0,                                 /* No arguments passed to OS_TaskIdle() */
                          //pdataΪ�㣬û�в������ݱ�ʾû�в�������OS_TaskIdle()
                          &OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE - 1], /* Set Top-Of-Stack                     */
                          //����ջ����ַ
                          OS_IDLE_PRIO,                              /* Lowest priority level                */
                          //���ȼ�
                          OS_TASK_IDLE_ID,//����ID
                          &OSTaskIdleStk[0],                         /* Set Bottom-Of-Stack                  */
                          //����ջ��
                          OS_TASK_IDLE_STK_SIZE,//��ջ��С�������ڿ��
                          (void *)0,                                 /* No TCB extension                     */
                          //��TCB��չ
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);/* Enable stack checking + clear stack  */
	                   //�����ջ���/�����ջ
    #else//��ջ�ӵ͵���
    (void)OSTaskCreateExt(OS_TaskIdle,
                          (void *)0,                                 /* No arguments passed to OS_TaskIdle() */
                          &OSTaskIdleStk[0],   /* Set Top-Of-Stack                     */
                          //�������෴
                          OS_IDLE_PRIO,                              /* Lowest priority level                */
                          OS_TASK_IDLE_ID,
                          &OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE - 1], /* Set Bottom-Of-Stack        */
                          //�������෴
                          OS_TASK_IDLE_STK_SIZE,
                          (void *)0,                                 /* No TCB extension                     */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);/* Enable stack checking + clear stack  */
    #endif
#else//�����������������OS_TASK_CREATE_EXT_EN 
    #if OS_STK_GROWTH == 1//��ջ�Ӹߵ���
    (void)OSTaskCreate(OS_TaskIdle,
                       (void *)0,
                       &OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE - 1],
                       OS_IDLE_PRIO);
    #else//��ջ�ӵ͵���
    (void)OSTaskCreate(OS_TaskIdle,
                       (void *)0,
                       &OSTaskIdleStk[0],
                       OS_IDLE_PRIO);
    #endif
#endif
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                                      CREATING THE STATISTIC TASK
*
* Description: This function creates the Statistic Task.
*
* Arguments  : none
*
* Returns    : none
                                               ����ͳ������
����������ͳ������
��������
���أ���
*********************************************************************************************************
*/
//�������ͬ����һ�����
#if OS_TASK_STAT_EN > 0
static  void  OS_InitTaskStat (void)
{
#if OS_TASK_CREATE_EXT_EN > 0
    #if OS_STK_GROWTH == 1
    (void)OSTaskCreateExt(OS_TaskStat,
                          (void *)0,                                   /* No args passed to OS_TaskStat()*/
                          &OSTaskStatStk[OS_TASK_STAT_STK_SIZE - 1],   /* Set Top-Of-Stack               */
                          OS_STAT_PRIO,                                /* One higher than the idle task  */
                          //�ȿ����������ȼ���һ
                          OS_TASK_STAT_ID,
                          &OSTaskStatStk[0],                           /* Set Bottom-Of-Stack            */
                          OS_TASK_STAT_STK_SIZE,
                          (void *)0,                                   /* No TCB extension               */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  /* Enable stack checking + clear  */
    #else
    (void)OSTaskCreateExt(OS_TaskStat,
                          (void *)0,                                   /* No args passed to OS_TaskStat()*/
                          &OSTaskStatStk[0],                           /* Set Top-Of-Stack               */
                          OS_STAT_PRIO,                                /* One higher than the idle task  */
                          OS_TASK_STAT_ID,
                          &OSTaskStatStk[OS_TASK_STAT_STK_SIZE - 1],   /* Set Bottom-Of-Stack            */
                          OS_TASK_STAT_STK_SIZE,
                          (void *)0,                                   /* No TCB extension               */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  /* Enable stack checking + clear  */
    #endif
#else
    #if OS_STK_GROWTH == 1
    (void)OSTaskCreate(OS_TaskStat,
                       (void *)0,                                      /* No args passed to OS_TaskStat()*/
                       &OSTaskStatStk[OS_TASK_STAT_STK_SIZE - 1],      /* Set Top-Of-Stack               */
                       OS_STAT_PRIO);                                  /* One higher than the idle task  */
    #else
    (void)OSTaskCreate(OS_TaskStat,
                       (void *)0,                                      /* No args passed to OS_TaskStat()*/
                       &OSTaskStatStk[0],                              /* Set Top-Of-Stack               */
                       OS_STAT_PRIO);                                  /* One higher than the idle task  */
    #endif
#endif
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                            INITIALIZE THE FREE LIST OF TASK CONTROL BLOCKS
*
* Description: This function is called by OSInit() to initialize the free list of OS_TCBs.
*
* Arguments  : none
*
* Returns    : none
                                                 ��ʼ��TCB����
��������OSInit()���ã���ʼ��OS_TCBs��������
*********************************************************************************************************
*/

static  void  OS_InitTCBList (void)
{
    INT8U    i;
    OS_TCB  *ptcb1;
    OS_TCB  *ptcb2;


    OSTCBList     = (OS_TCB *)0;                                 /* TCB Initialization                       */
	//��ʼ��˫��TCB����
    for (i = 0; i < (OS_LOWEST_PRIO + 1); i++) {                 /* Clear the priority table    */
        OSTCBPrioTbl[i] = (OS_TCB *)0;
    }//������ȼ������
    ptcb1 = &OSTCBTbl[0];
    ptcb2 = &OSTCBTbl[1];//ȡ��ַ
    for (i = 0; i < (OS_MAX_TASKS + OS_N_SYS_TASKS - 1); i++) {  /* Init. list of free TCBs                  */
        ptcb1->OSTCBNext = ptcb2;
        ptcb1++;
        ptcb2++;//����������һ��
    }
    ptcb1->OSTCBNext = (OS_TCB *)0;  /* Last OS_TCB    *///���һ��ָ���ָ��
    OSTCBFreeList    = &OSTCBTbl[0];//�ձ�ָ���һ����ַ
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                              SCHEDULER
*
* Description: This function is called by other uC/OS-II services to determine whether a new, high
*              priority task has been made ready to run.  This function is invoked by TASK level code
*              and is not used to reschedule tasks from ISRs (see OSIntExit() for ISR rescheduling).
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) This function is INTERNAL to uC/OS-II and your application should not call it.
*              2) Rescheduling is prevented when the scheduler is locked (see OS_SchedLock())
*********************************************************************************************************
*/
/*
************************************************************************************************
                                                                   ���ȳ���
�������������������ucosII�汾�������ȥ����Ƿ�һ���µģ������ȼ�������׼�����С�
                 �������������ȼ����뻽�ѣ��������ڴ��жϷ�����������°��Ŵ��롣
��������
���أ���
��ע�����������ucosII�ڲ��ģ���Ӧ�ó����в��ܵ���
                 �����ȳ��������Ļ������µ��Ƚ�����ֹ������OS_SchedLock()��

**************************************************************************************************
*/

void  OS_Sched (void)
{
#if OS_CRITICAL_METHOD == 3                            /* Allocate storage for CPU status register     */
    OS_CPU_SR  cpu_sr;//ΪCPU״̬�Ĵ�������洢�ռ�
#endif    
    INT8U      y;


    OS_ENTER_CRITICAL();//�����ٽ�״̬
    if ((OSIntNesting == 0) && (OSLockNesting == 0)) { /* Sched. only if all ISRs done & not locked    */
		//����ж�Ƕ�ײ�Ϊ�㣬������������Ƕ�ײ�Ϊ��
		//��ֻ��������ISR��ɣ���û������������µ���
        y             = OSUnMapTbl[OSRdyGrp];          /* Get pointer to HPT ready to run              */
		//�õ������ȼ�����̬������ָ��
        OSPrioHighRdy = (INT8U)((y << 3) + OSUnMapTbl[OSRdyTbl[y]]);
		//������ȼ�������������㷨����ʱ��Ҫ�о�һ�£����ڿ�����
        if (OSPrioHighRdy != OSPrioCur) {              /* No Ctx Sw if current task is highest rdy     */
			//���������ȼ������ǵ�ǰ����
            OSTCBHighRdy = OSTCBPrioTbl[OSPrioHighRdy];//��������ȼ����������������ָ��
            OSCtxSwCtr++;                              /* Increment context switch counter             */
			//������ת������һ
            OS_TASK_SW();                              /* Perform a context switch                     */
            //����������ת��
        }
    }
    OS_EXIT_CRITICAL();//�˳��ٽ�״̬
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                              IDLE TASK
*
* Description: This task is internal to uC/OS-II and executes whenever no other higher priority tasks
*              executes because they are ALL waiting for event(s) to occur.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 1) OSTaskIdleHook() is called after the critical section to ensure that interrupts will be
*                 enabled for at least a few instructions.  On some processors (ex. Philips XA), enabling
*                 and then disabling interrupts didn't allow the processor enough time to have interrupts
*                 enabled before they were disabled again.  uC/OS-II would thus never recognize
*                 interrupts.
*              2) This hook has been added to allow you to do such things as STOP the CPU to conserve 
*                 power.
                                           ��������
���������������ucos�ڲ������������������ڵ��¼�������
                û�и����ȼ��������е�ʱ����������
��������
���أ���
��ע��1�����ٽ��Ҫ����OSTaskIdleHook()��֤�ж�����������
                2������ܼ���չ����������һЩ���飬�磺Ϊ�˽��ܣ���CPU
                ֹͣ����
*********************************************************************************************************
*/

void  OS_TaskIdle (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    
    
    pdata = pdata;                               /* Prevent compiler warning for not using 'pdata'     */
    for (;;) {
        OS_ENTER_CRITICAL();
        OSIdleCtr++;//��һǰ���ж��ȹغ󿪣�����Ϊ8λ����ʮ��λ��������һ
        //��Ҫ����ָ���ֹ�жϴ��롣
        OS_EXIT_CRITICAL();
        OSTaskIdleHook();                        /* Call user definable HOOK                           */
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                            STATISTICS TASK
*
* Description: This task is internal to uC/OS-II and is used to compute some statistics about the
*              multitasking environment.  Specifically, OS_TaskStat() computes the CPU usage.
*              CPU usage is determined by:
*
*                                                     OSIdleCtr
*                 OSCPUUsage = 100 * (1 - ------------)     (units are in %)
*                                                    OSIdleCtrMax
*
* Arguments  : pdata     this pointer is not used at this time.
*
* Returns    : none
*
* Notes      : 1) This task runs at a priority level higher than the idle task.  In fact, it runs at the
*                 next higher priority, OS_IDLE_PRIO-1.
*              2) You can disable this task by setting the configuration #define OS_TASK_STAT_EN to 0.
*              3) We delay for 5 seconds in the beginning to allow the system to reach steady state and
*                 have all other tasks created before we do statistics.  You MUST have at least a delay
*                 of 2 seconds to allow for the system to establish the maximum value for the idle
*                 counter.
                                                         ͳ������
�����������������һЩͳ�ƣ�һ�����CPUʹ���ʣ���ʽ���£�
*                                                      OSIdleCtr
*                 OSCPUUsage = 100 * (1 - ------------)     (units are in %)
*                                                     OSIdleCtrMax
������pdata:��ʱû���õ�
���أ���
��ע��1�����������ȼ�ֻ��idle�ߣ�ʵ���ϣ��������ڽ�һ���ߵ�����
                       ���ϣ�OS_IDLE_PRIO-1
                2��ͨ������OS_TASK_STAT_ENΪ������ֹ������
                3��������ʱ5����ϵͳ�ȶ���ͳ��ǰ���ǽ�����������
                      ��������������ȥ�ÿ������������ֵ��

*********************************************************************************************************
*/

#if OS_TASK_STAT_EN > 0
void  OS_TaskStat (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    INT32U     run;
    INT32U     max;
    INT8S      usage;


    pdata = pdata;                               /* Prevent compiler warning for not using 'pdata'     */
    while (OSStatRdy == FALSE) {
        OSTimeDly(2 * OS_TICKS_PER_SEC);         /* Wait until statistic task is ready   */
		//��ʱ����OSIdleCr������û��ʲôӦ����������ʱ��������ô�������
		//��������ֵ��OSStatInit()�ڳ�ʼ��ʱ�������ڿ��м��������ֵOSIdleCtr�е�
    }
    max = OSIdleCtrMax / 100L;
    for (;;) {
        OS_ENTER_CRITICAL();
        OSIdleCtrRun = OSIdleCtr;                /* Obtain the of the idle counter for the past second */
        run          = OSIdleCtr;
        OSIdleCtr    = 0L;                       /* Reset the idle counter for the next second         */
		//�����������һ�β���
        OS_EXIT_CRITICAL();
        if (max > 0L) {
            usage = (INT8S)(100L - run / max);
            if (usage >= 0) {                    /* Make sure we don't have a negative percentage      */
                OSCPUUsage = usage;
            } else {
                OSCPUUsage = 0;
            }
        } else {
            OSCPUUsage = 0;
            max        = OSIdleCtrMax / 100L;
        }
        OSTaskStatHook();                        /* Invoke user definable hook           */
		//һ����ɣ��͵��������뺯��OSTaskStatHook()����ʹͳ������õ���չ��
		//�����û����Լ��㲢��ʾ����������ִ��ʱ�䣬ÿ������ִ�еİٷֱȵȡ�
        OSTimeDly(OS_TICKS_PER_SEC); /* Accumulate OSIdleCtr for the next second           */
		//Ϊ��һ����׼��
    }
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                            INITIALIZE TCB
*
* Description: This function is internal to uC/OS-II and is used to initialize a Task Control Block when
*              a task is created (see OSTaskCreate() and OSTaskCreateExt()).
*
* Arguments  : prio          is the priority of the task being created
*
*              ptos          is a pointer to the task's top-of-stack assuming that the CPU registers
*                            have been placed on the stack.  Note that the top-of-stack corresponds to a
*                            'high' memory location is OS_STK_GROWTH is set to 1 and a 'low' memory
*                            location if OS_STK_GROWTH is set to 0.  Note that stack growth is CPU
*                            specific.
*
*              pbos          is a pointer to the bottom of stack.  A NULL pointer is passed if called by
*                            'OSTaskCreate()'.
*
*              id            is the task's ID (0..65535)
*
*              stk_size      is the size of the stack (in 'stack units').  If the stack units are INT8Us
*                            then, 'stk_size' contains the number of bytes for the stack.  If the stack
*                            units are INT32Us then, the stack contains '4 * stk_size' bytes.  The stack
*                            units are established by the #define constant OS_STK which is CPU
*                            specific.  'stk_size' is 0 if called by 'OSTaskCreate()'.
*
*              pext          is a pointer to a user supplied memory area that is used to extend the task
*                            control block.  This allows you to store the contents of floating-point
*                            registers, MMU registers or anything else you could find useful during a
*                            context switch.  You can even assign a name to each task and store this name
*                            in this TCB extension.  A NULL pointer is passed if called by OSTaskCreate().
*
*              opt           options as passed to 'OSTaskCreateExt()' or,
*                            0 if called from 'OSTaskCreate()'.
*
* Returns    : OS_NO_ERR         if the call was successful
*              OS_NO_MORE_TCB    if there are no more free TCBs to be allocated and thus, the task cannot
*                                be created.
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/
/*
*******************************************************************************
                                           ��ʼ��������ƿ�
������prio�����񴴽�ʱ�����ȼ�
      ptos:�ٶ�CPU�Ĵ��������ڶ�ջ��ָ���ջջ����ָ�롣ջ����OS_STK_GROWTHΪ1ʱ�ǼĴ�����  

         ��λ����OS_STK_GROWTHΪ0ʱ�ǼĴ����ĵ�λ����ջ������CPU����Ȩ��
      pbos��ջ��ָ�롣��OSTaskCreate()����ʱ�����ָ�롣
      id�� �����ID
      stk_size:��ջ��С�� ����ջ��λ��int8usʱ����ջ��С������ջ�������ֽڣ�����ջ��λ��
               int32usʱ����ջ��С������4*stk_size�����ֽڡ���ջ��λ�ɡ�#define constant
               OS_STK������������CPU���С������OSTaskCreate()����stk_sizeΪ0��
      pext���û��ṩ�洢���ռ��ָ�룬����������ƿ顣����洢����Ĵ���������MMU�Ĵ�����
            ������������ת��ʱ���õĶ�����������TCB��չ��Ϊÿ������ָ��һ�����ִ浽����� �����档
            ����OSTaskCreate()���õ�ʱ��Ϊ��ָ�롣
      opt ������OSTaskCreateExt()ʱ����ѡ�񣬱�OSTaskCreate()���õ�ʱ��Ϊ0��
���أ�OS_NO_ERR ��������óɹ���
      OS_NO_MORE_TCB�����û�ж���TCB���ţ����������ܱ�������
��ע�����������uC/OS-II��˵���ڲ��ģ��Լ���Ӧ�ó����ܹ���������

*************************************************************************
*/

INT8U  OS_TCBInit (INT8U prio, OS_STK *ptos, OS_STK *pbos, INT16U id, INT32U stk_size, void *pext, INT16U opt)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
//ΪCPU״̬�Ĵ�������洢�ռ�
    OS_CPU_SR  cpu_sr;//#define OS_CPU_SR    unsigned int 
#endif    
    OS_TCB    *ptcb;//����������ƿ�


    OS_ENTER_CRITICAL();//�����ٽ�״̬
    ptcb = OSTCBFreeList;                                  /* Get a free TCB from the free TCB list    */
	  //�ӿ�TCB�б��еõ�һ���TCB
    if (ptcb != (OS_TCB *)0) {//�����TCB�ɹ�
        OSTCBFreeList        = ptcb->OSTCBNext;            /* Update pointer to free TCB list          */
		//���¿�TCB�б�������һ��
        OS_EXIT_CRITICAL();//�˳��ٽ�״̬
        ptcb->OSTCBStkPtr    = ptos;                       /* Load Stack pointer in TCB                */
		//װ��TCB�еĶ�ջָ��
        ptcb->OSTCBPrio      = (INT8U)prio;                /* Load task priority into TCB              */
		//װ��TCB���������ȼ�
        ptcb->OSTCBStat      = OS_STAT_RDY;                /* Task is ready to run                     */
		//����״̬��Ϊ����
        ptcb->OSTCBDly       = 0;                          /* Task is not delayed                      */
		//������ʱ

#if OS_TASK_CREATE_EXT_EN > 0//�����ʹ
        ptcb->OSTCBExtPtr    = pext;                       /* Store pointer to TCB extension           */
        //�洢TCB��չָ��
        ptcb->OSTCBStkSize   = stk_size;                   /* Store stack size */
		//�洢��ջ��С
        ptcb->OSTCBStkBottom = pbos;                       /* Store pointer to bottom of stack         */
		//�洢ջ��ָ��
        ptcb->OSTCBOpt       = opt;                        /* Store task options                       */
		//�洢����ѡ��
        ptcb->OSTCBId        = id;                         /* Store task ID                            */
		//��������ID
#else//�������ʹ
        pext                 = pext;                       /* Prevent compiler warning if not used     */
        stk_size             = stk_size;
        pbos                 = pbos;
        opt                  = opt;
        id                   = id;//��ֹ���������棬��Ҫɾ��
#endif

#if OS_TASK_DEL_EN > 0//�����������ɾ������
        ptcb->OSTCBDelReq    = OS_NO_ERR;//�Ƿ������Զ�ɾ��
#endif

        ptcb->OSTCBY         = prio >> 3;                  /* Pre-compute X, Y, BitX and BitY          */
        ptcb->OSTCBBitY      = OSMapTbl[ptcb->OSTCBY];//��ǰ����X,Y,λX��λY��
        ptcb->OSTCBX         = prio & 0x07;
        ptcb->OSTCBBitX      = OSMapTbl[ptcb->OSTCBX];//ΪʲôҪ�����㣿��֪��

#if OS_EVENT_EN > 0
		//��ʹ���д������&&������п��ƿ��������Ϊ��||��ʹ����������||
		//��ʹ�ź����������||��ʹ������������� 

        ptcb->OSTCBEventPtr  = (OS_EVENT *)0;              /* Task is not pending on an event          */
//����û�����¼��й���
#endif

#if (OS_VERSION >= 251) && (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0) && (OS_TASK_DEL_EN > 0)
//OS�汾���ڵ���251&&��ʹ�¼���־�������&&����־��������&&
//�����������ɾ������
        ptcb->OSTCBFlagNode  = (OS_FLAG_NODE *)0;          /* Task is not pending on an event flag     */
//����û�����¼���־�й���
#endif

#if (OS_MBOX_EN > 0) || ((OS_Q_EN > 0) && (OS_MAX_QS > 0))
//��������������||������д������&&�����п��ƿ������
        ptcb->OSTCBMsg       = (void *)0;                  /* No message received                      */
//û�н��յ�������Ϣ
#endif

#if OS_VERSION >= 204//ucos�汾���ڵ���204
        OSTCBInitHook(ptcb);
#endif

        OSTaskCreateHook(ptcb);                            /* Call user defined hook                   */
//�����û������hook
        
        OS_ENTER_CRITICAL();//�����ٽ�״̬
        OSTCBPrioTbl[prio] = ptcb;//������ƿ���뵽�Ѵ���������ƿ��б���
        ptcb->OSTCBNext    = OSTCBList;                    /* Link into TCB chain                      */
		//����TCB����OSTCBListΪTCB˫������ָ��
        ptcb->OSTCBPrev    = (OS_TCB *)0;//ǰ��Ϊ��
        if (OSTCBList != (OS_TCB *)0) {//�������Ϊ��
            OSTCBList->OSTCBPrev = ptcb;//ǰ��ָ����
        }
        OSTCBList               = ptcb;//����Ϊ��ͷ
        OSRdyGrp               |= ptcb->OSTCBBitY;         /* Make task ready to run                   */
		//λ�����������б��飬ʹ�������
        OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
		//Xλ�������Y���У�Ϊʲô��������
        OS_EXIT_CRITICAL();//�˳��ٽ�״̬
        return (OS_NO_ERR);//���سɹ�
    }
    OS_EXIT_CRITICAL();//������䲻�ɹ���Ҳ�˳��ٽ�״̬
    return (OS_NO_MORE_TCB);//����û�ж����TCB��
}
