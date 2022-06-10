from importlib.metadata import version


# -- Project information -----------------------------------------------------
project = 'DDSIM'
copyright = '2022, Stefan Hillmich'
author = 'Stefan Hillmich'

# The full version, including alpha/beta/rc tags
release = version('mqt.ddsim')
version = '.'.join(release.split('.')[:3])

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
    "sphinx_copybutton"
]

bibtex_bibfiles = ['refs.bib']
bibtex_reference_style = 'author_year'

copybutton_prompt_text = r"(?:\(venv\) )?\$ "
copybutton_prompt_is_regexp = True
copybutton_line_continuation_character = "\\"

autosummary_generate = True

# -- Options for HTML output -------------------------------------------------
html_theme = 'sphinx_rtd_theme'
