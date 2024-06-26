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

#include "PTN_Engine/PTN_Exception.h"
#include <memory>
#include <unordered_map>

namespace ptne
{

template <typename T>
class ManagerBase
{
protected:
	~ManagerBase() = default;
	ManagerBase() = default;
	ManagerBase(const ManagerBase &) = delete;
	ManagerBase(ManagerBase &&) = delete;
	ManagerBase &operator=(const ManagerBase &) = delete;
	ManagerBase &operator=(const ManagerBase &&) = delete;

	//!
	//! \brief Removes all places from the container.
	//!
	void clear()
	{
		m_items.clear();
	}

	bool contains(const std::string &itemName) const
	{
		return m_items.contains(itemName);
	}

	void insert(const std::shared_ptr<T> &item)
	{
		if (item == nullptr)
		{
			throw PTN_Exception("Tried to insert nullptr item");
		}
		const auto &itemName = item->getName();
		if (itemName.empty())
		{
			throw PTN_Exception("Empty item names are not supported.");
		}
		else if (m_items.contains(itemName))
		{
			throw RepeatedPlaceException(itemName);
		}
		m_items[itemName] = item;
	}

	std::shared_ptr<T> getItem(const std::string &itemName) const
	{
		if (!m_items.contains(itemName))
		{
			throw PTN_Exception("Cannot get:" + itemName);
		}
		return m_items.at(itemName);
	}

	std::unordered_map<std::string, std::shared_ptr<T>> m_items;
};

} // namespace ptne
