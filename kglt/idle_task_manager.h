#ifndef IDLE_TASK_MANAGER_H
#define IDLE_TASK_MANAGER_H

#include <cstdint>
#include <sigc++/sigc++.h>
#include <functional>
#include <map>
#include <mutex>

namespace kglt {

typedef uint32_t ConnectionID;

class IdleTaskManager {
public:
    IdleTaskManager();

    ConnectionID add(std::function<bool ()> callback);
    ConnectionID add_once(std::function<void ()> callback);
    
    void remove(ConnectionID connection);

    void execute();

private:
    std::map<ConnectionID, std::function<bool ()> > signals_;
    std::map<ConnectionID, std::function<void ()> > signals_once_;

    std::mutex signals_mutex_;
    std::mutex signals_once_mutex_;

};

}
#endif // IDLE_TASK_MANAGER_H
