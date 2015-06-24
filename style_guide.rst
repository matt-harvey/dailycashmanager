Copyright 2012, 2013, 2015 Matthew Harvey

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

    // don't do this

    if (some_condition)
        not_ok();

    if (some_condition) {
        not_ok();
    }

    // these are OK

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

    auto x = some_function_call(3, y, some_other_thing(y));

    auto x = some_quite_long_function_call
    (	with,
        lots,
        of,
        arguments_that_do_not_fit_on,
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
    p_parameter
    EnumName
    StructName
    MACRO
    Thing  // Name of class
    things  // name of SQL table
    thing_id  // name of primary key
    thing_id  // name of foreign key
    attribute // name of ordinary column

**Exceptions**

All exception classes should be derived from std::exception (either directly
or indirectly).

**Typedefs**::

    // don't do
    typedef Blah Bloob;

    // do do
    using Boob = Blah;

**Auto**::

Use auto unless there's a really good reason not to. Even if you need to
initialize a variable of one type by converting from another type, still prefer
auto - with a static_cast to make the conversion extra-obvious::

  auto const x = static_cast<int>(function_returning_a_double());

