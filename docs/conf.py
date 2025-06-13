# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Sphinx configuration file."""

from __future__ import annotations

import os
import subprocess
import warnings
from importlib import metadata
from pathlib import Path
from typing import TYPE_CHECKING

import pybtex.plugin
from pybtex.style.formatting.unsrt import Style as UnsrtStyle
from pybtex.style.template import field, href

if TYPE_CHECKING:
    from pybtex.database import Entry
    from pybtex.richtext import HRef

ROOT = Path(__file__).parent.parent.resolve()


try:
    from mqt.ddsim import __version__ as version
except ModuleNotFoundError:
    try:
        version = metadata.version("mqt.ddsim")
    except ModuleNotFoundError:
        msg = (
            "Package should be installed to produce documentation! "
            "Assuming a modern git archive was used for version discovery."
        )
        warnings.warn(msg, stacklevel=1)

        from setuptools_scm import get_version

        version = get_version(root=str(ROOT), fallback_root=ROOT)

# Filter git details from version
release = version.split("+")[0]

project = "MQT DDSIM"
author = "Chair for Design Automation, TUM & Munich Quantum Software Company"
language = "en"
project_copyright = "2023 - 2025 Chair for Design Automation, TUM & 2025 Munich Quantum Software Company"

master_doc = "index"

templates_path = ["_templates"]

extensions = [
    "myst_nb",
    "autoapi.extension",
    "sphinx.ext.autodoc",
    "sphinx.ext.intersphinx",
    "sphinx.ext.napoleon",
    "sphinx_copybutton",
    "sphinx_design",
    "sphinxext.opengraph",
    "sphinx.ext.viewcode",
    "sphinxcontrib.inkscapeconverter",
    "sphinxcontrib.bibtex",
    "breathe",
]

source_suffix = [".rst", ".md"]

exclude_patterns = [
    "_build",
    "**.ipynb_checkpoints",
    "**.jupyter_cache",
    "**jupyter_execute",
    "Thumbs.db",
    ".DS_Store",
    ".env",
    ".venv",
]

pygments_style = "colorful"

intersphinx_mapping = {
    "python": ("https://docs.python.org/3", None),
    "typing_extensions": ("https://typing-extensions.readthedocs.io/en/latest", None),
    "numpy": ("https://numpy.org/doc/stable/", None),
    "qiskit": ("https://docs.quantum.ibm.com/api/qiskit", None),
    "mqt": ("https://mqt.readthedocs.io/en/latest", None),
    "core": ("https://mqt.readthedocs.io/projects/core/en/latest", None),
    "qcec": ("https://mqt.readthedocs.io/projects/qcec/en/latest", None),
    "qmap": ("https://mqt.readthedocs.io/projects/qmap/en/latest", None),
    "qecc": ("https://mqt.readthedocs.io/projects/qecc/en/latest", None),
    "syrec": ("https://mqt.readthedocs.io/projects/syrec/en/latest", None),
}

myst_enable_extensions = [
    "amsmath",
    "colon_fence",
    "substitution",
    "deflist",
    "dollarmath",
]
myst_substitutions = {
    "version": version,
}
myst_heading_anchors = 3

# -- Options for {MyST}NB ----------------------------------------------------

nb_execution_mode = "cache"
nb_mime_priority_overrides = [
    # builder name, mime type, priority
    ("latex", "image/svg+xml", 15),
]


class CDAStyle(UnsrtStyle):
    """Custom style for including PDF links."""

    def format_url(self, _e: Entry) -> HRef:  # noqa: PLR6301
        """Format URL field as a link to the PDF.

        Returns:
            The formatted URL field.
        """
        url = field("url", raw=True)
        return href()[url, "[PDF]"]


pybtex.plugin.register_plugin("pybtex.style.formatting", "cda_style", CDAStyle)

bibtex_bibfiles = ["lit_header.bib", "refs.bib"]
bibtex_default_style = "cda_style"

copybutton_prompt_text = r"(?:\(\.?venv\) )?(?:\[.*\] )?\$ "
copybutton_prompt_is_regexp = True
copybutton_line_continuation_character = "\\"

modindex_common_prefix = ["mqt.ddsim."]

autoapi_dirs = ["../python/mqt"]
autoapi_python_use_implicit_namespaces = True
autoapi_root = "api"
autoapi_add_toctree_entry = False
autoapi_ignore = [
    "*/**/_version.py",
]
autoapi_options = [
    "members",
    "imported-members",
    "show-inheritance",
    "special-members",
    "undoc-members",
]
autoapi_keep_files = True
add_module_names = False
toc_object_entries_show_parents = "hide"
python_use_unqualified_type_names = True
napoleon_google_docstring = True
napoleon_numpy_docstring = False


breathe_projects = {"mqt.ddsim": "_build/doxygen/xml"}
breathe_default_project = "mqt.ddsim"

read_the_docs_build = os.environ.get("READTHEDOCS", None) == "True"
if read_the_docs_build:
    subprocess.call("mkdir -p _build/doxygen && doxygen", shell=True)  # noqa: S602, S607
    subprocess.call(  # noqa: S602
        "mkdir -p api/cpp && breathe-apidoc -o api/cpp -m -f -g file _build/doxygen/xml/",  # noqa: S607
        shell=True,
    )

# -- Options for HTML output -------------------------------------------------
html_theme = "furo"
html_static_path = ["_static"]
html_css_files = [
    "custom.css",
    "https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/fontawesome.min.css",
    "https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/solid.min.css",
    "https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/brands.min.css",
]
html_theme_options = {
    "light_logo": "mqt_dark.png",
    "dark_logo": "mqt_light.png",
    "source_repository": "https://github.com/munich-quantum-toolkit/ddsim/",
    "source_branch": "main",
    "source_directory": "docs/",
    "navigation_with_keys": True,
    "footer_icons": [
        {
            "name": "GitHub",
            "url": "https://github.com/munich-quantum-toolkit/ddsim/",
            "html": "",
            "class": "fa-brands fa-solid fa-github fa-2x",
        },
        {
            "name": "PyPI",
            "url": "https://pypi.org/project/mqt-ddsim/",
            "html": "",
            "class": "fa-brands fa-solid fa-python fa-2x",
        },
    ],
}

# -- Options for LaTeX output ------------------------------------------------

numfig = True
numfig_secnum_depth = 0

sd_fontawesome_latex = True
image_converter_args = ["-density", "300"]
latex_engine = "pdflatex"
latex_documents = [
    (
        master_doc,
        "mqt_ddsim.tex",
        r"MQT DDSIM\\{\Large A quantum circuit simulator based on decision diagrams}",
        r"""Chair for Design Automation\\ Technical University of Munich, Germany\\
        \href{mailto:quantum.cda@xcit.tum.de}{quantum.cda@xcit.tum.de}\\
        Munich Quantum Software Company GmbH\\Garching near Munich, Germany""",
        "howto",
        False,
    ),
]
latex_logo = "_static/mqt_dark.png"
latex_elements = {
    "papersize": "letterpaper",
    "releasename": "Version",
    "printindex": r"\footnotesize\raggedright\printindex",
    "tableofcontents": "",
    "sphinxsetup": "iconpackage=fontawesome",
    "extrapackages": r"\usepackage{qrcode,graphicx,calc,amsthm,etoolbox,flushend,mathtools}",
    "preamble": r"""
\patchcmd{\thebibliography}{\addcontentsline{toc}{section}{\refname}}{}{}{}
\DeclarePairedDelimiter\abs{\lvert}{\rvert}
\DeclarePairedDelimiter\mket{\lvert}{\rangle}
\DeclarePairedDelimiter\mbra{\langle}{\rvert}
\DeclareUnicodeCharacter{03C0}{$\pi$}
\DeclareUnicodeCharacter{2728}{\faicon{star}}
\DeclareUnicodeCharacter{1F6B8}{\faicon{user-plus}}
\DeclareUnicodeCharacter{1F4DD}{\faicon{book}}
\DeclareUnicodeCharacter{1F69A}{\faicon{truck}}
\DeclareUnicodeCharacter{267B}{\faicon{recycle}}
\DeclareUnicodeCharacter{2B06}{\faicon{arrow-up}}
\DeclareUnicodeCharacter{1F4C4}{\faicon{file-alt}}
\DeclareUnicodeCharacter{1F525}{\faicon{fire}}
\DeclareUnicodeCharacter{1F41B}{\faicon{bug}}
\DeclareUnicodeCharacter{1F4DA}{\faicon{book-open}}
\DeclareUnicodeCharacter{1F4E6}{\faicon{archive}}
\DeclareUnicodeCharacter{23EA}{\faicon{angle-double-left}}
\DeclareUnicodeCharacter{FE0F}{}

\newcommand*{\ket}[1]{\ensuremath{\mket{\mkern1mu#1}}}
\newcommand*{\bra}[1]{\ensuremath{\mbra{\mkern1mu#1}}}
\newtheorem{example}{Example}
\clubpenalty=10000
\widowpenalty=10000
\interlinepenalty 10000
\def\subparagraph{} % because IEEE classes don't define this, but titlesec assumes it's present
""",
    "extraclassoptions": r"journal, onecolumn",
    "fvset": r"\fvset{fontsize=\small}",
    "figure_align": "htb",
}
latex_domain_indices = False
latex_docclass = {
    "howto": "IEEEtran",
}
