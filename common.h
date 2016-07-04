// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: zhangdi05@baidu.com (Zhangdi Di)

#ifndef COMMON_H
#define COMMON_H

#include <string>

namespace sofa_php_ext
{

#define EXT_ZEND_FOREACH(iter, hash) \
	for (zend_hash_internal_pointer_reset_ex((hash), (iter)); \
            zend_hash_has_more_elements_ex((hash), (iter)) == SUCCESS; \
            zend_hash_move_forward_ex((hash), (iter)))

const static std::string PB_VALUES_PROPERTY = "values";
const static std::string PB_FIELDS_METHOD = "fields";
const static std::string PB_FIELD_TYPE = "type";
const static std::string PB_FIELD_REQUIRED = "required";
const static std::string PB_FIELD_REPEATED = "repeated";
const static std::string PB_FIELD_NAME = "name";
const static std::string CLASS_TYPE = "class_type";
const static std::string PARSE_FUNCTION = "parseFromString";
static const char* IMPL = "impl";

}
#endif  // COMMON_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
