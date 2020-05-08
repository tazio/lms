/*
 * Copyright (C) 2015 Emeric Poupon
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

#include <optional>
#include <string>
#include <vector>

#include <Wt/WDateTime.h>
#include <Wt/Dbo/Dbo.h>

#include "utils/UUID.hpp"

#include "TrackArtistLink.hpp"
#include "Types.hpp"

namespace Database
{

class Cluster;
class ClusterType;
class Release;
class Session;
class Track;
class User;

class Artist : public Wt::Dbo::Dbo<Artist>
{
	public:

		enum class SortMethod
		{
			None,
			ByName,
			BySortName,
		};

		using pointer = Wt::Dbo::ptr<Artist>;

		Artist() {}
		Artist(const std::string& name, const std::optional<UUID>& MBID = {});

		// Accessors
		static pointer			getByMBID(Session& session, const UUID& MBID);
		static pointer			getById(Session& session, IdType id);
		static std::vector<pointer>	getByName(Session& session, const std::string& name);		// exact match on name field
		static std::vector<pointer> 	getByClusters(Session& session,
								const std::set<IdType>& clusters,		// at least one track that belongs to  these clusters
								SortMethod sortMethod
								);
		static std::vector<pointer> 	getByFilter(Session& session,
								const std::set<IdType>& clusters,		// if non empty, at least one artist that belongs to these clusters
								const std::vector<std::string>& keywords,	// if non empty, name must match all of these keywords (name + sort name fields)
								std::optional<TrackArtistLink::Type> linkType, 	// if set, only artists that have produced at least one track with this link type
								SortMethod sortMethod,
								std::optional<Range> range,
								bool& moreExpected);

		static std::vector<pointer>	getAll(Session& session);
		static std::vector<pointer>	getAll(Session& session, SortMethod sortMethod);
		static std::vector<pointer>	getAll(Session& session, SortMethod sortMethod, std::optional<Range> range, bool& moreResults);
		static std::vector<IdType>	getAllIds(Session& session);
		static std::vector<IdType>	getAllIdsRandom(Session& session, const std::set<IdType>& clusters, std::optional<TrackArtistLink::Type> linkType, std::optional<std::size_t> size = {});
		static std::vector<pointer>	getAllOrphans(Session& session); // No track related
		static std::vector<pointer>	getLastWritten(Session& session,
								std::optional<Wt::WDateTime> after,
								const std::set<IdType>& clusters,
								std::optional<TrackArtistLink::Type> linkType, 	// if set, only artists that have produced at least one track with this link type
								std::optional<Range>,
								bool& moreResults);
		static std::vector<IdType>	getAllIdsWithClusters(Session& session, std::optional<std::size_t> limit = {});

		// Accessors
		const std::string&	getName() const { return _name; }
		const std::string&	getSortName() const { return _sortName; }
		std::optional<UUID>	getMBID() const { return UUID::fromString(_MBID); }

		std::vector<Wt::Dbo::ptr<Release>>	getReleases(const std::set<IdType>& clusterIds = {}) const; // if non empty, get the releases that match all these clusters
		std::size_t				getReleaseCount() const;
		std::vector<Wt::Dbo::ptr<Track>>	getTracks(std::optional<TrackArtistLink::Type> linkType = {}) const;
		std::vector<Wt::Dbo::ptr<Track>>	getTracksWithRelease(std::optional<TrackArtistLink::Type> linkType = {}) const;
		std::vector<Wt::Dbo::ptr<Track>>	getRandomTracks(std::optional<std::size_t> count) const;
		std::vector<pointer>			getSimilarArtists(std::optional<std::size_t> offset = {}, std::optional<std::size_t> count = {}) const;

		// Get the cluster of the tracks made by this artist
		// Each clusters are grouped by cluster type, sorted by the number of occurence
		// size is the max number of cluster per cluster type
		std::vector<std::vector<Wt::Dbo::ptr<Cluster>>> getClusterGroups(std::vector<Wt::Dbo::ptr<ClusterType>> clusterTypes, std::size_t size) const;

		void setName(std::string_view name)		{ _name  = name; }
		void setMBID(const std::optional<UUID>& mbid)	{ _MBID = mbid ? mbid->getAsString() : ""; }
		void setSortName(const std::string& sortName);

		// Create
		static pointer create(Session& session, const std::string& name, const std::optional<UUID>& UUID = {});

		template<class Action>
			void persist(Action& a)
			{
				Wt::Dbo::field(a, _name, "name");
				Wt::Dbo::field(a, _sortName, "sort_name");
				Wt::Dbo::field(a, _MBID, "mbid");

				Wt::Dbo::hasMany(a, _trackArtistLinks, Wt::Dbo::ManyToOne, "artist");
				Wt::Dbo::hasMany(a, _starringUsers, Wt::Dbo::ManyToMany, "user_release_starred", "", Wt::Dbo::OnDeleteCascade);
			}

	private:

		static const std::size_t _maxNameLength = 128;

		std::string _name;
		std::string _sortName;
		std::string _MBID;	// Musicbrainz Identifier

		Wt::Dbo::collection<Wt::Dbo::ptr<TrackArtistLink>> _trackArtistLinks; // Tracks involving this artist
		Wt::Dbo::collection<Wt::Dbo::ptr<User>>		_starringUsers; // Users that starred this artist
};

} // namespace Database

