/*
 * Copyright (C) 2018 Emeric Poupon
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

#include <Wt/WApplication>
#include <Wt/WAnchor>
#include <Wt/WLineEdit>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WTemplate>

#include "database/Types.hpp"

#include "utils/Logger.hpp"
#include "utils/Utils.hpp"

#include "LmsApplication.hpp"
#include "ReleasesView.hpp"

namespace UserInterface {

using namespace Database;

Releases::Releases(Wt::WContainerWidget* parent)
: Wt::WContainerWidget(parent)
{
	auto releases = new Wt::WTemplate(Wt::WString::tr("template-releases"), this);
	releases->addFunction("tr", &Wt::WTemplate::Functions::tr);

	auto search = new Wt::WLineEdit();
	releases->bindWidget("search", search);
	search->setPlaceholderText(Wt::WString::tr("msg-search-placeholder"));
	search->textInput().connect(std::bind([this, search]
	{
		auto keywords = splitString(search->text().toUTF8(), " ");
		refresh(keywords);
	}));

	_releasesContainer = new Wt::WContainerWidget();
	releases->bindWidget("releases", _releasesContainer);

	refresh();
}


void
Releases::refresh(std::vector<std::string> searchKeywords)
{
	_releasesContainer->clear();

	std::vector<Cluster::id_type> clusterIds; // TODO fill

	Wt::Dbo::Transaction transaction(DboSession());

	bool moreResults;
	auto releases = Release::getByFilter(DboSession(), clusterIds, searchKeywords, 0, 40, moreResults);

	for (auto release : releases)
	{
		Wt::WTemplate* entry = new Wt::WTemplate(Wt::WString::tr("template-releases-entry"), _releasesContainer);
		entry->addFunction("tr", Wt::WTemplate::Functions::tr);

		Wt::WAnchor *coverAnchor = new Wt::WAnchor(Wt::WLink(Wt::WLink::InternalPath, "/release/" + std::to_string(release.id())));
		Wt::WImage *cover = new Wt::WImage(coverAnchor);
		cover->setImageLink(SessionImageResource()->getReleaseUrl(release.id(), 128));
		// Some images may not be square
		cover->setWidth(128);
		entry->bindWidget("cover", coverAnchor);

		entry->bindString("release-name", Wt::WString::fromUTF8(release->getName()), Wt::PlainText);

		auto artists = release->getArtists();
		if (artists.size() > 1)
		{
			entry->bindString("artist-name", Wt::WString::tr("msg-various-artists"));
		}
		else
		{
			Wt::WAnchor *artistAnchor = new Wt::WAnchor(Wt::WLink(Wt::WLink::InternalPath, "/artist/" + std::to_string(artists.front().id())));
			Wt::WText *artist = new Wt::WText(artistAnchor);
			artist->setText(Wt::WString::fromUTF8(artists.front()->getName(), Wt::PlainText));
			entry->bindWidget("artist-name", artistAnchor);
		}
	}
}

} // namespace UserInterface

