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

#pragma once

//! View the main menu on screen.
class MainMenuView
{
public:
	~MainMenuView() = default;
	MainMenuView() = default;
	MainMenuView(const MainMenuView &) = delete;
	MainMenuView(MainMenuView &&) = delete;
	MainMenuView &operator=(const MainMenuView &) = delete;
	MainMenuView &operator=(MainMenuView &&) = delete;

	//! Print the menu on screen.
	void showMenu() const;

	//!
	//! Select an option.
	//! \param option Number of the option.
	//!
	void select(const int option);

private:
	//! Selected option.
	int m_selection = 0;
};
