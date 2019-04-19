#ifndef __XCORES_H__
#define __XCORES_H__

#include "../depends/define.h"
#include "xUtils.h"

namespace xlib
{

namespace xCores
{

	/*************************************************************************
	** Name		: ��java�ս���
	** Desc		: ��java��final�ؼ��֣�ʹ�÷�������̳�֮
	** Author	: hicker@2017-5-27 10:27:25
	*************************************************************************/
	template <class T>
	class Final
	{
		friend T;
	private:
		Final() {}
		Final(const Final&) {}
	};

	/*************************************************************************
	** Desc		: ��ƽ̨��3�ֶ��̹߳��ߣ�Engine��Mutex��Signal
	** Author	: hicker@2017-4-17 21:23:25
	*************************************************************************/
	class Mutex
	{
	public:
		Mutex()
		{
#ifdef __WIN32__
			m_mutex = (int)CreateMutex(NULL, FALSE, NULL);
#elif defined __LINUX__
			if (pthread_mutex_init(&m_mutex, NULL)<0)
			{
				perror("pthread_mutex_init");
				return;
			}
#endif
		};

		virtual ~Mutex()
		{
#ifdef __WIN32__
			CloseHandle((HANDLE)m_mutex);
#elif defined __LINUX__
			pthread_mutex_destroy(&m_mutex);
#endif
		};

		BOOL Lock()
		{
#ifdef __WIN32__
			return WAIT_OBJECT_0 == WaitForSingleObject((HANDLE)m_mutex, INFINITE);
#elif defined __LINUX__
			return 0 == pthread_mutex_lock(&m_mutex);
#endif
		};

		BOOL UnLock()
		{
#ifdef __WIN32__
			return 0 != ReleaseMutex((HANDLE)m_mutex);
#elif defined __LINUX__
			return 0 == pthread_mutex_unlock(&m_mutex);
#endif
		};

	private:
#ifdef __WIN32__
		int m_mutex;
#elif defined __LINUX__
		pthread_mutex_t m_mutex;
#endif
	};

	class Signal
	{
	public:
		Signal()
		{
#ifdef __WIN32__
			m_signal = (int)CreateEvent(NULL, FALSE, FALSE, NULL);
#elif __LINUX__
			for (int i = 0; i<255; i++)
			{
				m_signal = semget(ftok(".", i), 1, 0666 | IPC_CREAT | IPC_EXCL);
				if (-1 != m_signal)
					break;
			}
			if (m_signal == -1)
			{
				perror("semget failed!\n");
				return;
			}
			union semun sem_union;
			sem_union.val = 0; // ��ʼ��Ϊ0����ʾ�״ε���Wait��������1��ʾ�״ε���Wait������
			if (semctl(m_signal, 0, SETVAL, sem_union))
			{
				perror("semctl error!\n");
			}
#endif	
		};

		virtual ~Signal()
		{
#ifdef __WIN32__
			CloseHandle((HANDLE)m_signal);
#elif __LINUX__
			union semun sem_union;
			if (semctl(m_signal, 0, IPC_RMID, sem_union))
				printf("Failed to delete semaphore\n");
#endif
		};

		virtual int Wait()
		{
#ifdef __WIN32__
			return WAIT_OBJECT_0 != WaitForSingleObject((HANDLE)m_signal, INFINITE);
#elif __LINUX__
			struct sembuf sem_b = { 0/*�ź������*/, -1/*V����*/, SEM_UNDO };
			if (semop(m_signal, &sem_b, 1) == -1)
			{
				printf("semaphore_p failed\n");
				return 0;
			}
			return 1;
#endif
		};

		virtual int Notify()
		{
#ifdef __WIN32__
			return 0 != SetEvent((HANDLE)m_signal);
#elif __LINUX__
			struct sembuf sem_b = { 0/*�ź������*/, 1/*V����*/, SEM_UNDO };
			if (semop(m_signal, &sem_b, 1) == -1)
			{
				printf("semaphore_v failed\n");
				return 0;
			}
			return 1;
#endif
		};

	private:
		int m_signal; // linux sem_id
		union semun{
			int val;
			struct semid_ds *buf;
			unsigned short *arry;
		};
	};


	class Engine
	{
	public:
		// ���ý�����ڸ�����ȡ����������Ķ���ָ��
		template<class cls>
		Engine(cls pThis)
		{
			m_pObj = (int)pThis;
			m_pSignal = new Signal();
			m_pMutex = new Mutex();
			memset(m_vecInfo, 0, sizeof(m_vecInfo));
			m_tmpidx = 0;
		};
		virtual ~Engine()
		{
			delete m_pMutex;
			delete m_pSignal;
		};

		template<class int_func_int>
		/*************************************************************************
		** Desc		: ����һ���̣߳�������id
		** Param	: [in] idx �߳�Ψһ��������Χ[0,31]
		** 			: [in] p_exec �̺߳�����ַ������/����/��������벻����virtual��
		**					�Һ�������һ������ int(*)(int)��������������
		**					Run-Time Check Failure #0 - The value of ESP was not...
		**			  [in] i_exec
		**			  [in] nStackSize
		**			  [in] nPriority
		** Return	: ��0 ��ʾʧ�ܣ����򷵻ص�ǰ�߳�state
		** Author	: hicker@2017-2-26 13:03:10
		*************************************************************************/
		// hicker@2017-6-29 23:23:25 coding ��2�������Ż�
		//typedef int(Engine::*pfn)(int);
		int Ignite(int idx, int_func_int p_exec, int i_exec, int nStackSize = 0, int nPriority = 0)
		{
			int iRet = 0;
			if (!m_pMutex->Lock())
			{
				printf("m_pMutex->Lock failed!!\n");
				return iRet;
			}
			do
			{
				iRet = m_vecInfo[idx].state1;
				if (idx<0 || idx>(sizeof(m_vecInfo) / sizeof(m_vecInfo[0])) || (iRet && iRet != 4))
					break;

				// hicker@2018-6-2 23:49:10 coding win��lux�����������Ժ���ʱ�����������Engine��ϸ������
#ifdef __WIN32__
				m_vecInfo[idx].pthis = m_pObj;
#elif defined __LINUX__
				m_vecInfo[idx].pthis = (int)this;
#endif
#if 1
				m_vecInfo[idx].p_exec = (int(Engine::*)(int))p_exec;
#else
				m_vecInfo[idx].p_exec = union_cast<int(Engine::*)(int)>(p_exec);
#endif
				m_vecInfo[idx].i_exec = i_exec;
				m_tmpidx = idx;
				iRet = 0;
#ifdef __WIN32__ 
				m_vecInfo[idx].handle = (int)CreateThread(NULL, nStackSize, (LPTHREAD_START_ROUTINE)Engine::__Cylinder,
					(LPVOID)this, 0, (LPDWORD)&m_vecInfo[idx].n_tid);
				if (m_vecInfo[idx].handle < 0 || !SetThreadPriority((HANDLE)m_vecInfo[idx].handle, nPriority))
					break;
#elif __LINUX__
				int handle = pthread_create((pthread_t*)&m_vecInfo[idx].n_tid, NULL, Engine::__Cylinder, this);
				if (handle != 0)
					break;
#endif

				m_vecInfo[idx].state0 = 1;
				m_vecInfo[idx].state1 = 1; // hicker@2017-6-1 22:52:35 coding δ���ô�������
				iRet = m_vecInfo[idx].state1;
				m_pSignal->Wait();
			} while (0);
			if (idx<0 || idx>(sizeof(m_vecInfo) / sizeof(m_vecInfo[0]) - 1)) iRet = 0;
			m_pMutex->UnLock();
			return iRet;
		};

		// nTid=0ʱ����������߳�
		virtual void Stall(int idx, int nMsec = 3000)
		{
			m_vecInfo[idx].state0 = 0;
			for (int i = 0; i < nMsec; i+=50)
			{
#ifdef __WIN32__
				Sleep(50);
#elif defined __LINUX__
				usleep(50*1000);
#endif
				if (!m_vecInfo[idx].state1) return;
			}
#ifdef __WIN32__
			TerminateThread((HANDLE)m_vecInfo[idx].handle, -1);
#elif defined __LINUX__
			pthread_cancel(m_vecInfo[idx].n_tid);
#endif
			memset(&m_vecInfo[idx], 0, sizeof(m_vecInfo[0]));
		};

		virtual void Suspend(int idx)
		{
#ifdef __WIN32__
			SuspendThread((HANDLE)m_vecInfo[idx].handle);
#elif defined __LINUX__
#endif
		};
		virtual void Resume(int idx)
		{
#ifdef __WIN32__
			ResumeThread((HANDLE)m_vecInfo[idx].handle);
#elif defined __LINUX__
#endif
		};

		// 
		int TryState(int idx)
		{
			return m_vecInfo[idx].state0;
		}

		int GetState(int idx)
		{
			return m_vecInfo[idx].state1;
		}
		//virtual void Join();
		//virtual int Work(int nWtId) = 0;

		static int GetCurrentId()
		{
#ifdef __WIN32__
			// ��ȻGetCurrentThreadId����0����ֱ�Ӹ�0ֵ����ᵼ��PostThreadMessage�쳣
			return GetCurrentThreadId();
#elif defined __LINUX__
			return pthread_self();
#endif  
		};

	protected:
		int GetFreeId()
		{
			if (!m_pMutex->Lock())
				return -1;
			int iRet = -1;
			for (int i = 0; i < (sizeof(m_vecInfo) / sizeof(m_vecInfo[0])); i++)
			{
				if (m_vecInfo[i].state1 == 0 && m_vecInfo[i].state1 != 4)
				{
					m_vecInfo[i].state1 = 4;
					iRet = i;
					break;
				}
			}
			m_pMutex->UnLock();
			return iRet;
		};

	private:
		static void* __Cylinder(void *p)
		{
			Engine *pThis = (Engine*)p;
			int idx = pThis->m_tmpidx;
			Info info = pThis->m_vecInfo[idx];
			pThis->m_pSignal->Notify();
#if 0
			(pThis->*p_exec)(idx); // �𲻵���̬�����ã�ʵ�ⷢ��ĳЩ������̳й�ϵ����thisָ�벻׼ȷ
#else
			(((Engine*)info.pthis)->*(info.p_exec))(info.i_exec);
#endif
			memset(&pThis->m_vecInfo[idx], 0, sizeof(pThis->m_vecInfo[0]));
			return 0;
		};

	private:
		struct Info
		{
			int pthis; // ������������thisָ��
			int n_tid;
			int handle;
			int state0; // Ӧ��״̬������Stall�����0����0-stop, 1-start, 2-pause, 3-switch, 4-be GetFreeId()
			int state1; // ʵ��״̬��0-stop, 1-start, 2-pause, 3-switch, 4-be GetFreeId()
			int(Engine::*p_exec)(int);
			int i_exec; // p_exec�Ĳ���
		};
		int m_pObj;
		Info m_vecInfo[32];
		int m_tmpidx; // ��ʱ����
		//Vector<__WorkArgs> m_vecWorkArgs;
		//Signal *m_pSignal1;
		Signal *m_pSignal;
		Mutex *m_pMutex;
	};





	/*************************************************************************
	** Desc		: Find ��[it0,it1)�ڲ��ң����Ҳ�������it1
	** Param	: [in] it0
	** Param	: [in] it1
	** Param	: [in] v
	** Return	: ���Ҳ�������it1
	** Author	: hicker@2016-12-31 19:56:11
	*************************************************************************/
	template<class _Iter_, class _V_>
	static _Iter_ Find(_Iter_ it0, _Iter_ it1, _V_ v)
	{
		for (it0; it0 != it1; ++it0)
		{
			if (it0 == v)
				return it0;
		}
		return it1;
	}

	template <class T>
	class Vector
	{
	public:
		template <class N>
		struct Node
		{
			Node(const N &n) :value(n), next(0), prev(0){}
			Node *next, *prev;
			N value;
		};

		class Iterator
		{
		public:
			Iterator(Node<T>* pNode){ m_pNode = pNode; }
			T& operator* (void) const { return m_pNode->value; }
			T* operator-> (void) const { return &m_pNode->value; }
			// ���֡�==����ֻ�Ƚ�value
			bool operator==(Iterator const& it) const
			{
				// ������ȣ�Beginǿ����End���
				if (m_pNode == NULL && !it.m_pNode->prev)
					return true;
				else if (m_pNode->next && !it.m_pNode->next)// ���ⲻ��ȣ���Endǿ����End�����
					return false;
				return m_pNode->value == *it;
			}
			bool operator==(T const& n) const { return m_pNode->value == n; }
			bool operator!= (Iterator const& it) const { return !(*this == it); }

			Iterator& operator+ (int i) { while (i--)(*this)++; return *this; }
			Iterator& operator- (int i) { while (i--)(*this)--; return *this; }

			// ǰ��++
			Iterator& operator++ (void)
			{
				m_pNode = m_pNode->next;
				return *this;
			}

			// ����++
			Iterator& operator++ (int)
			{
				Iterator &old = *this;
				++*this;
				return old;
			}
			// ǰ��--
			Iterator& operator-- (void)
			{
				m_pNode = m_pNode->prev;
				return *this;
			}
			// ����--
			Iterator& operator-- (int)
			{
				Iterator &old = *this;
				--*this;
				return old;
			}

		private:
			Node<T>* m_pNode;
			friend class Vector;
		};

		Vector() :m_nSize(0), m_pHead(0){ m_pTail = new Node<T>(T()); }
		Vector(const Vector<T> &v) :m_nSize(0), m_pHead(0){ m_pTail = new Node<T>(T());  (*this) = v; }
		virtual ~Vector(){ Clear(); delete m_pTail; }
		Vector<T>& operator=(const Vector<T>& v)
		{
			if (this == &v) return *this;
			this->Clear();
			for (int i = 0; i < v.Size(); i++)
				this->Insert(i, v[i]);
			return (*this);
		};

		int Size() const{ return m_nSize; }
		bool Empty() const{ return m_nSize ? true : false; }

		void PushBack(const T& t){ Insert(-1, t); };
		T& PopBack(){ return Erase(Begin() + (m_nSize - 1)).m_pNode->value; };

		T& operator[](int idx) const { return find(idx)->value; };
		T& At(int idx){ return (*this)[idx]; }

		/*************************************************************************
		** Desc		: Insert
		** Param	: [in] idx �������Ԫ����������Ϊ(-1)��(>=m_nSize)ʱ����ӵ�ĩβ
		** Return	: ���ز���Ԫ�صĵ�����
		** Author	: hicker@2017-6-24 11:21:01
		*************************************************************************/
		Iterator& Insert(int idx, const T& t)
		{
			if (m_nSize == 0)
			{
				m_pHead = new Node<T>(t);
				m_pTail->prev = m_pHead;
				m_pHead->next = m_pTail;
				m_nSize++;
				return Begin();
			}

			if (idx < 0 || idx>m_nSize)idx = m_nSize;

			if (idx == m_nSize)
			{
				// ĩβ��
				Node<T> *p = find(idx - 1);
				
				p->next = new Node<T>(t);
				p->next->prev = p;
				p->next->next = m_pTail;
				m_pTail->prev = p->next;
			}
			else
			{
				// ǰ��
				Node<T> *p = find(idx);
				if (!p->prev)
				{
					p->prev = new Node<T>(t);
					p->prev->next = p;
					m_pHead = p->prev;
				}
				else
				{
					p->prev->next = new Node<T>(t);
					p->prev->next->prev = p->prev;
					p->prev->next->next = p;
					p->prev = p->prev->next;
				}
			}
			m_nSize++;
			
			return xCores::Find(Begin(), End(), t);
		}

		/*************************************************************************
		** Desc		: Erase ��caller��֤it�Ϸ��ԣ������쳣
		** Param	: [in] it ��������Ԫ�ص�����
		** Return	: �����ѱ�����Ԫ��
		** Author	: hicker@2017-6-24 11:21:01
		*************************************************************************/
		Iterator& Erase(Iterator& it)
		{
			if (it.m_pNode == m_pHead)
			{
				if (it.m_pNode->next == m_pTail)
				{
					m_pHead = NULL;
					m_pTail->prev = m_pHead;
				}
				else
				{
					m_pHead = it.m_pNode->next;
					m_pHead->prev = NULL;
				}
			}
			else
			{
				it.m_pNode->next->prev = it.m_pNode->prev;
				it.m_pNode->prev->next = it.m_pNode->next;
			}

			delete it.m_pNode;
			m_nSize--;

			return it;
		}

		void Clear()
		{
			while (m_pHead)
				Erase(Begin());
			/*m_pHead = NULL;
			m_pTail->prev = m_pHead;
			m_nSize = 0;*/
		}

		Iterator Begin(){ return Iterator(m_pHead); }
		Iterator End(){ return Iterator(m_pTail); }

	private:
		// hicker@2017-6-25 20:17:16 coding ���������ң��Ƿ��б�Ҫ���һ�����ݵ�����\Ԫ��ֵ�ң�
		Node<T>* find(int idx) const
		{
			Node<T> *p = m_pHead;
			while ((idx--)>0)p = p->next;
			return p;
		}

		int m_nSize;
		Node<T> *m_pHead;
		Node<T> *m_pTail;
	};


	/*****************************************************************************************************************************
	*   Copyright(c) 2016-2020  ������ -- ����
	*   All rights reserved.
	*
	*  ��  ��: ringqueue.h   Create By liufei    2015.1.10
	*  ��  �ܣ�����ģ�����;
	*  ˵  ��������ʵ�ֻ��ζ���;
	*  ��  ע����;
	*
	******************************************************************************************************************************/

	template <class TNode>
	class QueueR
	{
	public:
		QueueR(unsigned int nSize)
		{
			m_nCount = 0;

			m_nHeadIndex = 0;
			m_nTailIndex = 0;

			m_nSize = nSize;
			m_pNodes = new TNode[m_nSize];
			memset(m_pNodes, 0, m_nSize * sizeof(TNode));
			m_pFlags = new unsigned int[m_nSize];
			memset(m_pFlags, 0, m_nSize * sizeof(unsigned int));
		}

		virtual ~QueueR()
		{
			delete[]m_pNodes;
			delete[]m_pFlags;
		}

		/***************************************************************************************************************************
		*
		*  ��  ����inline unsigned int GetCount();
		*  ��  �ܣ���ȡ�����еĸ���;
		*  ��  ����tNode: ����ڵ�;
		*  ��  �أ��Ƿ�ɹ�;
		*      ע��֧�ֶ��߳�;
		****************************************************************************************************************************/
		inline unsigned int GetCount(){ return m_nCount; }

		/***************************************************************************************************************************
		*
		*  ��  ����inline bool PushBack(const TNode& tNode);
		*  ��  �ܣ��ڶ���β���һ���ڵ�;
		*  ��  ����tNode: ����ڵ�;
		*  ��  �أ��Ƿ�ɹ�;
		*      ע��֧�ֶ��߳�;
		****************************************************************************************************************************/
		inline bool Push(const TNode& tNode)
		{
			m_mtxPush.Lock();
			unsigned int  nCur_index = m_nTailIndex;
			unsigned int* nCur_flag = m_pFlags + nCur_index;

#ifdef __WIN32__
			unsigned int nOldValue = InterlockedCompareExchange(nCur_flag, 1, 0);
#elif defined __LINUX__
			unsigned int nOldValue = __sync_val_compare_and_swap(nCur_flag, 0, 1);
#endif
			if (nOldValue == 0) //�ڵ�ռ�óɹ�
			{
				m_nTailIndex++;
				if (m_nTailIndex == m_nSize)
				{
					m_nTailIndex = 0;
				}
				m_mtxPush.UnLock();
			}
			else         //�ڵ��ѱ�ռ��(��������)
			{
				m_mtxPush.UnLock();
				return false;
			}

#if defined(WIN32) || defined(WIN64)
			InterlockedIncrement(&m_nCount);   //��������
#else
			__sync_fetch_and_add(&m_nCount, 1);
#endif

			*(m_pNodes + nCur_index) = tNode;  //д��ڵ�

#if defined(WIN32) || defined(WIN64)
			InterlockedIncrement(nCur_flag);   //д�����
#else
			__sync_fetch_and_add(nCur_flag, 1);
#endif
			return true;
		}

		/***************************************************************************************************************************
		*
		*  ��  ����inline bool PopFront(TNode& _out_tNode);
		*  ��  �ܣ���ȡ����ǰ���һ���ڵ�;
		*  ��  ����_out_tNode: ����ڵ�;
		*  ��  �أ��Ƿ�ɹ�;
		*      ע��֧�ֶ��߳�;
		****************************************************************************************************************************/
		inline TNode *Pop()
		{
			TNode *_out_tNode = NULL;

			m_mtxPop.Lock();
			unsigned int  nCur_index = m_nHeadIndex;
			unsigned int* nCur_flag = m_pFlags + nCur_index;

#if defined(WIN32) || defined(WIN64)
			unsigned int nOldValue = InterlockedCompareExchange(nCur_flag, 3, 2);
#else
			unsigned int nOldValue = __sync_val_compare_and_swap(nCur_flag, 2, 3);
#endif
			if (nOldValue == 2) //�ڵ�ռ�óɹ�
			{
				m_nHeadIndex++;
				if (m_nHeadIndex == m_nSize)
				{
					m_nHeadIndex = 0;
				}
				m_mtxPop.UnLock();
			}
			else        //�ڵ�Ϊ��(����Ϊ��)
			{
				m_mtxPop.UnLock();
				return _out_tNode;
			}

#if defined(WIN32) || defined(WIN64)
			InterlockedDecrement(&m_nCount);    //�����ݼ�
#else
			__sync_fetch_and_sub(&m_nCount, 1);
#endif

			_out_tNode = (m_pNodes + nCur_index);   //ȡ���ڵ�

#if defined(WIN32) || defined(WIN64)
			InterlockedExchangeSubtract(nCur_flag, 3); //ȡ�����
#else
			__sync_fetch_and_sub(nCur_flag, 3);
#endif
			return _out_tNode;
		}

		inline TNode &Front(){ return *(m_pNodes + m_nHeadIndex); }

		inline TNode &Back()
		{
			unsigned int idx = m_nTailIndex;
#ifdef __WIN32__
			InterlockedCompareExchange(&idx, m_nSize, 0);
#else
			__sync_val_compare_and_swap(&idx, 0, m_nSize);
#endif
			return *(m_pNodes + idx - 1);
		}

	private:

		unsigned int      m_nSize;        //���ζ��еĴ�С;
		unsigned int      m_nCount;       //�����еĸ���

		Mutex			m_mtxPush;
		Mutex			m_mtxPop;

		TNode*            m_pNodes;      //Ԫ��;
		unsigned int*     m_pFlags;       //���ĳ��λ�õ�Ԫ��״̬; 0:�սڵ�; 1: �ѱ�����,����д��; 2: �Ѿ�д��,���Ե���; 3: ���ڵ�������;

		unsigned int      m_nHeadIndex;   //������ǰ��Ԫ�ص�λ��;
		unsigned int      m_nTailIndex;   //���������Ԫ�ص�λ��;
	};




	/*************************************************************************
	** Desc		: ����
	** Author	: hicker@2016-12-31 19:56:11
	*************************************************************************/
	template <typename T>
	class Queue
	{
	public:
		Queue()
		{
			Node<T> *node = new Node<T>();
			node->data = NULL;
			node->next = NULL;
			qfront = qrear = node;
		}
		bool Empty() const
		{
			return (qfront == qrear);
		};
		void Pop()
		{
			if (!Empty())
			{
				Node<T> *p = qfront->next;
				qfront->next = p->next;
				if (p == qrear)
					qrear = qfront;
				delete p;
				p = NULL;
			}
		};
		T Front()
		{
			if (!Empty())
			{
				Node<T> *p = qfront->next;
				return p->data;
			}
			return T();
		};
		T Back()
		{
			if (!Empty())
				return qrear->data;
			return T();
		};
		void Push(const T &t)
		{
			Node<T> *node = new Node<T>;
			node->data = t;
			node->next = NULL;
			qrear->next = node;
			qrear = node;
		};
		int Size() const
		{
			Node<T> *p = qfront;
			if (Empty())
				return 0;
			else
			{
				int i = 1;
				while (p->next != qrear)
				{
					p = p->next;
					i++;
				}
				return i;
			}
		};
	private:
		template <typename N>
		struct Node
		{
			Node<N> *next;
			N data;
		};

		Node<T> *qfront, *qrear;
	};


	/*************************************************************************
	** Desc		: map
	** Author	: hicker@2016-12-31 19:56:11
	*************************************************************************/
	template<typename K, typename V>
	class Map
	{
	public:
		class Node
		{
		public:
			Node(K k, V const& v) :prev(NULL), next(NULL), first(k), second(v){}
			K first;//key
			V second;// value
		private:
			Node* prev;
			Node* next;
			friend class Map;
		};
		class Iterator
		{
		public:
			Iterator(Node* pNode){ m_pNode = pNode; }
			// hicker@2017-6-24 20:49:38 coding �Ƚ�m_pNode����m_pNode->first????????????
			bool operator== (Iterator const& it) const {
				// ������ȣ�Beginǿ����End���
				if (m_pNode == NULL && !it.m_pNode->prev)
					return true;
				return (m_pNode == it.m_pNode);
			}
			bool operator== (K const& k) const {
				return m_pNode->first == k;
			}
			bool operator!= (Iterator const& it) const
			{
				return !(*this == it);
			}
			Iterator& operator+ (int i)
			{
				while (i--)(*this)++;
				return *this;
			}
			Iterator& operator- (int i)
			{
				while (i--)(*this)--;
				return *this;
			}
			Iterator& operator++ (void)
			{
				m_pNode = m_pNode->next;
				return *this;
			}
			Iterator const operator++ (int)
			{
				Iterator old = *this;
				++*this;
				return old;
			}
			Iterator& operator-- (void)
			{
				m_pNode = m_pNode->prev;
				return *this;
			}
			Iterator const operator-- (int)
			{
				Iterator old = *this;
				--*this;
				return old;
			}
			Node& operator* (void) const { return *m_pNode; }
			Node* operator-> (void) const { return m_pNode; }

		private:
			Node* m_pNode;
			friend class Map;
		};

		Map() :m_pHead(NULL), m_nSize(0){ m_pTail = new Node(K(), V()); }
		virtual ~Map(){ Clear();  delete m_pTail; }
		Map(Map<K, V>& m) :m_pHead(NULL), m_nSize(0){ m_pTail = new Node(K(), V()); (*this) = m; }
		Map<K, V>& operator=(Map<K, V>& m)
		{
			if (this != &m)
			{
				Clear();
				for (Iterator it = m.Begin(); it != m.End(); ++it)
					(*this)[it->first] = it->second;
			}
			return *this;
		}

		Iterator Begin(){ return Iterator(m_pHead); }
		Iterator End(){ return Iterator(m_pTail); }
		int Size(){ return m_nSize; }

		//����Ѵ��ڣ���ֱ�ӷ���
		Iterator Insert(K key, V val)
		{
			Node* pNode = find(key);
			if (NULL == pNode)
			{
				(*this)[key] = val;
			}
			return pNode;
		}

		//��������ڣ����ȴ����ٲ���
		V& operator[](K key)
		{
			Node* pNode = find(key);
			if (NULL == pNode)
			{
				pNode = new Node(key, V());
				pNode->prev = m_pTail->prev;
				pNode->next = m_pTail;
				if (m_pTail->prev)
					m_pTail->prev->next = pNode;
				else
					m_pHead = pNode;
				m_pTail->prev = pNode;
				m_nSize++;
			}
			return pNode->second;
		}

		//ɾ��ָ����Ԫ��ֵ
		Iterator Erase(K key)
		{
			Node *pNode = find(key);
			if (pNode)
			{
				if (pNode == m_pHead)
				{
					if (pNode->next == m_pTail)
					{
						m_pHead = NULL;
						m_pTail->prev = m_pHead;
					}
					else
					{
						m_pHead = pNode->next;
						m_pHead->prev = NULL;
					}
				}
				else
				{
					pNode->next->prev = pNode->prev;
					pNode->prev->next = pNode->next;
				}
				delete pNode;
				m_nSize--;
			}
			return Find(Begin(), End(), key);
		}

		// �еȺ�����ͨ�ð汾
		//bool Equal(V const& a,V const& b) const {return a == b;}

		void Clear()
		{
			while (m_pHead)
				Erase(m_pHead->first);
		}
	private:
		// ������û��keyֵ�Ľ�����
		Node* find(K& key)
		{
			for (Node* pNode = m_pHead; pNode && pNode != m_pTail; pNode = pNode->next)
			{
				if (pNode->first == key)
					return pNode;
			}
			return NULL;
		}
		Node* m_pHead;
		Node* m_pTail;
		int m_nSize;
	};

#if 0
	// �ػ��е�
	template<>
	bool Map<char const*>::Equal(char const* const& a,
		char const* const& b) const {
		return !strcmp(a, b);
	}

	template<>
	char& Map<char>::operator[](int key){
		// �����Ƿ����
		Node* pn = findkey(key);
		// �����ھ͹���һ���µ�Ĭ��ֵ
		if (NULL == pn){
			char  pdata = '\0';
			pn = Insert(key, pdata);
		}
		return pn->second;
	}
	// �ػ�[]
	template<>
	char const*& Map<char const*>::operator[](int key){
		// �����Ƿ����
		Node* pn = findkey(key);
		// �����ھ͹���һ���µ�Ĭ��ֵ
		if (NULL == pn){
			char*  pdata = "";
			pn = Insert(key, pdata);
		}
		return pn->second;
	}

	template<>
	short& Map<short>::operator[](int key){
		// �����Ƿ����
		Node* pn = findkey(key);
		// �����ھ͹���һ���µ�Ĭ��ֵ
		if (NULL == pn){
			short  pdata = 0;
			pn = Insert(key, pdata);
		}
		return pn->second;
	}
	template<>
	int& Map<int>::operator[](int key){
		// �����Ƿ����
		Node* pn = findkey(key);
		// �����ھ͹���һ���µ�Ĭ��ֵ
		if (NULL == pn){
			int  pdata = 0;
			pn = Insert(key, pdata);
		}
		return pn->second;
	}

	template<>
	long& Map<long>::operator[](int key){
		// �����Ƿ����
		Node* pn = findkey(key);
		// �����ھ͹���һ���µ�Ĭ��ֵ
		if (NULL == pn){
			long  pdata = 0;
			pn = Insert(key, pdata);
		}
		return pn->second;
	}

	template<>
	float& Map<float>::operator[](int key){
		// �����Ƿ����
		Node* pn = findkey(key);
		// �����ھ͹���һ���µ�Ĭ��ֵ
		if (NULL == pn){
			float  pdata = 0.0;
			pn = Insert(key, pdata);
		}
		return pn->second;
	}

	template<>
	double& Map<double>::operator[](int key){
		// �����Ƿ����
		Node* pn = findkey(key);
		// �����ھ͹���һ���µ�Ĭ��ֵ
		if (NULL == pn){
			double  pdata = 0.0;
			pn = Insert(key, pdata);
		}
		return pn->second;
	}
#endif

	template<class T>
	class MsgQue : public virtual Final<MsgQue<T> > // linux error: '>>' should be '> >' within a nested template argument list
	{
	public:
		struct QMSG
		{
		public:
			QMSG()
			{
				memset(this, 0, sizeof(QMSG));
			}
			QMSG(QMSG &qMsg)
			{
				if (this!=&qMsg) *this = qMsg;
			}
			QMSG(int i4, char *str4k, void *wp, void *lp)
			{
				QMSG();
				this->i4 = i4;
				if (str4k)strcpy(this->str4k, str4k);
				this->wp = wp;
				this->lp = lp;
			}
			QMSG & operator=(const QMSG &qMsg)
			{
				this->i4 = qMsg.i4;
				if (qMsg.str4k)strcpy(this->str4k, qMsg.str4k);
				this->wp = qMsg.wp;
				this->lp = qMsg.lp;
				return *this;
			}
			int i4;
			char str4k[4096];
			void *wp;
			void *lp;
		};

		inline MsgQue(int nSize)
		{
			m_nSize = nSize;
			m_nCount = 0;

			m_pNodes = NULL;
			m_pFlags = NULL;

			m_nHeadIndex = 0;
			m_nTailIndex = 0;

			m_pNodes = new QMSG[m_nSize];
			memset(m_pNodes, 0, m_nSize * sizeof(QMSG));

			m_pFlags = new unsigned int[m_nSize];
			memset(m_pFlags, 0, m_nSize * sizeof(int));
		}

		inline ~MsgQue()
		{
			delete[] m_pNodes;
			delete[] m_pFlags;
		}

		inline unsigned int GetCount(){ return m_nCount; }

		//inline bool Send(QMSG msg)
		//{
		//	return 0;
		//}

		// ��4���������Ϊָ����Ƕ�������ã���Ϊ�˿��Դ�Ĭ��NULLֵ
		inline bool Post(int i4, char *str4k = NULL, void *wp = NULL, void *lp = NULL)
		{
			if (!m_mxPush.Lock())
			{
				return false;
			}
			unsigned int  nCur_index = m_nTailIndex;
			unsigned int* nCur_flag = m_pFlags + nCur_index;

#if defined(WIN32) || defined(WIN64)
			unsigned int nOldValue = InterlockedCompareExchange(nCur_flag, 1, 0);
#else
			unsigned int nOldValue = __sync_val_compare_and_swap(nCur_flag, 0, 1);
#endif
			if (nOldValue == 0) //�ڵ�ռ�óɹ�
			{
				m_nTailIndex++;
				if (m_nTailIndex == m_nSize)
				{
					m_nTailIndex = 0;
				}
				m_mxPush.UnLock();
			}
			else         //�ڵ��ѱ�ռ��(��������)
			{
				m_mxPush.UnLock();
				return false;
			}

#if defined(WIN32) || defined(WIN64)
			InterlockedIncrement(&m_nCount);   //��������
#else
			__sync_fetch_and_add(&m_nCount, 1);
#endif
			*(m_pNodes + nCur_index) = QMSG(i4, str4k, wp, lp);  //д��ڵ�

#if defined(WIN32) || defined(WIN64)
			InterlockedIncrement(nCur_flag);   //д�����
#else
			__sync_fetch_and_add(nCur_flag, 1);
#endif
			m_Signal.Notify();
			return true;
		}

		bool PopFront(QMSG *pMsg)
		{
			if (!m_mxPop.Lock())
			{
				return FALSE;
			}
			unsigned int  nCur_index = m_nHeadIndex;
			unsigned int* nCur_flag = m_pFlags + nCur_index;

#if defined(WIN32) || defined(WIN64)
			unsigned int nOldValue = InterlockedCompareExchange(nCur_flag, 3, 2);
#else
			unsigned int nOldValue = __sync_val_compare_and_swap(nCur_flag, 2, 3);
#endif
			if (nOldValue == 2) //�ڵ�ռ�óɹ�
			{
				m_nHeadIndex++;
				if (m_nHeadIndex == m_nSize)
				{
					m_nHeadIndex = 0;
				}
				m_mxPop.UnLock();
			}
			else        //�ڵ�Ϊ��(����Ϊ��)
			{
				m_mxPop.UnLock();
				m_Signal.Wait();
				return PopFront(pMsg);
			}

#if defined(WIN32) || defined(WIN64)
			InterlockedDecrement(&m_nCount);    //�����ݼ�
#else
			__sync_fetch_and_sub(&m_nCount, 1);
#endif

			*pMsg = *(m_pNodes + nCur_index);   //ȡ���ڵ�

#if defined(WIN32) || defined(WIN64)
			InterlockedExchangeSubtract(nCur_flag, 3); //ȡ�����
#else
			__sync_fetch_and_sub(nCur_flag, 3);
#endif
			return true;
		}

		template<typename i_i_s_p>
		int Loop(i_i_s_p pfnMsgLoop, T *pThis)
		{
			QMSG msg;
			while (PopFront(&msg))
			{
				(pThis->*pfnMsgLoop)(msg.i4, msg.str4k, msg.wp, msg.lp);
			}
			return 0;
		}

	public:
		Mutex m_mxPush;
		Mutex m_mxPop;
		Signal m_Signal;

		inline bool Lock(unsigned int* pnlock);
		inline void UnLock(unsigned int* pnlock);

		unsigned int      m_nSize;        //���ζ��еĴ�С;
		unsigned int      m_nCount;       //�����еĸ���

		QMSG*				m_pNodes;      //Ԫ��;
		unsigned int*     m_pFlags;       //���ĳ��λ�õ�Ԫ��״̬; 0:�սڵ�; 1: �ѱ�����,����д��; 2: �Ѿ�д��,���Ե���; 3: ���ڵ�������;

		unsigned int      m_nHeadIndex;   //������ǰ��Ԫ�ص�λ��;
		unsigned int      m_nTailIndex;   //���������Ԫ�ص�λ��;
	};



	// ����Ϊ���Դ���

	class Data
	{
	public:
		Data(int y, int m, char *t)
		{
			this->y = y;
			this->m = m;
			strcpy(this->t, t);
			p = new char[256];
			memset(p, 0, 256);
			sprintf(p, "%d-%d %s", y, m, t);
		}
		virtual ~Data()
		{
			delete p;
		}

		int y;
		int m;
		char t[256];
		char *p;
	};

	static void TestVector()
	{
		{
			Vector<int> vec1;
			vec1.PushBack(1);
			vec1.PushBack(2);
			vec1.PushBack(3);
			Vector<int> vec2;
			vec2 = vec1;
			vec2.Insert(0, 0);
			vec2.Insert(-1, 4);
			Vector<int> vec3(vec2);
			Vector<int>::Iterator it = xCores::Find(vec3.Begin(), vec3.End(), 0);
			vec3.Erase(it);
			it = xCores::Find(vec3.Begin(), vec3.End(), 2);
			vec3.Erase(it);
			vec3.PopBack();
			vec3.Clear();
			vec3 = vec1;

			xCores::Vector<int>::Iterator it1 = xCores::Find(vec1.Begin(), vec1.End(), 1);
			xCores::Vector<int>::Iterator it2 = xCores::Find(vec1.Begin(), vec1.End(), 3);
			xCores::Vector<int>::Iterator it3 = xCores::Find(vec1.Begin(), vec1.End(), 10);
			if (it3 == vec1.End())
				int i = 0;
			it3--;
			it3--;
		}

		{
			Data *pData[5] =
			{
				new Data(2017, 1, "09:00"),
				new Data(2017, 2, "10:00"),
				new Data(2017, 3, "11:00"),
				new Data(2017, -1, "08:00"),
				new Data(2017, 4, "12:00")
			};

			Vector<Data*> vec1;
			vec1.PushBack(pData[0]);
			vec1.PushBack(pData[1]);
			vec1.PushBack(pData[2]);
			Vector<Data*> vec2;
			vec2 = vec1;
			vec2.Insert(0, pData[3]);
			vec2.Insert(-1, pData[4]);
			Vector<Data*> vec3(vec2);
			vec3.Erase(xCores::Find(vec3.Begin(), vec3.End(), pData[0]));
			vec3.Erase(xCores::Find(vec3.Begin(), vec3.End(), pData[2]));
			vec3.PopBack();
			vec3.Clear();

			xCores::Vector<Data*>::Iterator it1 = xCores::Find(vec1.Begin(), vec1.End(), pData[1]);
			xCores::Vector<Data*>::Iterator it2 = xCores::Find(vec1.Begin(), vec1.End(), pData[3]);
			xCores::Vector<Data*>::Iterator it3 = xCores::Find(vec1.Begin(), vec1.End(), (Data*)1);
			if (it3 == vec1.End())
				int i = 0;
		}
	}

	static void TestMap()
	{
		Data *pData[5] =
		{
			new Data(2017, 0, "08:00"),
			new Data(2017, 1, "09:00"),
			new Data(2017, 2, "10:00"),
			new Data(2017, 3, "11:00"),
			new Data(2017, 4, "12:00")
		};

		Map<int, Data*> map1;
		map1[1] = pData[1];
		map1[2] = pData[2];
		map1[3] = pData[3];
		Map<int, Data*> map2;
		map2 = map1;
		map2.Insert(0, pData[0]);
		map2.Insert(4, pData[4]);
		Map<int, Data*> map3(map2);
		map3.Erase(0);
		map3.Erase(2);
		//map3.PopBack();
		map3.Clear();

		xCores::Map<int, Data*>::Iterator it1 = xCores::Find(map1.Begin(), map1.End(), 1);
		xCores::Map<int, Data*>::Iterator it2 = xCores::Find(map1.Begin(), map1.End(), 3);
		xCores::Map<int, Data*>::Iterator it3 = xCores::Find(map1.Begin(), map1.End(), 10);
		if (it3 == map1.End())
			int i = 0;
	}
}

}

#endif

