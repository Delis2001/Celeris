#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>
//#include <set> //for future use, if we want to have a set of processes
#include "process/connectionRequest.h"
#include <functional>
#include <memory>
#include "ast/ast.h"
#include "platform/process_utils.h"

class CelProcess
{
public:

	CelProcess(CelProcess&& other) noexcept = default;
	CelProcess& operator=(CelProcess&& other) noexcept = default;
	//delete copy constructor and assignment operator to enforce singleton
	~CelProcess() = default;

	CelProcess() = default;

	bool attachProcess(ProcessEntry* process) noexcept;

	void beginprocess();

	void printProcesses() const noexcept;

	// Plugin process management
	bool startPluginProcess(const std::string& pluginPath, const std::vector<std::string>& args = {});
	bool stopPluginProcess(const std::string& pluginName);
	std::vector<ProcessInfo> getPluginProcesses() const;
	bool isPluginRunning(const std::string& pluginName) const;

	static CelProcess& getInstance() noexcept {
		static CelProcess instance;
		return instance;
	}

private:
	//AstreeNode* | a list of the nodes dependencies
	std::vector<ProcessEntry*> processNodes;
	
	// Plugin process tracking
	std::vector<ProcessInfo> pluginProcesses;
};
#endif // !PROCESS_H
