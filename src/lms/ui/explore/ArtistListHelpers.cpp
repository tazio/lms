/*
 * Copyright (C) 2020 Emeric Poupon
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
#include "ArtistListHelpers.hpp"

#include <Wt/WAnchor.h>

#include "database/Artist.hpp"
#include "LmsApplication.hpp"

namespace UserInterface::ArtistListHelpers
{
	std::unique_ptr<Wt::WTemplate>
	createEntry(const Wt::Dbo::ptr<Database::Artist>& artist)
	{
		auto res {std::make_unique<Wt::WTemplate>(Wt::WString::tr("Lms.Explore.Artists.template.entry"))};
		res->bindWidget("name", LmsApplication::createArtistAnchor(artist));

		return res;
	}

	std::unique_ptr<Wt::WTemplate>
	createEntrySmall(const Wt::Dbo::ptr<Database::Artist>& artist)
	{
		auto res {std::make_unique<Wt::WTemplate>(Wt::WString::tr("Lms.Explore.Artists.template.entry-small"))};
		res->bindWidget("name", LmsApplication::createArtistAnchor(artist));

		return res;
	}

}

