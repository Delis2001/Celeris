#include "include/platform/platform.h"
#include "include/platform/process_utils.h"
#include "include/net/socket.hpp"
#include <iostream>

int main() {
    std::cout << "=== Platform Detection Test ===" << std::endl;
    
#if PLATFORM_WINDOWS
    std::cout << "Platform: Windows" << std::endl;
#elif PLATFORM_LINUX
    std::cout << "Platform: Linux" << std::endl;
#elif PLATFORM_MACOS
    std::cout << "Platform: macOS" << std::endl;
#elif PLATFORM_UNIX
    std::cout << "Platform: Unix" << std::endl;
#endif

#if PLATFORM_POSIX
    std::cout << "POSIX Compatible: Yes" << std::endl;
#else
    std::cout << "POSIX Compatible: No" << std::endl;
#endif

    std::cout << "\n=== Socket Implementation Test ===" << std::endl;
    
    try {
        AmthSocket::SocketImpl socket;
        std::cout << "Socket initialization: SUCCESS" << std::endl;
        std::cout << "Socket is initialized: " << (socket.isInitialized() ? "Yes" : "No") << std::endl;
        
#if PLATFORM_WINDOWS
        std::cout << "WSA Data available: Yes" << std::endl;
#elif PLATFORM_POSIX
        std::cout << "WSA Data available: No (POSIX)" << std::endl;
#endif
        
    } catch (const std::exception& e) {
        std::cout << "Socket initialization: FAILED - " << e.what() << std::endl;
    }

    std::cout << "\n=== Process Management Test ===" << std::endl;
    
    auto processes = ProcessUtils::getRunningProcesses();
    std::cout << "Found " << processes.size() << " running processes" << std::endl;
    
    // Show first few processes
    int count = 0;
    for (const auto& proc : processes) {
        if (count >= 5) break;
        std::cout << "  PID: " << proc.pid << ", Name: " << proc.name 
                  << ", Running: " << (proc.isRunning ? "Yes" : "No") << std::endl;
        count++;
    }

    std::cout << "\n=== Plugin Process Test ===" << std::endl;
    
    auto pluginProcesses = ProcessUtils::getPluginProcesses();
    std::cout << "Found " << pluginProcesses.size() << " plugin processes" << std::endl;

    std::cout << "\n=== Test Complete ===" << std::endl;
    return 0;
}
