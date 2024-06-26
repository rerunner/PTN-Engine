/*
 * This file is part of PTN Engine
 *
 * Copyright (c) 2017-2024 Eduardo Valgôde
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

#include "PTN_Engine/Place.h"
#include "PTN_Engine/Executor/IActionsExecutor.h"
#include "PTN_Engine/PTN_EngineImp.h"
#include "PTN_Engine/PTN_Exception.h"
#include "PTN_Engine/Utilities/LockWeakPtr.h"
#include <mutex>
#include <string>
#include <thread>


namespace ptne
{
using namespace std;

Place::~Place() = default;

Place::Place(const PlaceProperties &placeProperties, const shared_ptr<IActionsExecutor> &executor)
: m_name(placeProperties.name)
, m_onEnterActionName(placeProperties.onEnterActionFunctionName)
, m_onEnterAction(placeProperties.onEnterAction)
, m_onExitActionName(placeProperties.onExitActionFunctionName)
, m_onExitAction(placeProperties.onExitAction)
, m_numberOfTokens(placeProperties.initialNumberOfTokens)
, m_isInputPlace(placeProperties.input)
, m_actionsExecutor(executor)
{
	if (!m_onEnterActionName.empty() && m_onEnterAction == nullptr)
	{
		throw PTN_Exception("On enter action function must be specified.");
	}

	if (!m_onExitActionName.empty() && m_onExitAction == nullptr)
	{
		throw PTN_Exception("On exit action function must be specified.");
	}
}

string Place::getName() const
{
	return m_name;
}

void Place::enterPlace(const size_t tokens)
{
	unique_lock guard(m_mutex);
	increaseNumberOfTokens(tokens);
	if (m_onEnterAction == nullptr)
	{
		return;
	}
	while (m_blockStartingOnEnterActions)
	{
		// TO DO make this wait period configurable
		// add a max wait time after which an exception
		// is thrown.
		this_thread::sleep_for(100ms);
	}
	lockWeakPtr(m_actionsExecutor)->executeAction(m_onEnterAction, m_onEnterActionsInExecution);
}

void Place::exitPlace(const size_t tokens)
{
	unique_lock guard(m_mutex);
	decreaseNumberOfTokens(tokens);
	if (m_onExitAction == nullptr)
	{
		return;
	}
	lockWeakPtr(m_actionsExecutor)->executeAction(m_onExitAction, m_onExitActionsInExecution);
}

void Place::increaseNumberOfTokens(const size_t tokens)
{
	if (tokens == 0)
	{
		throw NullTokensException();
	}

	if (tokens > ULLONG_MAX - m_numberOfTokens)
	{
		throw OverflowException(tokens);
	}

	m_numberOfTokens += tokens;
}

void Place::decreaseNumberOfTokens(const size_t tokens)
{
	if (m_numberOfTokens < tokens)
	{
		throw NotEnoughTokensException();
	}
	if (tokens == 0) // reset
	{
		m_numberOfTokens = 0;
	}
	else
	{
		m_numberOfTokens -= tokens;
	}
}

void Place::setNumberOfTokens(const size_t tokens)
{
	unique_lock guard(m_mutex);
	m_numberOfTokens = tokens;
}

size_t Place::getNumberOfTokens() const
{
	shared_lock guard(m_mutex);
	return m_numberOfTokens;
}

bool Place::isInputPlace() const
{
	shared_lock guard(m_mutex);
	return m_isInputPlace;
}

string Place::getOnEnterActionName() const
{
	shared_lock guard(m_mutex);
	return m_onEnterActionName;
}

string Place::getOnExitActionName() const
{
	shared_lock guard(m_mutex);
	return m_onExitActionName;
}

bool Place::isOnEnterActionInExecution() const
{
	return m_onEnterActionsInExecution > 0;
}

void Place::blockStartingOnEnterActions(const bool value)
{
	m_blockStartingOnEnterActions = value;
}

PlaceProperties Place::placeProperties() const
{
	PlaceProperties placeProperties;
	placeProperties.name = m_name;
	placeProperties.onEnterActionFunctionName = m_onEnterActionName;
	placeProperties.onExitActionFunctionName = m_onExitActionName;
	placeProperties.initialNumberOfTokens = m_numberOfTokens;
	placeProperties.onEnterAction = m_onEnterAction;
	placeProperties.onExitAction = m_onExitAction;
	placeProperties.input = m_isInputPlace;
	return placeProperties;
}

void Place::setActionsExecutor(shared_ptr<IActionsExecutor> &actionsExecutor)
{
	unique_lock guard(m_mutex);
	m_actionsExecutor = actionsExecutor;
}

} // namespace ptne
