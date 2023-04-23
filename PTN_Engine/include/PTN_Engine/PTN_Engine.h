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

#pragma once

#include "PTN_Engine/Utilities/Explicit.h"
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace ptne
{
class IExporter;
class IImporter;

using ConditionFunction = std::function<bool(void)>;
using ActionFunction = std::function<void(void)>;

//! Base class that implements the Petri net logic.
/*!
 * Base class that implements the Petri net logic.
 * Using the pimpl idiom.
 * \sa PTN_EngineImp
 */
class DLL_PUBLIC PTN_Engine
{
public:
	enum class ACTIONS_THREAD_OPTION
	{
		SINGLE_THREAD,
		EVENT_LOOP,
		DETACHED,
		JOB_QUEUE
	};

	using EventLoopSleepDuration = std::chrono::duration<long, std::ratio<1, 1000>>;

	virtual ~PTN_Engine();

	PTN_Engine(const PTN_Engine &) = delete;
	PTN_Engine(PTN_Engine &&) = delete;
	PTN_Engine &operator=(const PTN_Engine &) = delete;
	PTN_Engine &operator=(PTN_Engine &&) = delete;

	/*!
	 * Create a new transition
	 * \param activationPlaces A vector with the names of the activation places.
	 * \param activationWeights A vector with the weights of each activation place.
	 * \param destinationPlaces A vector with the names of the destination places.
	 * \param destinationWeights A vector with the weights of each destination place.
	 * \param inhibitorPlaces Places that cannot have tokens to fire the transition.
	 * \param additionalConditions A vector with functors that return bool.
	 */
	void createTransition(const std::vector<std::string> &activationPlaces,
						  const std::vector<size_t> &activationWeights,
						  const std::vector<std::string> &destinationPlaces,
						  const std::vector<size_t> &destinationWeights,
						  const std::vector<std::string> &inhibitorPlaces,
						  const std::vector<ConditionFunction> &additionalConditions,
	                      const bool requireNoActionsInExecution = false);

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
						  const std::vector<std::string> &additionalConditions,
						  const bool requireNoActionsInExecution = false);

	/*!
	 * Create a new transition
	 * \param activationPlaces A vector with the names of the activation places.
	 * \param activationWeights A vector with the weights of each activation place.
	 * \param destinationPlaces A vector with the names of the destination places.
	 * \param destinationWeights A vector with the weights of each destination place.
	 */
	void createTransition(const std::vector<std::string> &activationPlaces,
						  const std::vector<size_t> &activationWeights,
						  const std::vector<std::string> &destinationPlaces,
						  const std::vector<size_t> &destinationWeights,
	                      const bool requireNoActionsInExecution = false);

	/*!
	 * Create a new transition
	 * \param activationPlaces A vector with the names of the activation places.
	 * \param activationWeights A vector with the weights of each activation place.
	 * \param destinationPlaces A vector with the names of the destination places.
	 * \param destinationWeights A vector with the weights of each destination place.
	 * \param inhibitorPlaces Places that cannot have tokens to fire the transition.
	 */
	void createTransition(const std::vector<std::string> &activationPlaces,
						  const std::vector<size_t> &activationWeights,
						  const std::vector<std::string> &destinationPlaces,
						  const std::vector<size_t> &destinationWeights,
						  const std::vector<std::string> &inhibitorPlaces,
	                      const bool requireNoActionsInExecution = false);

	/*!
	 * Create a new transition
	 * \param activationPlaces A vector with the names of the activation places.
	 * \param activationWeights A vector with the weights of each activation place.
	 * \param destinationPlaces A vector with the names of the destination places.
	 * \param destinationWeights A vector with the weights of each destination place.
	 * \param additionalConditions A vector with functors that return bool.
	 */
	void createTransition(const std::vector<std::string> &activationPlaces,
						  const std::vector<size_t> &activationWeights,
						  const std::vector<std::string> &destinationPlaces,
						  const std::vector<size_t> &destinationWeights,
						  const std::vector<ConditionFunction> &additionalConditions,
						  const bool requireNoActionsInExecution = false);

	/*!
	 * Create a new transition
	 * \param activationPlaces A vector with the names of the activation places.
	 * \param destinationPlaces A vector with the names of the destination places.
	 */
	void createTransition(const std::vector<std::string> &activationPlaces,
						  const std::vector<std::string> &destinationPlaces,
	                      const bool requireNoActionsInExecution = false);

	/*!
	 * Create a new transition
	 * \param activationPlaces A vector with the names of the activation places.
	 * \param destinationPlaces A vector with the names of the destination places.
	 * \param inhibitorPlaces Places that cannot have tokens to fire the transition.
	 */
	void createTransition(const std::vector<std::string> &activationPlaces,
						  const std::vector<std::string> &destinationPlaces,
						  const std::vector<std::string> &inhibitorPlaces,
	                      const bool requireNoActionsInExecution = false);

	/*!
	 * Create a new transition
	 * \param activationPlaces A vector with the names of the activation places.
	 * \param destinationPlaces A vector with the names of the destination places.
	 * \param additionalConditions A vector with functors that return bool.
	 */
	void createTransition(const std::vector<std::string> &activationPlaces,
						  const std::vector<std::string> &destinationPlaces,
						  const std::vector<ConditionFunction> &additionalConditions,
	                      const bool requireNoActionsInExecution = false);

	/*!
	 * Create a new transition
	 * \param activationPlaces A vector with the names of the activation places.
	 * \param destinationPlaces A vector with the names of the destination places.
	 * \param inhibitorPlaces Places that cannot have tokens to fire the transition.
	 * \param additionalConditions A vector with functors that return bool.
	 */
	void createTransition(const std::vector<std::string> &activationPlaces,
						  const std::vector<std::string> &destinationPlaces,
						  const std::vector<std::string> &inhibitorPlaces,
						  const std::vector<ConditionFunction> &additionalConditions,
	                      const bool requireNoActionsInExecution = false);

	/*!
	 * Create a new transition
	 * \param activationPlaces A vector with the names of the activation places.
	 * \param destinationPlaces A vector with the names of the destination places.
	 * \param inhibitorPlaces Places that cannot have tokens to fire the transition.
	 * \param additionalConditions A vector with names to additional conditions.
	 */
	void createTransition(const std::vector<std::string> &activationPlaces,
						  const std::vector<std::string> &destinationPlaces,
						  const std::vector<std::string> &inhibitorPlaces,
						  const std::vector<std::string> &additionalConditions,
	                      const bool requireNoActionsInExecution = false);

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
	 * \param onExitAction Name of the function functor to be called once a token leaves the place.
	 * \param input A flag determining if this place can have tokens added manually.
	 */
	void createPlace(const std::string &name,
					 const size_t initialNumberOfTokens,
					 const std::string &onEnterAction,
					 const std::string &onExitAction,
					 const bool input = false);

	/*!
	 * Create a new place in the net.
	 * \param name The name of the place.
	 * \param initialNumberOfTokens The number of tokens to be initialized with.
	 * \param input A flag determining if this place can have tokens added manually.
	 */
	void createPlace(const std::string &name, const size_t initialNumberOfTokens, const bool input = false);

	/*!
	 * Create a new place in the net.
	 * \param name The name of the place.
	 * \param initialNumberOfTokens The number of tokens to be initialized with.
	 * \param onEnterAction The function to be called once a token enters the place.
	 * \param input A flag determining if this place can have tokens added manually.
	 */
	void createPlace(const std::string &name,
					 const size_t initialNumberOfTokens,
					 ActionFunction onEnterAction,
					 const bool input = false);

	/*!
	 * Create a new place in the net.
	 * \param name The name of the place.
	 * \param initialNumberOfTokens The number of tokens to be initialized with.
	 * \param onEnterAction Name of the function to be called once a token enters the place.
	 * \param input A flag determining if this place can have tokens added manually.
	 */
	void createPlace(const std::string &name,
					 const size_t initialNumberOfTokens,
					 const std::string &onEnterAction,
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
	 * \param returnWhenStopped run the net only until no transition is fireable.
	 * \param log Flag logging the state of the net on or off.
	 * \param o Log output stream.
	 */
	void execute(const bool log = false, std::ostream &o = std::cout);

	/*!
	 * Return the number of tokens in a given place.
	 * \param place The name of the place to get the number of tokens from.
	 * \return The number of tokens present in the place.
	 */
	size_t getNumberOfTokens(const std::string &place) const;

	/*!
	 * Add a token to an input place.
	 * \param place Name of the place to be incremented.
	 */
	void incrementInputPlace(const std::string &place);

	/*!
	 * Print the petri net places and number of tokens.
	 * \param o Output stream.
	 */
	void printState(std::ostream &o) const;

	/*!
	 * \brief Exports the Petri net using the provided exporter.
	 * \param exporter Object capable of exporting all important information about the Petri net's structure to
	 * some other format.
	 */
	void export_(IExporter &exporter) const;

	/*!
	 * \brief Imports the Petri net using the provided importer.
	 * \param importer Object containing all necessary information to create a new Petri net.
	 */
	void import(const IImporter &importer);

	//! Constructor
	explicit PTN_Engine(ACTIONS_THREAD_OPTION actionsRuntimeThread = ACTIONS_THREAD_OPTION::JOB_QUEUE);

	//! Specify the thread where the actions should be run.
	void setActionsThreadOption(const ACTIONS_THREAD_OPTION actionsThreadOption);

	//! Get the information on which thread the actions are run.
	ACTIONS_THREAD_OPTION getActionsThreadOption() const;

	/*!
	 * \brief Whether the Petri Net's event loop is running or not.
	 * \return True if the event loop is running, false otherwise.
	 */
	bool isEventLoopRunning() const;

	/*!
	 * \brief Stop the execution loop.
	 */
	void stop();

	/*!
	 * \brief setEventLoopSleepDuration
	 * \param sleepDuration
	 */
	void setEventLoopSleepDuration(const EventLoopSleepDuration sleepDuration);

	/*!
	 * \brief getEventLoopSleepDuration
	 * \return
	 */
	EventLoopSleepDuration getEventLoopSleepDuration() const;

private:
	class PTN_EngineImpProxy;

	//! Pointer to implementation of the PTN_Engine.
	std::unique_ptr<PTN_EngineImpProxy> m_impProxy;
};

} // namespace ptne
