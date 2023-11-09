from __future__ import annotations

import functools
from concurrent import futures
from typing import TYPE_CHECKING, Any, Callable, Mapping, Sequence, Union

from qiskit.providers import JobError, JobStatus, JobV1

if TYPE_CHECKING:
    from qiskit import QuantumCircuit
    from qiskit.circuit import Parameter
    from qiskit.circuit.parameterexpression import ParameterValueType
    from qiskit.providers import BackendV2
    from qiskit.result import Result

    Parameters = Union[Mapping[Parameter, ParameterValueType], Sequence[ParameterValueType]]


def requires_submit(func: Callable[..., Any]) -> Callable[..., Any]:
    """Decorator to ensure that a submit has been performed before
    calling the method.

    Args:
        func (callable): test function to be decorated.

    Returns:
        callable: the decorated function.
    """

    @functools.wraps(func)
    def _wrapper(self: DDSIMJob, *args: Any, **kwargs: Any) -> Any:  # noqa: ANN401
        if self._future is None:
            msg = "Job not submitted yet!. You have to .submit() first!"
            raise JobError(msg)
        return func(self, *args, **kwargs)

    return _wrapper


class DDSIMJob(JobV1):
    """DDSIMJob class.

    Attributes:
        _executor (futures.Executor): executor to handle asynchronous jobs
    """

    _executor = futures.ThreadPoolExecutor(max_workers=1)

    def __init__(
        self,
        backend: BackendV2,
        job_id: str,
        fn: Callable,
        experiments: Sequence[QuantumCircuit],
        parameter_values: Sequence[Parameters] | None,
        **args: dict[str, Any],
    ) -> None:
        super().__init__(backend, job_id)
        self._fn = fn
        self._experiments = experiments
        self._parameter_values = parameter_values
        self._args = args
        self._future: futures.Future | None = None

    def submit(self) -> None:
        """Submit the job to the backend for execution.

        Raises:
            JobError: if trying to re-submit the job.
        """
        if self._future is not None:
            msg = "Job was already submitted!"
            raise JobError(msg)

        self._future = self._executor.submit(
            self._fn,
            self._job_id,
            self._experiments,
            self._parameter_values,
            **self._args,
        )

    @requires_submit
    def result(self, timeout: float | None = None) -> Result:
        # pylint: disable=arguments-differ
        """Get job result. The behavior is the same as the underlying
        concurrent Future objects,
        https://docs.python.org/3/library/concurrent.futures.html#future-objects.

        Args:
            timeout (float): number of seconds to wait for results.

        Returns:
            qiskit.Result: Result object
        Raises:
            concurrent.futures.TimeoutError: if timeout occurred.
            concurrent.futures.CancelledError: if job cancelled before completed.
        """
        assert self._future is not None
        return self._future.result(timeout=timeout)

    @requires_submit
    def cancel(self) -> bool:
        """Attempt to cancel the job."""
        assert self._future is not None
        return self._future.cancel()

    @requires_submit
    def status(self) -> JobStatus:
        """Gets the status of the job by querying the Python's future.

        Returns:
            JobStatus: The current JobStatus
        Raises:
            JobError: If the future is in unexpected state
            concurrent.futures.TimeoutError: if timeout occurred.
        """
        # The order is important here
        assert self._future is not None
        if self._future.running():
            return JobStatus.RUNNING
        if self._future.cancelled():
            return JobStatus.CANCELLED
        if self._future.done():
            return JobStatus.DONE if self._future.exception() is None else JobStatus.ERROR

        # Note: There is an undocumented Future state: PENDING, that seems to show up when
        # the job is enqueued, waiting for someone to pick it up. We need to deal with this
        # state but there's no public API for it, so we are assuming that if the job is not
        # in any of the previous states, is PENDING, ergo INITIALIZING for us.
        return JobStatus.INITIALIZING

    def backend(self) -> BackendV2 | None:
        """Return the instance of the backend used for this job."""
        return self._backend
