#include "process/process.h"


//AstreeNode* | a list of the nodes dependencies


bool CelProcess::attachProcess(ProcessEntry* process) noexcept
{
		processNodes.emplace_back(process);
		return true;
}

//the process: this is a loop that runs the repeatables of each node in the AST
void CelProcess::beginprocess()
{
	while (true)
	{
		for (auto& repeatable : processNodes) {
			//TODO: check the node later for order/priority of execution
			repeatable->process(); //we will set the process to take the dependencies as an argument later
		}
	}
	return;
}

void CelProcess::printProcesses() const noexcept {
	for (const auto& process : processNodes) {
		std::cout << "Process: " << process->node->getTagName() << std::endl;
	}
}

bool CelProcess::startPluginProcess(const std::string& pluginPath, const std::vector<std::string>& args) {
	try {
		ProcessInfo pluginInfo = ProcessUtils::startPlugin(pluginPath, args);
		if (pluginInfo.pid != 0) {
			pluginProcesses.push_back(pluginInfo);
			return true;
		}
	} catch (const std::exception& e) {
		std::cerr << "Failed to start plugin " << pluginPath << ": " << e.what() << std::endl;
	}
	return false;
}

bool CelProcess::stopPluginProcess(const std::string& pluginName) {
	for (auto it = pluginProcesses.begin(); it != pluginProcesses.end(); ++it) {
		if (it->name.find(pluginName) != std::string::npos) {
			if (ProcessUtils::terminateProcess(it->pid)) {
				pluginProcesses.erase(it);
				return true;
			}
		}
	}
	return false;
}

std::vector<ProcessInfo> CelProcess::getPluginProcesses() const {
	return pluginProcesses;
}

bool CelProcess::isPluginRunning(const std::string& pluginName) const {
	for (const auto& plugin : pluginProcesses) {
		if (plugin.name.find(pluginName) != std::string::npos) {
			return ProcessUtils::isProcessRunning(plugin.pid);
		}
	}
	return false;
}
