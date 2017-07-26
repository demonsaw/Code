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

#ifndef _EJA_ROUTER_CONTROLLER_H_
#define _EJA_ROUTER_CONTROLLER_H_

#include <QColor>
#include <QString>

#include "entity/entity_object.h"

namespace eja
{
	class router_controller final : public entity_object
	{
		Q_OBJECT;

		// Router
		Q_PROPERTY(QString address READ get_address WRITE set_address NOTIFY addressChanged);
		Q_PROPERTY(u16 port READ get_port WRITE set_port NOTIFY portChanged);

		// Session
		Q_PROPERTY(size_t prime_size READ get_prime_size WRITE set_prime_size NOTIFY prime_sizeChanged);
		Q_PROPERTY(QString cipher READ get_cipher WRITE set_cipher NOTIFY cipherChanged);
		Q_PROPERTY(size_t key_size READ get_key_size WRITE set_key_size NOTIFY key_sizeChanged);
		Q_PROPERTY(QString hash READ get_hash WRITE set_hash NOTIFY hashChanged);
		Q_PROPERTY(size_t iterations READ get_iterations WRITE set_iterations NOTIFY iterationsChanged);
		Q_PROPERTY(QString salt READ get_salt WRITE set_salt NOTIFY saltChanged);

	private:
		// Get
		std::shared_ptr<entity> get_router() const;

	public:
		router_controller(QObject* parent = nullptr) : entity_object(parent) { }

		// Set
		void set_address(const QString& address);
		void set_port(const u16 port);

		void set_prime_size(const size_t size);
		void set_cipher(const QString& cipher);
		void set_key_size(const size_t key_size);
		void set_hash(const QString& hash);
		void set_iterations(const size_t iterations);
		void set_salt(const QString& salt);

		// Get
		QString get_address() const;
		u16 get_port() const;

		size_t get_prime_size() const;
		QString get_cipher() const;
		size_t get_key_size() const;
		QString get_hash() const;
		size_t get_iterations() const;
		QString get_salt() const;

	signals:
		void addressChanged();
		void portChanged();

		void prime_sizeChanged();
		void cipherChanged();
		void key_sizeChanged();
		void hashChanged();
		void iterationsChanged();
		void saltChanged();
	};
}

#endif
