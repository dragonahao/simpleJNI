﻿#include "stdafx.h"
#include <process-thread.h>
#include "Inject.h"

bool Inject(pid_t pid, const char* soPath) {
	RemoteProcess remoteProcess(pid);
	remoteProcess.Attach();
	remoteProcess.Detach();
	return false;
}
