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

#include "component/room_component.h"
#include "component/client/client_component.h"

#include "entity/entity.h"
#include "model/proxy/client_proxy_model.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Utility
	bool client_proxy_model::lessThan(const QModelIndex& lindex, const QModelIndex& rindex) const
	{
		// Entity
		const auto model = sourceModel();
		QVariant ldata = model->data(lindex);
		const auto lentity = ldata.value<entity::ptr>();
		if (!lentity)
			return true;

		QVariant rdata = model->data(rindex);
		const auto rentity = rdata.value<entity::ptr>();
		if (!rentity)
			return true;

		// Component
		const auto sorted = (sortOrder() == Qt::AscendingOrder);

		if (lentity->has<room_component>())
		{
			if (rentity->has<room_component>())
				return less_than_name<room_component>(lentity, rentity);

			return sorted;
		}
		else //if (lentity->has<client_component>())
		{
			if (rentity->has<client_component>())
			{
				const auto lclient = lentity->get<client_component>();
				const auto rclient = rentity->get<client_component>();

				// Verified
				if (lclient->is_verified())
				{
					if (rclient->is_verified())
						return less_than_name<client_component>(lentity, rentity);

					return sorted;
				}
				else if (rclient->is_verified())
				{
					return !sorted;
				}

				return less_than_name<client_component>(lentity, rentity);
			}

			return false;
		}

		return false;
	}
}
