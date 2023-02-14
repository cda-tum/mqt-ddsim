import pathlib
import subprocess
import sys
from importlib.metadata import version

sys.path.insert(0, str(pathlib.Path("../../mqt").resolve()))

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
    "breathe",
]

bibtex_bibfiles = ["refs.bib"]
bibtex_reference_style = "author_year"

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
