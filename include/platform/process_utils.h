#ifndef PROCESS_UTILS_H
#define PROCESS_UTILS_H

#include "platform/platform.h"
#include <string>
#include <vector>
#include <memory>

#if PLATFORM_WINDOWS
typedef HANDLE ProcessHandle;
typedef DWORD ProcessId;
#elif PLATFORM_POSIX
typedef pid_t ProcessHandle;
typedef pid_t ProcessId;
#endif

struct ProcessInfo {
    ProcessId pid;
    ProcessHandle handle;
    std::string name;
    bool isRunning;
};

class ProcessUtils {
public:
    // Process creation and management
    static ProcessInfo createProcess(const std::string& command, const std::vector<std::string>& args = {});
    static bool terminateProcess(ProcessId pid);
    static bool isProcessRunning(ProcessId pid);
    
    // Process enumeration
    static std::vector<ProcessInfo> getRunningProcesses();
    static ProcessInfo findProcessByName(const std::string& name);
    
    // Process monitoring
    static bool waitForProcess(ProcessId pid, int timeoutMs = 0);
    static int getExitCode(ProcessId pid);
    
    // Plugin-specific utilities
    static ProcessInfo startPlugin(const std::string& pluginPath, const std::vector<std::string>& args = {});
    static std::vector<ProcessInfo> getPluginProcesses();

private:
#if PLATFORM_WINDOWS
    static ProcessHandle openProcess(ProcessId pid, DWORD access);
    static std::string getProcessName(ProcessHandle handle);
#elif PLATFORM_POSIX
    static std::string getProcessName(ProcessId pid);
    static bool sendSignal(ProcessId pid, int signal);
#endif
};

#endif // PROCESS_UTILS_H
