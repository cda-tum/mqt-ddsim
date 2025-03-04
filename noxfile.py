"""Nox sessions."""

from __future__ import annotations

import argparse
import os
import shutil
import sys
from typing import TYPE_CHECKING

import nox

if TYPE_CHECKING:
    from collections.abc import Sequence

nox.needs_version = ">=2024.3.2"
nox.options.default_venv_backend = "uv"

nox.options.sessions = ["lint", "tests", "minimums"]

PYTHON_ALL_VERSIONS = ["3.9", "3.10", "3.11", "3.12", "3.13"]

if os.environ.get("CI", None):
    nox.options.error_on_missing_interpreters = True


@nox.session(reuse_venv=True)
def lint(session: nox.Session) -> None:
    """Run the linter."""
    if shutil.which("pre-commit") is None:
        session.install("pre-commit")

    session.run("pre-commit", "run", "--all-files", *session.posargs, external=True)


def _run_tests(
    session: nox.Session,
    *,
    install_args: Sequence[str] = (),
    run_args: Sequence[str] = (),
) -> None:
    env = {"UV_PROJECT_ENVIRONMENT": session.virtualenv.location}
    if os.environ.get("CI", None) and sys.platform == "win32":
        env["SKBUILD_CMAKE_ARGS"] = "-T ClangCL"

    if shutil.which("cmake") is None and shutil.which("cmake3") is None:
        session.install("cmake")
    if shutil.which("ninja") is None:
        session.install("ninja")

    # install build and test dependencies on top of the existing environment
    session.run(
        "uv",
        "sync",
        "--inexact",
        "--only-group",
        "build",
        "--only-group",
        "test",
        "--verbose",
        # Build mqt-core from source to work around pybind believing that two
        # compiled extensions might not be binary compatible.
        # This will be fixed in a new pybind11 release that includes https://github.com/pybind/pybind11/pull/5439.
        "--no-binary-package",
        "mqt-core",
        *install_args,
        env=env,
    )
    session.run(
        "uv",
        "run",
        "--no-dev",  # do not auto-install dev dependencies
        "--no-build-isolation-package",
        "mqt-ddsim",  # build the project without isolation
        "--verbose",
        *install_args,
        "pytest",
        *run_args,
        *session.posargs,
        "--cov-config=pyproject.toml",
        env=env,
    )


@nox.session(reuse_venv=True, python=PYTHON_ALL_VERSIONS)
def tests(session: nox.Session) -> None:
    """Run the test suite."""
    _run_tests(session)


@nox.session(reuse_venv=True, venv_backend="uv", python=PYTHON_ALL_VERSIONS)
def minimums(session: nox.Session) -> None:
    """Test the minimum versions of dependencies."""
    _run_tests(
        session,
        install_args=["--resolution=lowest-direct"],
        run_args=["-Wdefault"],
    )
    env = {"UV_PROJECT_ENVIRONMENT": session.virtualenv.location}
    session.run("uv", "tree", "--frozen", env=env)
    session.run("uv", "lock", "--refresh", env=env)


@nox.session(reuse_venv=True)
def docs(session: nox.Session) -> None:
    """Build the docs. Use "--non-interactive" to avoid serving. Pass "-b linkcheck" to check links."""
    parser = argparse.ArgumentParser()
    parser.add_argument("-b", dest="builder", default="html", help="Build target (default: html)")
    args, posargs = parser.parse_known_args(session.posargs)

    serve = args.builder == "html" and session.interactive
    if serve:
        session.install("sphinx-autobuild")

    env = {"UV_PROJECT_ENVIRONMENT": session.virtualenv.location}
    # install build and docs dependencies on top of the existing environment
    session.run(
        "uv",
        "sync",
        "--inexact",
        "--only-group",
        "build",
        "--only-group",
        "docs",
        # Build mqt-core from source to work around pybind believing that two
        # compiled extensions might not be binary compatible.
        # This will be fixed in a new pybind11 release that includes https://github.com/pybind/pybind11/pull/5439.
        "--no-binary-package",
        "mqt-core",
        env=env,
    )

    shared_args = (
        "-n",  # nitpicky mode
        "-T",  # full tracebacks
        f"-b={args.builder}",
        "docs",
        f"docs/_build/{args.builder}",
        *posargs,
    )

    session.run(
        "uv",
        "run",
        "--no-dev",  # do not auto-install dev dependencies
        "--no-build-isolation-package",
        "mqt-ddsim",  # build the project without isolation
        "sphinx-autobuild" if serve else "sphinx-build",
        *shared_args,
        env=env,
    )
