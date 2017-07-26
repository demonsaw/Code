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

#ifndef _EJA_WINDOW_
#define _EJA_WINDOW_

#include <QVector>

#include "command/client/request/client_browse_request_command.h"
#include "component/transfer/transfer_component.h"
#include "entity/entity.h"
#include "http/http_status.h"
#include "object/machine.h"
#include "object/status.h"
#include "object/client/client_security.h"
#include "object/group/group_security.h"
#include "object/io/file.h"
#include "security/hash.h"
#include "system/function/function_action.h"
#include "system/type.h"

Q_DECLARE_METATYPE(u64); 
Q_DECLARE_METATYPE(eja::client_browse_request_command::ptr);
Q_DECLARE_METATYPE(eja::client_hash_type); 
Q_DECLARE_METATYPE(eja::entity::ptr);
Q_DECLARE_METATYPE(eja::file_filter);
Q_DECLARE_METATYPE(eja::group_cipher_type);
Q_DECLARE_METATYPE(eja::group_hash_type);
Q_DECLARE_METATYPE(eja::http_status);
Q_DECLARE_METATYPE(eja::status);
Q_DECLARE_METATYPE(eja::transfer_type);
Q_DECLARE_METATYPE(eja::function_action);
Q_DECLARE_METATYPE(QVector<int>);

namespace eja
{
}

#endif
