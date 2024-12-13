#ifndef __KRT_H
#define __KRT_H

#include <unistd.h>

#ifndef KRT_T
#define KRT_T krt_t
#endif

#define KRTFLAGEXECED 0x0001 /* Indicates a Worker, not a Driver */
#define KRTFLAGDEBUG 0x0002  /* -d sets. Causes a breakpoint. */

typedef struct {
        int krt_flags;                       /* Those defined above */
        int krt_numtestops;                  /* Used by krt_selecttest(). Requires setting! */
        pid_t* krt_pids;                     /* Used by Driver to track Workers. */
        int* krt_status;                     /* Used by Driver to track Workers. */
        int krt_verbose;                     /* -v increments above 0. unused by harness. */
        int krt_workermax;                   /* Number of processes at highest test level */
        int krt_threadmax;                   /* Max. possible threads in given worker process */
        int krt_testmax;                     /* Success required for a Worker to finish */
        int krt_driverdelay;                 /* Seconds slept by Driver after krt_domisctask()
                              while waiting on unfinished Workers */
        int krt_threaddelay;                 /* Seconds slept between calls to 
                              krt_testdispatcher() by krt_testentity() */
        int krt_resdelay;                    /* Seconds slept waiting for test resources */
        volatile unsigned krt_testcompleted; /* Times krt_testentity() must call a
                              passing krt_testdispatcher() to return. */
        int krt_channel;                     /* Next 3 used privately with krt_doexecproc() */
        int krt_workerid;
        size_t krt_krtsize;   /* The size of the KRT_T structure */
        int krt_level;        /* Current test level. Used by Driver */
        int krt_maxlevel;     /* Maximum test level. Used by Driver */
        char** krt_envp;      /* used to implement krt_testexec */
        char** krt_newarg;    /* used to implement krt_testexec */
        char* krt_exename;    /* used to implement krt_testexec */
        char* krt_altexename; /* used to implement krt_testexec */
} krt_t;

/* Level 0, Driver */
int krt_driver(KRT_T* __kp, int __argc, char* __argv[], char* __envp[]);
int krt_initialize(KRT_T* __kp);
int krt_destroy(KRT_T* __kp);
int krt_numprocs(KRT_T* __kp, int __level);
int krt_currtestlevel(KRT_T* _kp);
int krt_domisctask(KRT_T* __kp);

/* Level 1, Worker */
int krt_workerprocess(KRT_T* __kp, int __id, int __completed);
int krt_ismultithreaded(KRT_T* __kp);
int krt_numthreads(KRT_T* __kp);

/* Level 2, Test Entity */
void* krt_testentity(void* __kp); /* takes KRT_T	*kp */
int krt_testdispatcher(KRT_T* __kp);
int krt_selecttest(KRT_T* __kp);
int krt_testexec(KRT_T* __kp);
const char* krt_getexename(KRT_T* __kp);

/* Misc. */
int krt_maxtestlevel(KRT_T* __kp);
int krt_rand();
int krt_randrng(int __range);
void krt_srand(unsigned int __seed);
int krt_exit(int __rc);
#define krt_exit(__rc) _krt_exit((__rc), __FILE__, __LINE__)
int _krt_exit(int __rc, const char* __file, int __line);
int krt_drivergone();
int krt_doexecproc(KRT_T* __kp);

#endif

/* ====================================================================== 
                 Kernel Regression Test Documentation.
   ====================================================================== 

  This is the Kernel Reliability Test harness. This framework uses 
 a three level hierarchy.
   0. The Driver is alive for the entire test. First it initializes
 resources for the test. Then it repeatedly launches a number of 
 level 1 Workers, and waits for them to complete. The number of
 Workers launched may depend on the "test level". When the test level 
 is set to 0, the remaining Workers are terminated and the test ends.
   1. The Workers may use threads and runs a number of Test Entitys. If
 the Worker is not multi-threaded, only one Test Entity is called.
   2. The Test Entity randomly selects tests performed by the test 
 case. Tests are selected until a maximum is reached.

-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
The KRT_T type. 

   The krt_t structure (fields labeled above) is used by the Driver to contain all
 the parameters of the test.  A copy of this structure is passed to every worker 
 when it is created.  This allows common resources created by krt_initialize to be 
 shared, and provides information to the Worker about the state of the test.

   Extension of the krt_t structure is done by defining KRT_T before the header
 "krt.h" is #include'd and setting kp->krt_krtsize in krt_initialize(). The new 
 KRT_T must have as it's first element the krt_t structure.  This allows a simple C 
 implementation of inheritance.

Example:	struct new_krt;
		#define KRT_T  struct new_krt
		#include <regress/krt.h>
		struct new_krt { krt_t base; int * totals; };

   As the KRT library routines are compiled expecting krt_t, they will 
only operate correctly with a krt_t first element of the KRT_T structure.

-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
Level 0. The main Driver call and functions that tailor it's behavior.

int krt_driver(KRT_T *__kp, int __argc, char *__argv[], char *__envp[]);
   Input:  The parameters correspond to those taken by the main() call for the
 process. 
   Output:  Returns an error number if the test was unable to run. This error may 
 have originated from the Divers user supplied functions or a failed system call.
   Description:  When called by the Management layer (the program is started)
 krt_driver() continues running as the "Driver."  It then forks or spawns itself as
 "Workers."  krt_workerprocess() is called once krt_driver has started as a Worker.
 The Driver also maintains the "test level" and sets up all shared data 
 structures, including those for interprocess communication.  When the Driver 
 terminates all Workers are required to follow suit.

int krt_initialize(KRT_T *__kp);
   Input:  __kp allows the elements of the Drivers KRT_T structure to be modified. 
   Output:  Returns 0 for success. An appropriate error number otherwise.
   Description:  Called once by the Driver to initialize the environment for
 creating and managing the Workers. Initially this function just returns. This
 function should be supplied if needed. 

int krt_destroy(KRT_T *__kp);
   Input:  __kp allows access to the Drivers KRT_T structure.
   Output:  Returns 0 for success. An appropriate error number otherwise.
   Description:  Called by the Driver once all testing is done. Performs any 
 required clean up activities.

int krt_numprocs(KRT_T *__kp, int __level);
   Input:  The Drivers KRT_T structure, and a "test level".
   Output:  The number of Workers to run at this "test level".
   Description:  This function should be changed if required.

int krt_currtestlevel(KRT_T *_kp);
   Input:  The Drivers KRT_T structure.
   Output:  The current "test level." (A product of kp->krt_level.)
   Description:  Customizable functions may use This value to vary the severity of
 stress testing. The Driver initializes its "krt_level" (see the structure above)
 to 1 at start up. When the test level changes, all Workers are slayed, and a new
 round of tests are begun. 

int krt_domisctask(KRT_T *__kp);
   Input:  The Drivers KRT_T structure.
   Output:  Returns 0 for normal operation. A error number will terminate the driver
 with that error number returned.
   Description:  Called by the driver while busy waiting for Workers to finish
 their tests. A non-zero return terminates the Driver. This function should be 
 supplied if needed.

-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
Level 1. The Worker call and functions that tailor their behavior.

int krt_workerprocess(KRT_T *__kp, int __id, int __completed);
   Input:  A copy of the Drivers KRT_T structure. And other information 
 identifying it to the Driver.
   Output:  Returns the error number from one of the Test Entities. ENOMEM is
 returned if insufficient memory is available for threads. If all tests pass 0 is 
 returned.
   Description:  If krt_ismultithreaded() does not evaluate as true,
 krt_testentity() is called directly, and it's return is returned. Otherwise
 krt_numthreads() number of threads are spawned. These threads call
 krt_testentity(). 

int krt_ismultithreaded(KRT_T *__kp);
   Input:  A copy of the Drivers KRT_T structure.
   Output:  Returns 1 when the Workers should create multiple Test Entity 
 threads. 0 when the Worker should perform 1 test itself.
   Description:  This is customizable and may be random.

int krt_numthreads(KRT_T *__kp);
   Input:  A copy of the Drivers KRT_T structure.
   Output:  When krt_ismultithreaded() evaluates as true, this customizable
 function should return the number of threads concurrently calling krt_testentity()
 from a Worker. 
   Description:  This is customizable and may be random.

-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
Level 2. The test entity and functions that tailor it's behavior.
For all functions in this section input is a copy of the Drivers KRT_T structure.

void * krt_testentity(KRT_T *__kp);
   Output: The error number returned by krt_testdispatcher() or 0 if all tests 
 dispatched were successful.
   Description:  Calls krt_testdispatcher() until it returns success as many times
 as the value of krt_testmax in the krt_t structure pointed to by __kp.  If the
 Driver terminates krt_testentity() will return ESRCH immediately. 

int krt_testdispatcher(KRT_T *__kp);
   Output:  The user supplied return value from the test selected.
   Description:  This it the function that calls your tests. Modify it to return
 the appropriate errno.  It uses krt_selecttest() to select a variety of tests to
 run at once. 

int krt_selecttest(KRT_T *__kp);
   Output:  Returns a value [0, __kp->krt_numtestops]. used by krt_selecttest().
   Description:  My be modified.

int krt_testexec(KRT_T *__kp);
   Output:  This function is unimplemented. Returns ENOSYS.
   Description:  Harness for tests that must be exec'd. May be supplied.

const char *krt_getexename(KRT_T *__kp);
   Output:  This function is unimplemented. Returns NULL.
   Description:  When one or multiple executable tests are used, a pathname
 to a test executable is to be returned.

-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
Miscellaneous. Although you are not expected to modify these they may be useful
resources in the customizable functions above. Random behavior is encouraged. 

int krt_maxtestlevel(KRT_T *__kp);
   Output: The highest test level that the test will reach.

int krt_rand();
   Output:  Returns thread safe random integer in [0, 0x7fffffff].

int krt_randrng(int __range);
   Input:  An integer one greater than the largest return expected.
   Output: Returns thread safe random integer in [0, range - 1].

void krt_srand(unsigned int __seed);
   Input:  A seed vale for the previous pseudo random functions.

int krt_exit(int __rc);
   Input:  The value to be passed to exit().
   Output:  This function should not return.
   Description:  An error message is printed.

int krt_drivergone();
   Output:  Returns a non-zero value if the driver is still alive. (0 otherwise.)

int krt_doexecproc(KRT_T *__kp);
   Input:  A copy of the Drivers KRT_T structure.
   Output:  An error value if exec() failed. No return otherwise.
   Description:  Private to the Driver.
*/
