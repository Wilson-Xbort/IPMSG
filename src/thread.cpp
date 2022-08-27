#include "thread.h"
#include "log.h"
#include "util.h"
namespace ipmsg {

/// thread_local关键字修饰的变量具有线程周期，在线程开始的时候被生成，在线程结束的时候被销毁
static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOW";

static ipmsg::Logger::ptr g_logger = LOG_NAME("system");

Semaphore::Semaphore(uint32_t count) {
    /* return 0 on success */
    if(sem_init(&m_semaphore, 0, count)) {
        throw std::logic_error("sem_init error");
    }

}
Semaphore::~Semaphore() {
    // std::cout << "~Semaphore start" << std::endl;
    sem_destroy(&m_semaphore);
    // std::cout << "~Semaphore end" << std::endl;
}

void Semaphore::wait() {

    // std::cout << "Enter wait" << std::endl;
    /* return 0 on success */
    if(sem_wait(&m_semaphore)) {
        throw std::logic_error("sem_wait error");
    }
    // std::cout << "Finish wait" << std::endl;


}
void Semaphore::notify() {
    /* return 0 on success */
    // std::cout << "Enter notify" << std::endl;
    if(sem_post(&m_semaphore)) {
         throw std::logic_error("sem_post error");
    }
    // std::cout << "Finish notify" << std::endl;
}


/// 获取当前线程
Thread* Thread::GetThis() {
    return t_thread;
}

/// 给日志获取当前线程名称
const std::string& Thread::GetName() {
    return t_thread_name;
}

void Thread::setName(const std::string& name) {
    if(t_thread) {
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

/**
 * @brief 构造函数
 * @param[in] cb 线程执行函数
 * @param[in] name 线程名称
 */
Thread::Thread(std::function<void()> cb, const std::string& name)
    :m_cb(cb)
    ,m_name(name) {
    if(name.empty()) {
        m_name = "UNKNOW";
    }
    // std::cout << "create thread" << std::endl;
    /* this pointer represents the thread class */
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    /* On  success,  pthread_create() returns 0; on error, it returns an error
       number, and the contents of *thread are undefined.*/
    if(rt) {
        LOG_ERROR(g_logger) << "pthread_create thread fail, rt = " << rt
            << " name=" << name;
        throw std::logic_error("pthread_create error");
    }

    m_semaphore.wait(); /// 有时候线程在出了构造函数后还没有跑起来，这边使用阻塞可以确保出构造函数前线程跑起来，不跑起来不出构造函数
}

Thread::~Thread() {
    if(m_thread) {
        pthread_detach(m_thread);
    }
}

/// 阻塞线程
void Thread::join() {
    if(m_thread) {
        int rt = pthread_join(m_thread, nullptr);
        if(rt) {
            LOG_ERROR(g_logger) << "pthread_join thread fail, rt = " << rt
                << " name=" << m_name;
            throw std::logic_error("pthread_join error");
        }
    }
    m_thread = 0;
}

void* Thread::run(void* arg) {

    // std::cout << "run start" << std::endl;
    Thread* thread = (Thread*)arg;
    t_thread = thread;
    thread->m_id = ipmsg::GetThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str()); /// 为线程设置名称, pthread_setname_np 只支持16位字符

    std::function<void()> cb; /// 防止引用被释放
    cb.swap(thread->m_cb);

    thread->m_semaphore.notify(); /// static 方法里面，用类实例来访问
    /* In order to prevent M_ CB is empty , we must be assigned to M_CB in the constructor*/
    cb();
    // std::cout << "run finish" << std::endl;
    return 0;
}






}
