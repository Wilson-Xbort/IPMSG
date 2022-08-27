#include "../src/ipmsg.h"

ipmsg::Logger::ptr g_logger = LOG_ROOT();
int count = 0;
// ipmsg::RWMutex s_mutex;
ipmsg::Mutex s_mutex;

void fun1() {
     LOG_INFO(g_logger) << "name:" << ipmsg::Thread::GetName()
                       << " this.name: " << ipmsg::Thread::GetThis()->getName()
                       << " id: " << ipmsg::GetThreadId()
                       << " this.id: " << ipmsg::Thread::GetThis()->getId();

    // std::cout << std::to_string(s_mutex.m_mutex) << std::endl;
    for(int i = 0; i < 100000; i++) {
        //ipmsg::RWMutex::WriteLock lock(s_mutex);
        ipmsg::Mutex::Lock lock(s_mutex);
        ++count;
    }
}

void fun2() {
    while(true) {
        LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }
}

void fun3() {
    while(true) {
        LOG_INFO(g_logger) << "================================================";
    }

}

int main(int argc, char **argv) {
    LOG_INFO(g_logger) << "thread test begin";
    YAML::Node root = YAML::LoadFile("/home/vxe/ipmsg/conf/log2.yml");
    ipmsg::Config::LoadFromYaml(root); /// fromString

    std::vector<ipmsg::Thread::ptr> thrs;
    for(int i = 0; i < 5; ++i) {
        // std::cout << "create :" << std::to_string(i) << std::endl;
        ipmsg::Thread::ptr thr(new ipmsg::Thread(&fun2, "name_" + std::to_string(i * 2)));
        ipmsg::Thread::ptr thr2(new ipmsg::Thread(&fun3, "name_" + std::to_string(i * 2 + 1)));
        thrs.push_back(thr);
        thrs.push_back(thr2);
    }

    for(size_t i = 0; i < thrs.size(); ++i) {
        // std::cout << "join running" << std::endl;
        thrs[i]->join();
    }

    LOG_INFO(g_logger) << "thread test end";
    LOG_INFO(g_logger) << "count =" << count;
    return 0;
}

