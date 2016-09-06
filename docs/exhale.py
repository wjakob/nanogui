# This file is part of exhale: https://github.com/svenevs/exhale
#
# Copyright (c) 2016, Stephen McDowell
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of exhale nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

from breathe.parser.index import parse as breathe_parse
import sys
import re
import os
import cStringIO
import itertools

__all__ = ['generate', 'ExhaleRoot', 'ExhaleNode', 'exclaimError', 'qualifyKind',
           'kindAsBreatheDirective', 'specificationsForKind', 'EXHALE_FILE_HEADING',
           'EXHALE_SECTION_HEADING', 'EXHALE_SUBSECTION_HEADING']
__name__ = "exhale"
__docformat__ = "reStructuredText"

EXHALE_API_TOCTREE_MAX_DEPTH = 5 # DO NOT EXPOSE
'''
The value used as ``:maxdepth:`` with restructured text ``.. toctree::`` directives.
The default value is 5, as any larger will likely produce errors with a LaTeX build.
Change this value by specifying the proper value to the dictionary passed to the
`generate` function.
'''

EXHALE_API_DOXY_OUTPUT_DIR = "" # DO NOT EXPOSE
'''
The path to the doxygen xml output **directory**, relative to ``conf.py`` (or whichever
file is calling `generate`.  This value **must** be set for `generate` to be able to do
anything.
'''

EXHALE_API_DOXYGEN_STRIP_FROM_PATH = None # DO NOT EXPOSE
'''
Accounts for broken STRIP_FROM_PATH handling on RTD.
'''

EXHALE_GENERATE_BREATHE_FILE_DIRECTIVES = False # DO NOT EXPOSE
'''
Currently, Exhale (I...) do not know how to extract the documentation string for a given
file being produced.  If True, then the breathe directive (``doxygenfile``) will be
incorporated at the bottom of the file.  This will duplicate a lot of information, but
will include the file's description at the beginning.
'''

EXHALE_FILE_HEADING = "=" * 88
''' The restructured text file heading separator (``"=" * 88``). '''

EXHALE_SECTION_HEADING = "-" * 88
''' The restructured text section heading separator (``"-" * 88``). '''

EXHALE_SUBSECTION_HEADING = "*" * 88
''' The restructured text sub-section heading separator (``"*" * 88``).'''

EXHALE_CUSTOM_SPECIFICATIONS_FUNCTION = None # DO NOT EXPOSE
'''
User specified override of `specificationsForKind`.  No safety checks are performed for
externally provided functions.  Change the functionality of `specificationsForKind` by
specifiying a function in the dictionary passed to `generate`.
'''


########################################################################################
#
##
###
####
##### Primary entry point.
####
###
##
#
########################################################################################
def generate(exhaleArgs):
    '''
    The main entry point to exhale, which parses and generates the full API.

    :Parameters:
        ``exhaleArgs`` (dict)
            The dictionary of arguments to configure exhale with.  All keys are strings,
            and most values should also be strings.  See below.

    **Required Entries:**

    **key**: ``"doxygenIndexXMLPath"`` --- value type: ``str``
        The absolute or relative path to where the Doxygen index.xml is.  A relative
        path must be relative to the file **calling** exhale.

    **key**: ``"containmentFolder"`` --- value type: ``str``
        The folder the generated API will be created in.  If the folder does not exist,
        exhale will create the folder.  The path can be absolute, or relative to the
        file that is **calling** exhale.  For example, ``"./generated_api"``.

    **key**: ``"rootFileName"`` --- value type: ``str``
        The name of the file that **you** will be linking to from your reStructuredText
        documents.  Do not include the ``containmentFolder`` path in this file name,
        exhale will create the file ``"{}/{}".format(containmentFolder, rootFileName)``.

        In order for Sphinx to be happy, you should include a ``.rst`` suffix.  All of
        the generated API uses reStructuredText, and that will not ever change.

        For example, if you specify

        - ``"containmentFolder" = "./generated_api"``, and
        - ``"rootFileName" = "library_root.rst"``

        Then exhale will generate the file ``./generated_api/library_root.rst``.

        You could include this file in a toctree directive (say in ``index.rst``) with::

            .. toctree:
               :maxdepth: 2

               generated_api/library_root

        Since Sphinx allows for some flexibility (e.g. your primary domain may be using
        ``.txt`` files), **no error checking will be performed**.

    **key**: ``"rootFileTitle"`` --- value type: ``str``
        The title to be written at the top of ``rootFileName``, which will appear in
        your file including it in the ``toctree`` directive.

    **key**: ``"doxygenStripFromPath"`` --- value type: ``str``
        When building on Read the Docs, there seem to be issues regarding the Doxygen
        variable ``STRIP_FROM_PATH`` when built remotely.  That is, it isn't stripped at
        all.  Provide me with a string path (e.g. ``".."``), and I will strip this for
        you for the File nodes being generated.  I will use the exact value of
        ``os.path.abspath("..")`` in the example above, so you can supply either a
        relative or absolute path.  The File view hierarchy **will** break if you do
        not give me a value for this, and therefore I hesitantly require this argument.
        The value ``".."`` assumes that ``conf.py`` is in a ``docs/`` or similar folder
        exactly one level below the repository's root.

    **Additional Options:**

    **key**: ``"afterTitleDescription"`` --- value type: ``str``
        Properly formatted reStructuredText with **no indentation** to be included
        directly after the title.  You can use any rst directives or formatting you wish
        in this string.  I suggest using the ``textwrap`` module, e.g.::

            description = textwrap.dedent(\'\'\'
            This is a description of the functionality of the library being documented.

            .. warning::

               Please be advised that this library does not do anything.
            \'\'\')

        Then you can add ``"afterTitleDescription" = description`` to your dictionary.

    **key**: ``"afterBodySummary"`` --- value type: ``str``
        Similar to ``afterTitleDescription``, this is a string with reStructuredText
        formatting.  This will be inserted after the generated API body.  The layout
        looks something like this::

            rootFileTitle
            ============================================================================

            afterTitleDescription (if provided)

            [[[ GENERATED API BODY ]]]

            afterBodySummary (if provided)

    **key**: ``"createTreeView"`` --- value type: ``bool``
        For portability, the default value if not specified is ``False``, which will
        generate reStructuredText bulleted lists for the Class View and File View
        hierarchies.  If ``True``, raw html unordered lists will be generated.  Please
        refer to the *Clickable Hierarchies* subsection of :ref:`usage_advanced_usage`
        for more details.

    **key**: ``"fullToctreeMaxDepth"`` --- value type: ``int``
        Beneath the Class View and File View hierarchies a Full API listing is generated
        as there are items that may not appear in the Class View hierarchy, as well as
        without this an obscene amount of warnings are generated from Sphinx because
        neither view actually uses a ``toctree``, they link directly.

        The default value is 5 if not specified, but you may want to give a smaller
        value depending on the framework being documented.  This value must be greater
        than or equal to 1 (this is the value of ``:maxdepth:``).

    **key**: ``"appendBreatheFileDirective"`` --- value type: ``bool``
        Currently, I do not know how to reliably extract the brief / detailed file
        descriptions for a given file node.  Therefore, if you have file level
        documentation in your project that has meaning, it would otherwise be omitted.
        As a temporary patch, if you specify this value as ``True`` then at the bottom
        of the file page the full ``doxygenfile`` directive output from Breathe will
        be appended to the file documentiation.  File level brief and detailed
        descriptions will be included, followed by a large amount of duplication.  I
        hope to remove this value soon, in place of either parsing the xml more
        carefully or finding out how to extract this information directly from Breathe.

        The default value of this behavior is ``False`` if it is not specified in the
        dictionary passed as input for this method.  Please refer to the *Customizing
        File Pages* subsection of :ref:`usage_customizing_file_pages` for more
        information on what the impact of this variable is.

    **key**: ``"customSpecificationFunction"`` --- value type: ``function``
        The custom specification function to override the default behavior of exhale.
        Please refer to the :func:`exhale.specificationsForKind` documentation.

    :raises ValueError:
        If the required dictionary arguments are not present, or any of the (key, value)
        pairs are invalid.

    :raises RuntimeError:
        If any **fatal** error is caught during the generation of the API.
    '''
    if type(exhaleArgs) is not dict:
        raise ValueError("The type of 'exhaleArgs' must be a dictionary.")
    # Gather mandatory input
    if "doxygenIndexXMLPath" not in exhaleArgs:
        raise ValueError("'doxygenIndexXMLPath' must be present in the arguments to generate.")
    try:
        global EXHALE_API_DOXY_OUTPUT_DIR
        doxygenIndexXMLPath = exhaleArgs["doxygenIndexXMLPath"]
        EXHALE_API_DOXY_OUTPUT_DIR = doxygenIndexXMLPath.split("index.xml")[0]
    except Exception as e:
        raise ValueError("Unable to utilize the provided 'doxygenIndexXMLPath'\n{}".format(e))

    if "containmentFolder" not in exhaleArgs:
        raise ValueError("'containmentFolder' must be present in the arguments to generate.")
    containmentFolder = exhaleArgs["containmentFolder"]
    if type(containmentFolder) is not str:
        raise ValueError("The type of the value for the key 'containmentFolder' must be a string.")

    if "rootFileName" not in exhaleArgs:
        raise ValueError("'rootFileName' must be present in the arguments passed to generate.")
    rootFileName = exhaleArgs["rootFileName"]
    if type(rootFileName) is not str:
        raise ValueError("The type of the value for the key 'rootFileName' must be a string.")

    if "rootFileTitle" not in exhaleArgs:
        raise ValueError("'rootFileTitle' must be present in the arguments passed to generate.")
    rootFileTitle = exhaleArgs["rootFileTitle"]
    if type(rootFileTitle) is not str:
        raise ValueError("The type of the value for the key 'rootFileTitle' must be a string.")

    if "doxygenStripFromPath" not in exhaleArgs:
        raise ValueError("'doxygenStripFromPath' must be present in the arguments passed to generate.")
    doxygenStripFromPath = exhaleArgs["doxygenStripFromPath"]
    if type(doxygenStripFromPath) is not str:
        raise ValueError("The type of the value for the key 'doxygenStripFromPath' must be a string.")
    try:
        strip = os.path.abspath(doxygenStripFromPath)
        if not os.path.isdir(strip):
            raise ValueError("The value for the key 'doxygenStripFromPath' does not appear to be a valid path")
    except Exception as e:
        raise RuntimeError("Error coordinating the 'doxygenStripFromPath' variable: {}".format(e))
    global EXHALE_API_DOXYGEN_STRIP_FROM_PATH
    EXHALE_API_DOXYGEN_STRIP_FROM_PATH = strip

    # gather the optional configurations
    if "afterTitleDescription" in exhaleArgs:
        afterTitleDescription = exhaleArgs["afterTitleDescription"]
        if type(afterTitleDescription) is not str:
            raise ValueError("The type of the value for the key 'afterTitleDescription' must be a string.")
    else:
        afterTitleDescription = ""

    if "afterBodySummary" in exhaleArgs:
        afterBodySummary = exhaleArgs["afterBodySummary"]
        if type(afterBodySummary) is not str:
            raise ValueError("The type of the value for the key 'afterBodySummary' must be a string.")
    else:
        afterBodySummary = ""

    if "createTreeView" in exhaleArgs:
        createTreeView = exhaleArgs["createTreeView"]
        if type(createTreeView) is not bool:
            raise ValueError("The type of the value for the key 'createTreeView' must be a boolean.")
    else:
        createTreeView = False

    if "fullToctreeMaxDepth" in exhaleArgs:
        fullToctreeMaxDepth = exhaleArgs["fullToctreeMaxDepth"]
        if type(fullToctreeMaxDepth) is not int:
            raise ValueError("The type of the value for the key 'fullToctreeMaxDepth' must be an int.")
        global EXHALE_API_TOCTREE_MAX_DEPTH
        EXHALE_API_TOCTREE_MAX_DEPTH = fullToctreeMaxDepth

    if "appendBreatheFileDirective" in exhaleArgs:
        appendBreatheFileDirective = exhaleArgs["appendBreatheFileDirective"]
        if type(appendBreatheFileDirective) is not bool:
            raise ValueError("The type of the value for the key 'appendBreatheFileDirective' must be a boolean.")
        global EXHALE_GENERATE_BREATHE_FILE_DIRECTIVES
        EXHALE_GENERATE_BREATHE_FILE_DIRECTIVES = appendBreatheFileDirective

    if "customSpecificationFunction" in exhaleArgs:
        customSpecificationFunction = exhaleArgs["customSpecificationFunction"]
        try:
            ret = customSpecificationFunction("class")
        except:
            raise ValueError("Unable to call your custom specification function with 'class' as input...")
        if type(ret) is not str:
            raise ValueError("Your custom specification function did not return a string...")
        global EXHALE_CUSTOM_SPECIFICATIONS_FUNCTION
        EXHALE_CUSTOM_SPECIFICATIONS_FUNCTION = customSpecificationFunction

    # input gathered, try creating the breathe root compound
    try:
        breatheRoot = breathe_parse(doxygenIndexXMLPath)
    except Exception as e:
        raise RuntimeError("Unable to use Breathe to parse the specified doxygen index.xml: {}".format(e))

    if breatheRoot is not None:
        # split into multiple try-except blocks to make it a little easier to identify
        # where the error comes from
        try:
            textRoot = ExhaleRoot(breatheRoot, containmentFolder, rootFileName,
                                  rootFileTitle, afterTitleDescription,
                                  afterBodySummary, createTreeView)
        except Exception as e:
            raise RuntimeError("Exception caught creating the ExhaleRoot object: {}".format(e))
        try:
            textRoot.parse()
        except Exception as e:
            raise RuntimeError("Exception caught while parsing: {}".format(e))
        try:
            textRoot.generateFullAPI()
        except Exception as e:
            raise RuntimeError("Exception caught while generating: {}".format(e))
    else:
        raise RuntimeError("Critical error: the returned Breathe root is 'None'.")


########################################################################################
#
##
###
####
##### Utility / helper functions.
####
###
##
#
########################################################################################
def qualifyKind(kind):
    '''
    Qualifies the breathe ``kind`` and returns an qualifier string describing this
    to be used for the text output (e.g. in generated file headings and link names).

    The output for a given kind is as follows:

    +-------------+------------------+
    | Input Kind  | Output Qualifier |
    +=============+==================+
    | "class"     | "Class"          |
    +-------------+------------------+
    | "define"    | "Define"         |
    +-------------+------------------+
    | "enum"      | "Enum"           |
    +-------------+------------------+
    | "enumvalue" | "Enumvalue"      |
    +-------------+------------------+
    | "file"      | "File"           |
    +-------------+------------------+
    | "function"  | "Function"       |
    +-------------+------------------+
    | "group"     | "Group"          |
    +-------------+------------------+
    | "namespace" | "Namespace"      |
    +-------------+------------------+
    | "struct"    | "Struct"         |
    +-------------+------------------+
    | "typedef"   | "Typedef"        |
    +-------------+------------------+
    | "union"     | "Union"          |
    +-------------+------------------+
    | "variable"  | "Variable"       |
    +-------------+------------------+

    The following breathe kinds are ignored:

    - "autodoxygenfile"
    - "doxygenindex"
    - "autodoxygenindex"

    Note also that although a return value is generated, neither "enumvalue" nor
    "group" are actually used.

    :Parameters:
        ``kind`` (str)
            The return value of a Breathe ``compound`` object's ``get_kind()`` method.

    :Return (str):
        The qualifying string that will be used to build the reStructuredText titles and
        other qualifying names.  If the empty string is returned then it was not
        recognized.
    '''
    if kind == "class":
        qualifier = "Class"
    elif kind == "struct":
        qualifier = "Struct"
    elif kind == "function":
        qualifier = "Function"
    elif kind == "enum":
        qualifier = "Enum"
    elif kind == "enumvalue":# unused
        qualifier = "Enumvalue"
    elif kind == "namespace":
        qualifier = "Namespace"
    elif kind == "define":
        qualifier = "Define"
    elif kind == "typedef":
        qualifier = "Typedef"
    elif kind == "variable":
        qualifier = "Variable"
    elif kind == "file":
        qualifier = "File"
    elif kind == "dir":
        qualifier = "Directory"
    elif kind == "group":
        qualifier = "Group"
    elif kind == "union":
        qualifier = "Union"
    else:
        qualifier = ""
    return qualifier


def kindAsBreatheDirective(kind):
    '''
    Returns the appropriate breathe restructured text directive for the specified kind.
    The output for a given kind is as follows:

    +-------------+--------------------+
    | Input Kind  | Output Directive   |
    +=============+====================+
    | "class"     | "doxygenclass"     |
    +-------------+--------------------+
    | "define"    | "doxygendefine"    |
    +-------------+--------------------+
    | "enum"      | "doxygenenum"      |
    +-------------+--------------------+
    | "enumvalue" | "doxygenenumvalue" |
    +-------------+--------------------+
    | "file"      | "doxygenfile"      |
    +-------------+--------------------+
    | "function"  | "doxygenfunction"  |
    +-------------+--------------------+
    | "group"     | "doxygengroup"     |
    +-------------+--------------------+
    | "namespace" | "doxygennamespace" |
    +-------------+--------------------+
    | "struct"    | "doxygenstruct"    |
    +-------------+--------------------+
    | "typedef"   | "doxygentypedef"   |
    +-------------+--------------------+
    | "union"     | "doxygenunion"     |
    +-------------+--------------------+
    | "variable"  | "doxygenvariable"  |
    +-------------+--------------------+

    The following breathe kinds are ignored:

    - "autodoxygenfile"
    - "doxygenindex"
    - "autodoxygenindex"

    Note also that although a return value is generated, neither "enumvalue" nor
    "group" are actually used.

    :Parameters:
        ``kind`` (str)
            The kind of the breathe compound / ExhaleNode object (same values).

    :Return (str):
        The directive to be used for the given ``kind``.  The empty string is returned
        for both unrecognized and ignored input values.
    '''
    if kind == "class":
        directive = "doxygenclass"
    elif kind == "struct":
        directive = "doxygenstruct"
    elif kind == "function":
        directive = "doxygenfunction"
    elif kind == "enum":
        directive = "doxygenenum"
    elif kind == "enumvalue":# unused
        directive = "doxygenenumvalue"
    elif kind == "namespace":
        directive = "doxygennamespace"
    elif kind == "define":
        directive = "doxygendefine"
    elif kind == "typedef":
        directive = "doxygentypedef"
    elif kind == "variable":
        directive = "doxygenvariable"
    elif kind == "file":
        directive = "doxygenfile"
    elif kind == "union":
        directive = "doxygenunion"
    elif kind == "group":# unused
        directive = "doxygengroup"
    else:
        directive = ""
    return directive


def specificationsForKind(kind):
    '''
    Returns the relevant modifiers for the restructured text directive associated with
    the input kind.  The only considered values for the default implementation are
    ``class`` and ``struct``, for which the return value is exactly::

        "   :members:\\n   :protected-members:\\n   :undoc-members:\\n"

    Formatting of the return is fundamentally important, it must include both the prior
    indentation as well as newlines separating any relevant directive modifiers.  The
    way the framework uses this function is very specific; if you do not follow the
    conventions then sphinx will explode.

    Consider a ``struct thing`` being documented.  The file generated for this will be::

        .. _struct_thing:

        Struct thing
        ================================================================================

        .. doxygenstruct:: thing
           :members:
           :protected-members:
           :undoc-members:

    Assuming the first two lines will be in a variable called ``link_declaration``, and
    the next three lines are stored in ``header``, the following is performed::

        directive = ".. {}:: {}\\n".format(kindAsBreatheDirective(node.kind), node.name)
        specifications = "{}\\n\\n".format(specificationsForKind(node.kind))
        gen_file.write("{}{}{}{}".format(link_declaration, header, directive, specifications))

    That is, **no preceding newline** should be returned from your custom function, and
    **no trailing newline** is needed.  Your indentation for each specifier should be
    **exactly three spaces**, and if you want more than one you need a newline in between
    every specification you want to include.  Whitespace control is handled internally
    because many of the directives do not need anything added.  For a full listing of
    what your specifier options are, refer to the breathe documentation:

        http://breathe.readthedocs.io/en/latest/directives.html

    :Parameters:
        ``kind`` (str)
            The kind of the node we are generating the directive specifications for.

    :Return (str):
        The correctly formatted specifier(s) for the given ``kind``.  If no specifier(s)
        are necessary or desired, the empty string is returned.
    '''
    # use the custom directives function
    if EXHALE_CUSTOM_SPECIFICATIONS_FUNCTION is not None:
        return EXHALE_CUSTOM_SPECIFICATIONS_FUNCTION(kind)

    # otherwise, just provide class and struct
    if kind == "class" or kind == "struct":
        directive = "   :members:\n   :protected-members:\n   :undoc-members:"
    else:
        directive = ""
    return directive


def exclaimError(msg, ansi_fmt="34;1m"):
    '''
    Prints ``msg`` to the console in color with ``(!)`` prepended in color.

    Example (uncolorized) output of ``exclaimError("No leading space needed.")``::

        (!) No leading space needed.

    All messages are written to ``sys.stderr``, and are closed with ``[0m``.  The
    default color is blue, but can be changed using ``ansi_fmt``.

    Documentation building has a verbose output process, this just helps distinguish an
    error message coming from exhale.

    :Parameters:
        ``msg`` (str)
            The message you want printed to standard error.

        ``ansi_fmt`` (str)
            An ansi color format.  ``msg`` is printed as
            ``"\\033[" + ansi_fmt + msg + "\\033[0m\\n``, so you should specify both the
            color code and the format code (after the semicolon).  The default value is
            ``34;1m`` --- refer to
            http://misc.flogisoft.com/bash/tip_colors_and_formatting for alternatives.
    '''
    sys.stderr.write("\033[{}(!) {}\033[0m\n".format(ansi_fmt, msg))


########################################################################################
#
##
###
####
##### Graph representation.
####
###
##
#
########################################################################################
class ExhaleNode:
    '''
    A wrapper class to track parental relationships, filenames, etc.

    :Parameters:
        ``breatheCompound`` (breathe.compound)
            The Breathe compound object we will use to gather the name, chilren, etc.

    :Attributes:
        ``compound`` (breathe.compound)
            The compound discovered from breathe that we are going to track.

        ``kind`` (str)
            The string returned by the ``breatheCompound.get_kind()`` method.  Used to
            qualify this node throughout the framework, as well as for hierarchical
            sorting.

        ``name`` (str)
            The string returned by the ``breatheCompound.get_name()`` method.  This name
            will be fully qualified --- ``class A`` inside of ``namespace n`` will have
            a ``name`` of ``n::A``.  Files and directories may have ``/`` characters as
            well.

        ``refid`` (str)
            The reference ID as created by Doxygen.  This will be used to scrape files
            and see if a given reference identification number should be associated with
            that file or not.

        ``children`` (list)
            A potentially empty list of ``ExhaleNode`` object references that are
            considered a child of this Node.  Please note that a child reference in any
            ``children`` list may be stored in **many** other lists.  Mutating a given
            child will mutate the object, and therefore affect other parents of this
            child.  Lastly, a node of kind ``enum`` will never have its ``enumvalue``
            children as it is impossible to rebuild that relationship without more
            Doxygen xml parsing.

        ``parent`` (ExhaleNode)
            If an ExhaleNode is determined to be a child of another ExhaleNode, this
            node will be added to its parent's ``children`` list, and a reference to
            the parent will be in this field.  Initialized to ``None``, make sure you
            check that it is an object first.

            .. warning::
               Do not ever set the ``parent`` of a given node if the would-be parent's
               kind is ``"file"``.  Doing so will break many important relationships,
               such as nested class definitions.  Effectively, **every** node will be
               added as a child to a file node at some point.  The file node will track
               this, but the child should not.

        The following three member variables are stored internally, but managed
        externally by the :class:`exhale.ExhaleRoot` class:

        ``file_name`` (str)
            The name of the file to create.  Set to ``None`` on creation, refer to
            :func:`exhale.ExhaleRoot.initializeNodeFilenameAndLink`.

        ``link_name`` (str)
            The name of the reStructuredText link that will be at the top of the file.
            Set to ``None`` on creation, refer to
            :func:`exhale.ExhaleRoot.initializeNodeFilenameAndLink`.

        ``title`` (str)
            The title that will appear at the top of the reStructuredText file
            ``file_name``. When the reStructuredText document for this node is being
            written, the root object will set this field.

        The following two fields are used for tracking what has or has not already been
        included in the hierarchy views.  Things like classes or structs in the global
        namespace will not be found by :func:`exhale.ExhaleNode.inClassView`, and the
        ExhaleRoot object will need to track which ones were missed.

        ``in_class_view`` (bool)
            Whether or not this node has already been incorporated in the class view.

        ``in_file_view`` (bool)
            Whether or not this node has already been incorporated in the file view.

        This class wields duck typing.  If ``self.kind == "file"``, then the additional
        member variables below exist:

        ``namespaces_used`` (list)
            A list of namespace nodes that are either defined or used in this file.

        ``includes`` (list)
            A list of strings that are parsed from the Doxygen xml for this file as
            include directives.

        ``included_by`` (list)
            A list of (refid, name) string tuples that are parsed from the Doxygen xml
            for this file presenting all of the other files that include this file.
            They are stored this way so that the root class can later link to that file
            by its refid.

        ``location`` (str)
            A string parsed from the Doxygen xml for this file stating where this file
            is physically in relation to the *Doxygen* root.

        ``program_listing`` (list)
            A list of strings that is the Doxygen xml <programlisting>, without the
            opening or closing <programlisting> tags.

        ``program_file`` (list)
            Managed externally by the root similar to ``file_name`` etc, this is the
            name of the file that will be created to display the program listing if it
            exists.  Set to ``None`` on creation, refer to
            :func:`exhale.ExhaleRoot.initializeNodeFilenameAndLink`.

        ``program_link_name`` (str)
            Managed externally by the root similar to ``file_name`` etc, this is the
            reStructuredText link that will be declared at the top of the
            ``program_file``. Set to ``None`` on creation, refer to
            :func:`exhale.ExhaleRoot.initializeNodeFilenameAndLink`.
    '''
    def __init__(self, breatheCompound):
        self.compound = breatheCompound
        self.kind     = breatheCompound.get_kind()
        self.name     = breatheCompound.get_name()
        self.refid    = breatheCompound.get_refid()
        self.children = [] # ExhaleNodes
        self.parent   = None # if reparented, will be an ExhaleNode
        # managed externally
        self.file_name = None
        self.link_name = None
        self.title     = None
        # representation of hierarchies
        self.in_class_view = False
        self.in_directory_view = False
        # kind-specific additional information
        if self.kind == "file":
            self.namespaces_used   = [] # ExhaleNodes
            self.includes          = [] # strings
            self.included_by       = [] # (refid, name) tuples
            self.location          = ""
            self.program_listing   = [] # strings
            self.program_file      = ""
            self.program_link_name = ""

    def __lt__(self, other):
        '''
        The ``ExhaleRoot`` class stores a bunch of lists of ``ExhaleNode`` objects.
        When these lists are sorted, this method will be called to perform the sorting.

        :Parameters:
            ``other`` (ExhaleNode)
                The node we are comparing whether ``self`` is less than or not.

        :Return (bool):
            True if ``self`` is less than ``other``, False otherwise.
        '''
        # allows alphabetical sorting within types
        if self.kind == other.kind:
            return self.name.lower() < other.name.lower()
        # treat structs and classes as the same type
        elif self.kind == "struct" or self.kind == "class":
            if other.kind != "struct" and other.kind != "class":
                return True
            else:
                if self.kind == "struct" and other.kind == "class":
                    return True
                elif self.kind == "class" and other.kind == "struct":
                    return False
                else:
                    return self.name < other.name
        # otherwise, sort based off the kind
        else:
            return self.kind < other.kind

    def findNestedNamespaces(self, lst):
        '''
        Recursive helper function for finding nested namespaces.  If this node is a
        namespace node, it is appended to ``lst``.  Each node also calls each of its
        child ``findNestedNamespaces`` with the same list.

        :Parameters:
            ``lst`` (list)
                The list each namespace node is to be appended to.
        '''
        if self.kind == "namespace":
            lst.append(self)
        for c in self.children:
            c.findNestedNamespaces(lst)

    def findNestedDirectories(self, lst):
        '''
        Recursive helper function for finding nested directories.  If this node is a
        directory node, it is appended to ``lst``.  Each node also calls each of its
        child ``findNestedDirectories`` with the same list.

        :Parameters:
            ``lst`` (list)
                The list each directory node is to be appended to.
        '''
        if self.kind == "dir":
            lst.append(self)
        for c in self.children:
            c.findNestedDirectories(lst)

    def findNestedClassLike(self, lst):
        '''
        Recursive helper function for finding nested classes and structs.  If this node
        is a class or struct, it is appended to ``lst``.  Each node also calls each of
        its child ``findNestedClassLike`` with the same list.

        :Parameters:
            ``lst`` (list)
                The list each class or struct node is to be appended to.
        '''
        if self.kind == "class" or self.kind == "struct":
            lst.append(self)
        for c in self.children:
            c.findNestedClassLike(lst)

    def findNestedEnums(self, lst):
        '''
        Recursive helper function for finding nested enums.  If this node is a class or
        struct it may have had an enum added to its child list.  When this occurred, the
        enum was removed from ``self.enums`` in the :class:`exhale.ExhaleRoot` class and
        needs to be rediscovered by calling this method on all of its children.  If this
        node is an enum, it is because a parent class or struct called this method, in
        which case it is added to ``lst``.

        **Note**: this is used slightly differently than nested directories, namespaces,
        and classes will be.  Refer to :func:`exhale.ExhaleRoot.generateNodeDocuments`
        function for details.

        :Parameters:
            ``lst`` (list)
                The list each enum is to be appended to.
        '''
        if self.kind == "enum":
            lst.append(self)
        for c in self.children:
            c.findNestedEnums(lst)

    def findNestedUnions(self, lst):
        '''
        Recursive helper function for finding nested unions.  If this node is a class or
        struct it may have had a union added to its child list.  When this occurred, the
        union was removed from ``self.unions`` in the :class:`exhale.ExhaleRoot` class
        and needs to be rediscovered by calling this method on all of its children.  If
        this node is a union, it is because a parent class or struct called this method,
        in which case it is added to ``lst``.

        **Note**: this is used slightly differently than nested directories, namespaces,
        and classes will be.  Refer to :func:`exhale.ExhaleRoot.generateNodeDocuments`
        function for details.

        :Parameters:
            ``lst`` (list)
                The list each union is to be appended to.
        '''
        if self.kind == "union":
            lst.append(self)
        for c in self.children:
            c.findNestedUnions(lst)

    def toConsole(self, level, printChildren=True):
        '''
        Debugging tool for printing hierarchies / ownership to the console.  Recursively
        calls children ``toConsole`` if this node is not a directory or a file, and
        ``printChildren == True``.

        :Parameters:
            ``level`` (int)
                The indentation level to be used, should be greater than or equal to 0.

            ``printChildren`` (bool)
                Whether or not the ``toConsole`` method for the children found in
                ``self.children`` should be called with ``level+1``.  Default is True,
                set to False for directories and files.
        '''
        indent = "  " * level
        print("{}- [{}]: {}".format(indent, self.kind, self.name))
        # files are children of directories, the file section will print those children
        if self.kind == "dir":
            for c in self.children:
                c.toConsole(level + 1, printChildren=False)
        elif printChildren:
            if self.kind == "file":
                print("{}[[[ location=\"{}\" ]]]".format("  " * (level + 1), self.location))
                for i in self.includes:
                    print("{}- #include <{}>".format("  " * (level + 1), i))
                for ref, name in self.included_by:
                    print("{}- included by: [{}]".format("  " * (level + 1), name))
                for n in self.namespaces_used:
                    n.toConsole(level + 1, printChildren=False)
                for c in self.children:
                    c.toConsole(level + 1)
            elif self.kind == "class" or self.kind == "struct":
                relevant_children = []
                for c in self.children:
                    if c.kind == "class" or c.kind == "struct" or \
                       c.kind == "enum"  or c.kind == "union":
                        relevant_children.append(c)

                for rc in sorted(relevant_children):
                    rc.toConsole(level + 1)
            elif self.kind != "union":
                for c in self.children:
                    c.toConsole(level + 1)

    def typeSort(self):
        '''
        Sorts ``self.children`` in place, and has each child sort its own children.
        Refer to :func:`exhale.ExhaleRoot.deepSortList` for more information on when
        this is necessary.
        '''
        self.children.sort()
        for c in self.children:
            c.typeSort()

    def inClassView(self):
        '''
        Whether or not this node should be included in the class view hierarchy.  Helper
        method for :func:`exhale.ExhaleNode.toClassView`.  Sets the member variable
        ``self.in_class_view`` to True if appropriate.

        :Return (bool):
            True if this node should be included in the class view --- either it is a
            node of kind ``struct``, ``class``, ``enum``, ``union``, or it is a
            ``namespace`` that one or more if its descendants was one of the previous
            four kinds.  Returns False otherwise.
        '''
        if self.kind == "namespace":
            for c in self.children:
                if c.inClassView():
                    return True
            return False
        else:
            # flag that this node is already in the class view so we can find the
            # missing top level nodes at the end
            self.in_class_view = True
            return self.kind == "struct" or self.kind == "class" or \
                   self.kind == "enum"   or self.kind == "union"

    def toClassView(self, level, stream, treeView, lastChild=False):
        '''
        Recursively generates the class view hierarchy using this node and its children,
        if it is determined by :func:`exhale.ExhaleNode.inClassView` that this node
        should be included.

        :Parameters:
            ``level`` (int)
                An integer greater than or equal to 0 representing the indentation level
                for this node.

            ``stream`` (cStringIO.StringIO)
                The stream that is being written to by all of the nodes (created and
                destroyed by the ExhaleRoot object).

            ``treeView`` (bool)
                If False, standard reStructuredText bulleted lists will be written to
                the ``stream``.  If True, then raw html unordered lists will be written
                to the ``stream``.

            ``lastChild`` (bool)
                When ``treeView == True``, the unordered lists generated need to have
                an <li class="lastChild"> tag on the last child for the
                ``collapsibleList`` to work correctly.  The default value of this
                parameter is False, and should only ever be set to True internally by
                recursive calls to this method.
        '''
        if self.inClassView():
            if not treeView:
                stream.write("{}- :ref:`{}`\n".format('    ' * level, self.link_name))
            else:
                indent = '  ' * (level * 2)
                if lastChild:
                    opening_li = '<li class="lastChild">'
                else:
                    opening_li = '<li>'
                # turn double underscores into underscores, then underscores into hyphens
                html_link = self.link_name.replace("__", "_").replace("_", "-")
                # should always have two parts
                title_as_link_parts = self.title.split(" ")
                qualifier = title_as_link_parts[0]
                link_title = title_as_link_parts[1]
                html_link = '{} <a href="{}.html#{}">{}</a>'.format(qualifier,
                                                                    self.file_name.split('.rst')[0],
                                                                    html_link,
                                                                    link_title)
                has_nested_children = False
                if self.kind == "class" or self.kind == "struct":
                    nested_enums      = []
                    nested_unions     = []
                    nested_class_like = []
                    # important: only scan self.children, do not use recursive findNested* methods
                    for c in self.children:
                        if c.kind == "enum":
                            nested_enums.append(c)
                        elif c.kind == "union":
                            nested_unions.append(c)
                        elif c.kind == "struct" or c.kind == "class":
                            nested_class_like.append(c)

                    has_nested_children = nested_enums or nested_unions or nested_class_like # <3 Python

                # if there are sub children, there needs to be a new html list generated
                if self.kind == "namespace" or has_nested_children:
                    next_indent = '  {}'.format(indent)
                    stream.write('{}{}\n{}{}\n{}<ul>\n'.format(indent, opening_li,
                                                               next_indent, html_link,
                                                               next_indent))
                else:
                    stream.write('{}{}{}</li>\n'.format(indent, opening_li, html_link))

            # include the relevant children (class like or nested namespaces / classes)
            if self.kind == "namespace":
                # pre-process and find everything that is relevant
                kids    = []
                nspaces = []
                for c in self.children:
                    if c.inClassView():
                        if c.kind == "namespace":
                            nspaces.append(c)
                        else:
                            kids.append(c)

                # always put nested namespaces last; parent dictates to the child if
                # they are the last child being printed
                kids.sort()
                num_kids = len(kids)

                nspaces.sort()
                num_nspaces = len(nspaces)

                last_child_index = num_kids + num_nspaces - 1
                child_idx = 0

                # first all of the child class like, then any nested namespaces
                for node in itertools.chain(kids, nspaces):
                    node.toClassView(level + 1, stream, treeView, child_idx == last_child_index)
                    child_idx += 1

                # now that all of the children haven been written, close the tags
                if treeView:
                    stream.write("  {}</ul>\n{}</li>\n".format(indent, indent))
            # current node is a class or struct with nested children
            elif has_nested_children:
                nested_class_like.sort()
                num_class_like = len(nested_class_like)

                nested_enums.sort()
                num_enums = len(nested_enums)

                nested_unions.sort()
                num_unions = len(nested_unions)

                last_child_index = num_class_like + num_enums + num_unions - 1
                child_idx = 0

                # first all of the classes / structs, then enums, then unions
                for node in itertools.chain(nested_class_like, nested_enums, nested_unions):
                    node.toClassView(level + 1, stream, treeView, child_idx == last_child_index)
                    child_idx += 1

                # now that all of the children haven been written, close the tags
                if treeView:
                    stream.write("  {}</ul>\n{}</li>\n".format(indent, indent))

    def inDirectoryView(self):
        '''
        Whether or not this node should be included in the file view hierarchy.  Helper
        method for :func:`exhale.ExhaleNode.toDirectoryView`.  Sets the member variable
        ``self.in_directory_view`` to True if appropriate.

        :Return (bool):
            True if this node should be included in the file view --- either it is a
            node of kind ``file``, or it is a ``dir`` that one or more if its
            descendants was a ``file``.  Returns False otherwise.
        '''
        if self.kind == "file":
            # flag that this file is already in the directory view so that potential
            # missing files can be found later.
            self.in_directory_view = True
            return True
        elif self.kind == "dir":
            for c in self.children:
                if c.inDirectoryView():
                    return True
        return False

    def toDirectoryView(self, level, stream, treeView, lastChild=False):
        '''
        Recursively generates the file view hierarchy using this node and its children,
        if it is determined by :func:`exhale.ExhaleNode.inDirectoryView` that this node
        should be included.

        :Parameters:
            ``level`` (int)
                An integer greater than or equal to 0 representing the indentation level
                for this node.

            ``stream`` (cStringIO.StringIO)
                The stream that is being written to by all of the nodes (created and
                destroyed by the ExhaleRoot object).

            ``treeView`` (bool)
                If False, standard reStructuredText bulleted lists will be written to
                the ``stream``.  If True, then raw html unordered lists will be written
                to the ``stream``.

            ``lastChild`` (bool)
                When ``treeView == True``, the unordered lists generated need to have
                an <li class="lastChild"> tag on the last child for the
                ``collapsibleList`` to work correctly.  The default value of this
                parameter is False, and should only ever be set to True internally by
                recursive calls to this method.
        '''
        if self.inDirectoryView():
            if not treeView:
                stream.write("{}- :ref:`{}`\n".format('    ' * level, self.link_name))
            else:
                indent = '  ' * (level * 2)
                if lastChild:
                    opening_li = '<li class="lastChild">'
                else:
                    opening_li = '<li>'
                # turn double underscores into underscores, then underscores into hyphens
                html_link = self.link_name.replace("__", "_").replace("_", "-")
                # should always have two parts
                title_as_link_parts = self.title.split(" ")
                qualifier = title_as_link_parts[0]
                link_title = title_as_link_parts[1]
                html_link = '{} <a href="{}.html#{}">{}</a>'.format(qualifier,
                                                                    self.file_name.split('.rst')[0],
                                                                    html_link,
                                                                    link_title)
                if self.kind == "dir":
                    next_indent = '  {}'.format(indent)
                    stream.write('{}{}\n{}{}\n{}<ul>\n'.format(indent, opening_li,
                                                               next_indent, html_link,
                                                               next_indent))
                else:
                    stream.write('{}{}{}</li>\n'.format(indent, opening_li, html_link))

            # include the relevant children (class like or nested namespaces)
            if self.kind == "dir":
                # pre-process and find everything that is relevant
                kids    = []
                dirs = []
                for c in self.children:
                    if c.inDirectoryView():
                        if c.kind == "dir":
                            dirs.append(c)
                        elif c.kind == "file":
                            kids.append(c)

                # always put nested namespaces last; parent dictates to the child if
                # they are the last child being printed
                kids.sort()
                num_kids = len(kids)

                dirs.sort()
                num_dirs = len(dirs)

                last_child_index = num_kids + num_dirs - 1
                child_idx = 0

                for k in kids:
                    k.toDirectoryView(level + 1, stream, treeView, child_idx == last_child_index)
                    child_idx += 1

                for n in dirs:
                    n.toDirectoryView(level + 1, stream, treeView, child_idx == last_child_index)
                    child_idx += 1

                # now that all of the children haven been written, close the tags
                if treeView:
                    stream.write("  {}</ul>\n{}</li>\n".format(indent, indent))


class ExhaleRoot:
    '''
    The full representation of the hierarchy graphs.  In addition to containing specific
    lists of ExhaleNodes of interest, the ExhaleRoot class is responsible for comparing
    the parsed breathe hierarchy and rebuilding lost relationships using the Doxygen
    xml files.  Once the graph parsing has finished, the ExhaleRoot generates all of the
    relevant reStructuredText documents and links them together.

    The ExhaleRoot class is not designed for reuse at this time.  If you want to
    generate a new hierarchy with a different directory or something, changing all of
    the right fields may be difficult and / or unsuccessful.  Refer to the bottom of the
    source code for :func:`exhale.generate` for safe usage (just exception handling),
    but the design of this class is to be used as follows:

    .. code-block:: py

       textRoot = ExhaleRoot(... args ...)
       textRoot.parse()
       textRoot.generateFullAPI()

    Zero checks are in place to enforce this usage, and if you are modifying the
    execution of this class and things are not working make sure you follow the ordering
    of those methods.

    :Parameters:
        ``breatheRoot`` (instance)
            Type unknown, this is the return value of ``breathe.breathe_parse``.

        ``rootDirectory`` (str)
            The name of the root directory to put everything in.  This should be the
            value of the key ``containmentFolder`` in the dictionary passed to
            :func:`exhale.generate`.

        ``rootFileName`` (str)
            The name of the file the root library api will be put into.  This should not
            contain the ``rootDirectory`` path.  This should be the value of the key
            ``rootFileName`` in the dictionary passed to :func:`exhale.generate`.

        ``rootFileTitle`` (str)
            The title to be written to the top of ``rootFileName``.  This should be the
            value of the key ``rootFileTitle`` in the dictionary passed to
            :func:`exhale.generate`.

        ``rootFileDescription`` (str)
            The description of the library api file placed after ``rootFileTitle``.
            This should be the value of the key ``afterTitleDescription`` in the
            dictionary passed to :func:`exhale.generate`.

        ``rootFileSummary`` (str)
            The summary of the library api placed after the generated hierarchy views.
            This should be the value of the key ``afterBodySummary`` in the dictionary
            passed to :func:`exhale.generate`.

        ``createTreeView`` (bool)
            Creates the raw html unordered lists for use with ``collapsibleList`` if
            True.  Otherwise, creates standard reStructuredText bulleted lists.  Should
            be the value of the key ``createTreeView`` in the dictionary passed to
            :func:`exhale.generate`.

    :Attributes:
        ``breathe_root`` (instance)
            The value of the parameter ``breatheRoot``.

        ``root_directory`` (str)
            The value of the parameter ``rootDirectory``.

        ``root_file_name`` (str)
            The value of the parameter ``rootFileName``.

        ``full_root_file_path`` (str)
            The full file path of the root file (``"root_directory/root_file_name"``).

        ``root_file_title`` (str)
            The value of the parameter ``rootFileTitle``.

        ``root_file_description`` (str)
            The value of the parameter ``rootFileDescription``.

        ``root_file_summary`` (str)
            The value of the parameter ``rootFileSummary``.

        ``class_view_file`` (str)
            The full file path the class view hierarchy will be written to.  This is
            incorporated into ``root_file_name`` using an ``.. include:`` directive.

        ``directory_view_file`` (str)
            The full file path the file view hierarchy will be written to.  This is
            incorporated into ``root_file_name`` using an ``.. include:`` directive.

        ``unabridged_api_file`` (str)
            The full file path the full API will be written to.  This is incorporated
            into ``root_file_name`` using a ``.. toctree:`` directive with a
            ``:maxdepth:`` according to the value of the key ``fullToctreeMaxDepth``
            in the dictionary passed into :func:`exhale.generate`.

        ``use_tree_view`` (bool)
            The value of the parameter ``createTreeView``.

        ``all_compounds`` (list)
            A list of all the Breathe compound objects discovered along the way.
            Populated during :func:`exhale.ExhaleRoot.discoverAllNodes`.

        ``all_nodes`` (list)
            A list of all of the ExhaleNode objects created.  Populated during
            :func:`exhale.ExhaleRoot.discoverAllNodes`.

        ``node_by_refid`` (dict)
            A dictionary with string ExhaleNode ``refid`` values, and values that are the
            ExhaleNode it came from.  Storing it this way is convenient for when the
            Doxygen xml file is being parsed.

        ``class_like`` (list)
            The full list of ExhaleNodes of kind ``struct`` or ``class``

        ``defines`` (list)
            The full list of ExhaleNodes of kind ``define``.

        ``enums`` (list)
            The full list of ExhaleNodes of kind ``enum``.

        ``enum_values`` (list)
            The full list of ExhaleNodes of kind ``enumvalue``.  Populated, not used.

        ``functions`` (list)
            The full list of ExhaleNodes of kind ``function``.

        ``dirs`` (list)
            The full list of ExhaleNodes of kind ``dir``.

        ``files`` (list)
            The full list of ExhaleNodes of kind ``file``.

        ``groups`` (list)
            The full list of ExhaleNodes of kind ``group``.  Pupulated, not used.

        ``namespaces`` (list)
            The full list of ExhaleNodes of kind ``namespace``.

        ``typedefs`` (list)
            The full list of ExhaleNodes of kind ``typedef``.

        ``unions`` (list)
            The full list of ExhaleNodes of kind ``union``.

        ``variables`` (list)
            The full list of ExhaleNodes of kind ``variable``.
    '''
    def __init__(self, breatheRoot, rootDirectory, rootFileName, rootFileTitle,
                 rootFileDescription, rootFileSummary, createTreeView):
        # the Breathe root object (main entry point to Breathe graph)
        self.breathe_root = breatheRoot

        # file generation location and root index data
        self.root_directory        = rootDirectory
        self.root_file_name        = rootFileName
        self.full_root_file_path   = "{}/{}".format(self.root_directory, self.root_file_name)
        self.root_file_title       = rootFileTitle
        self.root_file_description = rootFileDescription
        self.root_file_summary     = rootFileSummary
        self.class_view_file       = "{}.rst".format(
            self.full_root_file_path.replace(self.root_file_name, "class_view_hierarchy")
        )
        self.directory_view_file   = "{}.rst".format(
            self.full_root_file_path.replace(self.root_file_name, "directory_view_hierarchy")
        )
        self.unabridged_api_file   = "{}.rst".format(
            self.full_root_file_path.replace(self.root_file_name, "unabridged_api")
        )

        # whether or not we should generate the raw html tree view
        self.use_tree_view = createTreeView

        # track all compounds (from Breathe) to build all nodes (ExhaleNodes)
        self.all_compounds = [self.breathe_root.get_compound()]
        self.all_nodes = []

        # convenience lookup: keys are string Doxygen refid's, values are ExhaleNodes
        self.node_by_refid = {}

        # breathe directive    breathe kind
        #--------------------+----------------+
        # autodoxygenfile  <-+-> IGNORE       |
        # doxygenindex     <-+-> IGNORE       |
        # autodoxygenindex <-+-> IGNORE       |
        #--------------------+----------------+
        # doxygenclass     <-+-> "class"      |
        # doxygenstruct    <-+-> "struct"     |
        self.class_like      = [] #           |
        # doxygendefine    <-+-> "define"     |
        self.defines         = [] #           |
        # doxygenenum      <-+-> "enum"       |
        self.enums           = [] #           |
        # ---> largely ignored by framework,  |
        #      but stored if desired          |
        # doxygenenumvalue <-+-> "enumvalue"  |
        self.enum_values     = [] #           |
        # doxygenfunction  <-+-> "function"   |
        self.functions       = [] #           |
        # no directive     <-+-> "dir"        |
        self.dirs = []            #           |
        # doxygenfile      <-+-> "file"       |
        self.files           = [] #           |
        # not used, but could be supported in |
        # the future?                         |
        # doxygengroup     <-+-> "group"      |
        self.groups          = [] #           |
        # doxygennamespace <-+-> "namespace"  |
        self.namespaces      = [] #           |
        # doxygentypedef   <-+-> "typedef"    |
        self.typedefs        = [] #           |
        # doxygenunion     <-+-> "union"      |
        self.unions          = [] #           |
        # doxygenvariable  <-+-> "variable"   |
        self.variables       = [] #           |
        #-------------------------------------+

    ####################################################################################
    #
    ##
    ### Parsing
    ##
    #
    ####################################################################################
    def parse(self):
        '''
        The first method that should be called after creating an ExhaleRoot object.  The
        Breathe graph is parsed first, followed by the Doxygen xml documents.  By the
        end of this method, all of the ``self.<breathe_kind>``, ``self.all_compounds``,
        and ``self.all_nodes`` lists as well as the ``self.node_by_refid`` dictionary
        will be populated.  Lastly, this method sorts all of the internal lists.  The
        order of execution is exactly

        1. :func:`exhale.ExhaleRoot.discoverAllNodes`
        2. :func:`exhale.ExhaleRoot.reparentAll`
        3. Populate ``self.node_by_refid`` using ``self.all_nodes``.
        4. :func:`exhale.ExhaleRoot.fileRefDiscovery`
        5. :func:`exhale.ExhaleRoot.filePostProcess`
        6. :func:`exhale.ExhaleRoot.sortInternals`
        '''
        # Find and reparent everything from the Breathe graph.
        self.discoverAllNodes()
        self.reparentAll()

        # now that we have all of the nodes, store them in a convenient manner for refid
        # lookup when parsing the Doxygen xml files
        for n in self.all_nodes:
            self.node_by_refid[n.refid] = n

        # find missing relationships using the Doxygen xml files
        self.fileRefDiscovery()
        self.filePostProcess()

        # sort all of the lists we just built
        self.sortInternals()

    def discoverAllNodes(self):
        '''
        Stack based traversal of breathe graph, creates some parental relationships
        between different ExhaleNode objects.  Upon termination, this method will have
        populated the lists ``self.all_compounds``, ``self.all_nodes``, and the
        ``self.<breathe_kind>`` lists for different types of objects.
        '''
        # When you call the breathe_root.get_compound() method, it returns a list of the
        # top level source nodes.  These start out on the stack, and we add their
        # children if they have not already been visited before.
        nodes_remaining = [ExhaleNode(compound) for compound in self.breathe_root.get_compound()]
        while len(nodes_remaining) > 0:
            curr_node = nodes_remaining.pop()
            self.trackNodeIfUnseen(curr_node)
            self.discoverNeigbors(nodes_remaining, curr_node)

    def trackNodeIfUnseen(self, node):
        '''
        Helper method for :func:`exhale.ExhaleRoot.discoverAllNodes`.  If the node is
        not in self.all_nodes yet, add it to both self.all_nodes as well as the
        corresponding ``self.<breathe_kind>`` list.

        :Parameters:
            ``node`` (ExhaleNode)
                The node to begin tracking if not already present.
        '''
        if node not in self.all_nodes:
            self.all_nodes.append(node)
            if node.kind == "class" or node.kind == "struct":
                self.class_like.append(node)
            elif node.kind == "namespace":
                self.namespaces.append(node)
            elif node.kind == "enum":
                self.enums.append(node)
            elif node.kind == "enumvalue":
                self.enum_values.append(node)
            elif node.kind == "define":
                self.defines.append(node)
            elif node.kind == "file":
                self.files.append(node)
            elif node.kind == "dir":
                self.dirs.append(node)
            elif node.kind == "function":
                self.functions.append(node)
            elif node.kind == "variable":
                self.variables.append(node)
            elif node.kind == "group":
                self.groups.append(node)
            elif node.kind == "typedef":
                self.typedefs.append(node)
            elif node.kind == "union":
                self.unions.append(node)

    def discoverNeigbors(self, nodesRemaining, node):
        '''
        Helper method for :func:`exhale.ExhaleRoot.discoverAllNodes`.  Some of the
        compound objects received from Breathe have a member function ``get_member()``
        that returns all of the children.  Some do not.  This method checks to see if
        the method is present first, and if so performs the following::

            For every compound in node.compound.get_member():
                If compound not present in self.all_compounds:
                    - Add compound to self.all_compounds
                    - Create a child ExhaleNode
                    - If it is not a class, struct, or union, add to nodesRemaining
                    - If it is not an enumvalue, make it a child of node parameter

        :Parameters:
            ``nodesRemaining`` (list)
                The list of nodes representing the stack traversal being done by
                :func:`exhale.ExhaleRoot.discoverAllNodes`.  New neighbors found will
                be appended to this list.

            ``node`` (ExhaleNode)
                The node we are trying to discover potential new neighbors from.
        '''
        # discover neighbors of current node; some seem to not have get_member()
        if "member" in node.compound.__dict__:
            for member in node.compound.get_member():
                # keep track of every breathe compound we have seen
                if member not in self.all_compounds:
                    self.all_compounds.append(member)
                    # if we haven't seen this compound yet, make a node
                    child_node = ExhaleNode(member)
                    # if the current node is a class, struct, union, or enum ignore
                    # its variables, functions, etc
                    if node.kind == "class" or node.kind == "struct" or node.kind == "union":
                        if child_node.kind == "enum" or child_node.kind == "union":
                            nodesRemaining.append(child_node)
                    else:
                        nodesRemaining.append(child_node)
                    # the enum is presented separately, enumvals are haphazard and i hate them
                    # ... determining the enumvalue parent would be painful and i don't want to do it
                    if child_node.kind != "enumvalue":
                        node.children.append(child_node)
                        child_node.parent = node

    def reparentAll(self):
        '''
        Fixes some of the parental relationships lost in parsing the Breathe graph.
        File relationships are recovered in :func:`exhale.ExhaleRoot.fileRefDiscovery`.
        This method simply calls in this order:

        1. :func:`exhale.ExhaleRoot.reparentUnions`
        2. :func:`exhale.ExhaleRoot.reparentClassLike`
        3. :func:`exhale.ExhaleRoot.reparentDirectories`
        4. :func:`exhale.ExhaleRoot.renameToNamespaceScopes`
        5. :func:`exhale.ExhaleRoot.reparentNamespaces`
        '''
        self.reparentUnions()
        self.reparentClassLike()
        self.reparentDirectories()
        self.renameToNamespaceScopes()
        self.reparentNamespaces()

    def reparentUnions(self):
        '''
        Helper method for :func:`exhale.ExhaleRoot.reparentAll`.  Namespaces and classes
        should have the unions defined in them to be in the child list of itself rather
        than floating around.  Union nodes that are reparented (e.g. a union defined in
        a class) will be removed from the list ``self.unions`` since the Breathe
        directive for its parent (e.g. the class) will include the documentation for the
        union.  The consequence of this is that a union defined in a class will **not**
        appear in the full api listing of Unions.
        '''
        # unions declared in a class will not link to the individual union page, so
        # we will instead elect to remove these from the list of unions
        removals = []
        for u in self.unions:
            parts = u.name.split("::")
            num_parts = len(parts)
            if num_parts > 1:
                # it can either be a child of a namespace or a class_like
                if num_parts > 2:
                    namespace_name  = "::".join(p for p in parts[:-2])
                    potential_class = parts[-2]

                    # see if it belongs to a class like object first. if so, remove this
                    # union from the list of unions
                    reparented = False
                    for cl in self.class_like:
                        if cl.name == potential_class:
                            cl.children.append(u)
                            u.parent = cl
                            reparented = True
                            break

                    if reparented:
                        removals.append(u)
                        continue

                    # otherwise, see if it belongs to a namespace
                    alt_namespace_name = "{}::{}".format(namespace_name, potential_class)
                    for n in self.namespaces:
                        if namespace_name == n.name or alt_namespace_name == n.name:
                            n.children.append(u)
                            u.parent = n
                            break
                else:
                    name_or_class_name = "::".join(p for p in parts[:-1])

                    # see if it belongs to a class like object first. if so, remove this
                    # union from the list of unions
                    reparented = False
                    for cl in self.class_like:
                        if cl.name == name_or_class_name:
                            cl.children.append(u)
                            u.parent = cl
                            reparented = True
                            break

                    if reparented:
                        removals.append(u)
                        continue

                    # next see if it belongs to a namespace
                    for n in self.namespaces:
                        if n.name == name_or_class_name:
                            n.children.append(u)
                            u.parent = n
                            break

        # remove the unions from self.unions that were declared in class_like objects
        for rm in removals:
            self.unions.remove(rm)

    def reparentClassLike(self):
        '''
        Helper method for :func:`exhale.ExhaleRoot.reparentAll`.  Iterates over the
        ``self.class_like`` list and adds each object as a child to a namespace if the
        class, or struct is a member of that namespace.  Many classes / structs will be
        reparented to a namespace node, these will remain in ``self.class_like``.
        However, if a class or struct is reparented to a different class or struct (it
        is a nested class / struct), it *will* be removed from so that the class view
        hierarchy is generated correctly.
        '''
        removals = []
        for cl in self.class_like:
            parts = cl.name.split("::")
            if len(parts) > 1:
                # first try and reparent to namespaces
                namespace_name = "::".join(parts[:-1])
                parent_found = False
                for n in self.namespaces:
                    if n.name == namespace_name:
                        n.children.append(cl)
                        cl.parent = n
                        parent_found = True
                        break

                # if a namespace parent wasn not found, try and reparent to a class
                if not parent_found:
                    # parent class name would be namespace_name
                    for p_cls in self.class_like:
                        if p_cls.name == namespace_name:
                            p_cls.children.append(cl)
                            cl.parent = p_cls
                            removals.append(cl)
                            break

        for rm in removals:
            if rm in self.class_like:
                self.class_like.remove(rm)

    def reparentDirectories(self):
        '''
        Helper method for :func:`exhale.ExhaleRoot.reparentAll`.  Adds subdirectories as
        children to the relevant directory ExhaleNode.  If a node in ``self.dirs`` is
        added as a child to a different directory node, it is removed from the
        ``self.dirs`` list.
        '''
        dir_parts = []
        dir_ranks = []
        for d in self.dirs:
            parts = d.name.split("/")
            for p in parts:
                if p not in dir_parts:
                    dir_parts.append(p)
            dir_ranks.append((len(parts), d))

        traversal = sorted(dir_ranks)
        removals = []
        for rank, directory in reversed(traversal):
            # rank one means top level directory
            if rank < 2:
                break
            # otherwise, this is nested
            for p_rank, p_directory in reversed(traversal):
                if p_rank == rank - 1:
                    if p_directory.name == "/".join(directory.name.split("/")[:-1]):
                        p_directory.children.append(directory)
                        directory.parent = p_directory
                        if directory not in removals:
                            removals.append(directory)
                        break

        for rm in removals:
            self.dirs.remove(rm)

    def renameToNamespaceScopes(self):
        '''
        Helper method for :func:`exhale.ExhaleRoot.reparentAll`.  Some compounds in
        Breathe such as functions and variables do not have the namespace name they are
        declared in before the name of the actual compound.  This method prepends the
        appropriate (nested) namespace name before the name of any child that does not
        already have it.

        For example, the variable ``MAX_DEPTH`` declared in namespace ``external`` would
        have its ExhaleNode's ``name`` attribute changed from ``MAX_DEPTH`` to
        ``external::MAX_DEPTH``.
        '''
        for n in self.namespaces:
            namespace_name = "{}::".format(n.name)
            for child in n.children:
                if namespace_name not in child.name:
                    child.name = "{}{}".format(namespace_name, child.name)

    def reparentNamespaces(self):
        '''
        Helper method for :func:`exhale.ExhaleRoot.reparentAll`.  Adds nested namespaces
        as children to the relevant namespace ExhaleNode.  If a node in
        ``self.namespaces`` is added as a child to a different namespace node, it is
        removed from the ``self.namespaces`` list.  Because these are removed from
        ``self.namespaces``, it is important that
        :func:`exhale.ExhaleRoot.renameToNamespaceScopes` is called before this method.
        '''
        namespace_parts = []
        namespace_ranks = []
        for n in self.namespaces:
            parts = n.name.split("::")
            for p in parts:
                if p not in namespace_parts:
                    namespace_parts.append(p)
            namespace_ranks.append((len(parts), n))

        traversal = sorted(namespace_ranks)
        removals = []
        for rank, namespace in reversed(traversal):
            # rank one means top level namespace
            if rank < 2:
                break
            # otherwise, this is nested
            for p_rank, p_namespace in reversed(traversal):
                if p_rank == rank - 1:
                    if p_namespace.name == "::".join(namespace.name.split("::")[:-1]):
                        p_namespace.children.append(namespace)
                        namespace.parent = p_namespace
                        if namespace not in removals:
                            removals.append(namespace)
                        break

        for rm in removals:
            self.namespaces.remove(rm)

    def fileRefDiscovery(self):
        '''
        Finds the missing components for file nodes by parsing the Doxygen xml (which is
        just the ``doxygen_output_dir/node.refid``).  Additional items parsed include
        adding items whose ``refid`` tag are used in this file, the <programlisting> for
        the file, what it includes and what includes it, as well as the location of the
        file (with respsect to the *Doxygen* root).

        Care must be taken to only include a refid found with specific tags.  The
        parsing of the xml file was done by just looking at some example outputs.  It
        seems to be working correctly, but there may be some subtle use cases that break
        it.

        .. warning::
            Some enums, classes, variables, etc declared in the file will not have their
            associated refid in the declaration of the file, but will be present in the
            <programlisting>.  These are added to the files' list of children when they
            are found, but this parental relationship cannot be formed if you set
            ``XML_PROGRAMLISTING = NO`` with Doxygen.  An example of such an enum would
            be an enum declared inside of a namespace within this file.
        '''
        if EXHALE_API_DOXY_OUTPUT_DIR == "":
            exclaimError("The doxygen xml output directory was not specified!")
            return
        # parse the doxygen xml file and extract all refid's put in it
        # keys: file object, values: list of refid's
        doxygen_xml_file_ownerships = {}
        # innerclass, innernamespace, etc
        ref_regex    = re.compile(r'.*<inner.*refid="(\w+)".*')
        # what files this file includes
        inc_regex    = re.compile(r'.*<includes.*>(.+)</includes>')
        # what files include this file
        inc_by_regex = re.compile(r'.*<includedby refid="(\w+)".*>(.*)</includedby>')
        # the actual location of the file
        loc_regex    = re.compile(r'.*<location file="(.*)"/>')

        for f in self.files:
            doxygen_xml_file_ownerships[f] = []
            try:
                doxy_xml_path = "{}{}.xml".format(EXHALE_API_DOXY_OUTPUT_DIR, f.refid)
                with open(doxy_xml_path, "r") as doxy_file:
                    processing_code_listing = False # shows up at bottom of xml
                    for line in doxy_file:
                        # see if this line represents the location tag
                        match = loc_regex.match(line)
                        if match is not None:
                            f.location = match.groups()[0]
                            continue

                        if not processing_code_listing:
                            # gather included by references
                            match = inc_by_regex.match(line)
                            if match is not None:
                                ref, name = match.groups()
                                f.included_by.append((ref, name))
                                continue
                            # gather includes lines
                            match = inc_regex.match(line)
                            if match is not None:
                                inc = match.groups()[0]
                                f.includes.append(inc)
                                continue
                            # gather any classes, namespaces, etc declared in the file
                            match = ref_regex.match(line)
                            if match is not None:
                                match_refid = match.groups()[0]
                                if match_refid in self.node_by_refid:
                                    doxygen_xml_file_ownerships[f].append(match_refid)
                                continue
                            # lastly, see if we are starting the code listing
                            if "<programlisting>" in line:
                                processing_code_listing = True
                        elif processing_code_listing:
                            if "</programlisting>" in line:
                                processing_code_listing = False
                            else:
                                f.program_listing.append(line)
            except:
                exclaimError("Unable to process doxygen xml for file [{}].\n".format(f.name))

        #
        # IMPORTANT: do not set the parent field of anything being added as a child to the file
        #

        # hack to make things work right on RTD
        if EXHALE_API_DOXYGEN_STRIP_FROM_PATH is not None:
            for f in self.files:
                f.location = f.location.replace(EXHALE_API_DOXYGEN_STRIP_FROM_PATH, "")
                if f.location[0] == "/":
                    f.location = f.location[1:]

        # now that we have parsed all the listed refid's in the doxygen xml, reparent
        # the nodes that we care about
        for f in self.files:
            for match_refid in doxygen_xml_file_ownerships[f]:
                child = self.node_by_refid[match_refid]
                if child.kind == "struct" or child.kind == "class" or child.kind == "function" or \
                   child.kind == "typedef" or child.kind == "define" or child.kind == "enum"   or \
                   child.kind == "union":
                    already_there = False
                    for fc in f.children:
                        if child.name == fc.name:
                            already_there = True
                            break
                    if not already_there:
                        # special treatment for unions: ignore if it is a class union
                        if child.kind == "union":
                            for u in self.unions:
                                if child.name == u.name:
                                    f.children.append(child)
                                    break
                        else:
                            f.children.append(child)
                elif child.kind == "namespace":
                    already_there = False
                    for fc in f.namespaces_used:
                        if child.name == fc.name:
                            already_there = True
                            break
                    if not already_there:
                        f.namespaces_used.append(child)

        # last but not least, some different kinds declared in the file that are scoped
        # in a namespace they will show up in the programlisting, but not at the toplevel.
        for f in self.files:
            potential_orphans = []
            for n in f.namespaces_used:
                for child in n.children:
                    if child.kind == "enum"     or child.kind == "variable" or \
                       child.kind == "function" or child.kind == "typedef"  or \
                       child.kind == "union":
                        potential_orphans.append(child)

            # now that we have a list of potential orphans, see if this doxygen xml had
            # the refid of a given child present.
            for orphan in potential_orphans:
                unresolved_name = orphan.name.split("::")[-1]
                if f.refid in orphan.refid and any(unresolved_name in line for line in f.program_listing):
                    if orphan not in f.children:
                        f.children.append(orphan)

    def filePostProcess(self):
        '''
        The real name of this method should be ``reparentFiles``, but to avoid confusion
        with what stage this must happen at it is called this instead.  After the
        :func:`exhale.ExhaleRoot.fileRefDiscovery` method has been called, each file
        will have its location parsed.  This method reparents files to directories
        accordingly, so the file view hierarchy can be complete.
        '''
        for f in self.files:
            dir_loc_parts = f.location.split("/")[:-1]
            num_parts = len(dir_loc_parts)
            # nothing to do, at the top level
            if num_parts == 0:
                continue

            dir_path = "/".join(p for p in dir_loc_parts)
            nodes_remaining = [d for d in self.dirs]
            while len(nodes_remaining) > 0:
                d = nodes_remaining.pop()
                if d.name in dir_path:
                    # we have found the directory we want
                    if d.name == dir_path:
                        d.children.append(f)
                        f.parent = d
                        break
                    # otherwise, try and find an owner
                    else:
                        nodes_remaining = []
                        for child in d.children:
                            if child.kind == "dir":
                                nodes_remaining.append(child)

    def sortInternals(self):
        '''
        Sort all internal lists (``class_like``, ``namespaces``, ``variables``, etc)
        mostly how doxygen would, alphabetical but also hierarchical (e.g. structs
        appear before classes in listings).  Some internal lists are just sorted, and
        some are deep sorted (:func:`exhale.ExhaleRoot.deepSortList`).
        '''
        # some of the lists only need to be sorted, some of them need to be sorted and
        # have each node sort its children
        # leaf-like lists: no child sort
        self.defines.sort()
        self.enums.sort()
        self.enum_values.sort()
        self.functions.sort()
        self.groups.sort()
        self.typedefs.sort()
        self.variables.sort()

        # hierarchical lists: sort children
        self.deepSortList(self.class_like)
        self.deepSortList(self.namespaces)
        self.deepSortList(self.unions)
        self.deepSortList(self.files)
        self.deepSortList(self.dirs)

    def deepSortList(self, lst):
        '''
        For hierarchical internal lists such as ``namespaces``, we want to sort both the
        list as well as have each child sort its children by calling
        :func:`exhale.ExhaleNode.typeSort`.

        :Parameters:
            ``lst`` (list)
                The list of ExhaleNode objects to be deep sorted.
        '''
        lst.sort()
        for l in lst:
            l.typeSort()

    ####################################################################################
    #
    ##
    ### Library generation.
    ##
    #
    ####################################################################################
    def generateFullAPI(self):
        '''
        Since we are not going to use some of the breathe directives (e.g. namespace or
        file), when representing the different views of the generated API we will need:

        1. Generate a single file restructured text document for all of the nodes that
           have either no children, or children that are leaf nodes.
        2. When building the view hierarchies (class view and file view), provide a link
           to the appropriate files generated previously.

        If adding onto the framework to say add another view (from future import groups)
        you would link from a restructured text document to one of the individually
        generated files using the value of ``link_name`` for a given ExhaleNode object.

        This method calls in this order:

        1. :func:`exhale.ExhaleRoot.generateAPIRootHeader`
        2. :func:`exhale.ExhaleRoot.generateNodeDocuments`
        3. :func:`exhale.ExhaleRoot.generateAPIRootBody`
        4. :func:`exhale.ExhaleRoot.generateAPIRootSummary`
        '''
        self.generateAPIRootHeader()
        self.generateNodeDocuments()
        self.generateAPIRootBody()
        self.generateAPIRootSummary()

    def generateAPIRootHeader(self):
        '''
        This method creates the root library api file that will include all of the
        different hierarchy views and full api listing.  If ``self.root_directory`` is
        not a current directory, it is created first.  Afterward, the root API file is
        created and its title is written, as well as the value of
        ``self.root_file_description``.
        '''
        try:
            if not os.path.isdir(self.root_directory):
                os.mkdir(self.root_directory)
        except Exception as e:
            exclaimError("Cannot create the directory: {}\nError message: {}".format(self.root_directory, e))
            raise Exception("Fatal error generating the api root, cannot continue.")
        try:
            with open(self.full_root_file_path, "w") as generated_index:
                generated_index.write("{}\n{}\n\n{}\n\n".format(
                    self.root_file_title, EXHALE_FILE_HEADING, self.root_file_description)
                )
        except:
            exclaimError("Unable to create the root api file / header: {}".format(self.full_root_file_path))
            raise Exception("Fatal error generating the api root, cannot continue.")

    def generateNodeDocuments(self):
        '''
        Creates all of the reStructuredText documents related to types parsed by
        Doxygen.  This includes all leaf-like documents (``class``, ``struct``,
        ``enum``, ``typedef``, ``union``, ``variable``, and ``define``), as well as
        namespace, file, and directory pages.

        During the reparenting phase of the parsing process, nested items were added as
        a child to their actual parent.  For classes, structs, enums, and unions, if
        it was reparented to a ``namespace`` it will *remain* in its respective
        ``self.<breathe_kind>`` list.  However, if it was an internally declared child
        of a class or struct (nested classes, structs, enums, and unions), this node
        will be removed from its ``self.<breathe_kind>`` list to avoid duplication in
        the class hierarchy generation.

        When generating the full API, though, we will want to include all of these and
        therefore must call :func:`exhale.ExhaleRoot.generateSingleNodeRST` with all of
        the nested items.  For nested classes and structs, this is done by just calling
        ``node.findNestedClassLike`` for every node in ``self.class_like``.  The
        resulting list then has all of ``self.class_like``, as well as any nested
        classes and structs found.  With ``enum`` and ``union``, these would have been
        reparented to a **class** or **struct** if it was removed from the relevant
        ``self.<breathe_kind>`` list.  Meaning we must make sure that we genererate the
        single node RST documents for everything by finding the nested enums and unions
        from ``self.class_like``, as well as everything in ``self.enums`` and
        ``self.unions``.
        '''
        # initialize all of the nodes
        for node in self.all_nodes:
            self.initializeNodeFilenameAndLink(node)

        # find the potentially nested items that were reparented
        nested_enums      = []
        nested_unions     = []
        nested_class_like = []
        for cl in self.class_like:
            cl.findNestedEnums(nested_enums)
            cl.findNestedUnions(nested_unions)
            cl.findNestedClassLike(nested_class_like)

        # generate all of the leaf-like documents
        for node in itertools.chain(nested_class_like, self.enums, nested_enums,
                                    self.unions, nested_unions, self.functions,
                                    self.typedefs, self.variables, self.defines):
            self.generateSingleNodeRST(node)

        # generate the remaining parent-like documents
        self.generateNamespaceNodeDocuments()
        self.generateFileNodeDocuments()
        self.generateDirectoryNodeDocuments()

    def initializeNodeFilenameAndLink(self, node):
        '''
        Sets the ``file_name`` and ``link_name`` for the specified node.  If the kind
        of this node is "file", then this method will also set the ``program_file``
        as well as the ``program_link_name`` fields.

        Since we are operating inside of a ``containmentFolder``, this method **will**
        include ``self.root_directory`` in this path so that you can just use::

            with open(node.file_name, "w") as gen_file:
                ... write the file ...

        Having the ``containmentFolder`` is important for when we want to generate the
        file, but when we want to use it with ``include`` or ``toctree`` this will
        need to change.  Refer to :func:`exhale.ExhaleRoot.gerrymanderNodeFilenames`.

        This method also sets the value of ``node.title``, which will be used in both
        the reStructuredText document of the node as well as the links generated in the
        class view hierarchy (<a href="..."> for the ``createTreeView = True`` option).

        :type:  exhale.ExhaleNode
        :param: node
            The node that we are setting the above information for.
        '''
        # create the file and link names
        html_safe_name = node.name.replace(":", "_").replace("/", "_")
        node.file_name = "{}/exhale_{}_{}.rst".format(self.root_directory, node.kind, html_safe_name)
        node.link_name = "{}_{}".format(qualifyKind(node.kind).lower(), html_safe_name)
        if node.kind == "file":
            # account for same file name in different directory
            html_safe_name = node.location.replace("/", "_")
            node.file_name = "{}/exhale_{}_{}.rst".format(self.root_directory, node.kind, html_safe_name)
            node.link_name = "{}_{}".format(qualifyKind(node.kind).lower(), html_safe_name)
            node.program_file = "{}/exhale_program_listing_file_{}.rst".format(
                self.root_directory, html_safe_name
            )
            node.program_link_name = "program_listing_file_{}".format(html_safe_name)

        # create the title for this node.
        if node.kind == "dir":
            title = node.name.split("/")[-1]
        # breathe does not prepend the namespace for variables and typedefs, so
        # I choose to leave the fully qualified name in the title for added clarity
        elif node.kind == "variable" or node.kind == "typedef":
            title = node.name
        else:
            #
            # :TODO: This is probably breaking template specializations, need to redo
            #        the html_safe_name, file_name, and link_name to account for these
            #        as well as include documentation for how to link to partial
            #        template specializations.
            #
            #        That is, need to do something like
            #
            #        html_safe_name = node.name.replace(":", "_")
            #                                  .replace("/", "_")
            #                                  .replace(" ", "_")
            #                                  .replace("<", "LT_")
            #                                  .replace(">", "_GT")
            #
            #        Or something like that...
            #
            first_lt = node.name.find("<")
            last_gt  = node.name.rfind(">")
            # dealing with a template, special treatment necessary
            if first_lt > -1 and last_gt > -1:
                title = "{}{}".format(
                    node.name[:first_lt].split("::")[-1], # remove namespaces
                    node.name[first_lt:last_gt + 1]       # template params
                )
                html_safe_name = title.replace(":", "_").replace("/", "_").replace(" ", "_").replace("<", "LT_").replace(">", "_GT").replace(",", "")
                node.file_name = "{}/exhale_{}_{}.rst".format(self.root_directory, node.kind, html_safe_name)
                node.link_name = "{}_{}".format(qualifyKind(node.kind).lower(), html_safe_name)
                if node.kind == "file":
                    node.program_file = "{}/exhale_program_listing_file_{}.rst".format(
                        self.root_directory, html_safe_name
                    )
                    node.program_link_name = "program_listing_file_{}".format(html_safe_name)
            else:
                title = node.name.split("::")[-1]

            # additionally, I feel that nested classes should have their fully qualified
            # name without namespaces for clarity
            prepend_parent = False
            if node.kind == "class" or node.kind == "struct" or node.kind == "enum" or node.kind == "union":
                if node.parent is not None and (node.parent.kind == "class" or node.parent.kind == "struct"):
                    prepend_parent = True
            if prepend_parent:
                title = "{}::{}".format(node.parent.name.split("::")[-1], title)
        node.title = "{} {}".format(qualifyKind(node.kind), title)

    def generateSingleNodeRST(self, node):
        '''
        Creates the reStructuredText document for the leaf like node object.  This
        method should only be used with nodes in the following member lists:

        - ``self.class_like``
        - ``self.enums``
        - ``self.functions``
        - ``self.typedefs``
        - ``self.unions``
        - ``self.variables``
        - ``self.defines``

        File, directory, and namespace nodes are treated separately.

        :Parameters:
            ``node`` (ExhaleNode)
                The leaf like node being generated by this method.
        '''
        try:
            with open(node.file_name, "w") as gen_file:
                # generate a link label for every generated file
                link_declaration = ".. _{}:\n\n".format(node.link_name)
                header = "{}\n{}\n\n".format(node.title, EXHALE_FILE_HEADING)
                # link back to the file this was defined in
                file_included = False
                for f in self.files:
                    if node in f.children:
                        if file_included:
                            raise RuntimeError("Critical error: this node is parented to multiple files.\n\nNode: {}".format(node.name))
                        header = "{}- Defined in :ref:`{}`\n\n".format(header, f.link_name)
                        file_included = True
                # if this is a nested type, link back to its parent
                if node.parent is not None and (node.parent.kind == "struct" or node.parent.kind == "class"):
                    # still a chance to recover if the parent worked. probably doesn't work past one layer
                    # TODO: create like quadruple nested classes and find a way to reverse upward. parent links
                    #       should just be class or struct until it is a namespace or file?
                    if not file_included:
                        parent_traverser = node.parent
                        while parent_traverser is not None:
                            for f in self.files:
                                if node.parent in f.children:
                                    if file_included:
                                        raise RuntimeError("Critical error: this node is parented to multiple files.\n\nNode: {}".format(node.name))
                                    header = "{}- Defined in :ref:`{}`\n\n".format(header, f.link_name)
                                    file_included = True
                                    if node not in f.children:
                                        f.children.append(node)
                            if file_included:
                                parent_traverser = None
                            else:
                                parent_traverser = parent_traverser.parent

                    header = "{}- Nested type of :ref:`{}`\n\n".format(header, node.parent.link_name)
                # if this has nested types, link to them
                if node.kind == "class" or node.kind == "struct":
                    nested_children = []
                    for c in node.children:
                        c.findNestedEnums(nested_children)
                        c.findNestedUnions(nested_children)
                        c.findNestedClassLike(nested_children)

                    if nested_children:
                        # build up a list of links, custom sort function will force
                        # double nested and beyond to appear after their parent by
                        # sorting on their name
                        nested_children.sort(key=lambda x: x.name)
                        nested_child_stream = cStringIO.StringIO()
                        for nc in nested_children:
                            nested_child_stream.write("- :ref:`{}`\n".format(nc.link_name))

                        # extract the list of links and add them as a subsection in the header
                        nested_child_string = nested_child_stream.getvalue()
                        nested_child_stream.close()
                        header = "{}**Nested Types**:\n\n{}\n\n".format(header, nested_child_string)

                # inject the appropriate doxygen directive and name of this node
                directive = ".. {}:: {}\n".format(kindAsBreatheDirective(node.kind), node.name)
                # include any specific directives for this doxygen directive
                specifications = "{}\n\n".format(specificationsForKind(node.kind))
                gen_file.write("{}{}{}{}".format(link_declaration, header, directive, specifications))
        except:
            exclaimError("Critical error while generating the file for [{}]".format(node.file_name))

    def generateNamespaceNodeDocuments(self):
        '''
        Generates the reStructuredText document for every namespace, including nested
        namespaces that were removed from ``self.namespaces`` (but added as children
        to one of the namespaces in ``self.namespaces``).

        The documents generated do not use the Breathe namespace directive, but instead
        link to the relevant documents associated with this namespace.
        '''
        # go through all of the top level namespaces
        for n in self.namespaces:
            # find any nested namespaces
            nested_namespaces = []
            for child in n.children:
                child.findNestedNamespaces(nested_namespaces)
            # generate the children first
            for nested in reversed(sorted(nested_namespaces)):
                self.generateSingleNamespace(nested)
            # generate this top level namespace
            self.generateSingleNamespace(n)

    def generateSingleNamespace(self, nspace):
        '''
        Helper method for :func:`exhale.ExhaleRoot.generateNamespaceNodeDocuments`.
        Writes the reStructuredText file for the given namespace.

        :Parameters:
            ``nspace`` (ExhaleNode)
                The namespace node to create the reStructuredText document for.
        '''
        try:
            with open(nspace.file_name, "w") as gen_file:
                # generate a link label for every generated file
                link_declaration = ".. _{}:\n\n".format(nspace.link_name)
                # every generated file must have a header for sphinx to be happy
                nspace.title = "{} {}".format(qualifyKind(nspace.kind), nspace.name)
                header = "{}\n{}\n\n".format(nspace.title, EXHALE_FILE_HEADING)
                # generate the headings and links for the children
                children_string = self.generateNamespaceChildrenString(nspace)
                # write it all out
                gen_file.write("{}{}{}\n\n".format(link_declaration, header, children_string))
        except:
            exclaimError("Critical error while generating the file for [{}]".format(nspace.file_name))

    def generateNamespaceChildrenString(self, nspace):
        '''
        Helper method for :func:`exhale.ExhaleRoot.generateSingleNamespace`, and
        :func:`exhale.ExhaleRoot.generateFileNodeDocuments`.  Builds the
        body text for the namespace node document that links to all of the child
        namespaces, structs, classes, functions, typedefs, unions, and variables
        associated with this namespace.

        :Parameters:
            ``nspace`` (ExhaleNode)
                The namespace node we are generating the body text for.

        :Return (str):
            The string to be written to the namespace node's reStructuredText document.
        '''
        # sort the children
        nsp_namespaces        = []
        nsp_nested_class_like = []
        nsp_enums             = []
        nsp_functions         = []
        nsp_typedefs          = []
        nsp_unions            = []
        nsp_variables         = []
        for child in nspace.children:
            if child.kind == "namespace":
                nsp_namespaces.append(child)
            elif child.kind == "struct" or child.kind == "class":
                child.findNestedClassLike(nsp_nested_class_like)
                child.findNestedEnums(nsp_enums)
                child.findNestedUnions(nsp_unions)
            elif child.kind == "enum":
                nsp_enums.append(child)
            elif child.kind == "function":
                nsp_functions.append(child)
            elif child.kind == "typedef":
                nsp_typedefs.append(child)
            elif child.kind == "union":
                nsp_unions.append(child)
            elif child.kind == "variable":
                nsp_variables.append(child)

        # generate their headings if they exist (no Defines...that's not a C++ thing...)
        children_string = self.generateSortedChildListString("Namespaces", "", nsp_namespaces)
        children_string = self.generateSortedChildListString("Classes", children_string, nsp_nested_class_like)
        children_string = self.generateSortedChildListString("Enums", children_string, nsp_enums)
        children_string = self.generateSortedChildListString("Functions", children_string, nsp_functions)
        children_string = self.generateSortedChildListString("Typedefs", children_string, nsp_typedefs)
        children_string = self.generateSortedChildListString("Unions", children_string, nsp_unions)
        children_string = self.generateSortedChildListString("Variables", children_string, nsp_variables)

        return children_string

    def generateSortedChildListString(self, sectionTitle, previousString, lst):
        '''
        Helper method for :func:`exhale.ExhaleRoot.generateNamespaceChildrenString`.
        Used to build up a continuous string with all of the children separated out into
        titled sections.

        This generates a new titled section with ``sectionTitle`` and puts a link to
        every node found in ``lst`` in this section.  The newly created section is
        appended to ``previousString`` and then returned.

        :TODO:
            Change this to use string streams like the other methods instead.

        :Parameters:
            ``sectionTitle`` (str)
                The title of the section for this list of children.

            ``previousString`` (str)
                The string to append the newly created section to.

            ``lst`` (list)
                A list of ExhaleNode objects that are to be linked to from this section.
                This method sorts ``lst`` in place.
        '''
        if lst:
            lst.sort()
            new_string = "{}\n\n{}\n{}\n".format(previousString, sectionTitle, EXHALE_SECTION_HEADING)
            for l in lst:
                new_string = "{}\n- :ref:`{}`".format(new_string, l.link_name)
            return new_string
        else:
            return previousString

    def generateFileNodeDocuments(self):
        '''
        Generates the reStructuredText documents for files as well as the file's
        program listing reStructuredText document if applicable.  Refer to
        :ref:`usage_customizing_file_pages` for changing the output of this method.
        The remainder of the file lists all nodes that have been discovered to be
        defined (e.g. classes) or referred to (e.g. included files or files that include
        this file).
        '''
        for f in self.files:
            # if the programlisting was included, length will be at least 1 line
            if len(f.program_listing) > 0:
                include_program_listing = True
                full_program_listing = '.. code-block:: cpp\n\n'

                # need to reformat each line to remove xml tags / put <>& back in
                for pgf_line in f.program_listing:
                    fixed_whitespace = re.sub(r'<sp/>', ' ', pgf_line)
                    # for our purposes, this is good enough:
                    #     http://stackoverflow.com/a/4869782/3814202
                    no_xml_tags  = re.sub(r'<[^<]+?>', '', fixed_whitespace)
                    revive_lt    = re.sub(r'&lt;', '<', no_xml_tags)
                    revive_gt    = re.sub(r'&gt;', '>', revive_lt)
                    revive_quote = re.sub(r'&quot;', '"', revive_gt)
                    revive_apos  = re.sub(r'&apos;', "'", revive_quote)
                    revive_amp   = re.sub(r'&amp;', '&', revive_apos)
                    full_program_listing = "{}   {}".format(full_program_listing, revive_amp)

                # create the programlisting file
                try:
                    with open(f.program_file, "w") as gen_file:
                        # generate a link label for every generated file
                        link_declaration = ".. _{}:\n\n".format(f.program_link_name)
                        # every generated file must have a header for sphinx to be happy
                        prog_title = "Program Listing for {} {}".format(qualifyKind(f.kind), f.name)
                        header = "{}\n{}\n\n".format(prog_title, EXHALE_FILE_HEADING)
                        return_link = "- Return to documentation for :ref:`{}`\n\n".format(f.link_name)
                        # write it all out
                        gen_file.write("{}{}{}{}\n\n".format(
                            link_declaration, header, return_link, full_program_listing)
                        )
                except:
                    exclaimError("Critical error while generating the file for [{}]".format(f.file_name))
            else:
                include_program_listing = False

        for f in self.files:
            if len(f.location) > 0:
                file_definition = "Definition (``{}``)\n{}\n\n".format(
                    f.location, EXHALE_SECTION_HEADING
                )
            else:
                file_definition = ""

            if include_program_listing and file_definition != "":
                file_definition = "{}.. toctree::\n   :maxdepth: 1\n\n   {}\n\n".format(
                    file_definition, f.program_file.split("/")[-1] # file path still has directory
                )

            if len(f.includes) > 0:
                file_includes = "Includes\n{}\n\n".format(EXHALE_SECTION_HEADING)
                for incl in sorted(f.includes):
                    local_file = None
                    for incl_file in self.files:
                        if incl in incl_file.location:
                            local_file = incl_file
                            break
                    if local_file is not None:
                        file_includes = "{}- ``{}`` (:ref:`{}`)\n".format(
                            file_includes, incl, local_file.link_name
                        )
                    else:
                        file_includes = "{}- ``{}``\n".format(file_includes, incl)
            else:
                file_includes = ""

            if len(f.included_by) > 0:
                file_included_by = "Included By\n{}\n\n".format(EXHALE_SECTION_HEADING)
                for incl_ref, incl_name in f.included_by:
                    for incl_file in self.files:
                        if incl_ref == incl_file.refid:
                            file_included_by = "{}- :ref:`{}`\n".format(file_included_by, incl_file.link_name)
                            break
            else:
                file_included_by = ""

            # generate their headings if they exist --- DO NOT USE findNested*, these are included recursively
            file_structs    = []
            file_classes    = []
            file_enums      = []
            file_functions  = []
            file_typedefs   = []
            file_unions     = []
            file_variables  = []
            file_defines    = []
            for child in f.children:
                if child.kind == "struct":
                    file_structs.append(child)
                elif child.kind == "class":
                    file_classes.append(child)
                elif child.kind == "enum":
                    file_enums.append(child)
                elif child.kind == "function":
                    file_functions.append(child)
                elif child.kind == "typedef":
                    file_typedefs.append(child)
                elif child.kind == "union":
                    file_unions.append(child)
                elif child.kind == "variable":
                    file_variables.append(child)
                elif child.kind == "define":
                    file_defines.append(child)
            children_string = self.generateSortedChildListString("Namespaces", "", f.namespaces_used)
            children_string = self.generateSortedChildListString("Classes", children_string, file_structs + file_classes)
            children_string = self.generateSortedChildListString("Enums", children_string, file_enums)
            children_string = self.generateSortedChildListString("Functions", children_string, file_functions)
            children_string = self.generateSortedChildListString("Defines", children_string, file_defines)
            children_string = self.generateSortedChildListString("Typedefs", children_string, file_typedefs)
            children_string = self.generateSortedChildListString("Unions", children_string, file_unions)
            children_string = self.generateSortedChildListString("Variables", children_string, file_variables)

            try:
                with open(f.file_name, "w") as gen_file:
                    # generate a link label for every generated file
                    link_declaration = ".. _{}:\n\n".format(f.link_name)
                    # every generated file must have a header for sphinx to be happy
                    f.title = "{} {}".format(qualifyKind(f.kind), f.name)
                    header = "{}\n{}\n\n".format(f.title, EXHALE_FILE_HEADING)
                    # write it all out
                    gen_file.write("{}{}{}{}\n{}\n{}\n\n".format(
                        link_declaration, header, file_definition, file_includes, file_included_by, children_string)
                    )
            except:
                exclaimError("Critical error while generating the file for [{}]".format(f.file_name))

            if EXHALE_GENERATE_BREATHE_FILE_DIRECTIVES:
                try:
                    with open(f.file_name, "a") as gen_file:
                        # add the breathe directive ???
                        gen_file.write(
                            "\nFull File Listing\n{}\n\n"
                            ".. {}:: {}\n"
                            "{}\n\n".format(EXHALE_SECTION_HEADING, kindAsBreatheDirective(f.kind), f.location, specificationsForKind(f.kind))
                        )

                except:
                    exclaimError("Critical error while generating the breathe directive for [{}]".format(f.file_name))

    def generateDirectoryNodeDocuments(self):
        '''
        Generates all of the directory reStructuredText documents.
        '''
        all_dirs = []
        for d in self.dirs:
            d.findNestedDirectories(all_dirs)

        for d in all_dirs:
            self.generateDirectoryNodeRST(d)

    def generateDirectoryNodeRST(self, node):
        '''
        Helper method for :func:`exhale.ExhaleRoot.generateDirectoryNodeDocuments`.
        Generates the reStructuredText documents for the given directory node.
        Directory nodes will only link to files and subdirectories within it.

        :Parameters:
            ``node`` (ExhaleNode)
                The directory node to generate the reStructuredText document for.
        '''
        # find the relevant children: directories and files only
        child_dirs  = []
        child_files = []
        for c in node.children:
            if c.kind == "dir":
                child_dirs.append(c)
            elif c.kind == "file":
                child_files.append(c)

        # generate the subdirectory section
        if len(child_dirs) > 0:
            child_dirs_string = "Subdirectories\n{}\n\n".format(EXHALE_SECTION_HEADING)
            for child_dir in sorted(child_dirs):
                child_dirs_string = "{}- :ref:`{}`\n".format(child_dirs_string, child_dir.link_name)
        else:
            child_dirs_string = ""

        # generate the files section
        if len(child_files) > 0:
            child_files_string = "Files\n{}\n\n".format(EXHALE_SECTION_HEADING)
            for child_file in sorted(child_files):
                child_files_string = "{}- :ref:`{}`\n".format(child_files_string, child_file.link_name)
        else:
            child_files_string = ""

        # generate the file for this directory
        try:
            with open(node.file_name, "w") as gen_file:
                # generate a link label for every generated file
                link_declaration = ".. _{}:\n\n".format(node.link_name)
                header = "{}\n{}\n\n".format(node.title, EXHALE_FILE_HEADING)
                # generate the headings and links for the children
                # write it all out
                gen_file.write("{}{}{}\n{}\n\n".format(
                    link_declaration, header, child_dirs_string, child_files_string)
                )
        except:
            exclaimError("Critical error while generating the file for [{}]".format(node.file_name))

    def generateAPIRootBody(self):
        '''
        Generates the root library api file's body text.  The method calls
        :func:`exhale.ExhaleRoot.gerrymanderNodeFilenames` first to enable proper
        internal linkage between reStructuredText documents.  Afterward, it calls
        :func:`exhale.ExhaleRoot.generateViewHierarchies` followed by
        :func:`exhale.ExhaleRoot.generateUnabridgedAPI` to generate both hierarchies as
        well as the full API listing.  As a result, three files will now be ready:

        1. ``self.class_view_file``
        2. ``self.directory_view_file``
        3. ``self.unabridged_api_file``

        These three files are then *included* into the root library file.  The
        consequence of using an ``include`` directive is that Sphinx will complain about
        these three files never being included in any ``toctree`` directive.  These
        warnings are expected, and preferred to using a ``toctree`` because otherwise
        the user would have to click on the class view link from the ``toctree`` in
        order to see it.  This behavior has been acceptable for me so far, but if it
        is causing you problems please raise an issue on GitHub and I may be able to
        conditionally use a ``toctree`` if you really need it.
        '''
        try:
            self.gerrymanderNodeFilenames()
            self.generateViewHierarchies()
            self.generateUnabridgedAPI()
            with open(self.full_root_file_path, "a") as generated_index:
                generated_index.write(
                    ".. include:: {}\n\n".format(self.class_view_file.split("/")[-1])
                )
                generated_index.write(
                    ".. include:: {}\n\n".format(self.directory_view_file.split("/")[-1])
                )
                generated_index.write(
                    ".. include:: {}\n\n".format(self.unabridged_api_file.split("/")[-1])
                )
        except Exception as e:
            exclaimError("Unable to create the root api body: {}".format(e))

    def gerrymanderNodeFilenames(self):
        '''
        When creating nodes, the filename needs to be relative to ``conf.py``, so it
        will include ``self.root_directory``.  However, when generating the API, the
        file we are writing to is in the same directory as the generated node files so
        we need to remove the directory path from a given ExhaleNode's ``file_name``
        before we can ``include`` it or use it in a ``toctree``.
        '''
        for node in self.all_nodes:
            node.file_name = node.file_name.split("/")[-1]
            if node.kind == "file":
                node.program_file = node.program_file.split("/")[-1]

    def generateViewHierarchies(self):
        '''
        Wrapper method to create the view hierarchies.  Currently it just calls
        :func:`exhale.ExhaleRoot.generateClassView` and
        :func:`exhale.ExhaleRoot.generateDirectoryView` --- if you want to implement
        additional hierarchies, implement the additionaly hierarchy method and call it
        from here.  Then make sure to ``include`` it in
        :func:`exhale.ExhaleRoot.generateAPIRootBody`.
        '''
        self.generateClassView(self.use_tree_view)
        self.generateDirectoryView(self.use_tree_view)

    def generateClassView(self, treeView):
        '''
        Generates the class view hierarchy, writing it to ``self.class_view_file``.

        :Parameters:
            ``treeView`` (bool)
                Whether or not to use the collapsibleList version.  See the
                ``createTreeView`` description in :func:`exhale.generate`.
        '''
        class_view_stream = cStringIO.StringIO()

        for n in self.namespaces:
            n.toClassView(0, class_view_stream, treeView)

        # Add everything that was not nested in a namespace.
        missing = []
        # class-like objects (structs and classes)
        for cl in sorted(self.class_like):
            if not cl.in_class_view:
                missing.append(cl)
        # enums
        for e in sorted(self.enums):
            if not e.in_class_view:
                missing.append(e)
        # unions
        for u in sorted(self.unions):
            if not u.in_class_view:
                missing.append(u)

        if len(missing) > 0:
            idx = 0
            last_missing_child = len(missing) - 1
            for m in missing:
                m.toClassView(0, class_view_stream, treeView, idx == last_missing_child)
                idx += 1
        elif treeView:
            # need to restart since there were no missing children found, otherwise the
            # last namespace will not correctly have a lastChild
            class_view_stream.close()
            class_view_stream = cStringIO.StringIO()

            last_nspace_index = len(self.namespaces) - 1
            for idx in range(last_nspace_index + 1):
                nspace = self.namespaces[idx]
                nspace.toClassView(0, class_view_stream, treeView, idx == last_nspace_index)

        # extract the value from the stream and close it down
        class_view_string = class_view_stream.getvalue()
        class_view_stream.close()

        # inject the raw html for the treeView unordered lists
        if treeView:
            # we need to indent everything to be under the .. raw:: html directive, add
            # indentation so the html is readable while we are at it
            indented = re.sub(r'(.+)', r'        \1', class_view_string)
            class_view_string =                               \
                '.. raw:: html\n\n'                           \
                '   <ul class="treeView">\n'                  \
                '     <li>\n'                                 \
                '       <ul class="collapsibleList">\n'       \
                '{}'                                          \
                '       </ul><!-- collapsibleList -->\n'      \
                '     </li><!-- only tree view element -->\n' \
                '   </ul><!-- treeView -->\n'.format(indented)

        # write everything to file to be included in the root api later
        try:
            with open(self.class_view_file, "w") as cvf:
                cvf.write("Class Hierarchy\n{}\n\n{}\n\n".format(EXHALE_SECTION_HEADING,
                                                                 class_view_string))
        except Exception as e:
            exclaimError("Error writing the class hierarchy: {}".format(e))

    def generateDirectoryView(self, treeView):
        '''
        Generates the file view hierarchy, writing it to ``self.directory_view_file``.

        :Parameters:
            ``treeView`` (bool)
                Whether or not to use the collapsibleList version.  See the
                ``createTreeView`` description in :func:`exhale.generate`.
        '''
        directory_view_stream = cStringIO.StringIO()

        for d in self.dirs:
            d.toDirectoryView(0, directory_view_stream, treeView)

        # add potential missing files (not sure if this is possible though)
        missing = []
        for f in sorted(self.files):
            if not f.in_directory_view:
                missing.append(f)

        found_missing = len(missing) > 0
        if found_missing:
            idx = 0
            last_missing_child = len(missing) - 1
            for m in missing:
                m.toDirectoryView(0, directory_view_stream, treeView, idx == last_missing_child)
                idx += 1
        elif treeView:
            # need to restart since there were no missing children found, otherwise the
            # last directory will not correctly have a lastChild
            directory_view_stream.close()
            directory_view_stream = cStringIO.StringIO()

            last_dir_index = len(self.dirs) - 1
            for idx in range(last_dir_index + 1):
                curr_d = self.dirs[idx]
                curr_d.toDirectoryView(0, directory_view_stream, treeView, idx == last_dir_index)

        # extract the value from the stream and close it down
        directory_view_string = directory_view_stream.getvalue()
        directory_view_stream.close()

        # inject the raw html for the treeView unordered lists
        if treeView:
            # we need to indent everything to be under the .. raw:: html directive, add
            # indentation so the html is readable while we are at it
            indented = re.sub(r'(.+)', r'        \1', directory_view_string)
            directory_view_string =                           \
                '.. raw:: html\n\n'                           \
                '   <ul class="treeView">\n'                  \
                '     <li>\n'                                 \
                '       <ul class="collapsibleList">\n'       \
                '{}'                                          \
                '       </ul><!-- collapsibleList -->\n'      \
                '     </li><!-- only tree view element -->\n' \
                '   </ul><!-- treeView -->\n'.format(indented)

        # write everything to file to be included in the root api later
        try:
            with open(self.directory_view_file, "w") as dvf:
                dvf.write("File Hierarchy\n{}\n\n{}\n\n".format(EXHALE_SECTION_HEADING,
                                                                directory_view_string))
        except Exception as e:
            exclaimError("Error writing the directory hierarchy: {}".format(e))

    def generateUnabridgedAPI(self):
        '''
        Generates the unabridged (full) API listing into ``self.unabridged_api_file``.
        This is necessary as some items may not show up in either hierarchy view,
        depending on:

        1. The item.  For example, if a namespace has only one member which is a
           variable, then neither the namespace nor the variable will be declared in the
           class view hierarchy.  It will be present in the file page it was declared in
           but not on the main library page.

        2. The configurations of Doxygen.  For example, see the warning in
           :func:`exhale.ExhaleRoot.fileRefDiscovery`.  Items whose parents cannot be
           rediscovered withouth the programlisting will still be documented, their link
           appearing in the unabridged API listing.

        Currently, the API is generated in the following (somewhat arbitrary) order:

        - Namespaces
        - Classes and Structs
        - Enums
        - Unions
        - Functions
        - Variables
        - Defines
        - Typedefs
        - Directories
        - Files

        If you want to change the ordering, just change the order of the calls to
        :func:`exhale.ExhaleRoot.enumerateAll` in this method.
        '''
        try:
            with open(self.unabridged_api_file, "w") as full_api_file:
                # write the header
                full_api_file.write("Full API\n{}\n\n".format(EXHALE_SECTION_HEADING))

                # recover all namespaces that were reparented
                all_namespaces = []
                for n in self.namespaces:
                    n.findNestedNamespaces(all_namespaces)

                # recover all directories that were reparented
                all_directories = []
                for d in self.dirs:
                    d.findNestedDirectories(all_directories)

                # recover classes and structs that were reparented
                all_class_like = []
                for cl in self.class_like:
                    cl.findNestedClassLike(all_class_like)

                # write everything to file: reorder these lines for different outcomes
                self.enumerateAll("Namespaces", all_namespaces, full_api_file)
                self.enumerateAll("Classes and Structs", all_class_like, full_api_file)
                self.enumerateAll("Enums", self.enums, full_api_file)
                self.enumerateAll("Unions", self.unions, full_api_file)
                self.enumerateAll("Functions", self.functions, full_api_file)
                self.enumerateAll("Variables", self.variables, full_api_file)
                self.enumerateAll("Defines", self.defines, full_api_file)
                self.enumerateAll("Typedefs", self.typedefs, full_api_file)
                self.enumerateAll("Directories", all_directories, full_api_file)
                self.enumerateAll("Files", self.files, full_api_file)
        except Exception as e:
            exclaimError("Error writing the unabridged API: {}".format(e))

    def enumerateAll(self, subsectionTitle, lst, openFile):
        '''
        Helper function for :func:`exhale.ExhaleRoot.generateUnabridgedAPI`.  Simply
        writes a subsection to ``openFile`` (a ``toctree`` to the ``file_name``) of each
        ExhaleNode in ``sorted(lst)`` if ``len(lst) > 0``.  Otherwise, nothing is
        written to the file.

        :Parameters:
            ``subsectionTitle`` (str)
                The title of this subsection, e.g. ``"Namespaces"`` or ``"Files"``.

            ``lst`` (list)
                The list of ExhaleNodes to be enumerated in this subsection.

            ``openFile`` (File)
                The **already open** file object to write to directly.  No safety checks
                are performed, make sure this is a real file object that has not been
                closed already.
        '''
        if len(lst) > 0:
            openFile.write("{}\n{}\n\n".format(subsectionTitle, EXHALE_SUBSECTION_HEADING))
            for l in sorted(lst):
                openFile.write(
                    ".. toctree::\n"
                    "   :maxdepth: {}\n\n"
                    "   {}\n\n".format(EXHALE_API_TOCTREE_MAX_DEPTH, l.file_name)
                )

    def generateAPIRootSummary(self):
        '''
        Writes the library API root summary to the main library file.  See the
        documentation for the key ``afterBodySummary`` in :func:`exhale.generate`.
        '''
        try:
            with open(self.full_root_file_path, "a") as generated_index:
                generated_index.write("{}\n\n".format(self.root_file_summary))
        except Exception as e:
            exclaimError("Unable to create the root api summary: {}".format(e))

    ####################################################################################
    #
    ##
    ### Miscellaneous utility functions.
    ##
    #
    ####################################################################################
    def toConsole(self):
        '''
        Convenience function for printing out the entire API being generated to the
        console.  Unused in the release, but is helpful for debugging ;)
        '''
        self.consoleFormat("Classes and Structs", self.class_like)
        self.consoleFormat("Defines", self.defines)
        self.consoleFormat("Enums", self.enums)
        self.consoleFormat("Enum Values", self.enum_values)
        self.consoleFormat("Functions", self.functions)
        self.consoleFormat("Files", self.files)
        self.consoleFormat("Directories", self.dirs)
        self.consoleFormat("Groups", self.groups)
        self.consoleFormat("Namespaces", self.namespaces)
        self.consoleFormat("Typedefs", self.typedefs)
        self.consoleFormat("Unions", self.unions)
        self.consoleFormat("Variables", self.variables)

    def consoleFormat(self, sectionTitle, lst):
        '''
        Helper method for :func:`exhale.ExhaleRoot.toConsole`.  Prints the given
        ``sectionTitle`` and calls :func:`exhale.ExhaleNode.toConsole` with ``0`` as the
        level for every ExhaleNode in ``lst``.

        :Parameters:
            ``sectionTitle`` (str)
                The title that will be printed with some visual separators around it.

            ``lst`` (list)
                The list of ExhaleNodes to print to the console.
        '''
        print("###########################################################")
        print("## {}".format(sectionTitle))
        print("###########################################################")
        for l in lst:
            l.toConsole(0)
