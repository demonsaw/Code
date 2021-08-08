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

#ifndef _EJA_ROOM_CONTROLLER_H_
#define _EJA_ROOM_CONTROLLER_H_

#include <memory>

#include "entity/entity_controller.h"
#include "thread/thread_safe.h"

namespace eja
{
	class entity;

	class room_controller final : public entity_controller
	{
	private:
		// Interface
		virtual void on_create(const std::shared_ptr<entity> entity) override;
		virtual void on_destroy(const std::shared_ptr<entity> entity) override;
		virtual void on_destroy() override;

		virtual void on_add(const std::shared_ptr<entity> entity) override;
		virtual void on_update(const std::shared_ptr<entity> entity) override;
		virtual void on_update() override;

		virtual void on_clear(const std::shared_ptr<entity> entity) override;
		virtual void on_clear() override;

	public:
		room_controller(const std::shared_ptr<entity>& entity, QObject* parent = nullptr);

		// Get
		virtual const std::string& get_id(const std::shared_ptr<entity>& entity) const override;
	};
}

#endif
