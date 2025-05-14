# MQT DDSIM - A quantum circuit simulator based on Decision Diagrams

```{raw} latex
\begin{abstract}
```

MQT DDSIM is an open-source C++17 and Python library for classical quantum circuit simulation developed as part of the _{doc}`Munich Quantum Toolkit (MQT) <mqt:index>`_ [^1].

This documentation provides a comprehensive guide to the MQT DDSIM library, including {doc}`installation instructions <installation>`, a {doc}`quickstart guide <quickstart>`, and detailed {doc}`API documentation <api/mqt/ddsim/index>`.
The source code of MQT DDSIM is publicly available on GitHub at [munich-quantum-toolkit/ddsim](https://github.com/munich-quantum-toolkit/ddsim), while pre-built binaries are available via [PyPI](https://pypi.org/project/mqt.ddsim/) for all major operating systems and all modern Python versions.
MQT DDSIM is fully compatible with Qiskit 1.0 and above.

[^1]:
    The _[Munich Quantum Toolkit (MQT)](https://mqt.readthedocs.io)_ is a collection of software tools for quantum computing developed by the [Chair for Design Automation](https://www.cda.cit.tum.de/) at the [Technical University of Munich](https://www.tum.de/) as well as the [Munich Quantum Software Company (MQSC)](https://munichquantum.software).
    Among others, it is part of the [Munich Quantum Software Stack (MQSS)](https://www.munich-quantum-valley.de/research/research-areas/mqss) ecosystem, which is being developed as part of the [Munich Quantum Valley (MQV)](https://www.munich-quantum-valley.de) initiative.

````{only} latex
```{note}
A live version of this document is available at [mqt.readthedocs.io/projects/ddsim](https://mqt.readthedocs.io/projects/ddsim).
```
````

```{raw} latex
\end{abstract}

\sphinxtableofcontents
```

```{toctree}
:hidden:

self
```

```{toctree}
:maxdepth: 1
:caption: User Guide

installation
quickstart
simulators
primitives
references
CHANGELOG
UPGRADING
```

````{only} not latex
```{toctree}
:maxdepth: 2
:titlesonly:
:caption: Developers
:glob:

contributing
support
development_guide
```
````

```{toctree}
:caption: Python API Reference
:maxdepth: 1

api/mqt/ddsim/index
```

```{toctree}
:glob:
:caption: C++ API Reference
:maxdepth: 1

api/cpp/filelist
```
