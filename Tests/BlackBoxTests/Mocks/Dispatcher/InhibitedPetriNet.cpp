/*
 * This file is part of PTN Engine
 *
 * Copyright (c) 2017-2023 Eduardo Valgôde
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

#include "Mocks/Dispatcher/InhibitedPetriNet.h"

using namespace ptne;
using namespace std;

InhibitedPetriNet::InhibitedPetriNet(PTN_Engine::ACTIONS_THREAD_OPTION actionsThreadOption)
: PTN_Engine(actionsThreadOption)
{
	// Places
	createPlace("InputWaitPackage", 0, true);

	createPlace("P1", 1);
	createPlace("P2", 1);
	createPlace("P3", 1);
	createPlace("P4", 0);
	createPlace("P5", 0);

	// Transitions


	createTransition({ "InputWaitPackage", "P1", "P3" }, // activation
					 { "P4" } // destination
	);


	createTransition({ "P2" }, // activation
					 { "P5" }, // destination
					 { "P3" } // inhibitor arc
	);


	createTransition({ "InputWaitPackage", "P4" }, // activation
					 { "P1", "P3" } // destination
	);


	createTransition({ "P5" }, // activation
					 { "P2" }, // destination
					 { "P4" } // inhibitor arc
	);
}

void InhibitedPetriNet::dispatch()
{
	incrementInputPlace("InputWaitPackage");
	execute();
}

bool InhibitedPetriNet::stillRunning() const
{
	return PTN_Engine::isEventLoopRunning();
}

void InhibitedPetriNet::stop()
{
	PTN_Engine::stop();
}
