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

#include "entity_list_model.h"

namespace eja
{
	// Interface
	void entity_list_model::refresh()
	{
		const auto start = createIndex(0, 0);
		const auto end = createIndex(rowCount(), columnCount() - 1);
		emit dataChanged(start, end);
	}

	void entity_list_model::refresh(const QModelIndex& index)
	{
		const auto start = createIndex(index.row(), 0);
		const auto end = createIndex(index.row(), columnCount() - 1);
		emit dataChanged(start, end);
	}

	// Interface
	void entity_list_model::on_refresh(const entity::ptr entity)
	{
		if (entity)
		{
			const auto row = get_row(entity);
			if (row == type::npos)
				return;

			const auto start = createIndex(row, 0);
			const auto end = createIndex(row, columnCount() - 1);
			emit dataChanged(start, end);
		}
		else
		{
			const auto start = createIndex(0, 0);
			const auto end = createIndex(rowCount(), columnCount() - 1);
			emit dataChanged(start, end);
		}
	}
}