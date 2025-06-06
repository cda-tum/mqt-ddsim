<!-- Entries in each category are sorted by merge time, with the latest PRs appearing first. -->

# Changelog

All notable changes to this project will be documented in this file.

The format is based on a mixture of [Keep a Changelog] and [Common Changelog].
This project adheres to [Semantic Versioning], with the exception that minor releases may include breaking changes.

## [Unreleased]

_If you are upgrading: please see [`UPGRADING.md`](UPGRADING.md#Unreleased)._

### Added

- ✨ Add Windows 11 ARM64 wheelsa and C++ testing ([#556]) ([**@burgholzer**])

### Changed

- ♻️ Move the C++ code for the Python bindings to the top-level `bindings` directory ([#567]) ([**@denialhaag**])
- ♻️ Move all Python code (no tests) to the top-level `python` directory ([#567]) ([**@denialhaag**])
- **Breaking**: 🚚 Move MQT DDSIM to the [munich-quantum-toolkit] GitHub organization
- **Breaking**: ♻️ Use the `mqt-core` Python package for handling circuits ([#336]) ([**@burgholzer**])
- **Breaking**: ⬆️ Bump minimum required CMake version to `3.24.0` ([#538]) ([**@burgholzer**])
- 📝 Rework existing project documentation ([#556]) ([**@burgholzer**])

### Removed

- **Breaking**: 🔥 Remove the TN flow from the path simulator ([#336]) ([**@burgholzer**])
- **Breaking**: 🔥 Remove some superfluous C++ executables ([#336]) ([**@burgholzer**])
- **Breaking**: 🔥 Remove support for `.real`, `.qc`, `.tfc`, and `GRCS` files ([#538]) ([**@burgholzer**])

## [1.24.0] - 2024-10-10

_📚 Refer to the [GitHub Release Notes] for previous changelogs._

<!-- Version links -->

[unreleased]: https://github.com/munich-quantum-toolkit/ddsim/compare/v1.24.0...HEAD
[1.24.0]: https://github.com/munich-quantum-toolkit/ddsim/releases/tag/v1.24.0

<!-- PR links -->

[#556]: https://github.com/munich-quantum-toolkit/ddsim/pulls/556
[#538]: https://github.com/munich-quantum-toolkit/ddsim/pulls/538
[#336]: https://github.com/munich-quantum-toolkit/ddsim/pulls/336

<!-- Contributor -->

[**@burgholzer**]: https://github.com/burgholzer

<!-- General links -->

[Keep a Changelog]: https://keepachangelog.com/en/1.1.0/
[Common Changelog]: https://common-changelog.org
[Semantic Versioning]: https://semver.org/spec/v2.0.0.html
[GitHub Release Notes]: https://github.com/munich-quantum-toolkit/ddsim/releases
[munich-quantum-toolkit]: https://github.com/munich-quantum-toolkit
