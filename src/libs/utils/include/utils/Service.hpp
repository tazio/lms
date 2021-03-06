/*
 * Copyright (C) 2019 Emeric Poupon
 *
 * This file is part of LMS.
 *
 * LMS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LMS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LMS.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <memory>
#include <type_traits>

template <typename Class>
class ServiceProvider
{
	public:
		template <class DerivedClass, class ...Args>
		static
		Class&
		create(Args&&... args)
		{
			static_assert(std::is_base_of<Class, DerivedClass>::value);

			assign(std::make_unique<DerivedClass>(std::forward<Args>(args)...));
			return *get();
		}

		template <class ...Args>
		static
		Class&
		create(Args&&... args)
		{
			assign(std::make_unique<Class>(std::forward<Args>(args)...));
			return *get();
		}

		static
		Class&
		assign(std::unique_ptr<Class> service)
		{
			_service = std::move(service);
			return *get();
		}

		static void clear() { _service.reset(); }

		static Class* get() { return _service.get(); }

	private:
		static inline std::unique_ptr<Class> _service;
};

