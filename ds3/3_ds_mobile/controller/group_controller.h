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

#ifndef _EJA_GROUP_CONTROLLER_H_
#define _EJA_GROUP_CONTROLLER_H_

#include <memory>

#include "entity/entity_object.h"

namespace eja
{
	class group_model;

	class group_controller final : public entity_object
	{
		Q_OBJECT;

		Q_PROPERTY(size_t size READ get_size NOTIFY sizeChanged);

		Q_PROPERTY(bool enabled READ get_enabled WRITE set_enabled NOTIFY enabledChanged);
		Q_PROPERTY(QString id READ get_id NOTIFY idChanged);		
		Q_PROPERTY(QString entropy READ get_entropy WRITE set_entropy NOTIFY entropyChanged);
		Q_PROPERTY(QString cipher READ get_cipher WRITE set_cipher NOTIFY cipherChanged);
		Q_PROPERTY(size_t key_size READ get_key_size WRITE set_key_size NOTIFY key_sizeChanged);
		Q_PROPERTY(QString hash READ get_hash WRITE set_hash NOTIFY hashChanged);
		Q_PROPERTY(size_t iterations READ get_iterations WRITE set_iterations NOTIFY iterationsChanged);
		Q_PROPERTY(QString salt READ get_salt WRITE set_salt NOTIFY saltChanged);

	private:
		bool m_enabled = true;
		group_model* m_model = nullptr;

		// Get
		std::shared_ptr<entity> get_group() const;

	public:
		group_controller(QObject* parent = nullptr);

		// Interface
		void add(const std::shared_ptr<entity> entity);
		Q_INVOKABLE void clear();

		Q_INVOKABLE void update(); 
		Q_INVOKABLE void restart();
		Q_INVOKABLE void reset();

		// Utility
		Q_INVOKABLE bool enabled() const { return m_enabled; }
		Q_INVOKABLE bool empty() const;

		Q_INVOKABLE void mute(const int row, const bool value = true);
		Q_INVOKABLE void message(const int row);

		// Set
		virtual void set_entity(const std::shared_ptr<entity> entity)  override;

		void set_enabled(const bool enabled);
		void set_entropy(const QString& entropy);
		void set_cipher(const QString& cipher);
		void set_key_size(const size_t key_size);
		void set_hash(const QString& hash);
		void set_iterations(const size_t iterations);
		void set_salt(const QString& salt);

		// Get
		const group_model* get_model() const { return m_model; }
		group_model* get_model() { return m_model; }

		size_t get_size() const;
		
		bool get_enabled() const;
		QString get_id() const;		
		QString get_entropy() const;
		QString get_cipher() const;
		size_t get_key_size() const;
		QString get_hash() const;
		size_t get_iterations() const;
		QString get_salt() const;

	signals:
		void sizeChanged();

		void enabledChanged();
		void idChanged();
		void entropyChanged();
		void cipherChanged();
		void key_sizeChanged();
		void hashChanged();
		void iterationsChanged();
		void saltChanged();
	};
}

#endif
