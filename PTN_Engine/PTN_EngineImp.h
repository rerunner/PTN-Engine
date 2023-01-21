/*
 * This file is part of PTN Engine
 *
 * Copyright (c) 2017 Eduardo Valgôde
 * Copyright (c) 2021 Kale Evans
 * Copyright (c) 2023 Eduardo Valgôde
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

#include "PTN_Engine/PTN_Engine.h"
#include "PTN_Engine/PTN_Exception.h"
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <vector>

namespace ptne
{
class IConditionFunctor;
class IActionFunctor;
class IExporter;
class IImporter;
class JobQueue;
class Place;
class Transition;

using SharedPtrPlace = std::shared_ptr<Place>;
using WeakPtrPlace = std::weak_ptr<Place>;

//! Implements the Petri net logic.
/*!
 * Implements the Petri net logic.
 * Used by the PTN_Engine class.
 * \sa PTN_Engine
 */
class PTN_Engine::PTN_EngineImp final
{
public:
	//! Constructor.
	PTN_EngineImp(PTN_Engine &parent, ACTIONS_THREAD_OPTION actionsRuntimeThread);

	~PTN_EngineImp();

	/*!
	 * Create a new transition
	 * \param activationPlaces A vector with the names of the activation places.
	 * \param activationWeights A vector with the weights of each activation place.
	 * \param destinationPlaces A vector with the names of the destination places.
	 * \param destinationWeights A vector with the weights of each destination place.
	 * \param inhibitorPlaces Places that cannot have tokens to fire the transition.
	 * \param additionalConditions A vector with functions that return bool.
	 */
	void createTransition(const std::vector<std::string> &activationPlaces,
						  const std::vector<size_t> &activationWeights,
						  const std::vector<std::string> &destinationPlaces,
						  const std::vector<size_t> &destinationWeights,
						  const std::vector<std::string> &inhibitorPlaces,
						  const std::vector<ConditionFunction> &additionalConditions);

	/*!
	 * Create a new transition
	 * \param activationPlaces A vector with the names of the activation places.
	 * \param activationWeights A vector with the weights of each activation place.
	 * \param destinationPlaces A vector with the names of the destination places.
	 * \param destinationWeights A vector with the weights of each destination place.
	 * \param inhibitorPlaces Places that cannot have tokens to fire the transition.
	 * \param additionalConditions A vector with names to additional conditions.
	 */
	void createTransition(const std::vector<std::string> &activationPlaces,
						  const std::vector<size_t> &activationWeights,
						  const std::vector<std::string> &destinationPlaces,
						  const std::vector<size_t> &destinationWeights,
						  const std::vector<std::string> &inhibitorPlaces,
						  const std::vector<std::string> &additionalConditions);

	/*!
	 * Create a new place in the net.
	 * \param name The name of the place.
	 * \param initialNumberOfTokens The number of tokens to be initialized with.
	 * \param onEnterAction The function to be called once a token enters the place.
	 * \param onExitAction The function to be called once a token leaves the place.
	 * \param input A flag determining if this place can have tokens added manually.
	 */
	void createPlace(const std::string &name,
					 const size_t initialNumberOfTokens,
					 ActionFunction onEnterAction,
					 ActionFunction onExitAction,
					 const bool input = false);

	/*!
	 * Create a new place in the net.
	 * \param name The name of the place.
	 * \param initialNumberOfTokens The number of tokens to be initialized with.
	 * \param onEnterAction Name of the function to be called once a token enters the place.
	 * \param onExitAction Name of the function to be called once a token leaves the place.
	 * \param input A flag determining if this place can have tokens added manually.
	 */
	void createPlaceStr(const std::string &name,
						const size_t initialNumberOfTokens,
						const std::string &onEnterAction,
						const std::string &onExitAction,
						const bool input = false);

	/*!
	 * Register an action to be called by the Petri net.
	 * \param name The name of the place.
	 * \param action The function to be called once a token enters the place.
	 */
	void registerAction(const std::string &name, ActionFunction action);

	/*!
	 * Register a condition
	 * \param name The name of the condition
	 * \param conditions A function pointer to a condition.
	 */
	void registerCondition(const std::string &name, ConditionFunction condition);

	/*!
	 * Start the petri net event loop.
	 * \param log Flag logging the state of the net on or off.
	 * \param o Log output stream.
	 */
	void execute(const bool log = false, std::ostream &o = std::cout);

	/*!
	 * \brief Stop the execution loop.
	 */
	void stop();

	/*!
	 * Return the number of tokens in a given place.
	 * \param place The name of the place to get the number of tokens from.
	 * \return The number of tokens present in the place.
	 */
	size_t getNumberOfTokens(const std::string &place) const;

	/*!
	 * Add a token in an input place.
	 * \param place Name of the place to be incremented.
	 */
	void incrementInputPlace(const std::string &place);

	/*!
	 * Print the petri net places and number of tokens.
	 * \param o Output stream.
	 */
	void printState(std::ostream &o) const;

	/*!
	 * \brief export_
	 * \param exporter
	 */
	void export_(IExporter &exporter) const;

	/*!
	 * \brief import
	 * \param importer
	 */
	void import(const IImporter &importer);

	/*!
	 * \brief Imports the Petri net using the provided importer.
	 * \param importer Object containing all necessary information to create a new Petri net.
	 */
	bool isEventLoopRunning() const;

	/*!
	 * \brief Add job to job queue.
	 * \param Function to be executed in the job queue.
	 */
	void addJob(const ActionFunction &actionFunction);

	//! Specify the thread where the actions should be run.
	void setActionsThreadOption(const ACTIONS_THREAD_OPTION actionsThreadOption);

	//! Get the information on which thread the actions are run.
	ACTIONS_THREAD_OPTION getActionsThreadOption();

	/*!
	 * Exception to be thrown when trying to use a place with a wrong name.
	 */
	class InvalidNameException : public PTN_Exception
	{
	public:
		InvalidNameException(const std::string &name);
	};

	/*!
	 * Exception to be thrown if names of places to construct a transition are repeated.
	 */
	class RepeatedPlaceNamesException : public PTN_Exception
	{
	public:
		RepeatedPlaceNamesException();
	};

	/*!
	 * Exception to be thrown if the user tries to increment the number of tokens of a non input place.
	 */
	class NotInputPlaceException : public PTN_Exception
	{
	public:
		NotInputPlaceException(const std::string &name);
	};

	/*!
	 * Exception to be thrown if the user tries to add a Place to the net that with the name of an already
	 * existing one.
	 */
	class RepeatedPlaceException : public PTN_Exception
	{
	public:
		RepeatedPlaceException(const std::string &name);
	};

	/*!
	 * Exception to be thrown if the user tries to add a Condition to the net that with the name of an already
	 * existing one.
	 */
	class RepeatedActionException : public PTN_Exception
	{
	public:
		RepeatedActionException(const std::string &name);
	};

	/*!
	 * Exception to be thrown if the user tries to add an Action to the net that with the name of an already
	 * existing one.
	 */
	class RepeatedConditionException : public PTN_Exception
	{
	public:
		RepeatedConditionException(const std::string &name);
	};

	/*!
	 * Exception to be thrown if the user tries to use an invalid function name
	 */
	class InvalidFunctionNameException : public PTN_Exception
	{
	public:
		InvalidFunctionNameException(const std::string &name);
	};

private:
	static bool
	ACTIONS_THREAD_OPTION_fromString(const std::string &actionsThreadOptionStr, ACTIONS_THREAD_OPTION &out);

	static bool ACTIONS_THREAD_OPTION_toString(const ACTIONS_THREAD_OPTION actionsThreadOption, std::string &out);

	/*!
	 * Shared mutex to synchronize API calls, allowing simultaneous reads (readers-writer lock).
	 */
	mutable std::shared_timed_mutex m_mutex;

	/*!
	 * Shared mutex to synchronize the event loop thread (readers-writer lock).
	 */
	mutable std::shared_timed_mutex m_eventLoopMutex;

	/*!
	 * Clear the token counter from all input places.
	 */
	void clearInputPlaces();

	/*!
	 * Get the registered action function pointer identified by name.
	 * \param name The function name or identifier
	 * \return The functions pointer of the registered function
	 */
	ActionFunction getActionFunction(const std::string &name) const;

	/*!
	 * Get the registered condition function pointers identified by
	 * their names.
	 * \param names Vector of names
	 * \return Vector of conditions
	 */
	std::vector<std::pair<std::string, ConditionFunction>>
	getConditionFunctions(const std::vector<std::string> &names) const;

	/*!
	 * Collects and randomizes the order of all enabled transitions.
	 * \return Vector of unique pointers to enabled transitions.
	 */
	std::vector<Transition *> collectEnabledTransitionsRandomly();

	/*!
	 *
	 */
	template <class PtrType, class ContainerType>
	std::string findName(const PtrType &ptr, const ContainerType &container) const
	{
		std::string name;
		auto itFound =
		find_if(container.cbegin(), container.cend(), [&](const auto &it) { return it.second == ptr; });
		if (itFound != container.cend())
		{
			name = itFound->first;
		}
		return name;
	}

	/*!
	 * \brief exportPlaces
	 * \param exporter
	 */
	void exportPlaces(IExporter &exporter) const;

	/*!
	 * \brief exportTransitions
	 * \param exporter
	 */
	void exportTransitions(IExporter &exporter) const;

	/*!
	 * \brief clearNet
	 */
	void clearNet();

	/*!
	 * Create a new place in the net.
	 * \param name The name of the place.
	 * \param initialNumberOfTokens The number of tokens to be initialized with.
	 * \param onEnterAction The function to be called once a token enters the place.
	 * \param onExitAction The function to be called once a token leaves the place.
	 * \param input A flag determining if this place can have tokens added manually.
	 */
	void createPlaceImp(const std::string &name,
						const size_t initialNumberOfTokens,
						ActionFunction onEnterAction,
						ActionFunction onExitAction,
						const bool input = false);

	/*!
	 * Create a new place in the net.
	 * \param name The name of the place.
	 * \param initialNumberOfTokens The number of tokens to be initialized with.
	 * \param onEnterAction Name of the function to be called once a token enters the place.
	 * \param onExitAction Name of the function to be called once a token leaves the place.
	 * \param input A flag determining if this place can have tokens added manually.
	 */
	void createPlaceStrImp(const std::string &name,
						   const size_t initialNumberOfTokens,
						   const std::string &onEnterAction,
						   const std::string &onExitAction,
						   const bool input = false);

	/*!
	 * Create a new transition
	 * \param activationPlaces A vector with the names of the activation places.
	 * \param activationWeights A vector with the weights of each activation place.
	 * \param destinationPlaces A vector with the names of the destination places.
	 * \param destinationWeights A vector with the weights of each destination place.
	 * \param inhibitorPlaces Places that cannot have tokens to fire the transition.
	 * \param additionalConditions A vector with a pairs of a name and a function that returns bool.
	 */
	void createTransitionImp(const std::vector<std::string> &activationPlaces,
							 const std::vector<size_t> &activationWeights,
							 const std::vector<std::string> &destinationPlaces,
							 const std::vector<size_t> &destinationWeights,
							 const std::vector<std::string> &inhibitorPlaces,
							 const std::vector<std::pair<std::string, ConditionFunction>> &additionalConditions);

	/*!
	 * Create a new transition
	 * \param activationPlaces A vector with the names of the activation places.
	 * \param activationWeights A vector with the weights of each activation place.
	 * \param destinationPlaces A vector with the names of the destination places.
	 * \param destinationWeights A vector with the weights of each destination place.
	 * \param inhibitorPlaces Places that cannot have tokens to fire the transition.
	 * \param additionalConditions A vector with names to additional conditions.
	 */
	void createTransitionImp(const std::vector<std::string> &activationPlaces,
							 const std::vector<size_t> &activationWeights,
							 const std::vector<std::string> &destinationPlaces,
							 const std::vector<size_t> &destinationWeights,
							 const std::vector<std::string> &inhibitorPlaces,
							 const std::vector<std::string> &additionalConditions);


	/*!
	 * \brief createAnonymousConditions
	 * \param conditions
	 * \return
	 */
	const std::vector<std::pair<std::string, ConditionFunction>>
	createAnonymousConditions(const std::vector<ConditionFunction> &conditions) const;

	//! Reference to parent containing this implementation.
	PTN_Engine &m_ptnEngine;

	//! Vector with the transitions.
	/*!
	 * Insertions on construction. Otherwise (should remain) unchanged.
	 */
	std::vector<Transition> m_transitions;

	//! Vector with the input places.
	/*!
	 * Insertions on construction. Otherwise (should remain) unchanged.
	 */
	std::vector<WeakPtrPlace> m_inputPlaces;

	//! Map of places.
	/*!
	 * Insertion is only performed on construction and there are no removals during the lifetime of the net.
	 * On the other hand many access operations are expected.
	 */
	std::unordered_map<std::string, SharedPtrPlace> m_places;

	//! Actions that can be called by the Petri net.
	std::unordered_map<std::string, ActionFunction> m_actions;

	//! Conditions that can be used by the Petri net.
	std::unordered_map<std::string, ConditionFunction> m_conditions;

	//! Translates a vector of names of places to a vector of weak pointers to those places.
	/*!
	 * \param names Names of the places.
	 * \return Vector of weakt_ptr for each place given in "names".
	 */
	std::vector<WeakPtrPlace> getPlacesFromNames(const std::vector<std::string> &names) const;

	//! Flag to stop the execution of the net.
	std::atomic<bool> m_stop;

	//! Event loop thread.
	std::unique_ptr<std::thread> m_eventLoopThread;

	//! Mutex to synchronize the event notifier condition variable m_eventNotifier.
	std::mutex m_eventNotifierMutex;

	//! Condition variable to wake up the event loop thread when some event happens.
	std::condition_variable m_eventNotifier;

	//! Flag reporting a new input event.
	bool m_newInputReceived = false;

	//! Determines how the actions will be executed.
	ACTIONS_THREAD_OPTION m_actionsThreadOption;

	//! Job queue to dispatch actions.
	std::unique_ptr<JobQueue> m_jobQueue;
};

} // namespace ptne
