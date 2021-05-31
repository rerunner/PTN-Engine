/*
 * This file is part of PTN Engine
 *
 * Copyright (c) 2017 Eduardo Valgôde
 * Copyright (c) 2021 Kale Evans
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

#include "PTN_Engine/PTN_Engine/PTN_EngineImp.h"
#include "PTN_Engine/IExporter.h"
#include "PTN_Engine/IImporter.h"
#include "PTN_Engine/PTN_Engine/Transition.h"
#include "PTN_Engine/Place.h"
#include "PTN_Engine/Utilities/DetectRepeated.h"
#include "PTN_Engine/Utilities/LockWeakPtr.h"
#include <algorithm>

namespace ptne
{
using namespace std;

PTN_Engine::PTN_EngineImp::PTN_EngineImp()
: m_stop{ false }
{
}

PTN_Engine::PTN_EngineImp::~PTN_EngineImp()
{
}

void PTN_Engine::PTN_EngineImp::createTransition(const vector<string> &activationPlaces,
                                                 const vector<size_t> &activationWeights,
                                                 const vector<string> &destinationPlaces,
                                                 const vector<size_t> &destinationWeights,
                                                 const vector<string> &inhibitorPlaces,
                                                 const vector<ConditionFunction> &additionalConditions)
{
	unique_lock<shared_timed_mutex> guard(m_mutex);
	createTransitionImp(activationPlaces, activationWeights, destinationPlaces, destinationWeights,
						inhibitorPlaces, createAnonymousConditions(additionalConditions));
}

void PTN_Engine::PTN_EngineImp::createTransitionImp(
const vector<string> &activationPlaces,
const vector<size_t> &activationWeights,
const vector<string> &destinationPlaces,
const vector<size_t> &destinationWeights,
const vector<string> &inhibitorPlaces,
const vector<pair<string, ConditionFunction>> &additionalConditions)
{
	vector<WeakPtrPlace> activationPlacesVector = getPlacesFromNames(activationPlaces);

	vector<WeakPtrPlace> destinationPlacesVector = getPlacesFromNames(destinationPlaces);

	vector<WeakPtrPlace> inhibitorPlacesVector = getPlacesFromNames(inhibitorPlaces);

	m_transitions.push_back(Transition(activationPlacesVector, activationWeights, destinationPlacesVector,
									   destinationWeights, inhibitorPlacesVector, additionalConditions));
}

void PTN_Engine::PTN_EngineImp::createTransition(const vector<string> &activationPlaces,
                                                 const vector<size_t> &activationWeights,
                                                 const vector<string> &destinationPlaces,
                                                 const vector<size_t> &destinationWeights,
                                                 const vector<string> &inhibitorPlaces,
                                                 const vector<string> &additionalConditions)
{
	unique_lock<shared_timed_mutex> guard(m_mutex);
	createTransitionImp(activationPlaces, activationWeights, destinationPlaces, destinationWeights,
						inhibitorPlaces, getConditionFunctions(additionalConditions));
}

void PTN_Engine::PTN_EngineImp::createTransitionImp(const vector<string> &activationPlaces,
                                                    const vector<size_t> &activationWeights,
                                                    const vector<string> &destinationPlaces,
                                                    const vector<size_t> &destinationWeights,
                                                    const vector<string> &inhibitorPlaces,
                                                    const vector<string> &additionalConditions)
{
	createTransitionImp(activationPlaces, activationWeights, destinationPlaces, destinationWeights,
						inhibitorPlaces, getConditionFunctions(additionalConditions));
}

vector<Transition *> PTN_Engine::PTN_EngineImp::collectEnabledTransitionsRandomly()
{
	vector<Transition *> enabledTransitions;
	for (Transition &transition : m_transitions)
	{
		if (transition.isEnabled())
		{
			enabledTransitions.push_back(&transition);
		}
	}
	random_shuffle(enabledTransitions.begin(), enabledTransitions.end());
	return enabledTransitions;
}

void PTN_Engine::PTN_EngineImp::createPlace(const string &name,
                                            const size_t initialNumberOfTokens,
                                            ActionFunction onEnterAction,
                                            ActionFunction onExitAction,
                                            const bool input)
{
	unique_lock<shared_timed_mutex> guard(m_mutex);
	createPlaceImp(name, initialNumberOfTokens, onEnterAction, onExitAction, input);
}

void PTN_Engine::PTN_EngineImp::createPlaceImp(const string &name,
                                               const size_t initialNumberOfTokens,
                                               ActionFunction onEnterAction,
                                               ActionFunction onExitAction,
                                               const bool input)
{
	if (m_places.find(name) != m_places.end())
	{
		throw RepeatedPlaceException(name);
	}

	SharedPtrPlace place = make_shared<Place>(initialNumberOfTokens, onEnterAction, onExitAction, input);

	m_places[name] = place;
	if (place->isInputPlace())
	{
		m_inputPlaces.push_back(place);
	}
}

void PTN_Engine::PTN_EngineImp::createPlaceStr(const string &name,
                                               const size_t initialNumberOfTokens,
                                               const string &onEnterAction,
                                               const string &onExitAction,
                                               const bool input)
{
	unique_lock<shared_timed_mutex> guard(m_mutex);
	createPlaceStrImp(name, initialNumberOfTokens, onEnterAction, onExitAction, input);
}

void PTN_Engine::PTN_EngineImp::createPlaceStrImp(const string &name,
                                                  const size_t initialNumberOfTokens,
                                                  const string &onEnterActionName,
                                                  const string &onExitActionName,
                                                  const bool input)
{
	ActionFunction onEnterAction = getActionFunction(onEnterActionName);
	ActionFunction onExitAction = getActionFunction(onEnterActionName);

	if (m_places.find(name) != m_places.end())
	{
		throw RepeatedPlaceException(name);
	}

	SharedPtrPlace place = make_shared<Place>(initialNumberOfTokens, onEnterActionName, onEnterAction,
											  onExitActionName, onExitAction, input);

	m_places[name] = place;
	if (place->isInputPlace())
	{
		m_inputPlaces.push_back(place);
	}
}

void PTN_Engine::PTN_EngineImp::registerAction(const string &name, ActionFunction action)
{
	unique_lock<shared_timed_mutex> guard(m_mutex);
	if (name.empty())
	{
		throw InvalidFunctionNameException(name);
	}

	if (m_actions.find(name) != m_actions.end())
	{
		throw RepeatedActionException(name);
	}

	// verificar acções repetidas
	m_actions[name] = action;
}

void PTN_Engine::PTN_EngineImp::registerCondition(const string &name, ConditionFunction condition)
{
	unique_lock<shared_timed_mutex> guard(m_mutex);
	if (name.empty())
	{
		throw InvalidFunctionNameException(name);
	}

	if (m_conditions.find(name) != m_conditions.end())
	{
		throw RepeatedConditionException(name);
	}

	// verificar se há repetidos?
	m_conditions[name] = condition;
}

void PTN_Engine::PTN_EngineImp::execute(const bool log, ostream &o)
{
	unique_lock<shared_timed_mutex> guard(m_mutex);
	m_stop = false;
	bool transitionFired;

	do
	{
		if (log)
		{
			printState(o);
		}

		// TODO: Create an iterator for active transitions
		// Safe to use raw pointers here. Nothing justifies deleting a
		// transition from m_transitions, so there should never be an
		// invalid pointer. At the moment this is only single threaded,
		// so synchronization problems are not an issue.
		vector<Transition *> enabledTransitions(collectEnabledTransitionsRandomly());

		transitionFired = false;
		for (Transition *transition : enabledTransitions)
		{
			transitionFired |= transition->execute();
		}
	} while (transitionFired && !m_stop);

	clearInputPlaces();
	m_stop = false;
}

void PTN_Engine::PTN_EngineImp::execute()
{
	execute(false);
}

void PTN_Engine::PTN_EngineImp::clearInputPlaces()
{
	for (const WeakPtrPlace &place : m_inputPlaces)
	{
		SharedPtrPlace spPlace = lockWeakPtr(place);
		spPlace->setNumberOfTokens(0);
	}
}

ActionFunction PTN_Engine::PTN_EngineImp::getActionFunction(const string &name) const
{
	if (name.empty())
	{
		return nullptr;
	}

	if (m_actions.find(name) == m_actions.end())
	{
		throw InvalidFunctionNameException(name);
	}
	return m_actions.at(name);
}

vector<pair<string, ConditionFunction>>
PTN_Engine::PTN_EngineImp::getConditionFunctions(const vector<string> &names) const
{
	vector<pair<string, ConditionFunction>> conditions;
	for_each(names.cbegin(), names.cend(), [&](const string &name) {
		if (name.empty())
		{
			return;
		}

		if (m_conditions.find(name) == m_conditions.end())
		{
			throw InvalidFunctionNameException(name);
		}
		conditions.push_back(pair<string, ConditionFunction>(name, m_conditions.at(name)));
	});
	return conditions;
}

size_t PTN_Engine::PTN_EngineImp::getNumberOfTokens(const string &place) const
{
	shared_lock<shared_timed_mutex> guard(m_mutex);
	if (m_places.find(place) == m_places.end())
	{
		throw InvalidNameException(place);
	}
	return m_places.at(place)->getNumberOfTokens();
}

void PTN_Engine::PTN_EngineImp::incrementInputPlace(const string &place)
{
	unique_lock<shared_timed_mutex> guard(m_mutex);

	if (m_places.find(place) == m_places.end())
	{
		throw InvalidNameException(place);
	}
	if (!m_places.at(place)->isInputPlace())
	{
		throw NotInputPlaceException(place);
	}
	m_places.at(place)->enterPlace(1);
}

vector<WeakPtrPlace> PTN_Engine::PTN_EngineImp::getPlacesFromNames(const vector<string> &placesNames) const
{
	utility::detectRepeatedNames<string, RepeatedPlaceNamesException>(placesNames);

	vector<WeakPtrPlace> placesVector;
	for (const auto &placeName : placesNames)
	{
		if (m_places.find(placeName) == m_places.end())
		{
			throw InvalidNameException(placeName);
		}
		placesVector.push_back(m_places.at(placeName));
	}
	return placesVector;
}

void PTN_Engine::PTN_EngineImp::export_(IExporter &exporter) const
{
	shared_lock<shared_timed_mutex> guard(m_mutex);
	exportPlaces(exporter);
	exportTransitions(exporter);
}

void PTN_Engine::PTN_EngineImp::exportPlaces(IExporter &exporter) const
{
	for (const auto &place : m_places)
	{
		string onEnter;
		auto itFound = find_if(m_actions.cbegin(), m_actions.cend(),
							   [&](const auto &it) { return it.first == place.second->getOnEnterActionName(); });
		if (itFound != m_actions.cend())
		{
			onEnter = itFound->first; // on enter
		}

		string onExit;
		itFound = find_if(m_actions.cbegin(), m_actions.cend(),
						  [&](const auto &it) { return it.first == place.second->getOnExitActionName(); });
		if (itFound != m_actions.cend())
		{
			onExit = itFound->first; // on enter
		}

		exporter.exportPlace(place.first, std::to_string(place.second->getNumberOfTokens()),
							 place.second->isInputPlace() ? "true" : "false", onEnter, onExit);
	}
}

void PTN_Engine::PTN_EngineImp::exportTransitions(IExporter &exporter) const
{
	for (const auto &transition : m_transitions)
	{
		vector<tuple<string, size_t>> activationPlacesExport;
		for (const auto &activationPlace : transition.getActivationPlaces())
		{
			tuple<string, size_t> activationPlaceExport;
			get<0>(activationPlaceExport) = findName(get<0>(activationPlace).lock(), m_places);
			get<1>(activationPlaceExport) = get<1>(activationPlace);
			activationPlacesExport.emplace_back(activationPlaceExport);
		}

		vector<tuple<string, size_t>> destinationPlacesExport;
		for (const auto &destinationPlace : transition.getDestinationPlaces())
		{
			tuple<string, size_t> destinationPlaceExport;
			get<0>(destinationPlaceExport) = findName(get<0>(destinationPlace).lock(), m_places);
			get<1>(destinationPlaceExport) = get<1>(destinationPlace);
			destinationPlacesExport.emplace_back(destinationPlaceExport);
		}

		vector<string> activationConditionsNames;
		for (const auto &activationCondition : transition.getAdditionalActivationConditions())
		{
			if (m_conditions.find(activationCondition.first) != m_conditions.end())
			{
				activationConditionsNames.emplace_back(activationCondition.first);
			}
		}

		vector<string> inhibitorPlacesNames;
		for (const auto &inhibitorPlace : transition.getInhibitorPlaces())
		{
			inhibitorPlacesNames.emplace_back(findName(inhibitorPlace.lock(), m_places));
		}

		exporter.exportTransition(activationPlacesExport, destinationPlacesExport, activationConditionsNames,
								  inhibitorPlacesNames);
	}
}

void PTN_Engine::PTN_EngineImp::import(const IImporter &importer)
{
	unique_lock<shared_timed_mutex> guard(m_mutex);
	// TODO: Not exception safe, maybe this should go to a constructor instead.
	clearNet();

	// create places
	for (const IImporter::PlaceInfo &placeInfo : importer.getPlaces())
	{
		createPlaceStrImp(get<0>(placeInfo), get<1>(placeInfo), get<2>(placeInfo), get<3>(placeInfo),
						  get<4>(placeInfo));
	}

	// create transitions
	for (const IImporter::TransitionInfo &transitionInfo : importer.getTransitions())
	{
		createTransitionImp(get<0>(transitionInfo), get<1>(transitionInfo), get<2>(transitionInfo),
							get<3>(transitionInfo), get<4>(transitionInfo), get<5>(transitionInfo));
	}
}

void PTN_Engine::PTN_EngineImp::clearNet()
{
	m_inputPlaces.clear();
	m_places.clear();
	m_transitions.clear();
}

PTN_Engine::PTN_EngineImp::InvalidNameException::InvalidNameException(const string &name)
: PTN_Exception("Invalid name: " + name + ".")
{
}

PTN_Engine::PTN_EngineImp::RepeatedPlaceNamesException::RepeatedPlaceNamesException()
: PTN_Exception("Tried to create transition with repeated places.")
{
}

PTN_Engine::PTN_EngineImp::NotInputPlaceException::NotInputPlaceException(const string &name)
: PTN_Exception(name + " is not an input place.")
{
}

PTN_Engine::PTN_EngineImp::RepeatedPlaceException::RepeatedPlaceException(const string &name)
: PTN_Exception("Trying to add an already existing place: " + name + ".")
{
}

PTN_Engine::PTN_EngineImp::RepeatedActionException::RepeatedActionException(const string &name)
: PTN_Exception("Trying to add an already existing action: " + name + ".")
{
}

PTN_Engine::PTN_EngineImp::RepeatedConditionException::RepeatedConditionException(const string &name)
: PTN_Exception("Trying to add an already existing condition: " + name + ".")
{
}

PTN_Engine::PTN_EngineImp::InvalidFunctionNameException::InvalidFunctionNameException(const string &name)
: PTN_Exception("The function is not yet registered: " + name + ".")
{
}

void PTN_Engine::PTN_EngineImp::printState(ostream &o) const
{
	o << "Place; Tokens" << endl;
	for (const auto &p : m_places)
	{
		o << p.first.c_str() << ": " << p.second->getNumberOfTokens() << endl;
	}
	o << endl << endl;
}

const vector<pair<string, ConditionFunction>>
PTN_Engine::PTN_EngineImp::createAnonymousConditions(const vector<ConditionFunction> &conditions) const
{
	vector<pair<string, ConditionFunction>> anonymousConditionsVector;
	for (const auto &condition : conditions)
	{
		anonymousConditionsVector.emplace_back(pair<string, ConditionFunction>("", condition));
	}
	return anonymousConditionsVector;
}
} // namespace ptne
