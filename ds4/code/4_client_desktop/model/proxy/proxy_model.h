//
// The MIT License(MIT)
//
// Copyright(c) 2014 Demonsaw LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef _EJA_PROXY_MODEL_H_
#define _EJA_PROXY_MODEL_H_

#include <algorithm>
#include <memory>
#include <boost/algorithm/string.hpp>

#include <QSortFilterProxyModel>

#include "entity/entity.h"

namespace eja
{
	class archive_component;

	class proxy_model : public QSortFilterProxyModel
	{
		Q_OBJECT;

	protected:
		// Utility
		template <typename T>
		bool less_than_ext(const std::shared_ptr<entity>& lentity, const std::shared_ptr<entity>& rentity) const;

		template <typename T>
		bool less_than_name(const std::shared_ptr<entity>& lentity, const std::shared_ptr<entity>& rentity) const;

		template <typename T>
		bool less_than_path(const std::shared_ptr<entity>& lentity, const std::shared_ptr<entity>& rentity) const;

		template <typename T>
		bool less_than_size(const std::shared_ptr<entity>& lentity, const std::shared_ptr<entity>& rentity) const;

		template <typename T>
		bool less_than_time(const std::shared_ptr<entity>& lentity, const std::shared_ptr<entity>& rentity) const;

		// Get
		std::shared_ptr<archive_component> get_archive(const std::shared_ptr<entity>& entity) const;

	public:
		explicit proxy_model(QObject* parent = nullptr) : QSortFilterProxyModel(parent) { }
	};

	// Utility
	template <typename T>
	bool proxy_model::less_than_ext(const std::shared_ptr<entity>& lentity, const std::shared_ptr<entity>& rentity) const
	{
		const auto lcomp = lentity->get<T>();
		const auto rcomp = rentity->get<T>();
#if _WSTRING
		const auto& lextension = lcomp->get_wextension();
		const auto& rextension = rcomp->get_wextension();

		return std::lexicographical_compare(lextension.begin(), lextension.end(), rextension.begin(), rextension.end(), boost::is_iless());
#else
		const auto& lextension = lcomp->get_extension();
		const auto& rextension = rcomp->get_extension();

		return std::lexicographical_compare(lextension.begin(), lextension.end(), rextension.begin(), rextension.end(), boost::is_iless());
#endif
	}

	template <typename T>
	bool proxy_model::less_than_name(const std::shared_ptr<entity>& lentity, const std::shared_ptr<entity>& rentity) const
	{
		const auto lcomp = lentity->get<T>();
		const auto rcomp = rentity->get<T>();

		const auto& lname = lcomp->get_name();
		const auto& rname = rcomp->get_name();

		return std::lexicographical_compare(lname.begin(), lname.end(), rname.begin(), rname.end(), boost::is_iless());
	}

	template <typename T>
	bool proxy_model::less_than_path(const std::shared_ptr<entity>& lentity, const std::shared_ptr<entity>& rentity) const
	{
		const auto lcomp = lentity->get<T>();
		const auto rcomp = rentity->get<T>();
#if _WSTRING
		const auto& lname = lcomp->get_wname();
		const auto& rname = rcomp->get_wname();

		return std::lexicographical_compare(lname.begin(), lname.end(), rname.begin(), rname.end(), boost::is_iless());
#else
		const auto& lname = lcomp->get_name();
		const auto& rname = rcomp->get_name();

		return std::lexicographical_compare(lname.begin(), lname.end(), rname.begin(), rname.end(), boost::is_iless());
#endif
	}

	template <typename T>
	bool proxy_model::less_than_size(const std::shared_ptr<entity>& lentity, const std::shared_ptr<entity>& rentity) const
	{
		const auto lcomp = lentity->get<T>();
		const auto rcomp = rentity->get<T>();

		return lcomp->get_size() < rcomp->get_size();
	}

	template <typename T>
	bool proxy_model::less_than_time(const std::shared_ptr<entity>& lentity, const std::shared_ptr<entity>& rentity) const
	{
		const auto lcomp = lentity->get<T>();
		const auto rcomp = rentity->get<T>();

		return lcomp->get_time() < rcomp->get_time();
	}
}

#endif
