/*
 * This file is part of PTN Engine
 *
 * Copyright (c) 2019-2024 Eduardo Valgôde
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
#include "PTN_Engine/PTN_Engine.h"
#include <vector>

namespace ptne
{
class PTN_Engine;

//!
//! \brief The IExporter class
//!
class DLL_PUBLIC IFileExporter
{
public:
	virtual ~IFileExporter() = default;

	//!
	//! \brief _export
	//! \param ptnEngine
	//!
	virtual void _export(const PTN_Engine &ptnEngine, const std::string &filePath) = 0;

protected:
	void _export(const PTN_Engine &ptnEngine);

private:
	//!
	//! \brief exportActionsThreadOption
	//! \param actionsThreadOption
	//!
	virtual void exportActionsThreadOption(const std::string &actionsThreadOption) = 0;

	//!
	//! \brief exportPlace
	//! \param placeProperties
	//!
	virtual void exportPlace(const PlaceProperties &placeProperties) = 0;

	//!
	//!\brief exportTransition
	//!\param transitionPropereties
	//!
	virtual void exportTransition(const TransitionProperties &transitionPropereties) = 0;

	//!
	//! \brief exportArc
	//! \param arcPropereties
	//!
	virtual void exportArc(const std::vector<ArcProperties> &arcPropereties) = 0;
};

} // namespace ptne
