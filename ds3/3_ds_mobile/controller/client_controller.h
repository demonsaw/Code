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

#ifndef _EJA_CLIENT_CONTROLLER_H_
#define _EJA_CLIENT_CONTROLLER_H_

#include <memory>

#include <QColor>
//#include <QSortFilterProxyModel>
#include <QString>
#include <QVector4D>

#include "entity/entity_object.h"

namespace eja
{
	class entity;
	class entity_list;
	class client_model;

	class client_controller final : public entity_object
	{
		Q_OBJECT;

		Q_PROPERTY(size_t size READ get_size NOTIFY sizeChanged);
		Q_PROPERTY(size_t max READ get_max NOTIFY maxChanged);

		Q_PROPERTY(QString name READ get_name WRITE set_name NOTIFY nameChanged); 
		Q_PROPERTY(QColor color READ get_color WRITE set_color NOTIFY colorChanged);
		Q_PROPERTY(QColor text_color READ get_text_color NOTIFY text_colorChanged);
		Q_PROPERTY(QColor status READ get_status NOTIFY statusChanged);

		Q_PROPERTY(bool flash READ has_flash WRITE set_flash NOTIFY flashChanged);

	private:
		bool m_enabled = true;
		client_model* m_model = nullptr;
		//QSortFilterProxyModel* m_proxy = nullptr;

	public:
		client_controller(entity_list& list, QObject* parent = nullptr);

		// Interface
		Q_INVOKABLE void add();
		Q_INVOKABLE void remove(const int row);
		Q_INVOKABLE void clear();

		Q_INVOKABLE void update();
		Q_INVOKABLE void restart();
		Q_INVOKABLE void reset();

		// Utility
		Q_INVOKABLE bool enabled() const { return m_enabled; }
		Q_INVOKABLE bool empty() const;
		Q_INVOKABLE void sort();

		// Has
		bool has_flash() const;

		// Set
		virtual void set_entity(const std::shared_ptr<entity> entity) override;

		void set_id(const QString& id);
		void set_name(const QString& name);
		void set_color(const QColor& color);
		void set_flash(const bool flash);

		// Get		
		const client_model* get_model() const { return m_model; }
		client_model* get_model() { return m_model; }

		//Q_INVOKABLE const QObject* get_proxy() const { return m_proxy; }
		//Q_INVOKABLE QObject* get_proxy() { return m_proxy; }

		size_t get_size() const;
		size_t get_max() const { return 3; }

		QString get_id() const;
		QString get_name() const;
		QColor get_color() const;
		QColor get_text_color() const;
		Q_INVOKABLE QVector4D get_color_hsva() const;
		QColor get_status() const;

	signals:
		void sizeChanged();
		void maxChanged();

		void nameChanged();
		void colorChanged();
		void text_colorChanged();
		void statusChanged();
		void flashChanged();
	};
}

#endif
