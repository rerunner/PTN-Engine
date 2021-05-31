/*
 * This file is part of PTN Engine
 *
 * Copyright (c) 2017 Eduardo Valgôde
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

#include "Mocks/Controller.h"

Controller::Controller()
	: m_activationConditionCallCounter(0)
	, m_enterCounter(0)
	, m_exitCounter(0)
	, m_canFire(true)
{
}

void Controller::onEnter()
{
	++m_enterCounter;
}

void Controller::onExit()
{
	++m_exitCounter;
}

size_t Controller::getEnterCounter() const
{
	return m_enterCounter;
}

size_t Controller::getExitCounter() const
{
	return m_exitCounter;
}

bool Controller::activationCondition() const
{
	++m_activationConditionCallCounter;
	return m_canFire;
}

void Controller::setFireCondition(const bool canFire)
{
	m_canFire = canFire;
}

size_t Controller::activationConditionCallCounter() const
{
	return m_activationConditionCallCounter;
}
