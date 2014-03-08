Copyright 2012, 2013 Matthew Harvey

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Style guide
===========

**Conditional statements**::

    if (some_condition) this_is_acceptable();
    
    if (some_condition)
    {
        this_is_acceptable();
    }

    if (some_condition)
    {
        this_is_acceptable();
        and_so_is_this();
    }

**Function definitions**::

    int some_function(int x)
    {
        do_function_body();
        return x * 2;
    }

**Function calls**::

    int x = some_function_call(3, y, some_other_thing(y));

    int x = some_quite_long_function_call
    (	with,
        lots,
        of,
        arguments_that_don't_fit_on,
        one_line
    );

**Naming**::

    ClassName
    function_name
    local_variable_name
    local_constant_name
    file_name
    GUARD_header_guard_hpp
    m_member_variable
    s_static_class_variable
    p_parameter_if_required_to_distinguish_from_member_variable
    EnumName
    StructName
    MACRO
    Thing  // Name of class
    things  // name of SQL table
    thing_id  // name of primary key
    thing_id  // name of foreign key
    thing_property // name of ordinary column

**Exceptions**

All exception classes should be derived from std::exception (either directly
or indirectly).
