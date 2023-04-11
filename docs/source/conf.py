from __future__ import annotations

import pathlib
import subprocess
import sys
from importlib.metadata import version
from typing import TYPE_CHECKING, Any

if TYPE_CHECKING:
    from pybtex.richtext import HRef

import pybtex.plugin
from pybtex.style.formatting.unsrt import Style as UnsrtStyle
from pybtex.style.template import field, href

sys.path.insert(0, pathlib.Path(pathlib.Path(), "../..").resolve().as_posix())

# -- Project information -----------------------------------------------------
project = "DDSIM"
author = "Stefan Hillmich"
project_copyright = "Chair for Design Automation, Technical University of Munich"

# The full version, including alpha/beta/rc tags
release = version("mqt.ddsim")
version = ".".join(release.split(".")[:3])

# -- General configuration ---------------------------------------------------
extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.intersphinx",
    "sphinx.ext.autosummary",
    "sphinx.ext.mathjax",
    "sphinx.ext.napoleon",
    "sphinx.ext.viewcode",
    "sphinx.ext.githubpages",
    "sphinxcontrib.bibtex",
    "sphinx_copybutton",
    "sphinxext.opengraph",
    "sphinx_rtd_dark_mode",
    "nbsphinx",
    "breathe",
]

nbsphinx_execute = "auto"

nbsphinx_execute_arguments = [
    "--InlineBackend.figure_formats={'svg', 'pdf'}",
    "--InlineBackend.rc=figure.dpi=200",
]
nbsphinx_kernel_name = "python3"


class CDAStyle(UnsrtStyle):
    """Custom style for including PDF links."""

    def format_url(self, _e: Any) -> HRef:
        """Format URL field as a link to the PDF."""
        url = field("url", raw=True)
        return href()[url, "[PDF]"]


pybtex.plugin.register_plugin("pybtex.style.formatting", "cda_style", CDAStyle)

bibtex_bibfiles = ["refs.bib"]
bibtex_default_style = "cda_style"

copybutton_prompt_text = r"(?:\(venv\) )?\$ "
copybutton_prompt_is_regexp = True
copybutton_line_continuation_character = "\\"

autosummary_generate = True

breathe_projects = {"mqt.ddsim": "../doxygen/xml"}
breathe_default_project = "mqt.ddsim"
subprocess.call("cd ..; doxygen", shell=True)

# -- Options for HTML output -------------------------------------------------
html_theme = "sphinx_rtd_theme"
html_baseurl = "https://ddsim.readthedocs.io/en/latest/"
html_logo = "_static/mqt_light.png"
html_static_path = ["_static"]
html_css_files = ["custom.css"]
