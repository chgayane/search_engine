#include <string>
#include <queue>
#include <set>
#include <pthread.h>

typedef void (*linkHandler)(std::string);
class linkQueue 
{
    private:    
    std::queue<std::string> _linkQueue;
    std::set<linkHandler> _handlers;
    pthread_t thread;
    pthread_mutex_t mutex;//помогает при синхрон работы потоков чтобы не испортили очередь
    pthread_cond_t condvar;//

    public:
    linkQueue();
    void addLink(std::string link);
    void registerHandler (linkHandler handler);  
    friend void * msgReceiver(void * arg);  
    friend void* handlerInvoker(void* arg);
};
