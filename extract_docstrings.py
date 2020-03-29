#!/usr/bin/env python3
"""
Very crude extraction of doxygen formatted docstrings from the header file.
I want somthing simple, to get usable documentation out of my headers, Doxygen
is too much, others aren't mature, neither is this, but it's simple.
"""

import re

INPUT_FILE = "src/DYPlayer.h"
HEADING_LEVEL = 4
NAMESPACE = "DY"
CLASSNAME = "DYPlayer"
#: Find function definitions that have a docstring, matchesg:
#: /**
#:  * Description of function, line 1
#:  * line *n*
#:  * @param param_name param description
#:  * @return description of return value
#:  * @throw exception_type Description of when this may occur
#:  */
#:  return_type function_name(param_type param_name);
#: It captures: docstring as a single group, return type name of the function,
#: all arguments as a single group.
RE_FUNCTION_PARSE = re.compile((
        '\/\*\*\n\s+\*\s+(([^\n]+\n)+)\s+?\*\/\s*\n\s*'
        '([a-zA-Z_]{1}[a-zA-Z0-9-_]+)\s+([a-zA-Z_]{1}[a-zA-Z0-9-_]+)\((.*?)\);'
    ),
    re.MULTILINE
)

#: Removes the `*` prefixes from the docstring
RE_CLEAN_DESCRIPTION = re.compile('^\s*\* *', re.MULTILINE)

#: Find an entire definition of a return, param or throw up to the end of it.
RE_DEFINITION = re.compile((
        '@(?P<definition>return|param|throw)\s+'
        '(?P<description>(?P<name>[a-zA-Z0-9-_]+)(?P<rest>[^@]+))'
    ),
    re.MULTILINE + re.DOTALL
)

#: Filter out those definitions from the rest of the description
RE_NOT_DEFINITION = re.compile('@(?:return|param|throw)', re.DOTALL)

#: Find all enums contents
RE_ENUMS_PARSE = re.compile((
    '\/\*\*\n\s+\*\s+(?P<docstring>([^\n]+\n)+)\s+?\*\/\s*\n\s*'
    'typedef\s+enum\s+(class)?\s+(?P<classname>[a-zA-Z0-9-_]+)'
    '(?:\:\s*(?P<extends>[a-zA-Z0-9-_]+))?\s*{\n'
    '(?P<body>[^\}]+)[\n\s]+}\s+'
    '(?P<enum_t>[a-zA-Z0-9-_]+);'
    ),
    re.DOTALL
)

RE_ENUM_BODY_PARSE = re.compile(
    '(?P<const>[a-zA-Z0-9-_]+)\s*'
    '(?:=\s*(?P<value>[^\s,]*))?,?\s*'
    '(?P<comment>(?:(?:\/\/\s*)[^\n]+\s*\n\s*)*)',
    re.MULTILINE
)


def parse_enums(source):
    """
    Parses all methods, assumes they are all in src/DYPlayer.h and they are all
    part of the `DY` namespace
    """
    enums = {}
    for enum in RE_ENUMS_PARSE.finditer(source):
        enum_t = enum.group('enum_t')

        docstring = RE_CLEAN_DESCRIPTION.sub("", enum.group('docstring'))

        matches = RE_ENUM_BODY_PARSE.finditer(enum.group('body'))
        classname = enum.group('classname')
        extends = enum.group('extends')

        enum_list = []
        for i, definition in enumerate(matches):
            description = definition.group('comment')
            description = description.replace("//", "")
            description = re.sub('\s\s+', " ", description).strip(" ")

            value = definition.group('value')
            if not value:
                value = "0x%02x" % i
            enum_list.append({
                'description': description,
                'value': value,
                'const': "%(ns)s::%(class)s::%(const)s" % {
                    'ns': NAMESPACE,
                    'class': classname,
                    'const': definition.group('const')
                }
            })

        if any([e['description'] for e in enum_list]):
            body = "| const | value | description |\n"
            body += "|:------|:------|:------------|\n"
            enum_fmt = "| `%(const)s` | `%(value)s` | %(description)s |\n"
        else:
            body = "| const | value |\n"
            body += "|:------|:------|\n"
            enum_fmt = "| `%(const)s` | `%(value)s` |\n"

        for definition in enum_list:
            body += enum_fmt % definition

        heading_fmt = "%(hlevel)s typedef enum class %(namespace)s::%(enum)s"
        anchor_title_fmt = "[`%(namespace)s::%(enum)s`]"

        if classname:
            anchor_link_fmt = "(#typedef-enum-class-%(namespace)s%(enum)s)"
        else:
            anchor_link_fmt = "(#typedef-enum-%(namespace)s%(enum)s)"

        enum_data = {
            'hlevel': HEADING_LEVEL * "#",
            'namespace': NAMESPACE,
            'enum': enum_t
        }

        heading = heading_fmt % enum_data
        # The anchor github will also generate to link to this enum. E.g.:
        # https://github.com/SnijderC/dyplayer#typedef-enum-dydevicedevice_t
        anchor = anchor_title_fmt % enum_data
        anchor += (anchor_link_fmt % enum_data).lower().replace(":", "")

        if extends:
            body += "\nThis enum class is based off %s.\n" % extends

        enums[enum_t] = {
            'heading': heading,
            'docstring': docstring,
            'body' : body,
            'anchor': anchor
        }
    return enums

def print_enums(enums):
    for enum_t, enum in enums.items():
        print(
            "%s\n\n%s\n%s\n" % (
                enum['heading'],
                enum['docstring'],
                enum['body']
            )

        )

def print_class_methods(source, enums):
    """
    Parses all methods, assumes they are all in src/DYPlayer.h and they are all
    part of `DY::DYPlayer`.
    """

    # Find all functions with docstrings
    for method in RE_FUNCTION_PARSE.finditer(source):
        # Generate a heading with return type and function name directly from
        # the function match.
        return_type = method.group(3)
        function_name = method.group(4)
        enum_type = enums.get(return_type)
        if enum_type:
            return_type = enum_type["anchor"]
        else:
            return_type = "`%s`" % return_type
        heading = "%s %s %s(..)" % (
            HEADING_LEVEL * "#",
            return_type,
            "%s::%s::%s" % (NAMESPACE, CLASSNAME, function_name)
        )

        # Remove `*` prefixes ans spaces from docstring
        docstring = RE_CLEAN_DESCRIPTION.sub("", method.group(1))

        # Split the docstring on the first occurance of the definitions of
        # params, returns and throws and return the former.
        description = RE_NOT_DEFINITION.split(docstring)[0]

        # Cleanup the arguments and split them
        args = method.group(5).replace(", ", ",").split(",")
        # If there are any arguments
        if len(args[0]):
            # Reverse the order of the argument (name before type) and strip
            # `*` from pointers
            args = [[y.strip("*[]"), x] for [x, y] in map(str.split, args)]
            # Make a dict of the arguments for eay lookup, this is to match
            # them with the name in the definition later.
            args = dict(args)
        else:
            # If there are no arguments, set an empty dict.
            args = {}

        # See if there are any definitions of param, return or throw in the
        # docstring.
        table = ""
        if RE_DEFINITION.search(docstring):
            # If there are, start a table heading.
            table += "|           | __Type__ | __Name__ | __Description__  |\n"
            table += "|:----------|:---------|:---------|:-----------------|\n"
        # Iterate over all definitions of param, return and throw.
        for definition in RE_DEFINITION.finditer(docstring):
            # Are we dealing with a param, return or throw?
            def_type = definition.group('definition')
            # The description of the definition is always in the same place
            def_desc = definition.group("description")
            param_name = ""
            if def_type == "param":
                # Find the corresponding type for the argument name (first word
                # in the description of the definition).
                param_name = definition.group("name")
                param_type = args[param_name]
                param_name = "`%s`" % param_name
                enum_type = enums.get(param_type)
                if enum_type:
                    param_type = enum_type["anchor"]
                else:
                    param_type = "`%s`" % param_type
            elif def_type == "return":
                # We already parsed the return type.
                param_type = return_type
            elif def_type == "throw":
                # throws always start with the exception name as the first word.
                param_type = definition.group("name")
                # Remove the first word (definition type) from the description.
                desc = re.sub("^%s " % def_type, "", definition.group("rest"))

            table += re.sub(
                "[^|]\n",
                " ",
                "| __%s__ | %s | %s | %s |\n" % (
                    def_type,
                    param_type,
                    param_name,
                    def_desc
                )
            )
        # Output heading, description and definition table.
        print("%s\n\n%s\n\n%s\n" % (heading, description, table))

def main():
    # Read the input file in one go.
    with open(INPUT_FILE, "r") as fh:
        source = fh.read()

    # Don't include the private methods in the API docs.
    source = source.split("private:")[0]

    enums = parse_enums(source)
    print_class_methods(source, enums)
    print_enums(enums)

if __name__ == "__main__":
    main()
