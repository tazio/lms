#pragma once

#include <filesystem>
#include <memory>

#include "IChildProcess.hpp"

class IChildProcessManager
{
	public:
		virtual ~IChildProcessManager() = default;

		virtual std::unique_ptr<IChildProcess> spawnChildProcess(const std::filesystem::path& path, const IChildProcess::Args& args) = 0;
};

std::unique_ptr<IChildProcessManager> createChildProcessManager();

