#include "platform/process_utils.h"
#include <iostream>
#include <stdexcept>

#if PLATFORM_WINDOWS
#include <tlhelp32.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#elif PLATFORM_POSIX
#include <signal.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#endif

ProcessInfo ProcessUtils::createProcess(const std::string& command, const std::vector<std::string>& args) {
    ProcessInfo info = {};
    
#if PLATFORM_WINDOWS
    std::string fullCommand = command;
    for (const auto& arg : args) {
        fullCommand += " " + arg;
    }
    
    STARTUPINFOA si = {};
    PROCESS_INFORMATION pi = {};
    si.cb = sizeof(si);
    
    if (!CreateProcessA(
        NULL,                                   // Application name
        const_cast<char*>(fullCommand.c_str()), // Command line
        NULL,                                   // Process handle not inheritable
        NULL,                                   // Thread handle not inheritable
        FALSE,                                  // Set handle inheritance to FALSE
        0,                                      // No creation flags
        NULL,                                   // Use parent's environment block
        NULL,                                   // Use parent's starting directory 
        &si,                                    // Pointer to STARTUPINFO structure
        &pi                                     // Pointer to PROCESS_INFORMATION structure
    )) {
        throw std::runtime_error("Failed to create process: " + std::to_string(GetLastError()));
    }
    
    info.pid = pi.dwProcessId;
    info.handle = pi.hProcess;
    info.name = command;
    info.isRunning = true;
    
    CloseHandle(pi.hThread);
    
#elif PLATFORM_POSIX
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process
        std::vector<char*> argv;
        argv.push_back(const_cast<char*>(command.c_str()));
        for (const auto& arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);
        
        execvp(command.c_str(), argv.data());
        // If execvp returns, there was an error
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        throw std::runtime_error("Failed to fork process");
    }
    
    info.pid = pid;
    info.handle = pid;
    info.name = command;
    info.isRunning = true;
    
#endif
    
    return info;
}

bool ProcessUtils::terminateProcess(ProcessId pid) {
#if PLATFORM_WINDOWS
    ProcessHandle handle = openProcess(pid, PROCESS_TERMINATE);
    if (handle == NULL) return false;
    
    bool result = TerminateProcess(handle, 1);
    CloseHandle(handle);
    return result;
    
#elif PLATFORM_POSIX
    return sendSignal(pid, SIGTERM);
#endif
}

bool ProcessUtils::isProcessRunning(ProcessId pid) {
#if PLATFORM_WINDOWS
    ProcessHandle handle = openProcess(pid, PROCESS_QUERY_LIMITED_INFORMATION);
    if (handle == NULL) return false;
    
    DWORD exitCode;
    bool running = GetExitCodeProcess(handle, &exitCode) && (exitCode == STILL_ACTIVE);
    CloseHandle(handle);
    return running;
    
#elif PLATFORM_POSIX
    return sendSignal(pid, 0); // Signal 0 just checks if process exists
#endif
}

std::vector<ProcessInfo> ProcessUtils::getRunningProcesses() {
    std::vector<ProcessInfo> processes;
    
#if PLATFORM_WINDOWS
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return processes;
    
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    
    if (Process32First(snapshot, &pe32)) {
        do {
            ProcessInfo info = {};
            info.pid = pe32.th32ProcessID;
            info.handle = openProcess(info.pid, PROCESS_QUERY_LIMITED_INFORMATION);
            info.name = pe32.szExeFile;
            info.isRunning = true;
            processes.push_back(info);
        } while (Process32Next(snapshot, &pe32));
    }
    
    CloseHandle(snapshot);
    
#elif PLATFORM_POSIX
    DIR* dir = opendir("/proc");
    if (!dir) return processes;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_DIR) continue;
        
        ProcessId pid;
        try {
            pid = std::stoi(entry->d_name);
        } catch (...) {
            continue;
        }
        
        ProcessInfo info = {};
        info.pid = pid;
        info.handle = pid;
        info.name = getProcessName(pid);
        info.isRunning = true;
        processes.push_back(info);
    }
    
    closedir(dir);
#endif
    
    return processes;
}

ProcessInfo ProcessUtils::findProcessByName(const std::string& name) {
    auto processes = getRunningProcesses();
    for (const auto& proc : processes) {
        if (proc.name.find(name) != std::string::npos) {
            return proc;
        }
    }
    
    return {}; // Return empty ProcessInfo if not found
}

bool ProcessUtils::waitForProcess(ProcessId pid, int timeoutMs) {
#if PLATFORM_WINDOWS
    ProcessHandle handle = openProcess(pid, SYNCHRONIZE);
    if (handle == NULL) return false;
    
    DWORD result = WaitForSingleObject(handle, timeoutMs == 0 ? INFINITE : timeoutMs);
    CloseHandle(handle);
    return result == WAIT_OBJECT_0;
    
#elif PLATFORM_POSIX
    if (timeoutMs == 0) {
        int status;
        return waitpid(pid, &status, 0) == pid;
    } else {
        // For timeout, we'd need to use more complex mechanisms
        // For now, just check if process is still running
        return !isProcessRunning(pid);
    }
#endif
}

int ProcessUtils::getExitCode(ProcessId pid) {
#if PLATFORM_WINDOWS
    ProcessHandle handle = openProcess(pid, PROCESS_QUERY_LIMITED_INFORMATION);
    if (handle == NULL) return -1;
    
    DWORD exitCode;
    bool result = GetExitCodeProcess(handle, &exitCode);
    CloseHandle(handle);
    return result ? static_cast<int>(exitCode) : -1;
    
#elif PLATFORM_POSIX
    int status;
    if (waitpid(pid, &status, WNOHANG) == pid) {
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            return 128 + WTERMSIG(status);
        }
    }
    return -1;
#endif
}

ProcessInfo ProcessUtils::startPlugin(const std::string& pluginPath, const std::vector<std::string>& args) {
    try {
        return createProcess(pluginPath, args);
    } catch (const std::exception& e) {
        std::cerr << "Failed to start plugin " << pluginPath << ": " << e.what() << std::endl;
        return {};
    }
}

std::vector<ProcessInfo> ProcessUtils::getPluginProcesses() {
    auto allProcesses = getRunningProcesses();
    std::vector<ProcessInfo> pluginProcesses;
    
    for (const auto& proc : allProcesses) {
        // Filter processes that look like plugins
        if (proc.name.find("plugin") != std::string::npos ||
            proc.name.find("celeris") != std::string::npos) {
            pluginProcesses.push_back(proc);
        }
    }
    
    return pluginProcesses;
}

#if PLATFORM_WINDOWS
ProcessHandle ProcessUtils::openProcess(ProcessId pid, DWORD access) {
    return OpenProcess(access, FALSE, pid);
}

std::string ProcessUtils::getProcessName(ProcessHandle handle) {
    char buffer[MAX_PATH];
    DWORD size = MAX_PATH;
    if (QueryFullProcessImageNameA(handle, 0, buffer, &size)) {
        std::string path(buffer);
        size_t lastSlash = path.find_last_of("\\/");
        return (lastSlash != std::string::npos) ? path.substr(lastSlash + 1) : path;
    }
    return "unknown";
}
#elif PLATFORM_POSIX
std::string ProcessUtils::getProcessName(ProcessId pid) {
    std::ifstream commFile("/proc/" + std::to_string(pid) + "/comm");
    if (commFile.is_open()) {
        std::string name;
        std::getline(commFile, name);
        return name;
    }
    return "unknown";
}

bool ProcessUtils::sendSignal(ProcessId pid, int signal) {
    return kill(pid, signal) == 0;
}
#endif
