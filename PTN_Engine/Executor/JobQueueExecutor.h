/*
 * This file is part of PTN Engine
 *
 * Copyright (c) 2024 Eduardo Valgôde
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "PTN_Engine/Executor/IActionsExecutor.h"
#include "PTN_Engine/JobQueue/JobQueue.h"

namespace ptne
{

class JobQueueExecutor : public IActionsExecutor
{
public:
	void executeAction(const ActionFunction &action, std::atomic<size_t> &actionsInExecution) override;

private:
	//! Job queue to dispatch actions.
	JobQueue m_jobQueue;
};

} // namespace ptne
