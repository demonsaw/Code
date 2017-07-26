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

#ifndef _EJA_CLIENT_CIPHER_COMPONENT_
#define _EJA_CLIENT_CIPHER_COMPONENT_

#include "component/component.h"
#include "component/group/group_security_component.h"
#include "entity/entity.h"
#include "object/client/client_security.h"

namespace eja
{
	// Component
	class client_security_component final : public client_security, public component
	{
	public:
		using ptr = std::shared_ptr<client_security_component>;

	private:
		// Accessor
		std::string get_key(const group_security_component::ptr security, const byte* input, const size_t input_size, const size_t output_size) const;

	public:
		client_security_component() { }
		client_security_component(const client_security_component& security) : client_security(security) { }
		client_security_component(const client_security& security) : client_security(security) { }
		virtual ~client_security_component() override { }

		// Interface
		virtual void update() override;

		// Utility
		bool valid() const;
		bool invalid() const { return !valid(); }
		bool modified() const;
		bool empty() const;

		std::string encrypt(const byte* input, const size_t input_size) const;
		std::string encrypt(const CryptoPP::SecByteBlock& input) const { return encrypt(input.data(), input.size()); }
		std::string encrypt(const std::string& input) const { return encrypt(reinterpret_cast<const byte*>(input.c_str()), input.size()); }
		std::string encrypt(const char* input) const { return encrypt(reinterpret_cast<const byte*>(input), strlen(input)); }

		std::string decrypt(const byte* input, const size_t input_size) const;
		std::string decrypt(const CryptoPP::SecByteBlock& input) const { return decrypt(input.data(), input.size()); }
		std::string decrypt(const std::string& input) const { return decrypt(reinterpret_cast<const byte*>(input.c_str()), input.size()); }
		std::string decrypt(const char* input) const { return decrypt(reinterpret_cast<const byte*>(input), strlen(input)); }
		
		// Static
		static ptr create() { return ptr(new client_security_component()); }
		static ptr create(const client_security_component::ptr security) { return ptr(new client_security_component(*security)); }
		static ptr create(const client_security::ptr security) { return ptr(new client_security_component(*security)); }
		static ptr create(const client_security& security) { return ptr(new client_security_component(security)); }
	};

	// Container
	derived_type(client_security_entity_list_component, entity_list_component);
	derived_type(client_security_entity_map_component, entity_map_component);
	derived_type(client_security_entity_queue_component, entity_queue_component);
	derived_type(client_security_entity_vector_component, entity_vector_component);
}

#endif
