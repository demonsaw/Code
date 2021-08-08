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

#include "component/io/file_component.h"
#include "component/io/folder_component.h"

#include "entity/entity.h"
#include "model/share_model.h"
#include "model/proxy/share_proxy_model.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Utility
	bool share_proxy_model::lessThan(const QModelIndex& lindex, const QModelIndex& rindex) const
	{
		// Entity
		const auto model = sourceModel();
		const auto lvariant = model->data(lindex, Qt::UserRole);
		const auto lentity = lvariant.value<entity::ptr>();
		if (!lentity)
			return true;

		const auto rvariant = model->data(rindex, Qt::UserRole);
		const auto rentity = rvariant.value<entity::ptr>();
		if (!rentity)
			return true;

		// Sort
		const auto sorted = (sortOrder() == Qt::AscendingOrder);

		switch (lindex.column())
		{
			case share_model::column::name:
			{
				if (lentity->has<file_component>())
				{
					if (rentity->has<file_component>())
						return less_than_path<file_component>(lentity, rentity);

					return false;
				}
				else //if (lentity->has<folder_component>())
				{
					if (rentity->has<folder_component>())
					{
						const auto lfolder = lentity->get<folder_component>();
						const auto rfolder = rentity->get<folder_component>();

						if (lfolder->is_drive())
						{
							if (rfolder->is_drive())
								return less_than_path<folder_component>(lentity, rentity);

							return sorted;
						}
						if (rfolder->is_drive())
						{
							return !sorted;
						}

						return less_than_name<folder_component>(lentity, rentity);
					}

					return true;
				}

				// break;
			}
			case share_model::column::type:
			{
				// Type
				if (lentity->has<file_component>())
				{
					if (rentity->has<file_component>())
						return less_than_ext<file_component>(lentity, rentity);

					return false;
				}
				else //if (lentity->has<folder_component>())
				{
					if (rentity->has<folder_component>())
					{
						const auto lfolder = lentity->get<folder_component>();
						const auto rfolder = rentity->get<folder_component>();

						if (lfolder->is_drive())
						{
							if (rfolder->is_drive())
								return less_than_path<folder_component>(lentity, rentity);

							return true;
						}

						return false;
					}

					return true;
				}

				// break;
			}
			case share_model::column::size:
			{
				// Size
				const auto lcomp = get_archive(lentity);
				const auto rcomp = get_archive(rentity);

				return lcomp->get_size() < rcomp->get_size();
			}
			case share_model::column::time:
			{
				const auto lcomp = get_archive(lentity);
				const auto rcomp = get_archive(rentity);

				return lcomp->get_time() < rcomp->get_time();
			}
		}

		return false;
	}
}
