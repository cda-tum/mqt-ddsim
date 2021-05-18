from qiskit.providers import ProviderV1 as Provider
from qiskit.providers.providerutils import filter_backends

from .qasmsimulator import QasmSimulator
from .statevectorsimulator import StatevectorSimulator


class JKQProvider(Provider):
    _BACKENDS = None

    def __init__(self):
        if JKQProvider._BACKENDS is None:
            JKQProvider._BACKENDS = [
                ('qasm_simulator', QasmSimulator, None, None),
                ('statevector_simulator', StatevectorSimulator, None, None),
            ]

    def get_backend(self, name=None, **kwargs):
        return super().get_backend(name=name, **kwargs)

    def backends(self, name=None, filters=None, **kwargs):
        # pylint: disable=arguments-differ
        # Instantiate a new backend instance so if config options
        # are set they will only last as long as that backend object exists
        backends = []
        for backend_name, backend_cls, method, device in self._BACKENDS:
            opts = {'provider': self}
            if method is not None:
                opts['method'] = method
            if device is not None:
                opts['device'] = device
            if name is None or backend_name == name:
                backends.append(backend_cls(**opts))
        return filter_backends(backends, filters=filters)

    def __str__(self):
        return 'JKQProvider'
