/**
 *      Author : xbort  Time: 2021-3-11
 *      In  a System V-like environment, one has the two types mcontext_t and ucontext_t defined in <ucontext.h> and the four functions getcontext(), setcon‐
 *      text(), makecontext(3), and swapcontext(3) that allow user-level context switching between multiple threads of control within a process.
 *
 *      The mcontext_t type is machine-dependent and opaque.  The ucontext_t type is a structure that has at least the following fields:
 *
 *          typedef struct ucontext_t {
 *              struct ucontext_t *uc_link;
 *              sigset_t          uc_sigmask;
 *              stack_t           uc_stack;
 *              mcontext_t        uc_mcontext;
 *              ...
 *          } ucontext_t;
 *
 *      with sigset_t and stack_t defined in <signal.h>.  Here uc_link points to the context that will be resumed when the  current  context  terminates  (in
 *      case  the  current context was created using makecontext(3)), uc_sigmask is the set of signals blocked in this context (see sigprocmask(2)), uc_stack
 *      is the stack used by this context (see sigaltstack(2)), and uc_mcontext is the machine-specific representation of the saved  context,  that  includes
 *      the calling thread's machine registers.
 *
 *      The function getcontext() initializes the structure pointed at by ucp to the currently active context.
 *
 *      The  function setcontext() restores the user context pointed at by ucp.  A successful call does not return.  The context should have been obtained by
 *      a call of getcontext(), or makecontext(3), or passed as third argument to a signal handler.
 *
 *      If the context was obtained by a call of getcontext(), program execution continues as if this call just returned.
 *
 *      If the context was obtained by a call of makecontext(3), program execution continues by a call to the function func specified as the second  argument
 *      of  that  call  to  makecontext(3).   When the function func returns, we continue with the uc_link member of the structure ucp specified as the first
 *      argument of that call to makecontext(3).  When this member is NULL, the thread exits.
 *
 *      If the context was obtained by a call to a signal handler, then old standard text says that "program execution continues with the program instruction
 *      following the instruction interrupted by the signal".  However, this sentence was removed in SUSv2, and the present verdict is "the result is unspec‐
 *      ified".
 *
 *
 *      Thread --------> main_fiber  <-----------------------> sub_fiber            main_fiber 能创建sub_fiber(子协程)，sub_fiber不能创建子协程,子协程回收后回到主协程
 *                          ↑   ↓
 *                          ↑   ↓
 *                          ↑   ↓
 *                        sub_fiber
 *
 *
 */

/**
 *   uint64_t m_id = 0; /// 协程id
 *
 *   uint32_t m_stack_size = 0; /// 协程运行栈大小
 *
 *   State m_state = STATE_INIT; /// 协程状态
 *
 *   ucontext_t m_ctx;   /// 协程上下文
 *
 *   void* m_stack = nullptr;    /// 协程运行栈指针
 *
 *   std::function<void()> m_cb;    /// 协程运行函数
 **/



#include "fiber.h"
#include "ipmsg.h"

namespace ipmsg {
static Logger::ptr g_logger = LOG_NAME("system");
static std::atomic<uint64_t> s_fiber_id {0}; /// 协程id
static std::atomic<uint64_t> s_fiber_count {0}; /// 总协程数
static thread_local Fiber* t_fiber = nullptr;
//static thread_local std::shared_ptr<Fiber::ptr> t_threadFiber = nullptr;
static thread_local Fiber::ptr t_threadFiber = nullptr;

static ipmsg::ConfigVar<uint32_t>::ptr g_fiber_stack_size =
    ipmsg::Config::Lookup<uint32_t>("fiber.stack_size", 1024 * 1024, "fiber stack size");

//    ipmsg::ConfigVar<std::set<LogDefine> >::ptr g_log_defines =
//    ipmsg::Config::Lookup("logs", std::set<LogDefine>(), "logs config");

class MallocStackAllocator {
public:
    static void* Alloc(size_t size) {
        return malloc(size);
    }

    static void Dealloc(void *vp, size_t size) {
        return free(vp);
    }
};

using StackAllocator = MallocStackAllocator; /// the same as typedef MallocStackAllocator StackAllocator

uint64_t Fiber::GetFiberId() {
    if(t_fiber) {
        return t_fiber->getId();
    }
    return 0;
}
/// 不能用new的方式自己调用构造函数创建对象
Fiber::Fiber() {
    m_state = STATE_EXEC;
    SetThis(this);

    if(getcontext(&m_ctx)) {
        ASSERT_MACRO2(false, "getcontext");
    }

    ++s_fiber_count;
    LOG_DEBUG(g_logger) << "Fiber::Fiber main";
}

/**
 * @brief 构造函数
 * @param[in] cb 协程执行的函数
 * @param[in] stacksize 协程栈大小
 * @param[in] use_caller 是否在MainFiber上调度
 */
Fiber::Fiber(std::function<void()> cb, size_t stack_size)
    :m_id(++s_fiber_id)
    ,m_cb(cb) {
    ++s_fiber_count;
    m_stack_size = stack_size ? stack_size : g_fiber_stack_size->getValue();

    m_stack = StackAllocator::Alloc(m_stack_size);
    /* When successful, getcontext() returns 0; */
    if(getcontext(&m_ctx)) {
        ASSERT_MACRO2(false, "getcontext");
    }
    m_ctx.uc_link = nullptr; /// 保存当前context结束后继续执行的context记录
    m_ctx.uc_stack.ss_sp = m_stack;     /// context运行的栈信息
    m_ctx.uc_stack.ss_size = m_stack_size;

    /*  The makecontext() function modifies the context pointed to by ucp (which was obtained from a call  to  getcontext(3)).   Before  invoking  makecon‐
     *  text(),  the  caller  must allocate a new stack for this context and assign its address to ucp->uc_stack, and define a successor context and assign
     *  its address to ucp->uc_link.
     */
    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    LOG_DEBUG(g_logger) << "Fiber::Fiber id=" << m_id;
}



/**
 * @brief 协程结束后回收内存
 */
Fiber::~Fiber() {
    --s_fiber_count;
    if(m_stack) {
        ASSERT_MACRO(m_state == STATE_TERM
                || m_state == STATE_EXCEPT
                || m_state == STATE_INIT);
        StackAllocator::Dealloc(m_stack, m_stack_size);
    } else {
        ASSERT_MACRO(!m_cb);
        ASSERT_MACRO(m_state == STATE_EXEC);

        Fiber* cur = t_fiber;
        if(cur == this) {
            SetThis(nullptr);
        }
    }

    LOG_DEBUG(g_logger) << "Fiber::~Fiber id=" << m_id
                              << " total=" << s_fiber_count;
}

/**
 * @brief 重置协程执行函数,并设置状态
 * @pre getState() 为 INIT, TERM, EXCEPT
 * @post getState() = INIT
 */
void Fiber::reset(std::function<void()> cb) {
    ASSERT_MACRO(m_stack);
    ASSERT_MACRO(m_state == STATE_TERM
            || m_state == STATE_EXCEPT
            || m_state == STATE_INIT);
    m_cb = cb;
    if(getcontext(&m_ctx)) {
        ASSERT_MACRO2(false, "getcontext");
    }
    m_ctx.uc_link = nullptr; /// 保存当前context结束后继续执行的context记录
    m_ctx.uc_stack.ss_sp = m_stack;     /// context运行的栈信息
    m_ctx.uc_stack.ss_size = m_stack_size;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = STATE_INIT;
}

/**
 * @brief 将当前协程切换到运行状态
 * @pre getState() != EXEC
 * @post getState() = EXEC
 */
void Fiber::swapIn() {
    SetThis(this);
    ASSERT_MACRO(m_state != STATE_EXEC);
    m_state = STATE_EXEC;
    if(swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
        ASSERT_MACRO2(false, "swapcontext");
    }
}

/**
 * @brief 将当前协程切换到后台xu
 */
void Fiber::swapOut() {
    SetThis(t_threadFiber.get());
    if(swapcontext(&m_ctx, &t_threadFiber->m_ctx)) {
        ASSERT_MACRO2(false, "swapcontext");
    }

}

//设置当前协程
void Fiber::SetThis(Fiber* f) {
    t_fiber = f;
}

/**
 * @brief 返回当前执行的协程
 */
Fiber::ptr Fiber::GetThis() {
    if(t_fiber) {
        return t_fiber->shared_from_this();
    }

    Fiber::ptr main_fiber(new Fiber);
    ASSERT_MACRO(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
    // Fiber::ptr main_fiber(new Fiber);
}

/**
 * @brief 将当前协程切换到后台,并设置为READY状态
 * @post getState() = READY
 */
void Fiber::YieldToReady() {
    Fiber::ptr cur = GetThis();
    ASSERT_MACRO(cur->m_state == STATE_EXEC);
    cur->m_state = STATE_READY;
    cur->swapOut();
}

/**
 * @brief 将当前协程切换到后台,并设置为HOLD状态
 * @post getState() = HOLD
 */
void Fiber::YieldToHold() {
    Fiber::ptr cur = GetThis();
    ASSERT_MACRO(cur->m_state == STATE_EXEC);
    cur->m_state = STATE_HOLD;
    cur->swapOut();
}

/**
 * @brief 返回当前协程的总数量
 */
uint64_t Fiber::TotalFibers() {
    return s_fiber_count;
}

/**
 * @brief 协程执行函数
 * @post 执行完成返回到线程主协程
 */
void Fiber::MainFunc() {
    Fiber::ptr cur = GetThis();
    ASSERT_MACRO(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = STATE_TERM;
    } catch (std::exception& ex) {
        cur->m_state = STATE_EXCEPT;
        LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
            << " fiber_id=" << cur->getId()
            << std::endl
            << ipmsg::BacktraceToString();
    } catch (...) {
        cur->m_state = STATE_EXCEPT;
        LOG_ERROR(g_logger) << "Fiber Except"
            << " fiber_id=" << cur->getId()
            << std::endl
            << ipmsg::BacktraceToString();
    }

    /// 回主协程
    auto raw_ptr = cur.get();
    cur.reset(); /// deletes managed object
    raw_ptr->swapOut(); /// 整个栈被析构

    ASSERT_MACRO2(false, "never reach here");
}

}
